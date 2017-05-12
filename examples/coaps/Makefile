CONTIKI_PROJECT = coaps
all: $(CONTIKI_PROJECT)

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

# Add TinyDTLS
APPS += tinydtls/aes tinydtls/sha2 tinydtls
CFLAGS += -DWITH_TINYDTLS

# Add YaCoAP
APPS += yacoap
CFLAGS += -DWITH_YACOAP

# Debugging options
#CFLAGS += -DYACOAP_DEBUG
CFLAGS += -DNDEBUG

# Add IPv6 networking
CONTIKI_WITH_IPV6 = 1

CONTIKI = ../..
include $(CONTIKI)/Makefile.include