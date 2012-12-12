PROJECT_DIR = $(PANDORA_DIR)

DEFINES  = -DMONITORING=1
DEFINES += -DROOT_EVE=1

PROJECT_LIBRARY_DIR = $(PROJECT_DIR)/lib

INCLUDES  = -I$(PROJECT_DIR)/include
INCLUDES += -I$(PANDORA_DIR)/Framework/include
INCLUDES += -I$(shell $(ROOTSYS)/bin/root-config --incdir)

CC = g++
CFLAGS = -c -Wall -g -w -fPIC -O2
ifdef BUILD_32BIT_COMPATIBLE
    CFLAGS += -m32
endif

SOURCES = $(wildcard $(PROJECT_DIR)/src/*.cc)

OBJECTS = $(SOURCES:.cc=.o)
DEPENDS = $(OBJECTS:.o=.d)

LIBS = -L$(PANDORA_DIR)/lib -lPandoraFramework $(shell $(ROOTSYS)/bin/root-config --glibs) -lEve
ifdef BUILD_32BIT_COMPATIBLE
    LIBS += -m32
endif

LDFLAGS  = $(shell $(ROOTSYS)/bin/root-config --auxcflags)
LDFLAGS += $(LIBS) -Wl,-rpath

LIBRARY = $(PROJECT_DIR)/lib/libPandoraMonitoring.so

all: $(SOURCES) $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -shared -o $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(CC) $(LDFLAGS) -fPIC $(OBJECTS) -o $@

-include $(DEPENDS)

%.o:%.cc
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -MP -MMD -MT $*.o -MT $*.d -MF $*.d -o $*.o $*.cc

clean:
	rm -f $(OBJECTS)
	rm -f $(DEPENDS)
	rm -f $(LIBRARY)
