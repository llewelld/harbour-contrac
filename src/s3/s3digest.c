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

#include "s3.h"

#include <openssl/engine.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>

char *
s3_hmac_sign(const char *key, const char *str, size_t len) {
	unsigned char *digest; 
	char *buf;
	unsigned int digest_len = EVP_MAX_MD_SIZE; /* HMAC_Final needs at most EVP_MAX_MD_SIZE bytes */
    HMAC_CTX ctx;
	BIO *bmem, *b64;
	BUF_MEM *bufptr;
	
	ENGINE_load_builtin_engines();
	ENGINE_register_all_complete();

	/* Setup HMAC context, init with sha1 and our key*/ 
    HMAC_CTX_init(&ctx);
    HMAC_Init_ex(&ctx, key, strlen((char *)key), EVP_sha1(), NULL);

	/* Create Base64 BIO filter that outputs to memory */
	b64  = BIO_new(BIO_f_base64());
	bmem = BIO_new(BIO_s_mem());
	b64  = BIO_push(b64, bmem);

	/* Give us a buffer to write result into */
	digest = malloc(digest_len);

	/* Push data into HMAC */
    HMAC_Update(&ctx, (unsigned char *)str, (unsigned int)len);

	/* Flush HMAC data into buffer */
    HMAC_Final(&ctx, digest, &digest_len);

	/* Write data into BIO, flush and fetch the data */
	BIO_write(b64, digest, digest_len);
	(void) BIO_flush(b64);
	BIO_get_mem_ptr(b64, &bufptr);
	
	buf = malloc(bufptr->length);
	memcpy(buf, bufptr->data, bufptr->length - 1);
	buf[bufptr->length - 1] = '\0';

	BIO_free_all(b64);
    HMAC_CTX_cleanup(&ctx);

	ENGINE_cleanup();
	free(digest);
	return buf;
}

char *
s3_md5_sum(const char *content, size_t len) {

	const EVP_MD *md = EVP_md5();
	unsigned char *digest;
	EVP_MD_CTX *ctx;
	unsigned int digest_len;
	BIO *bmem, *b64;
	BUF_MEM *bufptr;
	char *buf;

	digest = malloc(EVP_MAX_MD_SIZE);
	
	ctx = EVP_MD_CTX_create();
	EVP_DigestInit_ex(ctx, md, NULL);
	EVP_DigestUpdate(ctx, content, len);
	EVP_DigestFinal_ex(ctx, digest, &digest_len);

	b64  = BIO_new(BIO_f_base64());
	bmem = BIO_new(BIO_s_mem());
	b64  = BIO_push(b64, bmem);

	BIO_write(b64, digest, digest_len);
	(void) BIO_flush(b64);
	BIO_get_mem_ptr(b64, &bufptr);
	
	buf = malloc(bufptr->length);
	memcpy(buf, bufptr->data, bufptr->length - 1);
	buf[bufptr->length - 1] = '\0';

	BIO_free_all(b64);
	free(digest);
	EVP_MD_CTX_destroy(ctx);

	return buf;
}
