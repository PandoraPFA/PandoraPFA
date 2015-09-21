########################################################
# cmake file for building PandoraPFANew
# @author Jan Engels, DESY
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.2 FATAL_ERROR)
########################################################


# project name
PROJECT( PandoraPFANew )


# project version
SET( ${PROJECT_NAME}_VERSION_MAJOR 02 )
SET( ${PROJECT_NAME}_VERSION_MINOR 02 )
SET( ${PROJECT_NAME}_VERSION_PATCH 00 )


### EXTERNAL PROJECT VERSIONS ###############################################
SET( svn_repository_root "https://svnsrv.desy.de/public/PandoraPFANew" )  # public (no authentication)

SET( PandoraSDK_repository "PandoraSDK" )
SET( PandoraMonitoring_repository "PandoraMonitoring" )
SET( LCContent_repository "LCContent" )
SET( LArContent_repository "LArContent" )

SET( PandoraSDK_version "trunk" )
SET( PandoraMonitoring_version "trunk" )
SET( LCContent_version "trunk" )
SET( LArContent_version "trunk" )


### EXTERNAL PROJECT SELECTION ##############################################
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
     "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH_FIXED}"
     "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
     "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
     "-DINSTALL_DOC=${INSTALL_DOC}" )

IF( CMAKE_PREFIX_PATH_FIXED )
    SET( common_cmake_args ${common_cmake_args} "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH_FIXED}" )
ENDIF()

IF( CMAKE_CXX_FLAGS )
    SET( common_cmake_args ${common_cmake_args} "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}" )
ENDIF()

IF( PANDORA_CMAKE_MODULES_ROOT )
    SET( common_cmake_args ${common_cmake_args} "-DPANDORA_CMAKE_MODULES_ROOT=${PANDORA_CMAKE_MODULES_ROOT}" )
ENDIF()

IF( PANDORA_MONITORING )
    SET( cmake_monitoring_args "-DPANDORA_MONITORING=ON" )
    SET( content_library_monitoring "PandoraMonitoring" )
ENDIF()


### EXTERNAL PROJECTS #######################################################
ExternalProject_Add( PandoraSDK
    SVN_REPOSITORY "${svn_repository_root}/${PandoraSDK_repository}/${PandoraSDK_version}"
    CMAKE_ARGS ${common_cmake_args}
    PREFIX PandoraSDK-${PandoraSDK_version}
    LIST_SEPARATOR %
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/PandoraSDK-${PandoraSDK_version}
)

IF( PANDORA_MONITORING )
    ExternalProject_Add( PandoraMonitoring
        DEPENDS PandoraSDK
        SVN_REPOSITORY "${svn_repository_root}/${PandoraMonitoring_repository}/${PandoraMonitoring_version}"
        CMAKE_ARGS ${common_cmake_args}
        PREFIX PandoraMonitoring-${PandoraMonitoring_version}
        LIST_SEPARATOR %
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/PandoraMonitoring-${PandoraMonitoring_version}
    )
ENDIF()

IF ( PANDORA_LC_CONTENT )
    ExternalProject_Add( LCContent
        DEPENDS PandoraSDK ${content_library_monitoring}
        SVN_REPOSITORY "${svn_repository_root}/${LCContent_repository}/${LCContent_version}"
        CMAKE_ARGS ${common_cmake_args} ${cmake_monitoring_args}
        PREFIX LCContent-${LCContent_version}
        LIST_SEPARATOR %
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/LCContent-${LCContent_version}
    )
ENDIF()

IF ( PANDORA_LAR_CONTENT )
    ExternalProject_Add( LArContent
        DEPENDS PandoraSDK ${content_library_monitoring}
        SVN_REPOSITORY "${svn_repository_root}/${LArContent_repository}/${LArContent_version}"
        CMAKE_ARGS ${common_cmake_args} ${cmake_monitoring_args}
        PREFIX LArContent-${LArContent_version}
        LIST_SEPARATOR %
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/LArContent-${LArContent_version}
    )
ENDIF()

INSTALL( FILES "ChangeLog.txt" DESTINATION "doc" )

# display some variables and write them to cache
DISPLAY_STD_VARIABLES()
