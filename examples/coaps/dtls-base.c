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

/**
 * @brief Called handler function when a new raw packet should be sent
 * @param dtls_context: Pointer to current DTLS context
 * @param session: Pointer to current session object
 * @param data: Pointer to data that should be transfered
 * @param length: Length of data that should be transfered
 * @return: Amount of sent bytes
 */
int handle_write(struct dtls_context_t *dtls_context, session_t *session, uint8 *data, size_t length)
{
#ifdef WITH_SERVER
	MEASUREMENT_DTLS_WRITE_ON;
#endif

	// Get UDP connection, extract IP and port
	struct uip_udp_conn *connection = (struct uip_udp_conn *) dtls_get_app_data(dtls_context);
	uip_ipaddr_copy(&connection->ripaddr, &session->addr);
	connection->rport = session->port;

	// Send data
	uip_udp_packet_send(connection, data, length);

	// Restore server connection to allow data from any node
	memset(&connection->ripaddr, 0, sizeof(connection->ripaddr));
	memset(&connection->rport, 0, sizeof(connection->rport));
#ifdef WITH_SERVER
	MEASUREMENT_DTLS_WRITE_OFF;
#endif
	return length;
}

/**
 * @brief Called handler function when a new packet is received
 * @param dtls_context: Pointer to current DTLS context
 * @param session: Pointer to current session object
 * @param data: Pointer to data that should be read
 * @param length: Length of data
 * @return:
 */
int handle_read(struct dtls_context_t *dtls_context, session_t *session, uint8 *data, size_t length)
{
#ifdef WITH_YACOAP

	// Server
#ifdef WITH_SERVER
	coap_packet_t requestPacket, responsePacket;
	uint8_t responseBuffer[DTLS_MAX_BUF];
	size_t responseBufferLength = sizeof(responseBuffer);

	// Parse data for CoAP content
	if ((coap_parse(data, length, &requestPacket)) < COAP_ERR)
	{
		// Get data from resources
		coap_handle_request(resources, &requestPacket, &responsePacket);

		// Build response packet
		if ((coap_build(&responsePacket, responseBuffer, &responseBufferLength)) < COAP_ERR)
		{
#ifdef WITH_TINYDTLS
			// Send response packet decrypted over DTLS
			MEASUREMENT_DTLS_WRITE_ON;
			dtls_write(dtls_context, session, responseBuffer, responseBufferLength);
			MEASUREMENT_DTLS_WRITE_OFF;
#endif
			return 0;
		}
	}
#endif

	// Client
#ifdef WITH_CLIENT
	MEASUREMENT_DTLS_TOTAL_OFF;
	coap_packet_t packet;
	coap_parse(data, length, &packet);
	PRINTF("(COAP) Answer was: %.*s", packet.payload.len, (char *)packet.payload.p);
	(void) dtls_context;
	(void) session;
#endif
#endif
	return -1;
}

/**
 * @brief Handler that is called when an event occurs
 * @param dtls_context: Pointer to current DTLS context
 * @param session: Pointer to current session object
 * @param level:
 * @param code:
 * @return:
 */
int handle_event(struct dtls_context_t *dtls_context, session_t *session, dtls_alert_level_t level, unsigned short code)
{
#ifndef NDEBUG
	if (code == DTLS_EVENT_CONNECTED) {
		dtls_debug("Event: Connected!\n");
	}else if (code == DTLS_EVENT_CONNECT){
		dtls_debug("Event: Connecting...\n");
	}else{
		dtls_debug("Event: Other event occurred!\n");
	}
#endif

	(void) dtls_context;
	(void) session;
	(void) level;
	(void) code;
	return 0;
}
#endif

#if defined(WITH_CLIENT) || defined(WITH_SERVER)
/**
 * @brief Handler called when a new raw UDP packet is received
 */
void read_packet()
{
	MEASUREMENT_DTLS_READ_ON;
#if WITH_SERVER
	MEASUREMENT_DTLS_TOTAL_ON;
#endif

#ifdef WITH_TINYDTLS
	// Secure Server / Client
	session_t session;

	// When new packet is received
	if(uip_newdata()) {
		// extract data into session
		dtls_session_init(&session);
		uip_ipaddr_copy(&session.addr, &UIP_IP_BUF->srcipaddr);
		session.port = UIP_UDP_BUF->srcport;

		// Hand over to TinyDTLS
		MEASUREMENT_DTLS_READ_OFF;
		dtls_handle_message(dtls_context, &session, uip_appdata, uip_datalen());
	}
#else
	// Insecure Server
	// When new packet is received
	if(uip_newdata()) {
		PRINTF("Received CoAP request...\n");
		MEASUREMENT_DTLS_READ_OFF;
		handle_message();
	}
#endif

#if WITH_SERVER
	MEASUREMENT_DTLS_TOTAL_OFF;
#endif
}

/**
 * @brief Handle incoming message for insecure CoAP Server
 */
void handle_message(){
	coap_packet_t requestPacket, responsePacket;
	uint8_t responseBuffer[DTLS_MAX_BUF];
	size_t responseBufferLength = sizeof(responseBuffer);

	// Parse received packet for CoAP request
	if ((coap_parse(uip_appdata, uip_datalen(), &requestPacket)) < COAP_ERR)
	{
		// Get data from resources
		coap_handle_request(resources, &requestPacket, &responsePacket);

		// Build response packet and send it back
		if ((coap_build(&responsePacket, responseBuffer, &responseBufferLength)) < COAP_ERR)
		{
			// Send response packet
			MEASUREMENT_DTLS_WRITE_ON;
			uip_ipaddr_copy(&udp_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
			udp_conn->rport = UIP_UDP_BUF->srcport;
			uip_udp_packet_send(udp_conn, responseBuffer, responseBufferLength);
			uip_create_unspecified(&udp_conn->ripaddr);
			MEASUREMENT_DTLS_WRITE_OFF;
		}
	}
}
#endif
