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

		LDLIBS += -lpthread -llua5.4
	endif

	ifeq ($(PLATFORM), WINDOWS)
		LDLIBS += -luser32 -lkernel32
		LDLIBS += -llua
	endif
endif

.PHONY: clean install uninstall

Thread$(FILE_EXTENSION): $(OBJECT_FILES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) $(OBJECT_FILES)

install:
	$(CP) Thread.lua $(RELEASE_EXTENSION_PATH)/Thread.lua
	$(CP) Thread$(FILE_EXTENSION) $(RELEASE_EXTENSION_PATH)/Thread$(FILE_EXTENSION)

uninstall:
	$(RM) $(RELEASE_EXTENSION_PATH)/Thread.lua
	$(RM) $(RELEASE_EXTENSION_PATH)/Thread$(FILE_EXTENSION)
