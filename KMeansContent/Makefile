ifdef MONITORING
    DEFINES = -DMONITORING=1
endif

PROJECT_INCLUDE_DIR = ./include
PROJECT_SOURCE_DIR  = ./src
PROJECT_LIBRARY_DIR = $(PROJECT_DIR)/lib

INCLUDES  = -I$(PROJECT_INCLUDE_DIR)
INCLUDES += -I$(PROJECT_DIR)/Framework/include
ifdef MONITORING
    INCLUDES += -I$(PROJECT_DIR)/Monitoring/include
endif

CC = g++
CFLAGS = -c -Wall -g -w -fPIC -O2
CFLAGS += $(INCLUDES)
ifdef BUILD_32BIT_COMPATIBLE
    CFLAGS += -m32
endif

SOURCES = $(wildcard $(PROJECT_SOURCE_DIR)/*.cc)

OBJECTS = $(SOURCES:.cc=.o)

LIBS = -L$(PROJECT_LIBRARY_DIR) -lPandoraFramework

ifdef MONITORING
    LIBS += -lPandoraMonitoring
endif

ifdef BUILD_32BIT_COMPATIBLE
    LIBS += -m32
endif

LDFLAGS = $(LIBS) -Wl,-rpath

LIBRARY = $(PROJECT_LIBRARY_DIR)/libPandoraKMeansContent.so

all: $(SOURCES) $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -shared -o $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(CC) $(LDFLAGS) -fPIC $(OBJECTS) -o $@

.cc.o:
	$(CC) $(CFLAGS) $(DEFINES) $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -f $(LIBRARY)
