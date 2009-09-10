/**
 *  @file   PandoraPFANew/include/BasicTypes.h
 * 
 *  @brief  Header file defining relevant typedefs, sort functions, etc
 * 
 *  $Log: $
 */
#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H 1

#include "StatusCodes.h"

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
class Track;
class MCParticle;

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
class PandoraType
{
public:
    /**
     *  @brief  Default constructor
     */
    PandoraType();

    /**
     *  @brief  Constructor
     *
     *  @param  t the initial value 
     */
    PandoraType(const T &t);

    /**
     *  @brief  Set the value held by the pandora type
     *
     *  @param  t the value
     */
    void Set(const T &t);
    
    /**
     *  @brief  Get the value held by the pandora type
     *
     *  @return the value
     */
    T Get() const;

    /**
     *  @brief  Reset the pandora type
     */   
    void Reset();

    /**
     *  @brief  Whether the pandora type is initialized
     *
     *  @return boolean
     */
    bool IsInitialized() const;

    /**
     *  @brief  Assignment operator for the pandora type
     * 
     *  @return whether assignment succeeded
     */   
    bool operator= (const T &rhs); 

private:
    T       m_value;            ///< The actual value being held by the pandora type
    bool    m_isInitialized;    ///< Whether the pandora type is initialized
};

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
PandoraType<T>::PandoraType() :
    m_isInitialized(false)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
PandoraType<T>::PandoraType(const T &t) :
    m_value(t),
    m_isInitialized(true)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
void PandoraType<T>::Set(const T &t)
{
    m_value = t;
    m_isInitialized = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
T PandoraType<T>::Get() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_value;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
void PandoraType<T>::Reset()
{
    m_isInitialized = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
bool PandoraType<T>::IsInitialized() const
{
    return m_isInitialized;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
bool PandoraType<T>::operator= (const T &rhs)
{
    this->Set(rhs);
    return m_isInitialized;
}

//------------------------------------------------------------------------------------------------------------------------------------------
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

// Container typedefs
typedef std::multiset<CaloHit *, PointerLess<CaloHit> > CaloHitList;
typedef std::multiset<Track *, PointerLess<Track> > TrackList;
typedef std::set<Cluster *> ClusterList;
typedef std::set<MCParticle *> MCParticleList;
typedef std::vector<CaloHit *> InputCaloHitList;
typedef std::vector<void *> CaloHitAddressList, TrackAddressList;
typedef std::vector<CaloHitAddressList> ClusterAddressList;

// Types used in api parameter classes
typedef PandoraType<unsigned int> UInt;
typedef PandoraType<int> Int;
typedef PandoraType<float> Float;
typedef PandoraType<void *> Address;

typedef unsigned int PseudoLayer;
typedef std::set<std::string> StringSet;
typedef std::vector<std::string> StringVector;

typedef const void * Uid;
typedef std::map<Uid, MCParticle *> UidToMCParticleMap;
typedef std::map<Uid, Uid> UidRelationMap;

} // namespace pandora

#endif // #ifndef BASIC_TYPES_H
