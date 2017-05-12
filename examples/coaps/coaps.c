/*
 * Copyright (c) 2017, University of Applied Sciences Munich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Implementation of a simple secure CoAP server based on TinyDTLS and YaCoAP
 *         Based on the DTLS echo server from the cetic/6lbr project
 * \author
 *         Michael Morscher <morscher@hm.edu>
 */

// Configuration
#define APP_NAME "Secure CoAP server process"
#define COAP_SERVICE_PORT 6666
#define DTLS_DEBUG_LEVEL DTLS_LOG_DEBUG

// Includes
#include "contiki.h"
#include <stdio.h>

#ifdef WITH_TINYDTLS
#include "dtls.h"
#include "dtls_debug.h"
#endif

#ifdef WITH_YACOAP
#include "coap.h"
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

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF  ((struct uip_udp_hdr *)&uip_buf[UIP_LLIPH_LEN])

PROCESS(coaps_process, APP_NAME);
AUTOSTART_PROCESSES(&coaps_process);

// tinyDTLS variables
#ifdef WITH_YACOAP
static struct uip_udp_conn *server_conn;
static dtls_context_t *dtls_context;
#endif

// YaCoAP variables
#ifdef WITH_TINYDTLS
extern void resource_setup(const coap_resource_t *resources);
extern coap_resource_t resources[];
#endif

/* Prints out all locally available addresses */
static void print_local_addresses(void)
{
	int i;
	uint8_t state;

	PRINTF("Server IPv6 addresses: ");
	for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
		state = uip_ds6_if.addr_list[i].state;
		if(state == ADDR_TENTATIVE || state == ADDR_PREFERRED) {
			PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
			PRINTF("\n");
			/* hack to make address "final" */
			if (state == ADDR_TENTATIVE) {
				uip_ds6_if.addr_list[i].state = ADDR_PREFERRED;
			}
		}
	}
}

/* Called handler function when a new packet is received */
int read_from_peer(struct dtls_context_t *context, session_t *session, uint8 *data, size_t length)
{

#ifdef WITH_YACOAP
	coap_packet_t requestPacket, responsePacket;
	uint8 responseBuffer[DTLS_MAX_BUF];
	size_t responseBufferLength = sizeof(responseBuffer);

	if ((coap_parse(data, length, &requestPacket)) < COAP_ERR)
	{
		// Get data from resources
		coap_handle_request(resources, &requestPacket, &responsePacket);

		// Build response packet
		if ((coap_build(&responsePacket, responseBuffer, &responseBufferLength)) < COAP_ERR)
		{
#ifdef WITH_TINYDTLS
			// Send response packet decrypted over DTLS
			dtls_write(context, session, responseBuffer, responseBufferLength);
			return 0;
#endif
		}
	}
#endif

	return -1;
}

/* Called handler function when a new raw packet should be sent */
int send_to_peer(struct dtls_context_t *ctx, session_t *session, uint8 *data, size_t len)
{
	struct uip_udp_conn *conn = (struct uip_udp_conn *)dtls_get_app_data(ctx);

	uip_ipaddr_copy(&conn->ripaddr, &session->addr);
	conn->rport = session->port;
	uip_udp_packet_send(conn, data, len);

	/* Restore server connection to allow data from any node */
	memset(&conn->ripaddr, 0, sizeof(conn->ripaddr));
	memset(&conn->rport, 0, sizeof(conn->rport));
	return len;
}

#if defined(DTLS_PSK) && defined(WITH_TINYDTLS)
/* This function is the "key store" for tinyDTLS. It is called to
 * retrieve a key for the given identiy within this particular
 * session. */
static int get_psk_info(struct dtls_context_t *ctx, const session_t *session,
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

/* Handler called when a new raw UDP packet is received */
static void dtls_handle_read(dtls_context_t *ctx)
{
	session_t session;

	if(uip_newdata()) {
		dtls_session_init(&session);
		uip_ipaddr_copy(&session.addr, &UIP_IP_BUF->srcipaddr);
		session.port = UIP_UDP_BUF->srcport;
		dtls_handle_message(ctx, &session, uip_appdata, uip_datalen());
	}
}

/* Initialise DTLS handling  */
void init_dtls()
{
	static dtls_handler_t cb = {
			.write = send_to_peer,
			.read  = read_from_peer,
			.event = NULL,
			.get_psk_info = get_psk_info,
	};

	server_conn = udp_new(NULL, 0, NULL);
	udp_bind(server_conn, UIP_HTONS(COAP_SERVICE_PORT));

	dtls_set_log_level(DTLS_DEBUG_LEVEL);

	dtls_context = dtls_new_context(server_conn);
	if (dtls_context)
		dtls_set_handler(dtls_context, &cb);
}

/* The main thread */
PROCESS_THREAD(coaps_process, ev, data)
{
	PROCESS_BEGIN();
	print_local_addresses();

#ifdef WITH_TINYDTLS
	dtls_init();
	init_dtls();
#endif

#ifdef WITH_YACOAP
	resource_setup(resources);
#endif

	if (!dtls_context) {
		dsrv_log(DTLS_LOG_EMERG, "cannot create context\n");
	    PROCESS_EXIT();
	}

	while(1) {
		PROCESS_WAIT_EVENT();
	    if(ev == tcpip_event) {
	    	dtls_handle_read(dtls_context);
	    }
	}

	PROCESS_END();
}
