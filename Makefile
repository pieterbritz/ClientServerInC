# Makefile

IDIR =../include
CC=gcc
CFLAGS= -g -O0 -I$(IDIR) -std=c99
LDFLAGS=
ODIR=obj
LDIR =../lib
LIBS=-lm

all: big_build

_DEPS = common.h logger.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# Client Makefile config
BUILD_CLIENT_DIR=build_client
CLIENT_SOURCE_PATH=src_client
CLIENT_DEPS = 	$(shell find $(CLIENT_SOURCE_PATH) -name '*.h')
CLIENT_SRCS	=	$(shell find $(CLIENT_SOURCE_PATH) -name '*.c')       # src_client/v_client.c src_client/menu.c
CLIENT_OBJS	=	$(addprefix $(BUILD_CLIENT_DIR)/,$(notdir $(CLIENT_SRCS:.c=.o)))

# Server Makefile config
BUILD_SERVER_DIR=build_server
SERVER_SOURCE_PATH=src_server
SERVER_DEPS = 	$(shell find $(SERVER_SOURCE_PATH) -name '*.h')
SERVER_SRCS =	$(shell find $(SERVER_SOURCE_PATH) -name '*.c')       # src_server/v_server.c src_server/menu.c
SERVER_OBJS	=	$(addprefix $(BUILD_SERVER_DIR)/,$(notdir $(SERVER_SRCS:.c=.o)))

$(BUILD_CLIENT_DIR)/v_client.o: $(CLIENT_SOURCE_PATH)/v_client.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_CLIENT_DIR)/menu.o: $(CLIENT_SOURCE_PATH)/menu.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_CLIENT_DIR)/logger.o: $(CLIENT_SOURCE_PATH)/logger.c
	$(CC) -c -o $@ $< $(CFLAGS) -I../include/logger.h

$(BUILD_SERVER_DIR)/v_server.o:	$(SERVER_SOURCE_PATH)/v_server.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_SERVER_DIR)/menu.o:	$(SERVER_SOURCE_PATH)/menu.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_SERVER_DIR)/logger.o:	$(SERVER_SOURCE_PATH)/logger.c
	$(CC) -c -o $@ $< $(CFLAGS) -I../include/logger.h

$(BUILD_SERVER_DIR):
	mkdir $@

$(BUILD_CLIENT_DIR):
	mkdir $@

%.o: %c $(CLIENT_DEPS)
	$(CC) -c -o $(BUILD_CLIENT_DIR)/$@ $< $(CFLAGS)

client: $(CLIENT_OBJS)
	@echo "[LINK] $^"
	$(CC) $^ $(CFLAGS) $(LIBS) -o $(BUILD_CLIENT_DIR)/$@

server:	$(SERVER_OBJS)
	@echo "[LINK] $^"
	$(CC) $^ $(CFLAGS) $(LIBS) -o $(BUILD_SERVER_DIR)/$@

big_build: $(server) $(client) 
	@echo "[CC] $<"
	@echo  .....Server and Client compiled.

.PHONY: clean

cleanserver:
	$(RM) $(BUILD_SERVER_DIR)/server 
	$(RM) $(BUILD_SERVER_DIR)/*.o

cleanclient:
	$(RM) $(BUILD_CLIENT_DIR)/client 
	$(RM) $(BUILD_CLIENT_DIR)/*.o 

list_src:
	@echo $(shell find src -name '*.c')

#  macros $@ and $^, which are the left and right sides of the :
#  the $< is the first item in the dependencies list