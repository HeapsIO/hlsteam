LBITS := $(shell getconf LONG_BIT)

UNAME := $(shell uname)

CFLAGS = -Wall -O3 -I src -I include -L lib -fPIC -I sdk/public/

ifndef ARCH
	ARCH = $(LBITS)
endif

SDKPATH=
STEAMAPILIB=

OS=$(UNAME)
LIBARCH=$(ARCH)

ifeq (${OS},Darwin)
	OS=osx
	ARCH=
	SDKPATH=sdk/redistributable_bin/osx
	LFLAGS+=-lsteam_api
	STEAMAPILIB=steam_api
else ifeq (${OS},Linux)
	CFLAGS += -std=c++0x
	LFLAGS+=-lsteam_api
	SDKPATH=sdk/redistributable_bin/linux${ARCH}
	STEAMAPILIB=steam_api
else
	OS=win
	ARCH=64
	LIBARCH=64
	SDKPATH=sdk/redistributable_bin/win64
	STEAMAPILIB=steam_api64
endif

LFLAGS = -lhl -lstdc++ -L ${SDKPATH} -l ${STEAMAPILIB}

SRC = native/cloud.o native/common.o native/controller.o native/friends.o native/gameserver.o \
	native/matchmaking.o native/networking.o native/stats.o native/ugc.o

all: ${SRC}
	${CC} ${CFLAGS} -shared -o steam.hdll ${SRC} ${LFLAGS}

install:
	cp steam.hdll /usr/lib

.SUFFIXES : .cpp .o

.cpp.o :
	${CC} ${CFLAGS} -o $@ -c $<

clean_o:
	rm -f ${SRC}

clean: clean_o
	rm -f steam.hdll
