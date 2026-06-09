LBITS := $(shell getconf LONG_BIT)
INSTALL_PREFIX ?= /usr/local/lib

UNAME := $(shell uname)

CFLAGS = -Wall -O3 -fPIC -I sdk/public -I $(HASHLINK_SRC)/src -std=c++0x

ifeq ($(UNAME),Darwin)
OS=osx
ARCH=
else
OS=linux
ARCH=$(LBITS)
endif

SDK_BIN_DIR = sdk/redistributable_bin/$(OS)$(ARCH)
LFLAGS = -lhl -lsteam_api -lstdc++ -L $(SDK_BIN_DIR)

SRC = native/cloud.o native/common.o native/controller.o native/friends.o native/gameserver.o \
	native/matchmaking.o native/networking.o native/stats.o native/ugc.o native/timeline.o


all: ${SRC}
	${CC} ${CFLAGS} -shared -o steam.hdll ${SRC} ${LFLAGS}

install:
	cp steam.hdll ${INSTALL_PREFIX}
	cp $(SDK_BIN_DIR)/libsteam_api.* ${INSTALL_PREFIX}

uninstall:
	rm -f ${INSTALL_PREFIX}/steam.hdll ${INSTALL_PREFIX}/libsteam_api.*

.SUFFIXES : .cpp .o

.cpp.o :
	${CC} ${CFLAGS} -o $@ -c $<

clean_o:
	rm -f ${SRC}

clean: clean_o
	rm -f steam.hdll

