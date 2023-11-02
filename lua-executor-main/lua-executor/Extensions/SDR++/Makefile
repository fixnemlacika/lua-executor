CP           ?= cp -f
RM           ?= rm -f
CXX          ?= g++

SOURCE_FILES  = main.cpp
OBJECT_FILES  = $(SOURCE_FILES:.cpp=.o)

LDFLAGS      += -static-libgcc -static-libstdc++ -shared
CPPFLAGS     += 
CXXFLAGS     += -Wall -Wfatal-errors $(BUILD_FLAGS) -std=c++20 -fPIC -I. -I"$(AL_INCLUDE)" -I"$(LIB_APRS_IS_INCLUDE)" -I"../../"

ifdef COMPILER
	ifeq ($(COMPILER), GNU)
		CXX = g++
	else ifeq ($(COMPILER), CLANG)
		CXX = clang++
	else
		CXX = $(COMPILER)
	endif
endif

ifdef PLATFORM
	ifeq ($(PLATFORM), LINUX)
#		liblua5.4-dev

		LDLIBS += -llua5.4
	endif

	ifeq ($(PLATFORM), WINDOWS)
		LDLIBS += -luser32 -lkernel32
		LDLIBS += -lwsock32 -lws2_32 -lIphlpapi
		LDLIBS += -llua
	endif
endif

.PHONY: clean install uninstall

SDR++$(FILE_EXTENSION): $(OBJECT_FILES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) $(OBJECT_FILES)

install:
	$(CP) SDR++.lua $(RELEASE_EXTENSION_PATH)/SDR++.lua
	$(CP) SDR++$(FILE_EXTENSION) $(RELEASE_EXTENSION_PATH)/SDR++$(FILE_EXTENSION)

uninstall:
	$(RM) $(RELEASE_EXTENSION_PATH)/SDR++.lua
	$(RM) $(RELEASE_EXTENSION_PATH)/SDR++$(FILE_EXTENSION)
