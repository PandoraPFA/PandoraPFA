########################################################
# cmake file for building Pandora KMeansContent
########################################################

# project name
PROJECT( KMeansContent )

# project version
SET( KMeansContent_MAJOR_VERSION 1 )
SET( KMeansContent_MINOR_VERSION 0 )
SET( KMeansContent_PATCH_LEVEL 0 )

# library *nix style versioning
SET( KMeansContent_SOVERSION "${KMeansContent_MAJOR_VERSION}.${KMeansContent_MINOR_VERSION}" )
SET( KMeansContent_VERSION "${KMeansContent_SOVERSION}.${KMeansContent_PATCH_LEVEL}" )

INCLUDE_DIRECTORIES( ${KMeansContent_SOURCE_DIR}/include )
INCLUDE_DIRECTORIES( ${Framework_SOURCE_DIR}/include )

AUX_SOURCE_DIRECTORY( src/ KMeansContent_SRCS )

# require proper C++
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

# need long long for int64 for now
ADD_DEFINITIONS( "-Wno-long-long -Wno-sign-compare -fno-strict-aliasing" )

ADD_LIBRARY( lib_PandoraKMeansContent ${KMeansContent_SRCS} )

TARGET_LINK_LIBRARIES( lib_PandoraKMeansContent lib_PandoraFramework )

SET_TARGET_PROPERTIES( lib_PandoraKMeansContent PROPERTIES
    # create *nix style library versions + symbolic links
    VERSION ${KMeansContent_VERSION}
    SOVERSION ${KMeansContent_SOVERSION}
    # allow creating static and shared libs without conflicts
    CLEAN_DIRECT_OUTPUT 1
    # avoid conflicts between library and binary target names
    OUTPUT_NAME PandoraKMeansContent
)

# install library
INSTALL( TARGETS lib_PandoraKMeansContent DESTINATION lib PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE )
