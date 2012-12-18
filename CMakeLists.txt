########################################################
# cmake file for building PandoraPFANew
# @author Jan Engels, DESY
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.2 FATAL_ERROR)
########################################################


# project name
PROJECT( PandoraPFANew )


### PACKAGE VERSIONS ########################################################
SET( ${PROJECT_NAME}_VERSION_MAJOR 0 )
SET( ${PROJECT_NAME}_VERSION_MINOR 11 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

# versions of PandoraPFANew subpackages
# don't forget to update when releasing PandoraPFANew
# e.g. SET( PandoraSDK_version "tags/v00-01" )
SET( PandoraSDK_version "trunk" )
SET( Monitoring_version "trunk" )
SET( FineGranularityContent_version "trunk" )
SET( MarlinPandora_version "trunk" )
SET( Settings_version "trunk" )


### DEPENDENCIES ############################################################

FIND_PACKAGE( ILCUTIL COMPONENTS ILCSOFT_CMAKE_MODULES REQUIRED )

# load default settings from ILCSOFT_CMAKE_MODULES
INCLUDE( ilcsoft_default_settings )
INCLUDE( ExternalProject )

OPTION( PANDORA_MONITORING "Build Monitoring library (requires ROOT)" OFF )
MESSAGE( STATUS "PANDORA_MONITORING: ${PANDORA_MONITORING}" )

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

# ----- download settings ----------------------------------------------------
#SET( desy_svn_repository "https://svnsrv.desy.de/svn" )    # use grid certificate (pkcs12)
#SET( desy_svn_repository "https://svnsrv.desy.de/desy" )   # use desy account (kerberos)
SET( desy_svn_repository "https://svnsrv.desy.de/public" )  # public (no authentication)
# ----------------------------------------------------------------------------

# in order to pass CMAKE_PREFIX_PATH over the command line to all packages
# we need to separate list elements in this variable with '%' instead of the
# standard cmake list separator ';' (is there another way to fix this??)
# The list separator needs also to be redefined in the ExternalProject_Add
# calls by setting the variable LIST_SEPARATOR
FOREACH( _path ${CMAKE_PREFIX_PATH} )
    SET( CMAKE_PREFIX_PATH_FIXED ${CMAKE_PREFIX_PATH_FIXED}%${_path} )
ENDFOREACH()

# ----- cmake arguments common to ALL cmake packages -------------------------
SET( common_cmake_args
     "-DILCSOFT_CMAKE_MODULES_ROOT=${ILCSOFT_CMAKE_MODULES_ROOT}"
     "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH_FIXED}"
     "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}"
     "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
     "-DINSTALL_DOC=${INSTALL_DOC}"
)
# ----------------------------------------------------------------------------

ExternalProject_Add( PandoraSDK
    SVN_REPOSITORY "${desy_svn_repository}/PandoraPFANew/PandoraSDK/${PandoraSDK_version}"
    CMAKE_ARGS ${common_cmake_args}
)

IF( PANDORA_MONITORING )
    ExternalProject_Add( Monitoring
        DEPENDS PandoraSDK
        SVN_REPOSITORY "${desy_svn_repository}/PandoraPFANew/Monitoring/${Monitoring_version}"
        CMAKE_ARGS ${common_cmake_args}
    )

    ExternalProject_Add( FineGranularityContent
        DEPENDS PandoraSDK Monitoring
        SVN_REPOSITORY "${desy_svn_repository}/PandoraPFANew/FineGranularityContent/${FineGranularityContent_version}"
        CMAKE_ARGS ${common_cmake_args} -DPANDORA_MONITORING=1
    )

    ExternalProject_Add( MarlinPandora
        DEPENDS PandoraSDK Monitoring FineGranularityContent
        SVN_REPOSITORY "${desy_svn_repository}/PandoraPFANew/MarlinPandora/${MarlinPandora_version}"
        CMAKE_ARGS ${common_cmake_args} -DPANDORA_MONITORING=1
        LIST_SEPARATOR % # needed for CMAKE_PREFIX_PATH
    )
ELSE()
    ExternalProject_Add( FineGranularityContent
        DEPENDS PandoraSDK
        SVN_REPOSITORY "${desy_svn_repository}/PandoraPFANew/FineGranularityContent/${FineGranularityContent_version}"
        CMAKE_ARGS ${common_cmake_args}
    )

    ExternalProject_Add( MarlinPandora
        DEPENDS PandoraSDK FineGranularityContent
        SVN_REPOSITORY "${desy_svn_repository}/PandoraPFANew/MarlinPandora/${MarlinPandora_version}"
        CMAKE_ARGS ${common_cmake_args}
        LIST_SEPARATOR % # needed for CMAKE_PREFIX_PATH
    )
ENDIF()

#ExternalProject_Add( Settings
#    SVN_REPOSITORY "${desy_svn_repository}/PandoraPFANew/Settings/${Settings_version}"
#    CMAKE_ARGS ${common_cmake_args}
#)

# display some variables and write them to cache
DISPLAY_STD_VARIABLES()
