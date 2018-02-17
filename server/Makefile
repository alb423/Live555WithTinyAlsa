LIVE555_INC = /home/albert/test/x86_live555/usr/local/include/
LIVE555_LIB = /home/albert/test/x86_live555/usr/local/lib/
LIVE_DIR = ../../../live

all:	wis-streamer

CC = gcc
CPLUSPLUS = g++

INCLUDES = -I . \
	-I$(LIVE555_INC)UsageEnvironment/ \
	-I$(LIVE555_INC)groupsock/ \
	-I$(LIVE555_INC)liveMedia/ \
	-I$(LIVE555_INC)BasicUsageEnvironment/

CFLAGS = $(INCLUDES) -D_LINUX -g -Wall -Wno-write-strings

LIBS =	-L$(LIVE555_LIB) \
	-lliveMedia \
	-lBasicUsageEnvironment \
	-lUsageEnvironment \
	-lgroupsock \
	-ltinyalsa

OBJS = wis-streamer.o WISInput.o WISServerMediaSubsession.o \
	AudioRTPCommon.o \
	WISPCMAudioServerMediaSubsession.o
	
wis-streamer: $(OBJS) 
	$(CPLUSPLUS) $(CFLAGS) -o wis-streamer $(OBJS) $(LIBS)


wis-streamer.cpp:				Options.hh 

Options.hh:					MediaFormat.hh

WISInput.cpp:				WISInput.hh Options.hh

WISServerMediaSubsession.cpp:		WISServerMediaSubsession.hh

WISServerMediaSubsession.hh:		WISInput.hh
WISPCMAudioServerMediaSubsession.hh:	WISServerMediaSubsession.hh MediaFormat.hh

AudioRTPCommon.cpp:			AudioRTPCommon.hh Options.hh WISInput.hh 

WISPCMAudioServerMediaSubsession.cpp:	WISPCMAudioServerMediaSubsession.hh Options.hh AudioRTPCommon.hh

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

.cpp.o:
	$(CPLUSPLUS) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~
	rm -f wis-streamer