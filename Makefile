#Path to pandora directory
ifndef PANDORA_DIR
    PANDORA_DIR = YOUR_PATH_HERE
endif

#Paths to project dependencies, note monitoring is optional
ARGUMENTS = PANDORA_DIR=$(PANDORA_DIR)

ifdef BUILD_32BIT_COMPATIBLE
    ARGUMENTS += BUILD_32BIT_COMPATIBLE=1
endif
ifdef MONITORING
    ARGUMENTS += MONITORING=1
endif

all:
	(cd $(PANDORA_DIR)/Framework; make $(ARGUMENTS))
ifdef MONITORING
	(cd $(PANDORA_DIR)/Monitoring; make $(ARGUMENTS))
endif
	-if test -d $(PANDORA_DIR)/FineGranularityContent; then (cd $(PANDORA_DIR)/FineGranularityContent; make $(ARGUMENTS)); fi
	-if test -d $(PANDORA_DIR)/LArContent; then (cd $(PANDORA_DIR)/LArContent; make $(ARGUMENTS)); fi

clean:
	(cd $(PANDORA_DIR)/Framework; make clean $(ARGUMENTS))
	(cd $(PANDORA_DIR)/Monitoring; make clean $(ARGUMENTS))
	-if test -d $(PANDORA_DIR)/FineGranularityContent; then (cd $(PANDORA_DIR)/FineGranularityContent; make clean $(ARGUMENTS)); fi
	-if test -d $(PANDORA_DIR)/LArContent; then (cd $(PANDORA_DIR)/LArContent; make clean $(ARGUMENTS)); fi
