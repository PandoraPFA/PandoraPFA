/**
 *  @file   PandoraPFANew/include/Typedefs.h
 * 
 *  @brief  Header file defining relevant typedefs, sort functions, etc
 * 
 *  $Log: $
 */
#ifndef BASIC_TYPEDEFS_H
#define BASIC_TYPEDEFS_H 1

#include "PandoraTypes.h"

#include <map>
#include <set>
#include <vector>

namespace pandora
{

class CaloHit;
class Cluster;
class OrderedCaloHitList;
class MCParticle;
class Track;
class TrackState;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Enable ordering of pointers based on properties of target objects
 */
template <typename T>
class PointerLess
{
public:
    bool operator()(const T *lhs, const T *rhs) const;
};

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline bool PointerLess<T>::operator()(const T *lhs, const T *rhs) const
{
    return (*lhs < *rhs);
}

//------------------------------------------------------------------------------------------------------------------------------------------

typedef std::multiset<CaloHit *, PointerLess<CaloHit> > CaloHitList;
typedef std::multiset<Track *, PointerLess<Track> > TrackList;
typedef std::set<Cluster *> ClusterList;
typedef std::set<MCParticle *> MCParticleList;
typedef std::vector<CaloHit *> InputCaloHitList;
typedef std::vector<TrackState *> TrackStateList;

typedef std::vector<const void *> CaloHitAddressList, TrackAddressList;
typedef std::vector<CaloHitAddressList> ClusterAddressList;

typedef unsigned int PseudoLayer;
typedef std::set<std::string> StringSet;
typedef std::vector<std::string> StringVector;

typedef const void * Uid;
typedef std::map<Uid, MCParticle *> UidToMCParticleMap;
typedef std::map<Uid, Uid> UidRelationMap;

} // namespace pandora

#endif // #ifndef BASIC_TYPEDEFS_H
