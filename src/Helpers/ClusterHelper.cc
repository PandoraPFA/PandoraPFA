/**
 *  @file   PandoraPFANew/src/Helpers/ClusterHelper.cc
 * 
 *  @brief  Implementation of the cluster helper class.
 * 
 *  $Log: $
 */

#include "Helpers/ClusterHelper.h"
#include "Helpers/GeometryHelper.h"

#include "Objects/CaloHit.h"

#include <cmath>

namespace pandora
{

StatusCode ClusterHelper::FitPoints(const ClusterFitPointList &clusterFitPointList, ClusterFitResult &clusterFitResult)
{
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

ClusterHelper::ClusterFitPoint::ClusterFitPoint(const CaloHit *const pCaloHit) :
    m_position(pCaloHit->GetPositionVector()),
    m_cellThickness(pCaloHit->GetCellThickness()),
    m_pseudoLayer(pCaloHit->GetPseudoLayer())
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

ClusterHelper::ClusterFitPoint::ClusterFitPoint(const CartesianVector &position, float cellThickness, PseudoLayer pseudoLayer) :
    m_position(position),
    m_cellThickness(cellThickness),
    m_pseudoLayer(pseudoLayer)
{
}

} // namespace pandora
