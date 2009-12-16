/**
 *  @file   PandoraPFANew/include/Helpers/XmlHelper.h
 * 
 *  @brief  Header file for the xml helper class.
 * 
 *  $Log: $
 */
#ifndef XML_HELPER_H
#define XML_HELPER_H 1

#include "Xml/tinyxml.h"

#include "Pandora/PandoraInternal.h"

namespace pandora
{

/**
 *  @brief  XmlHelper class
 */
class XmlHelper
{
public:
    /**
     *  @brief  Read a value from an xml element
     * 
     *  @param  xmlHandle the relevant xml handle
     *  @param  xmlElementName the name of the xml element to examine
     *  @param  t to receive the value
     */
    template <typename T>
    static StatusCode ReadValue(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, T &t);

    /**
     *  @brief  Read a vector of values from a (space separated) list in an xml element
     * 
     *  @param  xmlHandle the relevant xml handle
     *  @param  xmlElementName the name of the xml element to examine
     *  @param  vector to receive the vector of values
     */
    template <typename T>
    static StatusCode ReadVectorOfValues(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, std::vector<T> &vector);

    /**
     *  @brief  Process a list of daughter algorithms in an xml file
     * 
     *  @param  algorithm the parent algorithm calling this function
     *  @param  xmlHandle the relevant xml handle
     *  @param  listName the name of the algorithm list
     *  @param  algorithmNames to receive the names of the algorithm instances
     */
    static StatusCode ProcessAlgorithmList(const Algorithm &algorithm, const TiXmlHandle &xmlHandle, const std::string &listName,
        StringVector &algorithmNames);

    /**
     *  @brief  Process a single algorithm described in an xml file (the first found by the xml handle)
     * 
     *  @param  algorithm the parent algorithm calling this function
     *  @param  xmlHandle the relevant xml handle
     *  @param  algorithmName to receive the name of the algorithm instance
     */
    static StatusCode ProcessFirstAlgorithm(const Algorithm &algorithm, const TiXmlHandle &xmlHandle, std::string &algorithmName);

    /**
     *  @brief  Process a single algorithm from a list of daughter algorithms (the first found by the xml handle)
     * 
     *  @param  algorithm the parent algorithm calling this function
     *  @param  xmlHandle the relevant xml handle
     *  @param  listName the name of the algorithm list
     *  @param  algorithmName to receive the name of the algorithm instance
     */
    static StatusCode ProcessFirstAlgorithmInList(const Algorithm &algorithm, const TiXmlHandle &xmlHandle, const std::string &listName,
        std::string &algorithmName);

    /**
     *  @brief  Process an algorithm described in an xml element with a matching "description = ..." attribute
     * 
     *  @param  algorithm the parent algorithm calling this function
     *  @param  xmlHandle the relevant xml handle
     *  @param  description the description attribute of the algorithm xml element
     *  @param  algorithmName to receive the name of the algorithm instance
     */
    static StatusCode ProcessAlgorithm(const Algorithm &algorithm, const TiXmlHandle &xmlHandle, const std::string &description,
        std::string &algorithmName);

private:
    /**
     *  @brief  Tokenize a string
     * 
     *  @param  inputString the input string
     *  @param  tokens to receive the resulting tokens
     *  @param  delimiter the specified delimeter
     */
    static void TokenizeString(const std::string &inputString, StringVector &tokens, const std::string &delimiter = " ");
};

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline StatusCode XmlHelper::ReadValue(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, T &t)
{
    const TiXmlElement *const pXmlElement = xmlHandle.FirstChild(xmlElementName).Element();

    if (NULL == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

    if (!StringToType(pXmlElement->GetText(), t))
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

template <>
inline StatusCode XmlHelper::ReadValue<bool>(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, bool &t)
{
    const TiXmlElement *const pXmlElement = xmlHandle.FirstChild(xmlElementName).Element();

    if (NULL == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

    const std::string xmlElementString = pXmlElement->GetText();

    if ((xmlElementString == "1") || (xmlElementString == "true"))
    {
        t = true;
    }
    else if ((xmlElementString == "0") || (xmlElementString == "false"))
    {
        t = false;
    }
    else
    {
        return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline StatusCode XmlHelper::ReadVectorOfValues(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, std::vector<T> &vector)
{
    const TiXmlElement *const pXmlElement = xmlHandle.FirstChild(xmlElementName).Element();

    if (NULL == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

    StringVector tokens;
    TokenizeString(pXmlElement->GetText(), tokens);

    for (StringVector::const_iterator iter = tokens.begin(), iterEnd = tokens.end(); iter != iterEnd; ++iter)
    {
        T t;

        if (!StringToType(*iter, t))
            return STATUS_CODE_FAILURE;

        vector.push_back(t);
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef XML_HELPER_H
