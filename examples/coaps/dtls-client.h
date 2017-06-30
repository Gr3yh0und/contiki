/*
 * dtls-client.h
 *
 *  Created on: 21 May 2017
 *      Author: Michael Morscher, morscher@hm.edu
 */

#ifndef EXAMPLES_COAPS_DTLS_CLIENT_H_
#define EXAMPLES_COAPS_DTLS_CLIENT_H_

#ifdef WITH_TINYDTLS
#include "dtls-base.h"
int get_psk_info(struct dtls_context_t *ctx,
                        const session_t *session,
                        dtls_credentials_type_t type,
                        const unsigned char *id, size_t id_len,
                        unsigned char *result, size_t result_length);
#endif

#endif /* EXAMPLES_COAPS_DTLS_CLIENT_H_ */
