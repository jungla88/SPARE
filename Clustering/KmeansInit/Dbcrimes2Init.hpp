//  Dbcrimes2Init class, part of the SPARE library.
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
//  along with this program. If not, see <http://www.gnu.org/licenses/>.

/** @brief File Dbcrimes2Init.hpp
 *
 * The file contains the kmeans initialization that selects the representatives according to the
 * raking strategy of the DBCRIMES algorithm
 *
 * @file Dbcrimes2Init.hpp
 * @author filippo
 */

#ifndef Dbcrimes2Init_HPP_
#define Dbcrimes2Init_HPP_


//SPARE
#include <spare/SpareTypes.hpp>

//STD
#include <vector>
#include <list>
#include <math.h>

namespace spare{

/**
 * @brief c2VectorListSorter class used for sorting vectorList
 */
class c2VectorListSorter{
public:

    template<typename PairType>
    bool operator() (PairType p1, PairType p2){

        if( p1.first > p2.first )
            return false;
        else
            return true;

    }
};

/**
 * @brief c2OmniStruct struct containing the sample, its F value and its rank
 */
template<class SampleType>
class c2OmniStruct{
public:

    SampleType sample;

    RealType F;

    NaturalType rank;
};

/**
 * @brief c2OmniSorter class used for sorting elements of type c2OmniStruct according to their rank
 */
class c2OmniSorter{
public:
    template<typename StructType>
    bool operator() (StructType s1, StructType s2){

        if( s1.rank > s2.rank )
            return true;
        else
            return false;

    }
};

/**
 * @brief Dbcrimes2Init class containing the the kmeans initialization that selects the representatives according to the
 * ranking strategy of the DBCRIMES algorithm
 */
template<class DissimilarityType>
class Dbcrimes2Init{
public:

    /**
     * entry type in the vectorList, relative to the element i: <dissimilarity d_i,j, ID_j>
     */
    typedef std::pair<RealType, NaturalType> entryType;

    /**
     * vectorList type
     */
    typedef std::vector<std::list<entryType> > VectorList;

    /**
     * type of the container of the values of the density function
     */
    typedef std::vector<RealType> DensityFunctionContainer;

    /**
     * Type of the container of the ranking values
     */
    typedef std::list<NaturalType> RankingContainerType;

    /**
     * constructor
     */
    Dbcrimes2Init(){
        t_d = 1;
        t_stop = 0;
        Sigma = 0;
    }

    //ACCESS
    /**
     * return Read/Write access to dissimilarity agent
     */
    DissimilarityType& DissAgent(){
        return diss;
    }

    /**
     * return Read-Only access to dissimilarity agent
     */
    const DissimilarityType& DissAgent() const{
        return diss;
    }

    /**
     * return Read/Write access to the threshold t_d
     */
    RealType& T_D(){
        return t_d;
    }

    /**
     * return Read-Only access to the threshold t_d
     */
    const RealType& T_D() const{
        return t_d;
    }

    /**
     * return Read/Write access to the threshold t_stop
     */
    RealType& T_Stop(){
        return t_stop;
    }

    /**
     * return Read-Only access to the threshold t_stop
     */
    const RealType& T_Stop() const{
        return t_stop;
    }

    /**
     * return Read/Write access to the Sigma value
     */
    RealType& getSigma(){
        return Sigma;
    }

    /**
     * return Read-Only access to the Sigma value
     */
    const RealType& getSigma() const{
        return Sigma;
    }

    //MEMBER FUNCTION
    /**
     * Main representatives initialization method.
     * @param[in] K The K parameter of the kmeans algorithm
     * @param[in] itS Iterator pointing at the beginning of the samples container
     * @param[in] itE Iterator pointing at the end of the samples container
     * @param[out] representativeVector Reference to the vector of the kmeans representatives. Note that this container must be assumed to be already resized to contain the K representatives.
     */
    template <typename SamplesITType, typename RepVectorType>
    void Initialize(const NaturalType& K, const SamplesITType& itS, const SamplesITType& itE,
            RepVectorType& representativeVector) const;

private:

    /**
     * Dissimilarity measure
     */
    DissimilarityType diss;

    /**
     * neighborhood threshold
     */
    RealType t_d;

    /**
     * stop threshold
     */
    RealType t_stop;

    /**
     * Sigma of the kernel gaussian function
     */
    mutable RealType Sigma;



};

template<class DissimilarityType>
template <typename SamplesITType, typename RepVectorType>
void Dbcrimes2Init<DissimilarityType>::Initialize(const NaturalType& K, const SamplesITType& itS, const SamplesITType& itE,
        RepVectorType& representativeVector) const{

    RankingContainerType Ranking;
    DensityFunctionContainer F;
    VectorList vl;

    typedef typename SamplesITType::value_type sampleType;
    typedef c2OmniStruct<sampleType> DataStructType;

    NaturalType Nsamples = std::distance(itS, itE), i=0, j;
    vl.resize(Nsamples);
    DensityFunctionContainer f(Nsamples,0.0);
    F=f;
    SamplesITType samplesIT1 = itS, samplesIT2;

    if(Sigma == 0){
        //construct the vectorlist and extimate sigma
        RealType avgDist = 0;
        while(samplesIT1 != itE){
            sampleType s_i = *samplesIT1++;
            j=0;
            samplesIT2 = itS;

            while(samplesIT2 != itE){
                sampleType s_j = *samplesIT2++;
                RealType d = diss.Diss(s_i, s_j);
                avgDist += d;

                if(d <= t_d){
                    vl[i].push_back(std::make_pair(d,j));
                }
                j++;
            }
            i++;
        }
        avgDist = avgDist/(Nsamples*(Nsamples-1));

        //build vector F
        Sigma =  0.4164421*pow(avgDist,3) - 0.7946562*pow(avgDist,2) + 0.6016253*avgDist + 0.0070494;
        for(i=0; i < Nsamples; i++){
            std::list<entryType> L_i = vl[i];
            std::list<entryType>::iterator L_iIT = L_i.begin();
            while(L_iIT != L_i.end()){
                RealType dij = (*L_iIT++).first;
                F[i] += exp( -dij/(2*pow(Sigma,2)) ); //gaussian kernel
            }
        }

    }else{
        //construct the vectorlist using the given sigma
        RealType avgDist = 0;
        while(samplesIT1 != itE){
            sampleType s_i = *samplesIT1++;
            j=0;
            samplesIT2 = itS;

            while(samplesIT2 != itE){
                sampleType s_j = *samplesIT2++;
                RealType d = diss.Diss(s_i, s_j);
                avgDist += d;

                if(d <= t_d){
                    vl[i].push_back(std::make_pair(d,j));
                }
                j++;
            }
            i++;
        }
        avgDist = avgDist/(Nsamples*(Nsamples-1));

        //build vector F
        for(i=0; i < Nsamples; i++){
            std::list<entryType> L_i = vl[i];
            std::list<entryType>::iterator L_iIT = L_i.begin();
            while(L_iIT != L_i.end()){
                RealType dij = (*L_iIT++).first;
                F[i] += exp( -dij/(2*pow(Sigma,2)) ); //gaussian kernel
            }
        }
    }

    //sort the vectorList
    for(NaturalType i=0; i < Nsamples; i++){

        //sort the list
        std::list<entryType> L_i = vl[i];
        RealType F_i = F[i];
        c2VectorListSorter s1;
        L_i.sort(s1);

        //create the ranking
        IntegerType counter = -1;
        bool stop = 0;
        std::list<entryType>::iterator lIT = L_i.begin();
        while(!stop && lIT != L_i.end()){

            RealType F_j = F[(*lIT++).second];
            if(F_i >= F_j){
                //cout<<"DIFF: "<<F_i - F_j<<endl;
                counter++;
            }
            else
                stop=1;
        }

        Ranking.push_back(counter);
    }

    //extract representatives
    std::list<DataStructType> DataList;
    SamplesITType sIT = itS;
    std::list<NaturalType>::iterator rankIT = Ranking.begin();
    std::vector<RealType>::iterator FIT = F.begin();


    //construct a list of structure containing all the required informations (sample, label, F-value, rank)
    while(sIT != itE){
        DataStructType ds;
        ds.sample = *sIT++;
        ds.F = *FIT++;
        ds.rank = *rankIT++;
        DataList.push_back(ds);
    }


    //order that list depending on the ranking and extract representatives
    c2OmniSorter mSorter;
    DataList.sort(mSorter);
    typename std::list<DataStructType>::iterator DataListIT = DataList.begin();
    RealType stop = (*DataListIT).rank * t_stop;
    RealType currRank = (*DataListIT).rank;

    //saving the representants
    std::list<sampleType> rep;
    if(t_stop > 0){
        while(currRank >= stop && DataListIT != DataList.end()){
            rep.push_back((*DataListIT).sample);
            DataListIT++;
            currRank = (*DataListIT).rank;
        }
    }
    else{
        NaturalType k=K;
        while(k > 0){
            rep.push_back((*DataListIT).sample);
            DataListIT++;
            k--;
        }
    }

    //update kmeans representatives
    typename std::list<sampleType>::iterator repIT = rep.begin();
    for(NaturalType i=0; i < K; i++){
        representativeVector[i].Update(*repIT++);
    }

}

}

#endif /* Dbcrimes2Init_HPP_ */
