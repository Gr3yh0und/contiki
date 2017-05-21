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
#include "dtls-base.h"
#endif

PROCESS(coaps_process, APP_NAME);
AUTOSTART_PROCESSES(&coaps_process);

// UDP handling
#if defined(WITH_CLIENT) || defined(WITH_SERVER)
static struct uip_udp_conn *server_conn;
#endif

#ifdef WITH_TINYDTLS
static dtls_context_t *dtls_context;
#endif

/* The main thread */
PROCESS_THREAD(coaps_process, ev, data)
{
	PROCESS_BEGIN();

#if defined(WITH_CLIENT) || defined(WITH_SERVER)
	server_conn = udp_new(NULL, 0, NULL);
	udp_bind(server_conn, UIP_HTONS(COAP_SERVICE_PORT));
#endif

#ifdef WITH_YACOAP
	resource_setup(resources);
#endif

#ifdef WITH_TINYDTLS
	dtls_set_log_level(DTLS_DEBUG_LEVEL);
	dtls_context = dtls_new_context(server_conn);

	if (dtls_context)
		dtls_set_handler(dtls_context, &dtls_callback);

	dtls_init();
	if (!dtls_context) {
		dsrv_log(DTLS_LOG_EMERG, "cannot create context\n");
	    PROCESS_EXIT();
	}
#endif

	while(1) {
		PROCESS_WAIT_EVENT();
	    if(ev == tcpip_event) {
#ifdef WITH_TINYDTLS
	    	onUdpPacket(dtls_context);
#endif
	    }
	}

	PROCESS_END();
}
