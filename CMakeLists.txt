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
option(INSTALL_DOC "Build documentation" OFF)
option(BUILD_ONLY_SDK_AND_MONITORING "Build only PandoraSDK and PandoraMonitoring" OFF)
option(LAR_PANDORA_CONTENT "Build LAr Pandora content library" ON)
option(LC_PANDORA_CONTENT "Build LC Pandora content library" ON)
option(EXAMPLE_PANDORA_CONTENT "Build Example Pandora content library" ON)

#---------------------------------------------------------------------
# FetchContent setup
include(FetchContent)

# PandoraSDK
FetchContent_Declare(
    PandoraSDK
    GIT_REPOSITORY https://github.com/PandoraPFA/PandoraSDK.git
    GIT_TAG origin/master
)
FetchContent_MakeAvailable(PandoraSDK)

# PandoraMonitoring (optional)
if(PANDORA_MONITORING)
    FetchContent_Declare(
        PandoraMonitoring
        GIT_REPOSITORY https://github.com/PandoraPFA/PandoraMonitoring.git
        GIT_TAG origin/master
    )
    FetchContent_MakeAvailable(PandoraMonitoring)
endif()

# Eigen3 (required for LArContent)
if(LAR_PANDORA_CONTENT)
    FetchContent_Declare(
        Eigen3
        URL https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz
    )
    FetchContent_MakeAvailable(Eigen3)

    # LArContent
    FetchContent_Declare(
        LArContent
        GIT_REPOSITORY https://github.com/PandoraPFA/LArContent.git
        GIT_TAG origin/master
    )
    set(LArContent_DEPENDS PandoraSDK)
    if(PANDORA_MONITORING)
        list(APPEND LArContent_DEPENDS PandoraMonitoring)
    endif()
    list(APPEND LArContent_DEPENDS Eigen3)
    FetchContent_MakeAvailable(LArContent)
endif()

# LCContent
if(LC_PANDORA_CONTENT)
    FetchContent_Declare(
        LCContent
        GIT_REPOSITORY https://github.com/PandoraPFA/LCContent.git
        GIT_TAG origin/master
    )
    set(LCContent_DEPENDS PandoraSDK)
    if(PANDORA_MONITORING)
        list(APPEND LCContent_DEPENDS PandoraMonitoring)
    endif()
    FetchContent_MakeAvailable(LCContent)
endif()

# ExampleContent
if(EXAMPLE_PANDORA_CONTENT)
    FetchContent_Declare(
        ExampleContent
        GIT_REPOSITORY https://github.com/PandoraPFA/ExampleContent.git
        GIT_TAG origin/master
    )
    set(ExampleContent_DEPENDS PandoraSDK)
    if(PANDORA_MONITORING)
        list(APPEND ExampleContent_DEPENDS PandoraMonitoring)
    endif()
    FetchContent_MakeAvailable(ExampleContent)
endif()

# Install cmakemodules and changelog
install(DIRECTORY cmakemodules/ DESTINATION cmakemodules)
install(FILES ChangeLog.txt DESTINATION doc)

# Display variables
PANDORA_DISPLAY_STD_VARIABLES()

