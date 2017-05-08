/*
 * Copyright (c) 2015, Swedish Institute of Computer Science.
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
 */

#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

#ifndef WITH_NON_STORING
#define WITH_NON_STORING 0 /* Set this to run with non-storing mode */
#endif /* WITH_NON_STORING */

#undef NBR_TABLE_CONF_MAX_NEIGHBORS
#undef UIP_CONF_MAX_ROUTES

#ifdef TEST_MORE_ROUTES
/* configure number of neighbors and routes */
#define NBR_TABLE_CONF_MAX_NEIGHBORS     10
#define UIP_CONF_MAX_ROUTES   30
#else
/* configure number of neighbors and routes */
#define NBR_TABLE_CONF_MAX_NEIGHBORS     10
#define UIP_CONF_MAX_ROUTES   10
#endif /* TEST_MORE_ROUTES */

#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     nullrdc_driver
#undef NULLRDC_CONF_802154_AUTOACK
#define NULLRDC_CONF_802154_AUTOACK       1

/* Define as minutes */
#define RPL_CONF_DEFAULT_LIFETIME_UNIT   60

/* 10 minutes lifetime of routes */
#define RPL_CONF_DEFAULT_LIFETIME        10

#define RPL_CONF_DEFAULT_ROUTE_INFINITE_LIFETIME 1

#if WITH_NON_STORING
#undef RPL_NS_CONF_LINK_NUM
#define RPL_NS_CONF_LINK_NUM 40 /* Number of links maintained at the root. Can be set to 0 at non-root nodes. */
#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES 0 /* No need for routes */
#undef RPL_CONF_MOP
#define RPL_CONF_MOP RPL_MOP_NON_STORING /* Mode of operation*/
#endif /* WITH_NON_STORING */


#define IEEE802154_CONF_PANID 0xABCD
#define RF_CHANNEL 26
#define CC2538_RF_CONF_CHANNEL RF_CHANNEL
#define WITH_ADAPTIVESEC 1
#define NETSTACK_CONF_LLSEC nullsec_driver
#endif
#undef AES_128_CONF
#define AES_128_CONF aes_128_driver
#define CETIC_6LBR_6LOWPAN_CONTEXT_0 { 0xFD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define DTLS_PEER_MAX 3

#define DTLS_CONF_CONN_TIMEOUT 10


#if !UIP_CONF_IPV6_RPL
#undef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER            0
#endif

#ifndef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM          5
#endif

#ifndef UIP_CONF_BUFFER_SIZE
#if CONTIKI_TARGET_SKY || CONTIKI_TARGET_Z1
#define UIP_CONF_BUFFER_SIZE    260
#else
#define UIP_CONF_BUFFER_SIZE    600
#endif
#endif

#ifndef UIP_CONF_RECEIVE_WINDOW
#define UIP_CONF_RECEIVE_WINDOW  60
#endif

#ifndef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC     		csma_driver
#endif

#ifndef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     		nullrdc_driver
#endif

#ifndef SKY_CONF_MAX_TX_POWER
#define SKY_CONF_MAX_TX_POWER 	31
#endif

#ifndef RPL_CONF_INIT_LINK_METRIC
#define RPL_CONF_INIT_LINK_METRIC			2
#endif

#define RPL_MAX_DAG_PER_INSTANCE	2
#define RPL_MAX_INSTANCES		1

// Always use infinite upward route
#define RPL_CONF_DEFAULT_ROUTE_INFINITE_LIFETIME    1

//Select RPL MOP
#undef RPL_CONF_MOP
#if RPL_NON_STORING
#define RPL_CONF_MOP RPL_MOP_STORING_NO_MULTICAST
#else
#define RPL_CONF_MOP RPL_MOP_STORING_NO_MULTICAST
#endif

// Enable DAO-Ack
#define RPL_CONF_WITH_DAO_ACK       1

#define RPL_CONF_RPL_REPAIR_ON_DAO_NACK    0

#define RPL_CONF_DIO_REFRESH_DAO_ROUTES     0

/* Z1 platform has limited RAM */

#if defined CONTIKI_TARGET_Z1

#define RPL_CONF_MAX_PARENTS_PER_DAG    12
#define NEIGHBOR_CONF_MAX_NEIGHBORS     12

#ifndef UIP_CONF_DS6_NBR_NBU
#define UIP_CONF_DS6_NBR_NBU     12
#endif

#ifndef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES   12
#endif

#else

#define RPL_CONF_MAX_PARENTS_PER_DAG    24
#define NEIGHBOR_CONF_MAX_NEIGHBORS     24

#ifndef UIP_CONF_DS6_NBR_NBU
#define UIP_CONF_DS6_NBR_NBU     24
#endif

#if RPL_NON_STORING
#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES 0
#ifndef RPL_NS_CONF_LINK_NUM
#define RPL_NS_CONF_LINK_NUM   24
#endif
#else
#undef RPL_NS_CONF_LINK_NUM
#define RPL_NS_CONF_LINK_NUM   0
#ifndef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES   24
#endif
#endif

#endif

#define UIP_DS6_CONF_NO_STATIC_ADDRESS 1

