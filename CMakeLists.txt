########################################################
# cmake file for building PandoraPFANew
# @author Jan Engels, DESY
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.2 FATAL_ERROR)
########################################################


# project name
PROJECT( PandoraPFANew )


# project version
SET( ${PROJECT_NAME}_VERSION_MAJOR 01 )
SET( ${PROJECT_NAME}_VERSION_MINOR 00 )
SET( ${PROJECT_NAME}_VERSION_PATCH 00 )


### EXTERNAL PROJECT VERSIONS ###############################################
# e.g. SET( PandoraSDK_version "tags/vAB-CD" )
SET( PandoraSDK_version "tags/PandoraSDK_v01-00-00" )
SET( PandoraMonitoring_version "tags/PandoraMonitoring_v01-00-00" )
SET( LCContent_version "tags/LCContent_v01-00-00" )
SET( LArContent_version "tags/LArContent_v01-00-00" )

OPTION( PANDORA_MONITORING "Build PandoraMonitoring library (requires ROOT)" OFF )
OPTION( PANDORA_LC_CONTENT "Build Pandora LC content library" OFF )
OPTION( PANDORA_LAR_CONTENT "Build Pandora LAr content library" OFF )
OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

MESSAGE( STATUS "PANDORA_MONITORING: ${PANDORA_MONITORING}" )
MESSAGE( STATUS "PANDORA_LC_CONTENT: ${PANDORA_LC_CONTENT}" )
MESSAGE( STATUS "PANDORA_LAR_CONTENT: ${PANDORA_LAR_CONTENT}" )
MESSAGE( STATUS "INSTALL_DOC: ${INSTALL_DOC}" )


### DEPENDENCIES ############################################################
INCLUDE( ExternalProject )

FIND_PACKAGE( ILCUTIL COMPONENTS ILCSOFT_CMAKE_MODULES QUIET )

IF( ILCUTIL_FOUND )
    INCLUDE( ilcsoft_default_settings )
ELSEIF( PANDORA_CMAKE_MODULES_ROOT )
    LIST( APPEND CMAKE_MODULE_PATH "${PANDORA_CMAKE_MODULES_ROOT}" )
    INCLUDE( Default_settings )
ELSE()
    MESSAGE( FATAL_ERROR "Required path PANDORA_CMAKE_MODULES_ROOT not specified." )
ENDIF()

IF ( PANDORA_MONITORING AND ROOT_DIR )
    LIST( APPEND CMAKE_MODULE_PATH "${ROOT_DIR}" )
ENDIF()


### DOWNLOAD SETTINGS #######################################################
#SET( desy_svn_repository "https://svnsrv.desy.de/svn" )    # use grid certificate (pkcs12)
#SET( desy_svn_repository "https://svnsrv.desy.de/desy" )   # use desy account (kerberos)
SET( desy_svn_repository "https://svnsrv.desy.de/public" )  # public (no authentication)


### PASS ARGUMENTS TO EXTERNAL PROJECTS #####################################
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

SET( common_cmake_args
     "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH_FIXED}"
     "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH_FIXED}"
     "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
     "-DINSTALL_DOC=${INSTALL_DOC}" )

IF( PANDORA_CMAKE_MODULES_ROOT )
    SET( common_cmake_args ${common_cmake_args} "-DPANDORA_CMAKE_MODULES_ROOT=${PANDORA_CMAKE_MODULES_ROOT}" )
ENDIF()


### EXTERNAL PROJECTS #######################################################
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
            CMAKE_ARGS ${common_cmake_args} -DPANDORA_MONITORING=ON
            LIST_SEPARATOR %
        )
    ENDIF()

    IF ( PANDORA_LAR_CONTENT )
        ExternalProject_Add( LArContent
            DEPENDS PandoraSDK PandoraMonitoring
            SVN_REPOSITORY "${desy_svn_repository}/PandoraPFANew/LArContent/${LArContent_version}"
            CMAKE_ARGS ${common_cmake_args} -DPANDORA_MONITORING=ON
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
            CMAKE_ARGS ${common_cmake_args}
            LIST_SEPARATOR %
        )
    ENDIF()
ENDIF()

INSTALL( FILES "ChangeLog.txt" DESTINATION "doc" )

# display some variables and write them to cache
DISPLAY_STD_VARIABLES()
