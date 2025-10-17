cmake_minimum_required(VERSION 3.14)

# Ensure out-of-source build
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "PandoraPFA requires an out-of-source build.")
endif()

# Project definition
project(PandoraPFA VERSION 04.19.05 LANGUAGES CXX)

#---------------------------------------------------------------------
# Options
option(PANDORA_MONITORING "Build PandoraMonitoring library (requires ROOT)" OFF)
option(LAR_PANDORA_CONTENT "Build LAr Pandora content library" ON)
option(INSTALL_DOC "Build documentation" OFF)
option(LC_PANDORA_CONTENT "Build LC Pandora content library" ON)
option(EXAMPLE_PANDORA_CONTENT "Build Example Pandora content library" ON)

#---------------------------------------------------------------------
# External repository settings
set(GIT_REPO_ROOT "https://github.com/AndyChappell")

set(PandoraSDK_REPO "PandoraSDK.git")
set(PandoraMonitoring_REPO "PandoraMonitoring.git")
set(LCContent_REPO "LCContent.git")
set(LArContent_REPO "LArContent.git")
set(ExampleContent_REPO "ExampleContent.git")

set(PandoraSDK_TAG "origin/feature/modern_build")
set(PandoraMonitoring_TAG "origin/feature/modern_build")
set(LArContent_TAG "origin/feature/modern_build")
set(LCContent_TAG "origin/feature/modern_build")
set(ExampleContent_TAG "origin/master")
set(Eigen3_VERSION "3.4.0")

#---------------------------------------------------------------------
# Common CMake arguments for ExternalProject
set(COMMON_CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
    -DCMAKE_CXX_STANDARD_REQUIRED=ON
    -DINSTALL_DOC=${INSTALL_DOC}
)

# Add CMake module path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmakemodules)

#---------------------------------------------------------------------
include(ExternalProject)

# PandoraSDK
ExternalProject_Add(PandoraSDK
    GIT_REPOSITORY ${GIT_REPO_ROOT}/${PandoraSDK_REPO}
    GIT_TAG ${PandoraSDK_TAG}
    CMAKE_ARGS ${COMMON_CMAKE_ARGS}
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/PandoraSDK
)

# PandoraMonitoring (optional)
if(PANDORA_MONITORING)
    ExternalProject_Add(PandoraMonitoring
        DEPENDS PandoraSDK
        GIT_REPOSITORY ${GIT_REPO_ROOT}/${PandoraMonitoring_REPO}
        GIT_TAG ${PandoraMonitoring_TAG}
        CMAKE_ARGS ${COMMON_CMAKE_ARGS} -DPandoraSDK_DIR=${CMAKE_CURRENT_BINARY_DIR}/PandoraSDK/src/PandoraSDK
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/PandoraMonitoring
    )
endif()

# LAr Pandora Content
if(LAR_PANDORA_CONTENT)
    ExternalProject_Add(Eigen3
        URL https://gitlab.com/libeigen/eigen/-/archive/${Eigen3_VERSION}/eigen-${Eigen3_VERSION}.tar.gz
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/Eigen3
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/Eigen3
    )

    ExternalProject_Add(LArContent
        DEPENDS PandoraSDK PandoraMonitoring Eigen3
        GIT_REPOSITORY ${GIT_REPO_ROOT}/${LArContent_REPO}
        GIT_TAG ${LArContent_TAG}
        CMAKE_ARGS ${COMMON_CMAKE_ARGS} -DEigen3_DIR=${CMAKE_CURRENT_BINARY_DIR}/Eigen3/share/eigen3/cmake/
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/LArContent
    )
endif()

# LC Pandora Content
if(LC_PANDORA_CONTENT)
    ExternalProject_Add(LCContent
        DEPENDS PandoraSDK PandoraMonitoring
        GIT_REPOSITORY ${GIT_REPO_ROOT}/${LCContent_REPO}
        GIT_TAG ${LCContent_TAG}
        CMAKE_ARGS ${COMMON_CMAKE_ARGS}
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/LCContent
    )
endif()

# Example Pandora Content
if(EXAMPLE_PANDORA_CONTENT)
    ExternalProject_Add(ExampleContent
        DEPENDS PandoraSDK PandoraMonitoring
        GIT_REPOSITORY ${GIT_REPO_ROOT}/${ExampleContent_REPO}
        GIT_TAG ${ExampleContent_TAG}
        CMAKE_ARGS ${COMMON_CMAKE_ARGS}
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/ExampleContent
    )
endif()

# Install cmakemodules and changelog
install(DIRECTORY cmakemodules/ DESTINATION cmakemodules)
install(FILES ChangeLog.txt DESTINATION doc)

# Display variables
PANDORA_DISPLAY_STD_VARIABLES()

