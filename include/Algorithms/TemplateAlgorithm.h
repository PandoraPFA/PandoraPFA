/**
 *  @file   PandoraPFANew/include/Algorithms/TemplateAlgorithm.h
 * 
 *  @brief  Header file for the template algorithm class.
 * 
 *  $Log: $
 */
#ifndef TEMPLATE_ALGORITHM_H
#define TEMPLATE_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  TemplateAlgorithm class
 */
class TemplateAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        Algorithm *CreateAlgorithm() const;
    };

private:
    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    // Member variables here
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *TemplateAlgorithm::Factory::CreateAlgorithm() const
{
    return new TemplateAlgorithm();
}

#endif // #ifndef TEMPLATE_ALGORITHM_H
