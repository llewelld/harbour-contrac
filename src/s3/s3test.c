/*
 * Copyright (c) 2013,2014 Ian Delahorne <ian.delahorne@gmail.com>
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

#include <stdio.h>
#include <string.h>
#include <sys/queue.h>

#include <curl/curl.h>



int main (int argc, char **argv) {
	struct S3 *s3; 
	struct s3_string *out;
	struct s3_bucket_entry_head *bkt_entries;
	struct s3_bucket_entry *e;
	char *file_contents = "foo bar gazonk";


	char *s3_key_id = "accessKey1";
	char *s3_secret = "verySecretKey1";

	if (argc < 2) {
		fprintf(stderr, "Usage: s3test <bucket>\n");
		return 1;
	}	

	if (s3_key_id == NULL || s3_secret == NULL) {
		fprintf(stderr, "Error: Environment variable AWS_ACCESS_KEY_ID or AWS_SECRET_KEY not set\n");
		return 1;
	}

	char *bucket = argv[1];

	s3 = s3_init(s3_key_id, s3_secret, "localhost:8003");

	// aws s3 --profile cwa --endpoint-url=http://localhost:8003 sync s3://cwa .

	printf("Listing bucket root\n");
	bkt_entries = s3_list_bucket(s3, bucket, NULL);
	if (bkt_entries != NULL) {
		TAILQ_FOREACH(e, bkt_entries, list) {
			printf("\tKey %s\n", e->key);
		}
		s3_bucket_entries_free(bkt_entries);
	}


	printf("Listing /version/\n");
	bkt_entries = s3_list_bucket(s3, bucket, "version/");
	if (bkt_entries != NULL) {
		TAILQ_FOREACH(e, bkt_entries, list) {
			printf("\tKey %s\n", e->key);
		}
		s3_bucket_entries_free(bkt_entries);
	}

	printf("Uploading foo.txt\n");
	s3_put(s3, bucket, "foo.txt", "text/plain", file_contents, strlen(file_contents));

	printf("Downloading foo.txt\n");
	out = s3_string_init();
	s3_get(s3, bucket, "foo.txt", out);

	printf("Contents:\n%.*s\n", (int)out->len, out->ptr);

	s3_string_free(out);

	printf("Deleting foo.txt\n");
	s3_delete(s3, bucket, "foo.txt");
	s3_free(s3);

	return 0;
}
