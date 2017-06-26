/*
 * dtls-base.c
 *
 *  Created on: 21 May 2017
 *      Author: Michael Morscher, morscher@hm.edu
 */

#include "dtls-base.h"

#if defined(WITH_SERVER) || defined(WITH_CLIENT)
struct uip_udp_conn *udp_conn;
#endif

#ifdef WITH_TINYDTLS
dtls_context_t *dtls_context;

/* Definition of executed handlers */
dtls_handler_t dtls_callback = {
  .write = handle_write,
  .read  = handle_read,
  .event = handle_event,
#ifdef DTLS_PSK
  .get_psk_info = get_psk_info,
#endif
};

/* Called handler function when a new raw packet should be sent */
int handle_write(struct dtls_context_t *ctx, session_t *session, uint8 *data, size_t len)
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

/* Called handler function when a new packet is received */
int handle_read(struct dtls_context_t *context, session_t *session, uint8 *data, size_t length)
{
#ifdef WITH_YACOAP
#ifdef WITH_SERVER
	coap_packet_t requestPacket, responsePacket;
	uint8_t responseBuffer[DTLS_MAX_BUF];
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
#endif
			return 0;
		}
	}
#endif

#ifdef WITH_CLIENT
	MEASUREMENT_DTLS_TOTAL_OFF;
	coap_packet_t packet;
	coap_parse(data, length, &packet);
	PRINTF("(COAP) Answer was: %.*s", packet.payload.len, (char *)packet.payload.p);
	(void) context;
	(void) session;
#endif
#endif
	return -1;
}

/* Handler that is called when an event occurs */
int handle_event(struct dtls_context_t *ctx, session_t *session, dtls_alert_level_t level, unsigned short code)
{

#ifndef NDEBUG
	if (code == DTLS_EVENT_CONNECTED) {
		dtls_debug("EVENT Connected!\n");
	}
	else if (code == DTLS_EVENT_CONNECT){
		dtls_debug("EVENT Connecting...\n");
	}else{
		dtls_debug("EVENT Other event occurred!\n");
	}
#endif

	(void) ctx;
	(void) session;
	(void) level;
	(void) code;
	return 0;
}
#endif

#if defined(WITH_CLIENT) || defined(WITH_SERVER)
/* Handler called when a new raw UDP packet is received */
void read_packet()
{
	MEASUREMENT_DTLS_READ_ON;

#ifdef WITH_TINYDTLS
	session_t session;
	if(uip_newdata()) {
		dtls_session_init(&session);
		uip_ipaddr_copy(&session.addr, &UIP_IP_BUF->srcipaddr);
		session.port = UIP_UDP_BUF->srcport;
		dtls_handle_message(dtls_context, &session, uip_appdata, uip_datalen());
	}
#else
	if(uip_newdata()) {
		PRINTF("Received CoAP request...\n");

		coap_packet_t requestPacket, responsePacket;
		uint8_t responseBuffer[DTLS_MAX_BUF];
		size_t responseBufferLength = sizeof(responseBuffer);

		if ((coap_parse(uip_appdata, uip_datalen(), &requestPacket)) < COAP_ERR)
		{
			// Get data from resources
			coap_handle_request(resources, &requestPacket, &responsePacket);

			// Build response packet
			if ((coap_build(&responsePacket, responseBuffer, &responseBufferLength)) < COAP_ERR)
			{
				// Send response packet
				uip_ipaddr_copy(&udp_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
				udp_conn->rport = UIP_UDP_BUF->srcport;
				uip_udp_packet_send(udp_conn, responseBuffer, responseBufferLength);
				uip_create_unspecified(&udp_conn->ripaddr);
			}
		}
	}
#endif

	MEASUREMENT_DTLS_READ_OFF;
}
#endif
