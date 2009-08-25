/**
 *	@file	PandoraPFANew/include/BasicTypes.h
 * 
 *	@brief	Header file defining relevant typedefs, sort functions, etc
 * 
 *	$Log: $
 */
#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H 1

#include <iostream>
#include <map>
#include <set>
#include <vector>

namespace pandora
{

class CaloHit;
class Cluster;
class OrderedCaloHitList;
class Track;
class MCParticle;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *	@brief	Enable ordering of pointers based on properties of target objects
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
//------------------------------------------------------------------------------------------------------------------------------------------

// Containers exposed via content api - name as lists
typedef std::multiset<CaloHit *, PointerLess<CaloHit> > CaloHitList;
typedef std::multiset<Track *, PointerLess<Track> > TrackList;
typedef std::set<Cluster *> ClusterList;
typedef std::set<MCParticle *> MCParticleList;
typedef std::vector<CaloHit *> InputCaloHitList;

// Containers exposed via standard api - make container type explicit
typedef std::vector<void *> CaloHitAddressVector, TrackAddressVector;
typedef std::vector<CaloHitAddressVector> ClusterAddressVector;

typedef unsigned int PseudoLayer;
typedef std::set<std::string> StringSet;

typedef const void * Uid;
typedef std::map<Uid, MCParticle *> UidToMCParticleMap;
typedef std::map<Uid, Uid> UidRelationMap;

} // namespace pandora

#endif // #ifndef BASIC_TYPES_H
