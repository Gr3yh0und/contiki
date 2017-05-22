CONTIKI_PROJECT = coaps 
CONTIKI_SOURCEFILES = dtls-base.c dtls-server.c dtls-client.c
all: $(CONTIKI_PROJECT) 

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

# MODULES to integrate
CONTIKI_WITH_IPV6 = 1
#CFLAGS += -DCONTIKI_LLSEC_ACTIVATED
#CFLAGS += -DCONTIKI_CONTIKIMAC
#CFLAGS += -DCONTIKI_LOWPOWER

# APPS to integrate
# TinyDTLS
APPS += tinydtls/aes tinydtls/sha2 tinydtls
CFLAGS += -DWITH_TINYDTLS
# YaCoAP
APPS += yacoap
CFLAGS += -DWITH_YACOAP
CFLAGS += -DYACOAP_LEDS_ENABLED=1
# UDP Server/Client
CFLAGS += -DWITH_CLIENT
#CFLAGS += -DWITH_SERVER

# Debugging options
# Contiki
#CFLAGS += -DCC2538_CONF_QUIET=1 
CFLAGS += -g

# YaCoAP
#CFLAGS += -DYACOAP_DEBUG

# TinyDTLS
# ENABLED = NO DEBUG
# DISABLED = DEBUG
#CFLAGS += -DNDEBUG

# Optimizations
SMALL=0

CONTIKI = ../..
include $(CONTIKI)/Makefile.include