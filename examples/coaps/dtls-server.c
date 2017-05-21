/*
 * dtls-server.c
 *
 *  Created on: 21 May 2017
 *      Author: Michael Morscher, morscher@hm.edu
 */

#include "dtls-server.h"

#if defined(DTLS_PSK) && defined(WITH_TINYDTLS) && defined(WITH_SERVER)
/* This function is the "key store" for tinyDTLS. It is called to
 * retrieve a key for the given identiy within this particular
 * session. */
int get_psk_info(struct dtls_context_t *ctx, const session_t *session,
         dtls_credentials_type_t type,
         const unsigned char *id, size_t id_len,
         unsigned char *result, size_t result_length)
{

	struct keymap_t {
		unsigned char *id;
		size_t id_length;
		unsigned char *key;
		size_t key_length;
	} psk[1] = {
			{ (unsigned char *)DTLS_IDENTITY_HINT, DTLS_IDENTITY_HINT_LENGTH,
					(unsigned char *)DTLS_PSK_KEY_VALUE, DTLS_PSK_KEY_VALUE_LENGTH },
	};

	if (type != DTLS_PSK_KEY) {
	  return 0;
	}

	if (id) {
		int i;
		for (i = 0; i < sizeof(psk)/sizeof(struct keymap_t); i++) {
			if (id_len == psk[i].id_length && memcmp(id, psk[i].id, id_len) == 0) {
				if (result_length < psk[i].key_length) {
					dtls_warn("buffer too small for PSK");
					return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
				}

				memcpy(result, psk[i].key, psk[i].key_length);
				return psk[i].key_length;
			}
		}
	}

  return dtls_alert_fatal_create(DTLS_ALERT_DECRYPT_ERROR);
}
#endif
