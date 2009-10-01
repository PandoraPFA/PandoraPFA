/**
 *  @file   PandoraPFANew/include/Pandora/PandoraInternal.h
 * 
 *  @brief  Header file defining relevant internal typedefs, sort and string conversion functions
 * 
 *  $Log: $
 */
#ifndef PANDORA_INTERNAL_H
#define PANDORA_INTERNAL_H 1

#include <iostream>
#include <map>
#include <set>
#include <sstream>
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

template <class T>
inline bool StringToType(const std::string &s, T &t)
{
    std::istringstream iss(s);
    return !(iss >> t).fail();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template <class T>
inline std::string TypeToString(const T &t)
{
    std::ostringstream oss;

    if ((oss << t).fail())
        throw;

    return oss.str();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Enable ordering of pointers based on properties of target objects
 */
template <typename T>
class PointerLessThan
{
public:
    bool operator()(const T *lhs, const T *rhs) const;
};

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline bool PointerLessThan<T>::operator()(const T *lhs, const T *rhs) const
{
    return (*lhs < *rhs);
}

//------------------------------------------------------------------------------------------------------------------------------------------

typedef std::set<CaloHit *> CaloHitList;
typedef std::set<Track *> TrackList;
typedef std::set<Cluster *> ClusterList;
typedef std::set<MCParticle *> MCParticleList;
typedef std::vector<TrackState *> TrackStateList;
typedef std::vector<CaloHit *> CaloHitVector;

typedef std::set<CaloHit *, PointerLessThan<CaloHit> > EnergySortedCaloHitList;
typedef std::set<Track *, PointerLessThan<Track> > MomentumSortedTrackList;

typedef unsigned int PseudoLayer;
typedef std::set<std::string> StringSet;
typedef std::vector<std::string> StringVector;

typedef const void * Uid;
typedef std::map<Uid, MCParticle *> UidToMCParticleMap;

} // namespace pandora

#endif // #ifndef PANDORA_INTERNAL_H
