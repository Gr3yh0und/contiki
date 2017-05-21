/*
 * dtls-base.h
 *
 *  Created on: 21 May 2017
 *      Author: Michael Morscher, morscher@hm.edu
 */

#ifndef EXAMPLES_COAPS_DTLS_BASE_H_
#define EXAMPLES_COAPS_DTLS_BASE_H_

#ifdef WITH_TINYDTLS
#ifdef WITH_SERVER
#include "dtls-server.h"
#endif
#ifdef WITH_CLIENT
#include "dtls-client.h"
#endif
#include "dtls.h"
#include "dtls_debug.h"
#endif

#ifdef WITH_YACOAP
#include "coap.h"
#endif

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF  ((struct uip_udp_hdr *)&uip_buf[UIP_LLIPH_LEN])

// YaCoAP variables
#ifdef WITH_YACOAP
extern void resource_setup(const coap_resource_t *resources);
extern coap_resource_t resources[];
#endif

// DTLS defines
#if defined DTLS_CONF_IDENTITY_HINT && defined DTLS_CONF_IDENTITY_HINT_LENGTH
#define DTLS_IDENTITY_HINT DTLS_CONF_IDENTITY_HINT
#define DTLS_IDENTITY_HINT_LENGTH DTLS_CONF_IDENTITY_HINT_LENGTH
#else
#define DTLS_IDENTITY_HINT "Client_identity"
#define DTLS_IDENTITY_HINT_LENGTH 15
#endif

#if defined DTLS_CONF_PSK_KEY && defined DTLS_CONF_PSK_KEY_LENGTH
#define DTLS_PSK_KEY_VALUE DTLS_CONF_PSK_KEY
#define DTLS_PSK_KEY_VALUE_LENGTH DTLS_CONF_PSK_KEY_LENGTH
#else
#define DTLS_PSK_KEY_VALUE "secretPSK"
#define DTLS_PSK_KEY_VALUE_LENGTH 9
#endif

dtls_handler_t dtls_callback;

void onUdpPacket(dtls_context_t *ctx);
int handle_write(struct dtls_context_t *ctx, session_t *session, uint8 *data, size_t len);
int handle_read(struct dtls_context_t *context, session_t *session, uint8 *data, size_t length);
int handle_event(struct dtls_context_t *ctx, session_t *session, dtls_alert_level_t level, unsigned short code);

#endif /* EXAMPLES_COAPS_DTLS_BASE_H_ */
