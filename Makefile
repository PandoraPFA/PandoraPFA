#Path to project directory
PROJECT_DIR = YOUR_PATH_HERE

#Paths to project dependencies, note monitoring is optional
ARGUMENTS = PROJECT_DIR=$(PROJECT_DIR)

ifdef BUILD_32BIT_COMPATIBLE
    ARGUMENTS += BUILD_32BIT_COMPATIBLE=1
endif
ifdef MONITORING
    ARGUMENTS += MONITORING=1
endif

all:
	(cd $(PROJECT_DIR)/Framework; make $(ARGUMENTS))
ifdef MONITORING
	(cd $(PROJECT_DIR)/Monitoring; make $(ARGUMENTS))
endif
	(cd $(PROJECT_DIR)/FineGranularityContent; make $(ARGUMENTS))
	(cd $(PROJECT_DIR)/KMeansContent; make $(ARGUMENTS))

clean:
	(cd $(PROJECT_DIR)/Framework; make clean $(ARGUMENTS))
	(cd $(PROJECT_DIR)/Monitoring; make clean $(ARGUMENTS))
	(cd $(PROJECT_DIR)/FineGranularityContent; make clean $(ARGUMENTS))
	(cd $(PROJECT_DIR)/KMeansContent; make clean $(ARGUMENTS))
