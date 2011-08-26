########################################################
# cmake file for building PandoraMonitoring
########################################################

# PandoraMonitoring requires ROOT
FIND_PACKAGE( ROOT 5.26.00 REQUIRED COMPONENTS Eve Geom RGL EG ) # minimum required ROOT version
INCLUDE_DIRECTORIES( ${ROOT_INCLUDE_DIRS} )
LINK_LIBRARIES( ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES} )


### LIBRARY #################################################################

# definitions to pass to the compiler
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )
ADD_DEFINITIONS( "-Wno-long-long" )

# Add additional linker flag for Apple to defer linking against ROOT until PandoraPFANew build.
IF( APPLE ) 
    SET( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup" )
    # Add Unix compiler definition.  Already happens automatically on Linux.
    IF( UNIX )
        ADD_DEFINITIONS(-D__unix__)
    ENDIF()
ENDIF()


# include directories
INCLUDE_DIRECTORIES( ./include )
INCLUDE_DIRECTORIES( "../Framework/include" )
#INSTALL_DIRECTORY( ./include DESTINATION . FILES_MATCHING PATTERN "*.h" )

# add library
AUX_SOURCE_DIRECTORY( ./src library_sources )
ADD_SHARED_LIBRARY( PandoraMonitoring ${library_sources} )
INSTALL_SHARED_LIBRARY( PandoraMonitoring DESTINATION lib )

