########################################################
# cmake file for building PandoraPFANew Framework
########################################################

# project name
PROJECT( PandoraFramework )

# project version
SET( PandoraFramework_MAJOR_VERSION 1 )
SET( PandoraFramework_MINOR_VERSION 0 )
SET( PandoraFramework_PATCH_LEVEL 0 )

# library *nix style versioning
SET( PandoraFramework_SOVERSION "${PandoraFramework_MAJOR_VERSION}.${PandoraFramework_MINOR_VERSION}" )
SET( PandoraFramework_VERSION "${PandoraFramework_SOVERSION}.${PandoraFramework_PATCH_LEVEL}" )

INCLUDE_DIRECTORIES( ${PandoraFramework_SOURCE_DIR}/include )

AUX_SOURCE_DIRECTORY( src PandoraFramework_SRCS )
AUX_SOURCE_DIRECTORY( src/Api PandoraFramework_SRCS )
AUX_SOURCE_DIRECTORY( src/Helpers PandoraFramework_SRCS )
AUX_SOURCE_DIRECTORY( src/Managers PandoraFramework_SRCS )
AUX_SOURCE_DIRECTORY( src/Objects PandoraFramework_SRCS )
AUX_SOURCE_DIRECTORY( src/Pandora PandoraFramework_SRCS )
AUX_SOURCE_DIRECTORY( src/Utilities PandoraFramework_SRCS )
AUX_SOURCE_DIRECTORY( src/Xml PandoraFramework_SRCS )

# require proper C++
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

# need long long for int64 for now
ADD_DEFINITIONS( "-Wno-long-long -Wno-sign-compare -fno-strict-aliasing" )

ADD_LIBRARY( lib_PandoraFramework ${PandoraFramework_SRCS} )

SET_TARGET_PROPERTIES( lib_PandoraFramework PROPERTIES
    # create *nix style library versions + symbolic links
    VERSION ${PandoraFramework_VERSION}
    SOVERSION ${PandoraFramework_SOVERSION}
    # allow creating static and shared libs without conflicts
    CLEAN_DIRECT_OUTPUT 1
    # avoid conflicts between library and binary target names
    OUTPUT_NAME PandoraFramework
)

# install library
INSTALL( TARGETS lib_PandoraFramework DESTINATION lib PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE )
