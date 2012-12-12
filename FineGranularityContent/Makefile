ifndef PROJECT_DIR
    PROJECT_DIR = $(PANDORA_DIR)
endif

ifdef MONITORING
    DEFINES = -DMONITORING=1
endif

INCLUDES  = -I$(PROJECT_DIR)/include
INCLUDES += -I$(PANDORA_DIR)/Framework/include
ifdef MONITORING
    INCLUDES += -I$(PANDORA_DIR)/Monitoring/include
endif

CC = g++
CFLAGS = -c -Wall -g -w -fPIC -O2
ifdef BUILD_32BIT_COMPATIBLE
    CFLAGS += -m32
endif

SOURCES  = $(wildcard $(PROJECT_DIR)/src/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/Cheating/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/Clustering/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/FragmentRemoval/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/Monitoring/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/ParticleId/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/Persistency/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/PfoConstruction/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/Reclustering/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/TopologicalAssociation/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/TrackClusterAssociation/*.cc)
SOURCES += $(wildcard $(PROJECT_DIR)/src/Utility/*.cc)

OBJECTS = $(SOURCES:.cc=.o)
DEPENDS = $(OBJECTS:.o=.d)

LIBS = -L$(PANDORA_DIR)/lib -lPandoraFramework

ifdef MONITORING
    LIBS += -lPandoraMonitoring
endif

ifdef BUILD_32BIT_COMPATIBLE
    LIBS += -m32
endif

LDFLAGS = $(LIBS) -Wl,-rpath

LIBRARY = $(PROJECT_DIR)/lib/libPandoraFineGranularityContent.so

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
