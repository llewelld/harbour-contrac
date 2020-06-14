/*
 * Copyright (c) 2014 Ian Delahorne <ian.delahorne@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "s3.h"
#include "s3internal.h"

void 
s3_bucket_entry_free(struct s3_bucket_entry *entry) {
	if (entry->key)
		free(entry->key);
	if (entry->lastmod)
		free(entry->lastmod);
	if (entry->etag)
		free(entry->etag);

	free(entry);
}

void
s3_bucket_entries_free(struct s3_bucket_entry_head *entries) {
	struct s3_bucket_entry *e;

	while ((e = TAILQ_FIRST(entries)) != NULL) {
		TAILQ_REMOVE(entries, e, list);
		s3_bucket_entry_free(e);
	}
	free(entries);
}


static struct s3_bucket_entry *
s3_bucket_entry_from_node(xmlNode *root) {
	struct s3_bucket_entry *entry = malloc(sizeof(struct s3_bucket_entry));
	xmlNode *node = NULL;
	for (node = root; node; node = node->next) {
		if (node->type == XML_ELEMENT_NODE) {
			xmlChar *value =  xmlNodeGetContent(node->children);
			size_t len = strlen((const char *) value);

			if (strcasecmp("key", (const char *)node->name) == 0) {
				entry->key = malloc(len + 2);
				strlcpy(entry->key, (const char *)value, len + 1);
			} else if (strcasecmp("lastmodified", (const char *)node->name) == 0) {
				entry->lastmod = malloc(len + 2);
				strlcpy(entry->lastmod, (const char *)value, len + 1);
			} else if (strcasecmp("etag", (const char *)node->name) == 0) {
				entry->etag = malloc(len + 2);
				strlcpy(entry->etag, (const char *)value, len + 1);
			} else {
#ifdef DEBUG
				printf("node type: Element, name: %s\n", node->name);
				printf("node xmlNodeGetContent: %s\n",value);
#endif
			}
			xmlFree(value);
		}
	}
	return entry;	
}

#ifdef WALK_CONTENT_PREFIXES
static void
s3_parse_bucket_prefixes(xmlNode *root) {
	xmlNode *node = NULL;
	for (node = root; node; node = node->next) {
		if (node->type == XML_ELEMENT_NODE) {
			xmlChar *value =  xmlNodeGetContent(node->children);
			
			printf("Prefix node type: Element, name: %s\n", node->name);
			printf("Prefix node xmlsNodeGetContent: %s\n",value);
			xmlFree(value);
		}
	}
}

static void
s3_walk_content_prefixes(xmlNodeSetPtr nodes, void *data) {
	xmlNodePtr cur;
	int size;
	int i;

	size = (nodes) ? nodes->nodeNr : 0;
#ifdef DEBUG
	printf("prefix size is %d nodes\n", size);
#endif
	for (i = 0; i < size ; i++) {
		if (nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
			cur = nodes->nodeTab[i];
			s3_parse_bucket_prefixes(cur->children);
		}
	}
}
#endif

static void 
s3_walk_content_nodes(xmlNodeSetPtr nodes, void *data) {
	struct s3_bucket_entry_head *head;
	xmlNodePtr cur;
	int size;
	int i;

	head = (struct s3_bucket_entry_head *) data;

	size = (nodes) ? nodes->nodeNr : 0;
#ifdef DEBUG
	printf("size is %d nodes\n", size);
#endif
	for (i = 0; i < size ; i++) {
		if (nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
			cur = nodes->nodeTab[i];
			
			struct s3_bucket_entry *entry = s3_bucket_entry_from_node(cur->children);
			TAILQ_INSERT_TAIL(head, entry, list);
		}
	}
}

struct s3_bucket_entry_head *
s3_parse_bucket_response(char *xml) {
	struct s3_bucket_entry_head *entries;
	xmlDocPtr doc;

	doc = xmlReadMemory(xml, strlen(xml), "noname.xml", NULL, 0);
	if (doc == NULL)
		return NULL;

	entries = malloc(sizeof (*entries));
	TAILQ_INIT(entries);
	
	/* 
	 * Since Amazon uses an XML Namespace, we need to declare it
	 * and use it as a prefix in Xpath queries, even though it's
	 * not written out in the tag names - libxml2 follows the
	 * standard where others don't
	 */
	s3_execute_xpath_expr(doc, (const xmlChar *)"//amzn:Contents", s3_walk_content_nodes, entries);
#ifdef WALK_CONTENT_PREFIXES
	s3_execute_xpath_expr(doc, (const xmlChar *)"//amzn:CommonPrefixes", s3_walk_content_prefixes, NULL);
#endif
	xmlFreeDoc(doc);
	xmlCleanupParser();

	return entries;
}

struct s3_bucket_entry_head *
s3_list_bucket(struct S3 *s3, const char *bucket, const char *prefix) {
	char *date;
	char *sign_data;	
	char *url;
	struct s3_string *str;
	const char *method = "GET";
	struct s3_bucket_entry_head *entries;

	str = s3_string_init();		
	date = s3_make_date();

	asprintf(&sign_data, "%s\n\n\n%s\n/%s/", method, date, bucket);	
    asprintf(&url, "http://%s.%s/?delimiter=/%s%s", bucket, s3->base_url, prefix ? "&prefix=" : "", prefix ? prefix : "");

	s3_perform_op(s3, method, url, sign_data, date, str, NULL, NULL, NULL);

	entries = s3_parse_bucket_response(str->ptr);

	s3_string_free(str);
	free(url);
	free(sign_data);
	free(date);

	return entries;
}
