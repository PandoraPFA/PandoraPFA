/**
 *	@file	PandoraPFANew/include/Test/TestPandora.h
 * 
 *	@brief	Header file for the Pandora test-class.
 * 
 *	$Log: $
 */
#ifndef TEST_PANDORA_H
#define TEST_PANDORA_H 1

#include "Pandora/PandoraInternal.h"

#include "StatusCodes.h"

#include <assert.h>
#include <iostream>
#include <iomanip>

namespace pandora
{

/**
 *	@brief TestPandora class
 */
class TestPandora
{
public:
    /**
     *  @brief  Run all tests
     */
    StatusCode Test_All();

};

} // namespace pandora

#endif // #ifndef TEST_MC_MANAGER_H
