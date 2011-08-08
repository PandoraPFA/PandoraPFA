/**
 *  @file   PandoraPFANew/KMeansContent/src/KMeansClusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the k-means clustering algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "KMeansClusteringAlgorithm.h"
#include "KMlocal.h"

#include <cstdlib>

using namespace pandora;

StatusCode KMeansClusteringAlgorithm::Run()
{
    const CaloHitList *pInputCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentCaloHitList(*this, pInputCaloHitList));

    // Make list of available hits
    CaloHitList caloHitList;

    for (CaloHitList::const_iterator hitIter = pInputCaloHitList->begin(), hitIterEnd = pInputCaloHitList->end(); hitIter != hitIterEnd; ++hitIter)
    {
        if ((m_shouldUseIsolatedHits || !(*hitIter)->IsIsolated()) && PandoraContentApi::IsCaloHitAvailable(*this, *hitIter))
            caloHitList.insert(*hitIter);
    }

    if (caloHitList.empty())
        return STATUS_CODE_SUCCESS;

    const KMterm term(100, 0, 0, 0, 0.10, 0.10, 3, 0.50, 10, 0.95); // Run for 100 stages
    const int k(static_cast<int>(m_nMeans));
    const int dim(3);
    const int nPts(caloHitList.size());

    KMdata dataPts(dim, nPts);
    KMpointArray pointArray(dataPts.getPts());

    int hitIndex(0);

    for (CaloHitList::const_iterator iter = caloHitList.begin(), iterEnd = caloHitList.end(); iter != iterEnd; ++iter)
    {
        if (hitIndex >= nPts)
            break;

        const CartesianVector &positionVector((*iter)->GetPositionVector());
        pointArray[hitIndex][0] = positionVector.GetX();
        pointArray[hitIndex][1] = positionVector.GetY();
        pointArray[hitIndex][2] = positionVector.GetZ();

        ++hitIndex;
    }

    dataPts.buildKcTree();
    KMfilterCenters ctrs(k, dataPts);

    // KMlocalLloyds    kmAlg(ctrs, term);       // repeated Lloyd's
    // KMlocalSwap      kmAlg(ctrs, term);       // Swap heuristic
    // KMlocalEZ_Hybrid kmAlg(ctrs, term);       // EZ-Hybrid heuristic
    KMlocalHybrid kmAlg(ctrs, term);             // Hybrid heuristic
    ctrs = kmAlg.execute();

    // Print assignments
    // std::cout << "Number of stages: " << kmAlg.getTotalStages() << "\n";
    // std::cout << "Average distortion: " << ctrs.getDist()/nPts << "\n";
    // ctrs.print();

    KMctrIdxArray closeCtr = new KMctrIdx[dataPts.getNPts()];
    double *sqDist = new double[dataPts.getNPts()];
    ctrs.getAssignments(closeCtr, sqDist);

    // std::cout << "  (Cluster assignments:\n" << "    Point  Center  Squared Dist      x             y            z\n"
    //           << "    -----  ------  ------------  ----------   ----------   ----------\n";

    // Make the clusters
    hitIndex = 0;
    IndexToClusterMap indexToClusterMap;

    for (CaloHitList::const_iterator iter = caloHitList.begin(), iterEnd = caloHitList.end(); iter != iterEnd; ++iter)
    {
        if (hitIndex >= nPts)
            break;

        CaloHit *pCaloHit = *iter;
        const int centerIndex = closeCtr[hitIndex];
        IndexToClusterMap::const_iterator clusterIter = indexToClusterMap.find(centerIndex);

        if (indexToClusterMap.end() == clusterIter)
        {
            Cluster *pCluster = NULL;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, pCaloHit, pCluster));

            if (!indexToClusterMap.insert(IndexToClusterMap::value_type(centerIndex, pCluster)).second)
                throw StatusCodeException(STATUS_CODE_ALREADY_PRESENT);
        }
        else
        {
            Cluster *pCluster = clusterIter->second;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddCaloHitToCluster(*this, pCluster, pCaloHit));
        }

        //        std::cout << "   " << setw(5) << hitIndex
        //                  << "   " << setw(5) << closeCtr[hitIndex]
        //                  << "   " << setw(10) << sqDist[hitIndex]
        //                  << "   " << setw(10) << dataPts.getPts()[hitIndex][0]
        //                  << "   " << setw(10) << dataPts.getPts()[hitIndex][1]
        //                  << "   " << setw(10) << dataPts.getPts()[hitIndex][2] << "\n";
        ++hitIndex;
    }

    delete [] closeCtr;
    delete [] sqDist;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode KMeansClusteringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_shouldUseIsolatedHits = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldUseIsolatedHits", m_shouldUseIsolatedHits));

    m_nMeans = 200;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NMeans", m_nMeans));

    return STATUS_CODE_SUCCESS;
}
