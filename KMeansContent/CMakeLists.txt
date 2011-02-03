########################################################
# cmake file for building Pandora KMeansContent
########################################################

INCLUDE_DIRECTORIES( ./include )
INCLUDE_DIRECTORIES( ../Framework/include )

AUX_SOURCE_DIRECTORY( ./src KMeansContent_SRCS )

# require proper C++
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

# need long long for int64 for now
ADD_DEFINITIONS( "-Wno-long-long -Wno-sign-compare -fno-strict-aliasing" )


# add library
ADD_SHARED_LIBRARY( PandoraKMeansContent ${KMeansContent_SRCS} )

TARGET_LINK_LIBRARIES( PandoraKMeansContent PandoraFramework )

# install library
INSTALL_SHARED_LIBRARY( PandoraKMeansContent DESTINATION lib )

# install header files
INSTALL_DIRECTORY( ./include DESTINATION . )
