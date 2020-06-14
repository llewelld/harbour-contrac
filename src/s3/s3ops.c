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

#include <stdlib.h>

#include <curl/curl.h>

#include "s3.h"
#include "s3internal.h"
#include <stdio.h>

struct S3 *
s3_init(const char *id, const char *secret, const char *base_url) {
	struct S3 *s3 = malloc(sizeof (struct S3));

	s3->id = malloc(S3_ID_LENGTH);
	s3->secret = malloc(S3_SECRET_LENGTH);
	/* XXX better length */
	s3->base_url = malloc(256); 
	
	strlcpy(s3->id, id, S3_ID_LENGTH);
	strlcpy(s3->secret, secret, S3_SECRET_LENGTH);
	strlcpy(s3->base_url, base_url, 255);
	
	s3->proxy = NULL;

	return s3;
}

void
s3_free(struct S3 *s3) {
	free(s3->id);
	free(s3->secret);
	free(s3->base_url);
	
	free(s3);
}

char *
s3_make_date(void) {
	char *date;
	time_t now;
	struct tm tm;

	date = malloc(128);
	now = time(0);
	tm = *gmtime(&now);
	strftime(date, 128, "%a, %d %b %Y %H:%M:%S %Z", &tm);

	return date;

}

/* Add return values later */
void
s3_perform_op(struct S3 *s3, const char *method, const char *url, const char *sign_data, const char *date, struct s3_string *out, struct s3_string *in, const char *content_md5, const char *content_type) {
	char *digest;
	char *hdr;
	
	CURL *curl;
	struct curl_slist *headers = NULL;



	digest = s3_hmac_sign(s3->secret, sign_data, strlen(sign_data));
#ifdef DEBUG
	fprintf(stderr, "DEBUG: data to sign:%s\n", sign_data);
	fprintf(stderr, "DEBUG: Authentication: AWS %s:%s\n", s3->id, digest);
#endif
	curl = curl_easy_init();
	
	hdr = malloc(1024);

	if (strcmp(method, "DELETE") == 0) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
	} else if (strcmp(method, "PUT") == 0) {
		if (content_type) {
			snprintf(hdr, 1023, "Content-Type: %s", content_type);
			headers = curl_slist_append(headers, hdr);
		}
		
		if (content_md5) {
			snprintf(hdr, 1023, "Content-MD5: %s", content_md5);
			headers = curl_slist_append(headers, hdr);
		}		
		
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, s3_string_curl_readfunc);
		curl_easy_setopt(curl, CURLOPT_READDATA, in);
		curl_easy_setopt(curl, CURLOPT_INFILESIZE, in->len);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
	}

	snprintf(hdr, 1023, "Date: %s", date);
	headers = curl_slist_append(headers, hdr);

	snprintf(hdr, 1023, "Authorization: AWS %s:%s", s3->id, digest);
	headers = curl_slist_append(headers, hdr);
	free(hdr);
	
#ifdef DEBUG
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(curl, CURLOPT_HEADER, 1);
#endif
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, s3_string_curl_writefunc);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, out);

	curl_easy_setopt(curl, CURLOPT_URL, url);

	if (s3->proxy) {
		curl_easy_setopt(curl, CURLOPT_PROXY, s3->proxy);
	}

	curl_easy_perform(curl);

	curl_easy_cleanup(curl);
	curl_slist_free_all(headers);

	free(digest);	
}


void
s3_get(struct S3 *s3, const char *bucket, const char *key, struct s3_string *out) {
	const char *method = "GET";
	char *sign_data;
	char *date;
	char *url;
	
	date = s3_make_date();

	asprintf(&sign_data, "%s\n\n\n%s\n/%s/%s", method, date, bucket, key);	
	asprintf(&url, "http://%s.%s/%s", bucket, s3->base_url, key);
	
	s3_perform_op(s3, method, url, sign_data, date, out, NULL, NULL, NULL);

	free(sign_data);
	free(date);
	free(url);
}


void
s3_delete(struct S3 *s3, const char *bucket, const char *key) {
	char *sign_data;
	char *url;
	char *date;
	const char *method = "DELETE";
	struct s3_string *out;
       
	out = s3_string_init();
	date = s3_make_date();

	asprintf(&sign_data, "%s\n\n\n%s\n/%s/%s", method, date, bucket, key);
	asprintf(&url, "http://%s.%s/%s", bucket, s3->base_url, key);
	
	s3_perform_op(s3, method, url, sign_data, date, out, NULL, NULL, NULL);

	s3_string_free(out);
	free(date);
	free(url);
	free(sign_data);
	
}

void
s3_put(struct S3 *s3, const char *bucket, const char *key, const char *content_type, const char *data, size_t len) {
	const char *method = "PUT";
	char *sign_data;
	char *date;
	char *url;
	char *md5;
	struct s3_string *in, *out;

	in = s3_string_init();
	out = s3_string_init();

	in->ptr = realloc(in->ptr, len + 1);
	memcpy(in->ptr, data, len);
	in->len = len;

	md5 = s3_md5_sum(data, len);

	date = s3_make_date();
	asprintf(&sign_data, "%s\n%s\n%s\n%s\n/%s/%s", method, md5, content_type ? content_type : "", date, bucket, key);  


	asprintf(&url, "http://%s.%s/%s", bucket, s3->base_url, key);

	s3_perform_op(s3, method, url, sign_data, date, out, in, md5, content_type);
	s3_string_free(in);
	s3_string_free(out);

	free(url);
	free(md5);
	free(date);
	free(sign_data);
}
