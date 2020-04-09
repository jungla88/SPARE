//  Bsas class, part of the SPARE library.
//  Copyright (C) 2011 Guido Del Vescovo
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

/** @brief File BsasPartitions.hpp
 *
 * Contains the implementation of the generation of an esembles of partitions using a clustering algorithm.
 * Each partition is generated using  a different value of theta.
 * Candidates value of theta are generated applying binary search method inside an interval
 * defined by a lower and an upper bound, and progressing inside the interval using
 * a given step
 *
 * @file BsasPartitions.hpp
 * @author Filippo Maria Bianchi
 */


#ifndef BSASPARTITIONS_HPP
#define BSASPARTITIONS_HPP


//SPARE
#include <spare/SpareTypes.hpp>

//STL
#include <vector>


namespace spare {  // Inclusion in namespace spare.


/** @brief %Ensembling Bsas clustering algorithm.
 *
 * This algorithm performs a log-like linear search finding an ensemble of partitions.
 * The search is performed in an interval defined by two real parameters.
 *
 */
template <class BsasClustering>
class BsasPartitions {

public:

    /*
     * Default constructor
     */
    BsasPartitions(){}

    /*
     * Constructor
     * @param[in] up: upperbound of values for binary search of theta's BSAS
     * @param[in] bot: lowerbound of values for binary search of theta's BSAS
     * @param[in] stp: increment of the value of theta's BSAS during binary search
     */
    BsasPartitions(spare::RealType up, spare::RealType bot, spare::RealType stp){
        clusteringThresholds.first =  bot;
        clusteringThresholds.second = up;
        tStep = stp;
    }

    //Bsas theta range type
    typedef std::pair<spare::RealType, spare::RealType> parameterRange;

    //access to the type that represent a partition (a vector of MinSods, one per cluster)
    typedef typename BsasClustering::RepVector partitionType;

    //access to the type of the container of the partitions
    typedef typename std::vector<partitionType> partitionContainerType;

    //type of the container of theta determinations
    typedef std::vector<spare::RealType> thetaContainerType;

     //value type of the labels of the current clustering algorithm
    typedef typename BsasClustering::LabelType bsasLabels;

    //type of the container of labels of the clusters
    typedef std::vector<bsasLabels> LabelContainerType;


    /*
    *  computes an ensemble of partitions and put it into PartitionsContainer
    *  @param[in] = SamplesContainer: is a conteiner of all subgraphs of the graphs of training set, from order 1 to r
    *  @param[out] = PartitionsContainer: vector of vectors (one per partition) of representatives (one per cluster in the partition)
    */
    template <typename SamplesContainer>
    void Process(const SamplesContainer& samples);

    //***************** Access to member variables ******************

    //R/W access to clustering algorithm
    BsasClustering& ClustAlgo(){
        return clustAlgo;
    }

    //R-Only access to clustering algorithm
    const BsasClustering& ClustAlgo() const{
        return clustAlgo;
    }

    // R/W access to clustering thresholds
    parameterRange& thetaRange(){
            return clusteringThresholds;
    }

    // R-only access to clustering thresholds
    const parameterRange& thetaRange() const{
        return clusteringThresholds;
    }

    // R/W accesso to theta step
    spare::RealType& thetaStep(){
        return tStep;
    }

    // R-only accesso to theta step
    const spare::RealType& thetaStep() const{
            return tStep;
    }

    // R/W accesso to theta step
    spare::NaturalType& getQ(){
        return Q;
    }

    // R-only accesso to theta step
    const spare::NaturalType& getQ() const{
            return Q;
    }

    // R-Only access to partitions container
    const partitionContainerType& getPartitions() const{
           return partitions;
    }

    // R-Only access to theta values container
    const thetaContainerType& getThetaValues() const{
            return tV;
    }

    std::vector<LabelContainerType>& PartitionsLabels(){
        return partitionsLabels;
    }

    const std::vector<LabelContainerType>& PartitionsLabels() const{
        return partitionsLabels;
    }

private:

    //Bsas clustering algorithm
    BsasClustering clustAlgo;

    //Clustering thresholds range
    parameterRange clusteringThresholds;

    //binary search increment
    spare::RealType tStep;

    //where the values of theta generating a partitions are stored
    thetaContainerType tV;

    //where the partitions are stored
    partitionContainerType partitions;

    //max number of partitions allowed in a BSAS run
    spare::NaturalType Q;

    //container of the cluster's labels generated in each partition
    std::vector<LabelContainerType> partitionsLabels;


    /*
     * Recursive auxiliary function. Evaluates if the median value between tm (theta min) and tM (theta max)
     * differs from the one of extremities of a value major of the allowed step of theta. Another control
     * is then performed on the numbers of clusters generated executing BSAS with tm and tM. If the numbers are different
     * a new partition is computed using the new median value theta mid.
     * Two new recursive calls are then performed on the intervals [theta min, theta mid] and [theta mid, theta max]
     *
     * @param[in] tm: theta minimum value
     * @param[in] tM: theta minimum value
     * @param[in] Nc1: numbers of clusters generated by BSAS with tm
     * @param[in] Nc2: numbers of clusters generated by BSAS with tM
     * @param[in] TR: container of patterns on which the algorithm is executed
     * @param[in] tStep: increment in binary search
     */
    template <typename SamplCont>
    inline void BSP(spare::RealType tm, spare::RealType tM, spare::NaturalType Nc1, spare::NaturalType Nc2, const SamplCont& TR, spare::RealType tStep);

};


// IMPL
template <class BsasClustering>
template <typename SamplesContainer>
void BsasPartitions<BsasClustering>::Process(const SamplesContainer& samples){

    typename partitionContainerType::value_type pmin, pMax;
    //ADDED tV clear - Luca Baldini
    tV.clear();
    partitions.clear();
    partitionsLabels.clear();
    spare::NaturalType nG = samples.size();
    LabelContainerType clustLabels;
    clustLabels.resize(nG);
    clustAlgo.Q() = Q;
    
    //Bsas with theta min
    clustAlgo.Theta() = clusteringThresholds.first;
    tV.insert(tV.end(), clusteringThresholds.first);
    clustAlgo.Process(samples.begin(),samples.end(),clustLabels.begin());
    //saving results
    partitions.insert(partitions.end(), clustAlgo.GetRepresentatives());
    spare::NaturalType Nc1 = clustAlgo.GetRepresentatives().size();
    partitionsLabels.push_back(clustLabels);

    //Bsas with theta MAX
    clustAlgo.Theta() = clusteringThresholds.second;
    tV.insert(tV.end(), clusteringThresholds.second);
    clustLabels.clear();
    clustLabels.resize(nG);
    clustAlgo.Process(samples.begin(),samples.end(),clustLabels.begin());
    //saving results
    partitions.insert(partitions.end(), clustAlgo.GetRepresentatives());
    spare::NaturalType Nc2 = clustAlgo.GetRepresentatives().size();
    partitionsLabels.push_back(clustLabels);

    //binary search method
    BSP(clusteringThresholds.first, clusteringThresholds.second, Nc1, Nc2, samples, tStep);

}

template <class BsasClustering>
template <typename SamplCont>
inline void BsasPartitions<BsasClustering>::BSP(spare::RealType tm, spare::RealType tM, spare::NaturalType Nc1, spare::NaturalType Nc2, const SamplCont& TR, spare::RealType tStep){
    spare::RealType dt = (tM - tm)/2;
    //check if the input partitions have different # of clusters and if thier theta differs less than theta step
    if (Nc1 != Nc2 && (dt >= tStep) && (tM-(tm+dt) >= tStep)){

        //Bsas Initialization
        spare::NaturalType nG = TR.size();
        LabelContainerType  clustLabels;
        clustLabels.resize(nG);
        clustAlgo.Theta() = dt+tm;
        tV.insert(tV.end(), dt+tm);

        //bsas execution
        clustAlgo.Process(TR.begin(),TR.end(),clustLabels.begin());

        //saving results
        partitions.insert(partitions.end(), clustAlgo.GetRepresentatives());
        partitionsLabels.push_back(clustLabels);
        spare::NaturalType nC = clustAlgo.GetRepresentatives().size();

        //recursive calls
        BSP(tm, tm+dt, Nc1, nC, TR, tStep);
        BSP(tm+dt, tM, nC, Nc2, TR, tStep);

    }
}

}

#endif

