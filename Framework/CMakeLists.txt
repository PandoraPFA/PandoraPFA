########################################################
# cmake file for building Pandora Framework
########################################################

# project name
PROJECT( Framework )

# project version
SET( Framework_MAJOR_VERSION 1 )
SET( Framework_MINOR_VERSION 0 )
SET( Framework_PATCH_LEVEL 0 )

# library *nix style versioning
SET( Framework_SOVERSION "${Framework_MAJOR_VERSION}.${Framework_MINOR_VERSION}" )
SET( Framework_VERSION "${Framework_SOVERSION}.${Framework_PATCH_LEVEL}" )

INCLUDE_DIRECTORIES( ${Framework_SOURCE_DIR}/include )

AUX_SOURCE_DIRECTORY( src Framework_SRCS )
AUX_SOURCE_DIRECTORY( src/Api Framework_SRCS )
AUX_SOURCE_DIRECTORY( src/Helpers Framework_SRCS )
AUX_SOURCE_DIRECTORY( src/Managers Framework_SRCS )
AUX_SOURCE_DIRECTORY( src/Objects Framework_SRCS )
AUX_SOURCE_DIRECTORY( src/Pandora Framework_SRCS )
AUX_SOURCE_DIRECTORY( src/Utilities Framework_SRCS )
AUX_SOURCE_DIRECTORY( src/Xml Framework_SRCS )

# require proper C++
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

# need long long for int64 for now
ADD_DEFINITIONS( "-Wno-long-long -Wno-sign-compare -fno-strict-aliasing" )

ADD_LIBRARY( lib_PandoraFramework ${Framework_SRCS} )

SET_TARGET_PROPERTIES( lib_PandoraFramework PROPERTIES
    # create *nix style library versions + symbolic links
    VERSION ${Framework_VERSION}
    SOVERSION ${Framework_SOVERSION}
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
