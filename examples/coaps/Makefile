CONTIKI_PROJECT = coaps
all: $(CONTIKI_PROJECT)

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

APPS += tinydtls/aes tinydtls/sha2 tinydtls/ecc tinydtls
APPS += microcoap
#APPS += serial-shell

CONTIKI_WITH_IPV6 = 1

CONTIKI = ../..
include $(CONTIKI)/Makefile.include
