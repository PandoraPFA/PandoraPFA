########################################################
# cmake file for building PandoraPFANew KMeansClustering
########################################################

# project name
PROJECT( PandoraKMeansClustering )

# project version
SET( PandoraKMeansClustering_MAJOR_VERSION 1 )
SET( PandoraKMeansClustering_MINOR_VERSION 0 )
SET( PandoraKMeansClustering_PATCH_LEVEL 0 )

# library *nix style versioning
SET( PandoraKMeansClustering_SOVERSION "${PandoraKMeansClustering_MAJOR_VERSION}.${PandoraKMeansClustering_MINOR_VERSION}" )
SET( PandoraKMeansClustering_VERSION "${PandoraKMeansClustering_SOVERSION}.${PandoraKMeansClustering_PATCH_LEVEL}" )

INCLUDE_DIRECTORIES( ${PandoraKMeansClustering_SOURCE_DIR}/include )
INCLUDE_DIRECTORIES( ${PandoraFramework_SOURCE_DIR}/include )

AUX_SOURCE_DIRECTORY( src/ PandoraKMeansClustering_SRCS )

# require proper C++
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

# need long long for int64 for now
ADD_DEFINITIONS( "-Wno-long-long -Wno-sign-compare -fno-strict-aliasing" )

ADD_LIBRARY( lib_PandoraKMeansClustering ${PandoraKMeansClustering_SRCS} )

TARGET_LINK_LIBRARIES( lib_PandoraKMeansClustering lib_PandoraFramework )

SET_TARGET_PROPERTIES( lib_PandoraKMeansClustering PROPERTIES
    # create *nix style library versions + symbolic links
    VERSION ${PandoraKMeansClustering_VERSION}
    SOVERSION ${PandoraKMeansClustering_SOVERSION}
    # allow creating static and shared libs without conflicts
    CLEAN_DIRECT_OUTPUT 1
    # avoid conflicts between library and binary target names
    OUTPUT_NAME PandoraKMeansClustering
)

# install library
INSTALL( TARGETS lib_PandoraKMeansClustering DESTINATION lib PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE )
