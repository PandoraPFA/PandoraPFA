/**
 *  @file   PandoraPFANew/src/Objects/MCParticle.cc
 * 
 *  @brief  Implementation of the mc particle class.
 * 
 *  $Log: $
 */

//#include <iomanip.h>
#include <iostream>
#include <iomanip>

#include "Objects/MCParticle.h"

namespace pandora
{

MCParticle::MCParticle(const PandoraApi::MCParticleParameters &mcParticleParameters) :
    m_uid(mcParticleParameters.m_pParentAddress.Get()),
    m_energy(mcParticleParameters.m_energy.Get()),
    m_momentum(mcParticleParameters.m_momentum.Get()),
    m_innerRadius(mcParticleParameters.m_innerRadius.Get()),
    m_outerRadius(mcParticleParameters.m_outerRadius.Get()),
    m_particleId(mcParticleParameters.m_particleId.Get()),
    m_pPfoTarget(NULL),
    m_isInitialized(true)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

MCParticle::MCParticle(const Uid uid) :
    m_uid(uid),
    m_pPfoTarget(NULL),
    m_isInitialized(false)
{
}


//------------------------------------------------------------------------------------------------------------------------------------------

MCParticle::~MCParticle()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void MCParticle::SetProperties(const PandoraApi::MCParticleParameters &mcParticleParameters)
{
    m_energy = mcParticleParameters.m_energy.Get();
    m_momentum = mcParticleParameters.m_momentum.Get();
    m_innerRadius = mcParticleParameters.m_innerRadius.Get();
    m_outerRadius = mcParticleParameters.m_outerRadius.Get();
    m_particleId = mcParticleParameters.m_particleId.Get();

    m_isInitialized = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCParticle::SetPfoTargetInTree(MCParticle* mcParticle, bool onlyDaughters)
{
    if (this->IsPfoTargetSet())
        return STATUS_CODE_SUCCESS;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SetPfoTarget(mcParticle));

    for (MCParticleList::iterator iter = m_daughterList.begin(), iterEnd = m_daughterList.end(); iter != iterEnd; ++iter)
    {
       PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, (*iter)->SetPfoTargetInTree(mcParticle));
    }

    if(!onlyDaughters)
    {
        for (MCParticleList::iterator iter = m_parentList.begin(), iterEnd = m_parentList.end(); iter != iterEnd; ++iter)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, (*iter)->SetPfoTargetInTree(mcParticle));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void MCParticle::Print( std::ostream & o ) const
{
//    static const char* none         = "" ;
//    static const char* white        = "\033[1;37m";  // white
//    static const char* black        = "\033[30m";    // black
//    static const char* blue         = "\033[34m";    // blue
//    static const char* red          = "\033[1;31m" ; // red
//    static const char* yellow       = "\033[1;33m";  // yellow
//    static const char* darkred      = "\033[31m";    // dark red
   static const char* darkgreen    = "\033[32m";    // dark green
//    static const char* darkyellow   = "\033[33m";    // dark yellow
                                    
//    static const char* bold         = "\033[1m"    ; // bold 
//    static const char* black_b      = "\033[30m"   ; // bold black
//    static const char* lblue_b      = "\033[1;34m" ; // bold light blue
//    static const char* cyan_b       = "\033[0;36m" ; // bold cyan
//    static const char* lgreen_b     = "\033[1;32m";  // bold light green
                                    
//    static const char* blue_bg      = "\033[44m";    // blue background
   static const char* red_bg       = "\033[1;41m";  // white on red background
//    static const char* whiteonblue  = "\033[1;44m";  // white on blue background
    static const char* whiteongreen = "\033[1;42m";  // white on green background
//    static const char* grey_bg      = "\033[47m";    // grey background

   static const char* reset  = "\033[0m";     // reset

   if( IsRootParticle() )
   {
      o << whiteongreen << "/ROOT/" << reset;
   }
   if( IsPfoTarget() )
   {
      o << red_bg << darkgreen << "|PFO|";
   }
   o << "[" << this << "]"
     << " E=" << m_energy 
     << " p=" << m_momentum
     << " pid=" << m_particleId
     << " r_i=" << m_innerRadius
     << " r_o=" << m_outerRadius
     << " uid=" << GetUid();
   if( IsPfoTarget() )
   {
      o << "|PFO|" << reset;
   }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void MCParticle::Print( std::ostream & o, int depth, int maxDepthAfterPFOTarget, const MCParticle* parent, int stepSize  ) const
{
   static const char* whiteonblue  = "\033[1;44m";  // white on blue background
   static const char* reset  = "\033[0m";     // reset

   int printDepth = depth*stepSize + (int)(m_outerRadius/5); // this can be changed if the printout doesn't look good
   o << std::setw (printDepth) << " ";
   Print( o );
   o << std::endl;

   // if there are other parents than the calling one
   for( MCParticleList::const_iterator itParent = m_parentList.begin(), itParentEnd = m_parentList.end(); itParent != itParentEnd; itParent++ )
   {
      if( (*itParent) != parent )
      {
	 o << std::setw(printDepth) << " " << whiteonblue << "PARENT=" << (*itParent) << reset << std::endl;
      }
   }

   if( maxDepthAfterPFOTarget < 0 )
   {
      maxDepthAfterPFOTarget++;
   }
   if( this->IsPfoTarget() && (maxDepthAfterPFOTarget >= 0) ){
      maxDepthAfterPFOTarget = -maxDepthAfterPFOTarget -1;
   }
   if( maxDepthAfterPFOTarget != -1 ){
      // descend into the daughters and let them print themselves
      for( MCParticleList::const_iterator itDaughter = m_daughterList.begin(), itDaughterEnd = m_daughterList.end(); itDaughter != itDaughterEnd; itDaughter++ )
      {
	 (*itDaughter)->Print( o, depth+1, maxDepthAfterPFOTarget, this, stepSize );
      }
   }
}


} // namespace pandora
