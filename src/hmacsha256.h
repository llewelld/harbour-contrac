#ifndef HMACSHA256_H
#define HMACSHA256_H

// HMAC-SHA156 is only available from OpenSSL 1.1.1 onwards
// Code from https://github.com/aperezdc/hmac-sha256

/*
 * hmac-sha256.h
 * Copyright (C) 2017 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <stddef.h>
#include <stdint.h>

#define HMAC_SHA256_DIGEST_SIZE 32  /* Same as SHA-256's output size. */

void
hmac_sha256 (uint8_t out[HMAC_SHA256_DIGEST_SIZE],
             const uint8_t *data, size_t data_len,
             const uint8_t *key, size_t key_len);



#endif // HMACSHA256_H
