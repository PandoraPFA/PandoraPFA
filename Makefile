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
ifdef INCLUDE_TARGET
    ARGUMENTS += INCLUDE_TARGET=${INCLUDE_TARGET}
endif
ifdef LIB_TARGET
    ARGUMENTS += LIB_TARGET=${LIB_TARGET}
endif

all:
	(cd $(PANDORA_DIR)/PandoraSDK; make $(ARGUMENTS))
ifdef MONITORING
	(cd $(PANDORA_DIR)/PandoraMonitoring; make $(ARGUMENTS))
endif
	-if test -d $(PANDORA_DIR)/LCContent; then (cd $(PANDORA_DIR)/LCContent; make $(ARGUMENTS)); fi
	-if test -d $(PANDORA_DIR)/LArContent; then (cd $(PANDORA_DIR)/LArContent; make $(ARGUMENTS)); fi

clean:
	(cd $(PANDORA_DIR)/PandoraSDK; make clean $(ARGUMENTS))
	-if test -d $(PANDORA_DIR)/PandoraMonitoring; then (cd $(PANDORA_DIR)/PandoraMonitoring; make clean $(ARGUMENTS)); fi
	-if test -d $(PANDORA_DIR)/LCContent; then (cd $(PANDORA_DIR)/LCContent; make clean $(ARGUMENTS)); fi
	-if test -d $(PANDORA_DIR)/LArContent; then (cd $(PANDORA_DIR)/LArContent; make clean $(ARGUMENTS)); fi

install:
	(cd $(PANDORA_DIR)/PandoraSDK; make install $(ARGUMENTS))
ifdef MONITORING
	(cd $(PANDORA_DIR)/PandoraMonitoring; make install $(ARGUMENTS))
endif
	-if test -d $(PANDORA_DIR)/LCContent; then (cd $(PANDORA_DIR)/LCContent; make install $(ARGUMENTS)); fi
	-if test -d $(PANDORA_DIR)/LArContent; then (cd $(PANDORA_DIR)/LArContent; make install $(ARGUMENTS)); fi
