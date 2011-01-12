########################################################
# cmake file for building PandoraPFANew KMeansClustering
########################################################

# project name
PROJECT( PandoraKMeansContent )

# project version
SET( PandoraKMeansContent_MAJOR_VERSION 1 )
SET( PandoraKMeansContent_MINOR_VERSION 0 )
SET( PandoraKMeansContent_PATCH_LEVEL 0 )

# library *nix style versioning
SET( PandoraKMeansContent_SOVERSION "${PandoraKMeansContent_MAJOR_VERSION}.${PandoraKMeansContent_MINOR_VERSION}" )
SET( PandoraKMeansContent_VERSION "${PandoraKMeansContent_SOVERSION}.${PandoraKMeansContent_PATCH_LEVEL}" )

INCLUDE_DIRECTORIES( ${PandoraKMeansContent_SOURCE_DIR}/include )
INCLUDE_DIRECTORIES( ${PandoraFramework_SOURCE_DIR}/include )

AUX_SOURCE_DIRECTORY( src/ PandoraKMeansContent_SRCS )

# require proper C++
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

# need long long for int64 for now
ADD_DEFINITIONS( "-Wno-long-long -Wno-sign-compare -fno-strict-aliasing" )

ADD_LIBRARY( lib_PandoraKMeansContent ${PandoraKMeansContent_SRCS} )

TARGET_LINK_LIBRARIES( lib_PandoraKMeansContent lib_PandoraFramework )

SET_TARGET_PROPERTIES( lib_PandoraKMeansContent PROPERTIES
    # create *nix style library versions + symbolic links
    VERSION ${PandoraKMeansContent_VERSION}
    SOVERSION ${PandoraKMeansContent_SOVERSION}
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
