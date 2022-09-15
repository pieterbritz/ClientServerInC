# Makefile

IDIR =../include
CC=gcc
CFLAGS= -g -O0 -I$(IDIR) -std=c99
LDFLAGS=
ODIR=obj
LDIR =../lib
LIBS=-lm

all: big_build

_DEPS = common.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# Client Makefile config
BUILD_CLIENT_DIR=build_client
CLIENT_SOURCE_PATH=src_client
CLIENT_SRCS	=	$(shell find $(CLIENT_SOURCE_PATH) -name '*.c')       # src_client/v_client.c
CLIENT_OBJS	=	$(addprefix $(BUILD_CLIENT_DIR)/,$(notdir $(CLIENT_SRCS:.c=.o)))

# Server Makefile config
BUILD_SERVER_DIR=build_server
SERVER_SOURCE_PATH=src_server
SERVER_SRCS =	$(shell find $(SERVER_SOURCE_PATH) -name '*.c')       # src_server/v_server.c
#SERVER_OBJS	=	$(addprefix $(BUILD_SERVER_DIR)/,$(notdir $(SERVER_SRCS:.c=.o)))
SERVER_OBJS_LIST = v_server.o
SERVER_OBJS = $(patsubst %,$(BUILD_SERVER_DIR)/%,$(SERVER_OBJS_LIST))

$(CLIENT_OBJS):
	@echo "[CC] $<"
	$(CC) $(CLIENT_SRCS) -c $(CFLAGS)  $< -o $@

$(SERVER_OBJS):
	@echo "[CC] $<"
	$(CC) $(SERVER_SRCS) -c $(CFLAGS)  $< -o $@

$(BUILD_SERVER_DIR):
	mkdir $@

$(BUILD_CLIENT_DIR):
	mkdir $@

client: $(CLIENT_OBJS)
	@echo "[LINK] $<"
	$(CC) $(CFLAGS) $(LIBS) $< -o $(BUILD_CLIENT_DIR)/$@

server:	$(SERVER_OBJS)
	@echo "[LINK] $<"
	$(CC) $(CFLAGS) $(LIBS) $< -o $(BUILD_SERVER_DIR)/$@

big_build: $(server) $(client) 
	@echo "[CC] $<"
	@echo  .....Server and Client compiled.

.PHONY: depend clean

clean:
	$(RM) server $(BUILD_SERVER_DIR)/* client $(BUILD_CLIENT_DIR)/* 

depend: $(SRCS)
		makedepend $(INCLUDES) $^

list_src:
	@echo $(shell find src -name '*.c')

#  macros $@ and $^, which are the left and right sides of the :
#  the $< is the first item in the dependencies list