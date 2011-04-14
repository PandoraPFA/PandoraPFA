########################################################
# cmake file for building PandoraPFANew
# @author Jan Engels, DESY
CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
########################################################


# project name
PROJECT( PandoraPFANew )


# project version
SET( ${PROJECT_NAME}_VERSION_MAJOR 0 )
SET( ${PROJECT_NAME}_VERSION_MINOR 7 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )



### DEPENDENCIES ############################################################

FIND_PACKAGE( ILCUTIL COMPONENTS ILCSOFT_CMAKE_MODULES REQUIRED )

# load default settings from ILCSOFT_CMAKE_MODULES
INCLUDE( ilcsoft_default_settings )

FIND_PACKAGE( PandoraMonitoring QUIET )

IF( PandoraMonitoring_FOUND )
    ADD_DEFINITIONS( -DMONITORING )
    INCLUDE_DIRECTORIES( "${PandoraMonitoring_INCLUDE_DIRS}" )
    LINK_LIBRARIES( "${PandoraMonitoring_LIBRARIES}" )
ELSE()
    MESSAGE( STATUS "PandoraMonitoring not found -- please set PandoraMonitoring_DIR to activate PandoraMonitoring" )
ENDIF()



### DOCUMENTATION ###########################################################
OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

IF( INSTALL_DOC )

    FIND_PACKAGE( Doxygen )

    IF( DOXYGEN_EXECUTABLE )
        ADD_SUBDIRECTORY( ./doc )
    ELSE()
        MESSAGE( SEND_ERROR "Could not find doxygen required to build documentation" )
        MESSAGE( "Please install doxygen or set INSTALL_DOC to OFF" )
        MESSAGE( "" )
    ENDIF()

ENDIF()


### LIBRARY AND TOOLS #######################################################


ADD_SUBDIRECTORY( Framework )
ADD_SUBDIRECTORY( FineGranularityContent )
ADD_SUBDIRECTORY( KMeansContent )


# display some variables and write them to cache
DISPLAY_STD_VARIABLES()

# generate and install following configuration files
GENERATE_PACKAGE_CONFIGURATION_FILES( ${PROJECT_NAME}Config.cmake ${PROJECT_NAME}ConfigVersion.cmake ${PROJECT_NAME}LibDeps.cmake )
