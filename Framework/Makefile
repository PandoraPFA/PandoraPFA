PROJECT_INCLUDE_DIR = ./include
PROJECT_SOURCE_DIR  = ./src
PROJECT_LIBRARY_DIR = $(PROJECT_DIR)/lib

INCLUDES = -I$(PROJECT_INCLUDE_DIR)

CC = g++
CFLAGS = -c -Wall -g -w -fPIC -O2
CFLAGS += $(INCLUDES)
ifdef BUILD_32BIT_COMPATIBLE
    CFLAGS += -m32
endif

SOURCES  = $(wildcard $(PROJECT_SOURCE_DIR)/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)/Api/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)/Helpers/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)/Managers/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)/Objects/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)/Pandora/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)/Utilities/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)/Xml/*.cc)

OBJECTS = $(SOURCES:.cc=.o)

LIBS =

ifdef BUILD_32BIT_COMPATIBLE
    LIBS += -m32
endif

LDFLAGS = $(LIBS) -Wl,-rpath

LIBRARY = $(PROJECT_LIBRARY_DIR)/libPandoraFramework.so

all: $(SOURCES) $(OBJECTS)
	@echo $(PROJECT_SOURCE_DIR)
	$(CC) $(OBJECTS) $(LIBS) -shared -o $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(CC) $(LDFLAGS) -fPIC $(OBJECTS) -o $@

.cc.o:
	$(CC) $(CFLAGS) $(DEFINES) $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -f $(LIBRARY)
