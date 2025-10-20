# Modern CMakeLists.txt for the PandoraPFA super-build
#-------------------------------------------------------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(PandoraPFA LANGUAGES CXX)

# Enforce out-of-source builds.
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "PandoraPFA requires an out-of-source build.")
endif()

# --- Staging Area for Installation ---
# All sub-projects will be installed into this single directory inside the build folder.
# This allows them to find each other easily using find_package.
set(STAGING_INSTALL_DIR "${CMAKE_BINARY_DIR}/install")
message(STATUS "All dependencies will be built and installed into: ${STAGING_INSTALL_DIR}")

#-------------------------------------------------------------------------------------------------------------------------------------------
# External project versions and repository locations
set(GIT_REPOSITORY_ROOT "https://github.com/AndyChappell")

set(PANDORA_SDK_VERSION "feature/modern_build")
set(PANDORA_MONITORING_VERSION "feature/modern_build")
set(LAR_CONTENT_VERSION "feature/modern_build")
set(EIGEN3_VERSION "3.4.0")

#-------------------------------------------------------------------------------------------------------------------------------------------
# Options to select which content libraries to build
include(CMakeDependentOption)
option(PANDORA_MONITORING "Build PandoraMonitoring library (requires ROOT)" ON)
option(LAR_PANDORA_CONTENT "Build LAr Pandora content library" ON)

message(STATUS "Build Configuration:")
message(STATUS "  - PandoraSDK: ON")
message(STATUS "  - PandoraMonitoring: ${PANDORA_MONITORING}")
message(STATUS "  - LArContent: ${LAR_PANDORA_CONTENT}")

#-------------------------------------------------------------------------------------------------------------------------------------------
# Common CMake arguments to pass to all external projects
# We enforce the same install prefix, build type, and C++ standard for consistency.
list(APPEND COMMON_CMAKE_ARGS
    "-DCMAKE_INSTALL_PREFIX=${STAGING_INSTALL_DIR}"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_CXX_STANDARD=17"
    # Pass the CMAKE_PREFIX_PATH so sub-projects can find dependencies (like ROOT)
    # that the user has specified for the super-build.
    "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}"
)

#-------------------------------------------------------------------------------------------------------------------------------------------
# External Projects Definition
include(ExternalProject)

# --- PandoraSDK (Core dependency) ---
ExternalProject_Add(PandoraSDK
    GIT_REPOSITORY    ${GIT_REPOSITORY_ROOT}/PandoraSDK.git
    GIT_TAG           ${PANDORA_SDK_VERSION}
    CMAKE_ARGS        ${COMMON_CMAKE_ARGS}
    UPDATE_COMMAND    "" # Avoid re-pulling on every configure
)

# --- PandoraMonitoring (Optional) ---
if(PANDORA_MONITORING)
    ExternalProject_Add(PandoraMonitoring
        DEPENDS           PandoraSDK
        GIT_REPOSITORY    ${GIT_REPOSITORY_ROOT}/PandoraMonitoring.git
        GIT_TAG           ${PANDORA_MONITORING_VERSION}
        CMAKE_ARGS        ${COMMON_CMAKE_ARGS}
        UPDATE_COMMAND    ""
    )
endif()

# --- LArContent (Optional) ---
if(LAR_PANDORA_CONTENT)
    # Eigen3 is a dependency for LArContent. We build it first.
    ExternalProject_Add(Eigen3
        URL               "https://gitlab.com/libeigen/eigen/-/archive/${EIGEN3_VERSION}/eigen-${EIGEN3_VERSION}.tar.gz"
        URL_HASH          SHA256=8586084f71f9bde545ee7fa6d00288b264a2b7ac3607b974e54d13e7162c1c72
        CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX=${STAGING_INSTALL_DIR}
        UPDATE_COMMAND    ""
    )

    # Determine the dependencies for LArContent
    set(LAR_CONTENT_DEPS PandoraSDK Eigen3)
    if(PANDORA_MONITORING)
        list(APPEND LAR_CONTENT_DEPS PandoraMonitoring)
    endif()

    ExternalProject_Add(LArContent
        DEPENDS           ${LAR_CONTENT_DEPS}
        GIT_REPOSITORY    ${GIT_REPOSITORY_ROOT}/LArContent.git
        GIT_TAG           ${LAR_CONTENT_VERSION}
        CMAKE_ARGS        ${COMMON_CMAKE_ARGS} -DPANDORA_MONITORING=${PANDORA_MONITORING}
        UPDATE_COMMAND    ""
    )
endif()

# --- Final Installation ---
# This step allows a user to run "cmake --install ." on the super-build
# to copy the artifacts from the staging area to a final desired location.
install(DIRECTORY "${STAGING_INSTALL_DIR}/"
    DESTINATION "."
    USE_SOURCE_PERMISSIONS
)
