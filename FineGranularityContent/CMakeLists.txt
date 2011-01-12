########################################################
# cmake file for building Pandora FineGranularityContent
########################################################

# project name
PROJECT( FineGranularityContent )

# project version
SET( FineGranularityContent_MAJOR_VERSION 1 )
SET( FineGranularityContent_MINOR_VERSION 0 )
SET( FineGranularityContent_PATCH_LEVEL 0 )

# library *nix style versioning
SET( FineGranularityContent_SOVERSION "${FineGranularityContent_MAJOR_VERSION}.${FineGranularityContent_MINOR_VERSION}" )
SET( FineGranularityContent_VERSION "${FineGranularityContent_SOVERSION}.${FineGranularityContent_PATCH_LEVEL}" )

INCLUDE_DIRECTORIES( ${FineGranularityContent_SOURCE_DIR}/include )
INCLUDE_DIRECTORIES( ${Framework_SOURCE_DIR}/include )

AUX_SOURCE_DIRECTORY( src/ FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( src/Cheating FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( src/Clustering FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( src/FragmentRemoval FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( src/Monitoring FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( src/PfoConstruction FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( src/Reclustering FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( src/TopologicalAssociation FineGranularityContent_SRCS )

# require proper C++
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

# need long long for int64 for now
ADD_DEFINITIONS( "-Wno-long-long -Wno-sign-compare -fno-strict-aliasing" )

ADD_LIBRARY( lib_PandoraFineGranularityContent ${FineGranularityContent_SRCS} )

TARGET_LINK_LIBRARIES( lib_PandoraFineGranularityContent lib_PandoraFramework )

SET_TARGET_PROPERTIES( lib_PandoraFineGranularityContent PROPERTIES
    # create *nix style library versions + symbolic links
    VERSION ${FineGranularityContent_VERSION}
    SOVERSION ${FineGranularityContent_SOVERSION}
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
