/**
 *  @file   PandoraPFANew/src/Xml/XmlHelper.cc
 * 
 *  @brief  Implementation of the xml helper class.
 * 
 *  $Log: $
 */
 
#include "Xml/XmlHelper.h"

namespace pandora
{

template <typename T>
StatusCode XmlHelper::ReadValue(TiXmlHandle &xmlHandle, const std::string &xmlElementName, T &t)
{
    

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode XmlHelper::ReadVectorOfValues(TiXmlHandle &xmlHandle, const std::string &xmlElementName, std::vector<T> &vector)
{
    

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlHelper::ProcessAlgorithmList(TiXmlHandle &xmlHandle, const std::string &listName, StringVector &algorithmNames)
{
    

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlHelper::ProcessFirstAlgorithm(TiXmlHandle &xmlHandle, std::string &algorithmName)
{
    

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlHelper::ProcessFirstAlgorithmInList(TiXmlHandle &xmlHandle, const std::string &listName, std::string &algorithmName)
{
    

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlHelper::ProcessAlgorithm(TiXmlHandle &xmlHandle, const std::string &description, std::string &algorithmName)
{
    

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
