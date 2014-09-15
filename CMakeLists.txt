########################################################
# cmake file for building PandoraPFANew
# @author Jan Engels, DESY
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.2 FATAL_ERROR)
########################################################


# project name
PROJECT( PandoraPFANew )


### PACKAGE VERSIONS ########################################################
SET( ${PROJECT_NAME}_VERSION_MAJOR 0 )
SET( ${PROJECT_NAME}_VERSION_MINOR 17 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

# versions of PandoraPFANew subpackages
# don't forget to update when releasing PandoraPFANew
# e.g. SET( PandoraSDK_version "tags/vAB-CD" )
SET( PandoraSDK_version "trunk" )
SET( PandoraMonitoring_version "trunk" )
SET( LCContent_version "trunk" )
SET( LArContent_version "trunk" )


### DEPENDENCIES ############################################################

FIND_PACKAGE( ILCUTIL COMPONENTS ILCSOFT_CMAKE_MODULES REQUIRED )

# load default settings from ILCSOFT_CMAKE_MODULES
INCLUDE( ilcsoft_default_settings )
INCLUDE( ExternalProject )

OPTION( PANDORA_MONITORING "Build PandoraMonitoring library (requires ROOT)" OFF )
MESSAGE( STATUS "PANDORA_MONITORING: ${PANDORA_MONITORING}" )

OPTION( PANDORA_LC_CONTENT "Build Pandora LC content library" ON )
MESSAGE( STATUS "PANDORA_LC_CONTENT: ${PANDORA_LC_CONTENT}" )

OPTION( PANDORA_LAR_CONTENT "Build Pandora LAr content library" OFF )
MESSAGE( STATUS "PANDORA_LAR_CONTENT: ${PANDORA_LAR_CONTENT}" )

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

# ----- download settings ----------------------------------------------------
#SET( desy_svn_repository "https://svnsrv.desy.de/svn" )    # use grid certificate (pkcs12)
#SET( desy_svn_repository "https://svnsrv.desy.de/desy" )   # use desy account (kerberos)
SET( desy_svn_repository "https://svnsrv.desy.de/public" )  # public (no authentication)
# ----------------------------------------------------------------------------

# In order to pass semicolon-separated lists over the command line to all packages,
# we need to separate list elements with '%' instead of the standard cmake list separator ';'
# The list separator needs also to be redefined in the ExternalProject_Add calls by setting
# the variable LIST_SEPARATOR
FOREACH( _path ${CMAKE_PREFIX_PATH} )
    SET( CMAKE_PREFIX_PATH_FIXED ${CMAKE_PREFIX_PATH_FIXED}%${_path} )
ENDFOREACH()

FOREACH( _path ${CMAKE_MODULE_PATH} )
    SET( CMAKE_MODULE_PATH_FIXED ${CMAKE_MODULE_PATH_FIXED}%${_path} )
ENDFOREACH()

# ----- cmake arguments common to ALL cmake packages -------------------------
SET( common_cmake_args
     "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH_FIXED}"
     "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH_FIXED}"
     "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
     "-DINSTALL_DOC=${INSTALL_DOC}"
)
# ----------------------------------------------------------------------------

ExternalProject_Add( PandoraSDK
    SVN_REPOSITORY "${desy_svn_repository}/PandoraPFANew/PandoraSDK/${PandoraSDK_version}"
    CMAKE_ARGS ${common_cmake_args}
    LIST_SEPARATOR %
)

IF( PANDORA_MONITORING )
    ExternalProject_Add( PandoraMonitoring
        DEPENDS PandoraSDK
        SVN_REPOSITORY "${desy_svn_repository}/PandoraPFANew/PandoraMonitoring/${PandoraMonitoring_version}"
        CMAKE_ARGS ${common_cmake_args}
        LIST_SEPARATOR %
    )

    IF ( PANDORA_LC_CONTENT )
        ExternalProject_Add( LCContent
            DEPENDS PandoraSDK PandoraMonitoring
            SVN_REPOSITORY "${desy_svn_repository}/PandoraPFANew/LCContent/${LCContent_version}"
            CMAKE_ARGS ${common_cmake_args} -DPANDORA_MONITORING=1
            LIST_SEPARATOR %
        )
    ENDIF()

    IF ( PANDORA_LAR_CONTENT )
        ExternalProject_Add( LArContent
            DEPENDS PandoraSDK PandoraMonitoring
            SVN_REPOSITORY "${desy_svn_repository}/PandoraPFANew/LArContent/${LArContent_version}"
            CMAKE_ARGS ${common_cmake_args} -DPANDORA_MONITORING=1
            LIST_SEPARATOR %
        )
    ENDIF()
ELSE()
    IF ( PANDORA_LC_CONTENT )
        ExternalProject_Add( LCContent
            DEPENDS PandoraSDK
            SVN_REPOSITORY "${desy_svn_repository}/PandoraPFANew/LCContent/${LCContent_version}"
            CMAKE_ARGS ${common_cmake_args}
            LIST_SEPARATOR %
        )
    ENDIF()

    IF ( PANDORA_LAR_CONTENT )
        ExternalProject_Add( LArContent
            DEPENDS PandoraSDK
            SVN_REPOSITORY "${desy_svn_repository}/PandoraPFANew/LArContent/${LArContent_version}"
            CMAKE_ARGS ${common_cmake_args} -DPANDORA_MONITORING=1
            LIST_SEPARATOR %
        )
    ENDIF()
ENDIF()

INSTALL( FILES "ChangeLog.txt" DESTINATION "doc" )

# display some variables and write them to cache
DISPLAY_STD_VARIABLES()
