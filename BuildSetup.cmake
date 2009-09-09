#############################################################################
# cmake build setup for PandoraPFANew
#
# For building PandoraPFANew with cmake type:
# (1) $ mkdir build
# (2) $ cd build
# (3) $ cmake -C ../BuildSetup.cmake ..
# (4) $ make install
#
# @author Jan Engels, DESY
#############################################################################


#############################################################################
# Setup path variables
#############################################################################

# The three path variables below are only required if you wish to use the pandora monitoring package

# PANDORA_HOME
#SET( PANDORA_HOME "path_to_pandora_home"
#    CACHE PATH "Path to pandora Software" FORCE )

# Path to PandoraMonitoring
#SET( PandoraMonitoring_HOME "${PANDORA_HOME}/PandoraMonitoring"
#    CACHE PATH "Path to PandoraMonitoring" FORCE )

# CMake Modules Path
#SET( CMAKE_MODULE_PATH "${PANDORA_HOME}/CMakeModules/v01-08"
#    CACHE PATH "Path to CMake Modules" FORCE )

###############################################
# Project Options
###############################################

#SET( INSTALL_DOC OFF CACHE BOOL "Set to OFF to skip build/install Documentation" FORCE )

# set cmake build type
# possible options are: None Debug Release RelWithDebInfo MinSizeRel
#SET( CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build" FORCE )

###############################################
# Advanced Options
###############################################

#SET( BUILD_SHARED_LIBS OFF CACHE BOOL "Set to OFF to build static libraries" FORCE )

# installation path for PandoraPFA
# SET( CMAKE_INSTALL_PREFIX "foo/bar" CACHE STRING "Where to install PandoraPFANew" FORCE )
