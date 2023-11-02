CP                     ?= cp -f
RM                     ?= rm -f
CXX                    ?= g++
ZIP                    ?= zip -r -o

SOURCE_FILES            = main.cpp API.cpp API_Extensions.cpp
SOURCE_FILES           += Extensions/Script.cpp
OBJECT_FILES            = $(SOURCE_FILES:.cpp=.o)

BUILD_FLAGS            ?= -O2

LDFLAGS                += -static-libgcc -static-libstdc++
CPPFLAGS               += 
CXXFLAGS               += -Wall -Wfatal-errors $(BUILD_FLAGS) -std=c++20 -I. -I"$(AL_INCLUDE)" -I"$(LIB_APRS_IS_INCLUDE)"

EXTENSIONS              = $(wildcard Extensions/*)
EXTENSIONS             := $(filter-out Extensions/Script.%, $(EXTENSIONS))

RELEASE_PATH           ?= ../Build
RELEASE_EXTENSION_PATH ?= $(RELEASE_PATH)/Extensions

PUBLISH_PATH           ?= ../LuaAPRS-IS.zip

ENVIRONMENT_VARIABLES   = COMPILER=$(COMPILER) PLATFORM=$(PLATFORM) BUILD_FLAGS="$(BUILD_FLAGS)" RELEASE_PATH=../../$(RELEASE_PATH) RELEASE_EXTENSION_PATH=../../$(RELEASE_EXTENSION_PATH)

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

		LDLIBS += -ldl
		LDLIBS += -lpthread -llua5.4

		FILE_EXTENSION         =
		ENVIRONMENT_VARIABLES += FILE_EXTENSION=.so
	endif

	ifeq ($(PLATFORM), WINDOWS)
		LDLIBS += -luser32 -lkernel32 -lshlwapi
		LDLIBS += -lwsock32 -lws2_32 -lIphlpapi
		LDLIBS += -llua

		FILE_EXTENSION         = .exe
		ENVIRONMENT_VARIABLES += FILE_EXTENSION=.dll
	endif
endif

.PHONY: build build_extensions install install_extensions uninstall uninstall_extensions clean clean_extensions publish

build: LuaAPRS-IS$(FILE_EXTENSION) build_extensions

LuaAPRS-IS$(FILE_EXTENSION): $(OBJECT_FILES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

build_extensions:
	$(foreach EXTENSION, $(EXTENSIONS), $(MAKE) -C $(EXTENSION) -e $(ENVIRONMENT_VARIABLES) &&) true

install: uninstall install_extensions
	$(CP) LuaAPRS-IS$(FILE_EXTENSION) $(RELEASE_PATH)/LuaAPRS-IS$(FILE_EXTENSION)

install_extensions:
	$(foreach EXTENSION, $(EXTENSIONS), $(MAKE) -C $(EXTENSION) -e $(ENVIRONMENT_VARIABLES) install &&) true

uninstall: uninstall_extensions
	$(RM) $(RELEASE_PATH)/LuaAPRS-IS$(FILE_EXTENSION)

uninstall_extensions:
	$(foreach EXTENSION, $(EXTENSIONS), $(MAKE) -C $(EXTENSION) -e $(ENVIRONMENT_VARIABLES) uninstall &&) true

clean: clean_extensions
	$(RM) $(OBJECT_FILES)

clean_extensions:
	$(foreach EXTENSION, $(EXTENSIONS), $(MAKE) -C $(EXTENSION) -e $(ENVIRONMENT_VARIABLES) clean &&) true

publish: build install
	$(RM) $(PUBLISH_PATH)
	$(CP) ../LICENSE ./LICENSE
	$(ZIP) $(PUBLISH_PATH) $(RELEASE_PATH) LICENSE
	$(RM) ./LICENSE
