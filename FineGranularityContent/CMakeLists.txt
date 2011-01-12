########################################################
# cmake file for building PandoraPFANew Algorithms
########################################################

# project name
PROJECT( PandoraFineGranularityContent )

# project version
SET( PandoraFineGranularityContent_MAJOR_VERSION 1 )
SET( PandoraFineGranularityContent_MINOR_VERSION 0 )
SET( PandoraFineGranularityContent_PATCH_LEVEL 0 )

# library *nix style versioning
SET( PandoraFineGranularityContent_SOVERSION "${PandoraFineGranularityContent_MAJOR_VERSION}.${PandoraFineGranularityContent_MINOR_VERSION}" )
SET( PandoraFineGranularityContent_VERSION "${PandoraFineGranularityContent_SOVERSION}.${PandoraFineGranularityContent_PATCH_LEVEL}" )

INCLUDE_DIRECTORIES( ${PandoraFineGranularityContent_SOURCE_DIR}/include )
INCLUDE_DIRECTORIES( ${PandoraFramework_SOURCE_DIR}/include )

AUX_SOURCE_DIRECTORY( src/ PandoraFineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( src/Cheating PandoraFineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( src/Clustering PandoraFineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( src/FragmentRemoval PandoraFineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( src/Monitoring PandoraFineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( src/PfoConstruction PandoraFineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( src/Reclustering PandoraFineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( src/TopologicalAssociation PandoraFineGranularityContent_SRCS )

# require proper C++
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

# need long long for int64 for now
ADD_DEFINITIONS( "-Wno-long-long -Wno-sign-compare -fno-strict-aliasing" )

ADD_LIBRARY( lib_PandoraFineGranularityContent ${PandoraFineGranularityContent_SRCS} )

TARGET_LINK_LIBRARIES( lib_PandoraFineGranularityContent lib_PandoraFramework )

SET_TARGET_PROPERTIES( lib_PandoraFineGranularityContent PROPERTIES
    # create *nix style library versions + symbolic links
    VERSION ${PandoraFineGranularityContent_VERSION}
    SOVERSION ${PandoraFineGranularityContent_SOVERSION}
    # allow creating static and shared libs without conflicts
    CLEAN_DIRECT_OUTPUT 1
    # avoid conflicts between library and binary target names
    OUTPUT_NAME PandoraFineGranularityContent
)

# install library
INSTALL( TARGETS lib_PandoraFineGranularityContent DESTINATION lib PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE )
