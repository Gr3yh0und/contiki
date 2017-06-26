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

#include "contiki.h"

// General configuration
#define APP_NAME "Secure CoAP server process"
#define SEND_INTERVAL (1 * CLOCK_SECOND)

PROCESS(coaps_process, APP_NAME);
AUTOSTART_PROCESSES(&coaps_process);

#include "dtls-base.h"

#ifdef WITH_TINYDTLS
extern dtls_context_t *dtls_context;
#endif

#if defined(WITH_SERVER) || defined(WITH_CLIENT)
extern struct uip_udp_conn *udp_conn;
#endif

#ifdef WITH_CLIENT
static struct etimer periodic;
static session_t session;
#endif

/* The main thread */
PROCESS_THREAD(coaps_process, ev, data)
{
	PROCESS_BEGIN();

#ifdef DEBUG_LOG
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
#endif

#ifdef GPIO_OUTPUT_ENABLE
	measurement_init_gpio();
#endif

#ifdef WITH_CLIENT
	// Setup client UDP
	// Define destination session
	session.size = sizeof(session.addr);
	session.port = UIP_HTONS(UDP_REMOTE_PORT);
	uip_ip6addr(&session.addr,0xfd00,0,0,0,0,0,0,0x0001);

	// Start client UDP connection
	udp_conn = udp_new(&session.addr, UIP_HTONS(UDP_REMOTE_PORT), NULL);
	udp_bind(udp_conn, UIP_HTONS(UDP_LOCAL_PORT));
	PRINTF("Created a connection with the server ");
	PRINT6ADDR(&udp_conn->ripaddr);
	PRINTF(" local/remote port %u/%u\n", UIP_HTONS(udp_conn->lport), UIP_HTONS(udp_conn->rport));

	// Start timer
	etimer_set(&periodic, SEND_INTERVAL);

	static uint8 buffer[32];
	static size_t bufferLength = sizeof(buffer);

#ifdef WITH_YACOAP
	static coap_packet_t requestPacket;
	static uint8 messageId = 0;

#ifdef WITH_CLIENT_PUT
	// PUT light
	static coap_resource_path_t resourcePath = {1, {"status"}};
	static coap_resource_t request = {COAP_RDY, COAP_METHOD_PUT, COAP_TYPE_CON, NULL, &resourcePath, COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_TXT_PLAIN)};
	coap_make_request(messageId, NULL, &request, &messageId, sizeof(messageId), &requestPacket);
#else
	// GET time
	static coap_resource_path_t resourcePath = {1, {"status"}};
	static coap_resource_t request = {COAP_RDY, COAP_METHOD_GET, COAP_TYPE_CON, NULL, &resourcePath, COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_TXT_PLAIN)};
	coap_make_request(messageId, NULL, &request, NULL, 0, &requestPacket);
#endif

	coap_build(&requestPacket, buffer, &bufferLength);
#endif
#endif

#ifdef WITH_SERVER
	// Setup server UDP
	PRINTF("Starting UDP server...\n");
	udp_conn = udp_new(NULL, 0, NULL);
	udp_bind(udp_conn, UIP_HTONS(UDP_LOCAL_PORT));
#ifdef WITH_YACOAP
	resource_setup(resources);
#endif
#endif

#ifdef WITH_TINYDTLS
	// Setup DTLS
	dtls_set_log_level(DTLS_DEBUG_LEVEL);
	dtls_context = dtls_new_context(udp_conn);

	if (dtls_context)
		dtls_set_handler(dtls_context, &dtls_callback);

	dtls_init();
	if (!dtls_context) {
		PRINTF("cannot create context\n");
	    PROCESS_EXIT();
	}
#endif

	while(1) {
		PROCESS_WAIT_EVENT();

		if(ev == tcpip_event) {
			read_packet();
		}

#ifdef WITH_CLIENT
	    if(etimer_expired(&periodic))
	    {
	    	PRINTF(".");
	    	MEASUREMENT_DTLS_TOTAL_ON;
	    	MEASUREMENT_DTLS_WRITE_ON;
	    	dtls_write(dtls_context, &session, buffer, bufferLength);
	    	MEASUREMENT_DTLS_WRITE_OFF;
	    	etimer_reset(&periodic);
	    }
#endif
	}

	PROCESS_END();
}
