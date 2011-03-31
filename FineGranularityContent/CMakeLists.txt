########################################################
# cmake file for building Pandora FineGranularityContent
########################################################

INCLUDE_DIRECTORIES( ./include )
INCLUDE_DIRECTORIES( ../Framework/include )

AUX_SOURCE_DIRECTORY( ./src/ FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Cheating FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Clustering FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/FragmentRemoval FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Monitoring FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/ParticleId FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Persistency FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/PfoConstruction FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Reclustering FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/TopologicalAssociation FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/TrackClusterAssociation FineGranularityContent_SRCS )
AUX_SOURCE_DIRECTORY( ./src/Utility FineGranularityContent_SRCS )

# require proper C++
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

# need long long for int64 for now
ADD_DEFINITIONS( "-Wno-long-long -Wno-sign-compare -fno-strict-aliasing" )


# add library
ADD_SHARED_LIBRARY( PandoraFineGranularityContent ${FineGranularityContent_SRCS} )
TARGET_LINK_LIBRARIES( PandoraFineGranularityContent PandoraFramework )

# install library
INSTALL_SHARED_LIBRARY( PandoraFineGranularityContent DESTINATION lib )

# install header files
INSTALL_DIRECTORY( ./include DESTINATION . )
