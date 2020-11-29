// HKDF-SHA256 is only available from OpenSSL 1.1.1 onwards
// This is a hack until the default Sailfish OS install is upgraded
// Code from https://boringssl.googlesource.com/boringssl/+/HEAD/crypto/hkdf/hkdf.c

/* Copyright (c) 2014, Google Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. */

#ifndef HKDFSHA256_H
#define HKDFSHA256_H

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#include <openssl/hmac.h>

int HKDF(uint8_t *out_key, size_t out_len, const EVP_MD *digest,
         const uint8_t *secret, size_t secret_len, const uint8_t *salt,
         size_t salt_len, const uint8_t *info, size_t info_len);

#endif // HKDFSHA256_H
