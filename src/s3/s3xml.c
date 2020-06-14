/*
 * Copyright (c) 2013, Ian Delahorne <ian.delahorne@gmail.com>
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

#include "s3xml.h"

static void
s3_register_namespaces(xmlXPathContextPtr ctx, const xmlChar *nsList) {
	xmlChar* nsListDup;
	xmlChar* prefix;
	xmlChar* href;
	xmlChar* next;
	
	nsListDup = xmlStrdup(nsList);
	if(nsListDup == NULL) {
		fprintf(stderr, "Error: unable to strdup namespaces list\n");
		return;
	}
	
	next = nsListDup; 
	while(next != NULL) {
		/* skip spaces */
		while((*next) == ' ') next++;
		if((*next) == '\0') break;
		
		/* find prefix */
		prefix = next;
		next = (xmlChar*)xmlStrchr(next, '=');
		if(next == NULL) {
			fprintf(stderr,"Error: invalid namespaces list format\n");
			xmlFree(nsListDup);
			return;
		}
		*(next++) = '\0';	
		
		/* find href */
		href = next;
		next = (xmlChar*)xmlStrchr(next, ' ');
		if(next != NULL) {
			*(next++) = '\0';	
		}
		
		/* do register namespace */
		if(xmlXPathRegisterNs(ctx, prefix, href) != 0) {
			fprintf(stderr,"Error: unable to register NS with prefix=\"%s\" and href=\"%s\"\n", prefix, href);
			xmlFree(nsListDup);
			return;
		}
	}
	
	xmlFree(nsListDup);
}

void 
s3_execute_xpath_expr(const xmlDocPtr doc, const xmlChar *xpath_expr, void (*nodeset_cb)(xmlNodeSetPtr, void *), void *cb_data) {
	xmlXPathContextPtr xpath_ctx;
	xmlXPathObjectPtr xpath_obj;
	const xmlChar *ns_list = (const xmlChar *)"amzn=http://s3.amazonaws.com/doc/2006-03-01/";
	
	xpath_ctx = xmlXPathNewContext(doc);
	if (xpath_ctx == NULL) {
		fprintf(stderr,"Error: unable to create new XPath context\n");
		return;
	}

	s3_register_namespaces(xpath_ctx, ns_list);
	
	xpath_obj = xmlXPathEvalExpression(xpath_expr, xpath_ctx);
	if(xpath_obj == NULL) {
		fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", xpath_expr);
	} else {

		nodeset_cb(xpath_obj->nodesetval, cb_data);
		xmlXPathFreeObject(xpath_obj);
	}

	xmlXPathFreeContext(xpath_ctx); 
}
