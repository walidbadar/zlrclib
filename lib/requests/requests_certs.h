/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __REQUESTS_CERTS_INTERNAL_H_
#define __REQUESTS_CERTS_INTERNAL_H_

#include <stdint.h>
#include <zephyr/net/tls_credentials.h>

#if defined(CONFIG_NET_SOCKETS_SOCKOPT_TLS)
static const unsigned char ca_certificate[] = {
#include "lrclib.der.inc"
};
#else
#define TLS_CREDENTIAL_CA_CERTIFICATE 1
static const unsigned char ca_certificate[] = {};
#endif

#define CA_CERTIFICATE_TAG 1

#endif /* __REQUESTS_CERTS_INTERNAL_H_ */
