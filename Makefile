#Path to project directory
PROJECT_DIR = YOUR_PATH_HERE

#Paths to project dependencies, note monitoring is optional
PANDORAMONITORING_DIR = YOUR_PATH_HERE

ifdef MONITORING
    DEFINES = -DMONITORING=1
endif

PROJECT_INCLUDE_DIR = $(PROJECT_DIR)/include/
PROJECT_SOURCE_DIR  = $(PROJECT_DIR)/src/
PROJECT_LIBRARY_DIR = $(PROJECT_DIR)/lib/

INCLUDES = -I$(PROJECT_INCLUDE_DIR)
ifdef MONITORING
    INCLUDES += -I$(PANDORAMONITORING_DIR)/include
endif

CC = gcc
CFLAGS = -c -Wall -g -w -fPIC
CFLAGS += $(INCLUDES)
ifdef BUILD_32BIT_COMPATIBLE
    CFLAGS += -m32
endif

SOURCES  = $(wildcard $(PROJECT_SOURCE_DIR)Algorithms/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)Algorithms/Cheating/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)Algorithms/Clustering/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)Algorithms/FragmentRemoval/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)Algorithms/Monitoring/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)Algorithms/PfoConstruction/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)Algorithms/Reclustering/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)Algorithms/TopologicalAssociation/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)Api/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)Helpers/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)Managers/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)Objects/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)Pandora/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)Utilities/*.cc)
SOURCES += $(wildcard $(PROJECT_SOURCE_DIR)Xml/*.cc)

OBJECTS = $(SOURCES:.cc=.o)

LIBS =

ifdef MONITORING
    LIBS += -L$(PANDORAMONITORING_DIR)/lib -lPandoraMonitoring
endif

ifdef BUILD_32BIT_COMPATIBLE
    LIBS += -m32
endif

LDFLAGS = $(LIBS) -Wl,-rpath

LIBRARY = $(PROJECT_LIBRARY_DIR)/libPandoraPFANew.so

all: $(SOURCES) $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -shared -o $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(CC) $(LDFLAGS) -fPIC $(OBJECTS) -o $@

.cc.o:
	$(CC) $(CFLAGS) $(DEFINES) $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -f $(LIBRARY)
