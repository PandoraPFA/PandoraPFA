/**
 *	@file	PandoraPFANew/src/Test/TestPandora.cc
 * 
 * 	@brief	Implementation of the tester for the mc manager class.
 * 
 *	$Log: $
 */

#include "Pandora/Pandora.h"

#include "Test/TestPandora.h"

#include <assert.h>
#include <iostream>

namespace pandora
{


StatusCode TestPandora::Test_All()
{
    std::cout << "--- --- ALL | START ------------------------------" << std::endl;
    std::cout << "--- --- ALL | END ------------------------------" << std::endl;
    return STATUS_CODE_SUCCESS;
}



} // namespace pandora
