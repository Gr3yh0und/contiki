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
// Set this to run with non-storing mode
#define WITH_NON_STORING 0
#endif /* WITH_NON_STORING */

// Define default 802.15.4 Settings
#ifndef CONTIKI_RADIODEFAULTS
#define NETSTACK_CONF_NETWORK sicslowpan_driver
#define IEEE802154_CONF_PANID 0xABCD
#define RF_CHANNEL 26
#define CC2538_RF_CONF_CHANNEL RF_CHANNEL
#define CC2538_RF_CONF_TX_POWER 0xFF
#endif

// MAC driver
// CSMA
#ifdef CONTIKI_MAC_CSMA
#define NETSTACK_CONF_MAC     					csma_driver
#endif

// NullMAC
#ifdef CONTIKI_MAC_NULL
#define NETSTACK_CONF_MAC     					nullmac_driver
#endif

// RDC driver
// ContikiMAC
#ifdef CONTIKI_RDC_CONTIKI
#define NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE 	8
#define NETSTACK_CONF_RDC     					contikimac_driver
#endif

// NullRDC
#ifdef CONTIKI_RDC_NULL
#define NETSTACK_CONF_RDC     					nullrdc_driver
#undef NULLRDC_CONF_802154_AUTOACK
#define NULLRDC_CONF_802154_AUTOACK     		1
#endif


// 802.15.4 Link Layer Security (LLSEC)
#ifdef CONTIKI_LLSEC_ACTIVATED
#undef LLSEC802154_CONF_ENABLED
#define LLSEC802154_CONF_ENABLED          		1
#undef NETSTACK_CONF_FRAMER
#define NETSTACK_CONF_FRAMER              		noncoresec_framer
#undef NETSTACK_CONF_LLSEC
#define NETSTACK_CONF_LLSEC               		noncoresec_driver
#undef NONCORESEC_CONF_SEC_LVL
#define NONCORESEC_CONF_SEC_LVL           		7

// Define network wide key
#define NONCORESEC_CONF_KEY { 0x00 , 0x01 , 0x02 , 0x03 , \
                              0x04 , 0x05 , 0x06 , 0x07 , \
                              0x08 , 0x09 , 0x0A , 0x0B , \
                              0x0C , 0x0D , 0x0E , 0x0F }
#else
#define NETSTACK_CONF_LLSEC nullsec_driver
#endif


// Energy Modes
#ifdef CONTIKI_LOWPOWER
#define ENERGEST_CONF_ON	1
#define LPM_CONF_ENABLE     1
#define LPM_CONF_MAX_PM     1
#define LPM_CONF_STATS      0
#endif


// UIP network stack
#define UIP_DS6_CONF_NO_STATIC_ADDRESS 		1

#undef NBR_TABLE_CONF_MAX_NEIGHBORS
#define NBR_TABLE_CONF_MAX_NEIGHBORS     	5

#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES   				5

#define NEIGHBOR_CONF_MAX_NEIGHBORS     	5

#undef UIP_CONF_DS6_NBR_NBU
#define UIP_CONF_DS6_NBR_NBU     			5

#undef UIP_CONF_BUFFER_SIZE
#define UIP_CONF_BUFFER_SIZE    			512

#undef UIP_CONF_RECEIVE_WINDOW
#define UIP_CONF_RECEIVE_WINDOW  			60

#if !UIP_CONF_IPV6_RPL
#undef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER            			0
#endif

#ifndef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM          			5
#endif


// Define RPL Routing options
/* Define as minutes */
#define RPL_CONF_DEFAULT_LIFETIME_UNIT   	60

/* 10 minutes lifetime of routes */
#define RPL_CONF_DEFAULT_LIFETIME        	10

#define RPL_CONF_DEFAULT_ROUTE_INFINITE_LIFETIME 1

#if WITH_NON_STORING
#undef RPL_NS_CONF_LINK_NUM
#define RPL_NS_CONF_LINK_NUM 	40
#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES 	0
#undef RPL_CONF_MOP
#define RPL_CONF_MOP RPL_MOP_NON_STORING
#endif /* WITH_NON_STORING */

#if RPL_NON_STORING
#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES 	0
#ifndef RPL_NS_CONF_LINK_NUM
#define RPL_NS_CONF_LINK_NUM    24
#endif
#else
#undef RPL_NS_CONF_LINK_NUM
#define RPL_NS_CONF_LINK_NUM   0
#ifndef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES    24
#endif
#endif

#ifndef RPL_CONF_INIT_LINK_METRIC
#define RPL_CONF_INIT_LINK_METRIC			2
#endif

#define RPL_MAX_DAG_PER_INSTANCE	2
#define RPL_MAX_INSTANCES			1

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
#define RPL_CONF_WITH_DAO_ACK       		1
#define RPL_CONF_RPL_REPAIR_ON_DAO_NACK    	0
#define RPL_CONF_DIO_REFRESH_DAO_ROUTES     0
#define RPL_CONF_MAX_PARENTS_PER_DAG    	12

#endif
