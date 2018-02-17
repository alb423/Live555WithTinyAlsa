LIVE555_INC = /home/albert/test/x86_live555/usr/local/include/
LIVE555_LIB = /home/albert/test/x86_live555/usr/local/lib/

INCLUDES = \
	-I$(LIVE555_INC)UsageEnvironment/ \
	-I$(LIVE555_INC)groupsock/ \
	-I$(LIVE555_INC)liveMedia/ \
	-I$(LIVE555_INC)BasicUsageEnvironment/

# Default library filename suffixes for each library that we link with.  The "config.*" file might redefine these later.
libliveMedia_LIB_SUFFIX = $(LIB_SUFFIX)
libBasicUsageEnvironment_LIB_SUFFIX = $(LIB_SUFFIX)
libUsageEnvironment_LIB_SUFFIX = $(LIB_SUFFIX)
libgroupsock_LIB_SUFFIX = $(LIB_SUFFIX)

##### Change the following for your environment:
COMPILE_OPTS =		$(INCLUDES) -g -m64  -fPIC -I.  -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64

C =			c
C_COMPILER =		cc
C_FLAGS =		$(COMPILE_OPTS)
CPP =			cpp
CPLUSPLUS_COMPILER =	c++
CPLUSPLUS_FLAGS =	$(COMPILE_OPTS) -Wall -DBSD=1
OBJ =			o
LINK =			c++ -o
LINK_OPTS =		-L.
CONSOLE_LINK_OPTS =	$(LINK_OPTS)
LIBRARY_LINK =		ar cr 
LIBRARY_LINK_OPTS =	
LIB_SUFFIX = so
LIBS_FOR_CONSOLE_APPLICATION =
LIBS_FOR_GUI_APPLICATION =
EXE =
##### End of variables to change

UNICAST_RECEIVER_APPS = testRTSPClient$(EXE)
UNICAST_APPS = $(UNICAST_RECEIVER_APPS)

PREFIX = /usr/local
ALL = $(UNICAST_APPS)
all: $(ALL)

extra:	testGSMStreamer$(EXE)

.$(C).$(OBJ):
	$(C_COMPILER) -c $(C_FLAGS) $<
.$(CPP).$(OBJ):
	$(CPLUSPLUS_COMPILER) -c $(CPLUSPLUS_FLAGS) $<

TEST_RTSP_CLIENT_OBJS    = testRTSPClient.$(OBJ)

USAGE_ENVIRONMENT_LIB = $(LIVE555_LIB)/libUsageEnvironment.$(libUsageEnvironment_LIB_SUFFIX)
BASIC_USAGE_ENVIRONMENT_LIB = $(LIVE555_LIB)/libBasicUsageEnvironment.$(libBasicUsageEnvironment_LIB_SUFFIX)
LIVEMEDIA_LIB = $(LIVE555_LIB)/libliveMedia.$(libliveMedia_LIB_SUFFIX)
GROUPSOCK_LIB = $(LIVE555_LIB)/libgroupsock.$(libgroupsock_LIB_SUFFIX)
LOCAL_LIBS =	$(LIVEMEDIA_LIB) $(GROUPSOCK_LIB) \
		$(BASIC_USAGE_ENVIRONMENT_LIB) $(USAGE_ENVIRONMENT_LIB) -ltinyalsa
		
LIBS =	$(LOCAL_LIBS) $(LIBS_FOR_CONSOLE_APPLICATION) 


testRTSPClient$(EXE):	$(TEST_RTSP_CLIENT_OBJS) $(LOCAL_LIBS)
	$(LINK)$@ $(CONSOLE_LINK_OPTS) $(TEST_RTSP_CLIENT_OBJS) $(LIBS)
	

clean:
	-rm -rf *.$(OBJ) $(ALL) core *.core *~ include/*~

