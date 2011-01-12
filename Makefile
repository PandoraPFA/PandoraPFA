#Path to project directory
PROJECT_DIR = YOUR_PATH_HERE

#Paths to project dependencies, note monitoring is optional
PANDORAMONITORING_DIR = YOUR_PATH_HERE

ARGUMENTS = PROJECT_DIR=$(PROJECT_DIR)

ifdef BUILD_32BIT_COMPATIBLE
    ARGUMENTS += BUILD_32BIT_COMPATIBLE=1
endif

ifdef MONITORING
    ARGUMENTS += MONITORING=1 PANDORAMONITORING_DIR=$(PANDORAMONITORING_DIR)
endif

all:
	(cd Framework; make $(ARGUMENTS))
	(cd FineGranularityContent; make $(ARGUMENTS))
	(cd KMeansContent; make $(ARGUMENTS))

clean:
	(cd Framework; make clean $(ARGUMENTS))
	(cd FineGranularityContent; make clean $(ARGUMENTS))
	(cd KMeansContent; make clean $(ARGUMENTS))
