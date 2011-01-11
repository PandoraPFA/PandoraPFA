########################################################
# cmake file for building PandoraPFANew Algorithms
########################################################

# project name
PROJECT( PandoraAlgorithms )

# project version
SET( PandoraAlgorithms_MAJOR_VERSION 1 )
SET( PandoraAlgorithms_MINOR_VERSION 0 )
SET( PandoraAlgorithms_PATCH_LEVEL 0 )

# library *nix style versioning
SET( PandoraAlgorithms_SOVERSION "${PandoraAlgorithms_MAJOR_VERSION}.${PandoraAlgorithms_MINOR_VERSION}" )
SET( PandoraAlgorithms_VERSION "${PandoraAlgorithms_SOVERSION}.${PandoraAlgorithms_PATCH_LEVEL}" )

INCLUDE_DIRECTORIES( ${PandoraAlgorithms_SOURCE_DIR}/include )
INCLUDE_DIRECTORIES( ${PandoraFramework_SOURCE_DIR}/include )

AUX_SOURCE_DIRECTORY( src/ PandoraAlgorithms_SRCS )
AUX_SOURCE_DIRECTORY( src/Cheating PandoraAlgorithms_SRCS )
AUX_SOURCE_DIRECTORY( src/Clustering PandoraAlgorithms_SRCS )
AUX_SOURCE_DIRECTORY( src/FragmentRemoval PandoraAlgorithms_SRCS )
AUX_SOURCE_DIRECTORY( src/Monitoring PandoraAlgorithms_SRCS )
AUX_SOURCE_DIRECTORY( src/PfoConstruction PandoraAlgorithms_SRCS )
AUX_SOURCE_DIRECTORY( src/Reclustering PandoraAlgorithms_SRCS )
AUX_SOURCE_DIRECTORY( src/TopologicalAssociation PandoraAlgorithms_SRCS )

# require proper C++
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

# need long long for int64 for now
ADD_DEFINITIONS( "-Wno-long-long -Wno-sign-compare -fno-strict-aliasing" )

ADD_LIBRARY( lib_PandoraAlgorithms ${PandoraAlgorithms_SRCS} )

TARGET_LINK_LIBRARIES( lib_PandoraAlgorithms lib_PandoraFramework )

SET_TARGET_PROPERTIES( lib_PandoraAlgorithms PROPERTIES
    # create *nix style library versions + symbolic links
    VERSION ${PandoraAlgorithms_VERSION}
    SOVERSION ${PandoraAlgorithms_SOVERSION}
    # allow creating static and shared libs without conflicts
    CLEAN_DIRECT_OUTPUT 1
    # avoid conflicts between library and binary target names
    OUTPUT_NAME PandoraAlgorithms
)

# install library
INSTALL( TARGETS lib_PandoraAlgorithms DESTINATION lib PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE )
