//  DbcrimesInit class, part of the SPARE library.
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

/** @brief File DbcrimesInit.hpp
 *
 * The file contains the kmeans initialization that selects the representatives according to the
 * radial decrement strategy of the DBCRIMES algorithm
 *
 * @file DbcrimesInit.hpp
 * @author filippo
 */

#ifndef DbcrimesInit_HPP_
#define DbcrimesInit_HPP_

//SPARE
#include <spare/SpareTypes.hpp>
using namespace spare;

//STD
#include <vector>
#include <list>
#include <math.h>

namespace spare{

/**
 * @brief DbcrimesInit class containing the the kmeans initialization that selects the representatives according to the
 * radial decrement strategy of the DBCRIMES algorithm
 */
template<class DissimilarityType>
class DbcrimesInit{

public:

    //TYPEDEFS
    /**
     * entry type in the vectorList: <dissimilarity d_i,j, ID_j>
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

    //ACCESS
    /**
     * return Read/Write access to dissimilarity agent
     */
    DissimilarityType& DissAgent(){
        return mDiss;
    }

    /**
     * return Read-Only access to dissimilarity agent
     */
    const DissimilarityType& DissAgent() const{
        return mDiss;
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

    /**
     * return Read/Write access to the Psi value
     */
    RealType& getPsi(){
        return Psi;
    }

    /**
     * return Read-Only access to the Sigma value
     */
    const RealType& getPsi() const{
        return Psi;
    }

    /**
     * Constructor
     */
    DbcrimesInit(){
        t_d = 1;
        t_stop = 0;
        Sigma = 0;
        Psi = 0;
        avgDist = 0;
    }

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
     * t_d
     */
    RealType t_d;

    /**
     * t_stop
     */
    RealType t_stop;

    /**
     * sigma
     */
    mutable RealType Sigma;

    /**
     * psi
     */
    mutable RealType Psi;

    /**
     * avgdist
     */
    mutable RealType avgDist;

    /**
     * Dissimilarity measure
     */
    DissimilarityType mDiss;

    /**
     * Vector containing the value of the density function evaluated on each pattern
     */
    mutable DensityFunctionContainer F;

    /**
     * generate the vectorList dissimilarity representation
     * @param[in] itS Iterator pointing at the beginning of the samples container
     * @param[in] itE Iterator pointing at the end of the samples container
     * @param[out] vlOut Reference to  vectorList containing dissimilarities
     */
    template<typename SamplesITType>
    void getVector(const SamplesITType& itS, const SamplesITType& itE, VectorList& vlOUT) const;

};


template<class DissimilarityType>
template <typename SamplesITType, typename RepVectorType>
void DbcrimesInit<DissimilarityType>::Initialize(const NaturalType& K, const SamplesITType& itS, const SamplesITType& itE,
        RepVectorType& representativeVector) const{


    //Construction of adjacency VectorList
    VectorList vl;
    getVector(itS, itE, vl);

    //extract representatives
    typedef typename SamplesITType::value_type sampleType;
    typedef typename VectorList::value_type listType;
    typedef typename listType::value_type elementType;
    typedef typename listType::const_iterator listIteratorType;

    std::list<sampleType> representatives;

    if(avgDist == 0){
        std::cerr<<"avgDist not initialized"<<std::endl;
        return;
    }
    //if not initialized set Psi to a default value
    if(Psi == 0){
        Psi = 0.883*avgDist;
    }

    //find the maximum value in vector F
    NaturalType max=std::numeric_limits<NaturalType>::max();
    RealType Fmax=0;
    for(NaturalType i=0; i<F.size(); i++){
        if(F[i] > Fmax){
            Fmax = F[i];
            max = i;
        }
    }

    //set the threshold used in the stop criterion
    RealType STOP = t_stop*Fmax;

    NaturalType j=1;

    if(t_stop > 0){
        //main cycle
        while(Fmax >= STOP && STOP > 0){

            //select the representative of the first cluster
            SamplesITType sampleIT = itS;
            std::advance(sampleIT, max);
            sampleType rep = *sampleIT;
            representatives.push_back(rep);

            //decrement the values in F vector
            listType listMax = vl[max];
            listIteratorType listIT = listMax.begin();
            while(listIT != listMax.end()){
                elementType e = *listIT++;
                //F[e.second] -= Fmax*exp(-e.first/(Psi));
                F[e.second] -= Fmax*exp(-e.first/(Psi));
                if(F[e.second] <0)
                    F[e.second] = 0;
            }


            //find the new max
            Fmax=0;
            for(NaturalType i=0; i<F.size(); i++){
                if(F[i] > Fmax){
                    Fmax = F[i];
                    max = i;
                }
            }
        }//main cycle
    }//K==0


    else{
        NaturalType k=K;
        while(k > 0){

            //select the representative of the first cluster
            SamplesITType sampleIT = itS;
            std::advance(sampleIT, max);
            sampleType rep = *sampleIT;
            representatives.push_back(rep);

            //decrement the values in F vector
            listType listMax = vl[max];
            listIteratorType listIT = listMax.begin();
            while(listIT != listMax.end()){
                elementType e = *listIT++;
                F[e.second] -= Fmax*exp(-e.first/(Psi));
                //F[e.second] -= F[e.second]*exp(-pow(e.first,0.5)/(Psi));
                if(F[e.second] <0)
                    F[e.second] = 0;
            }

            //find the new max
            Fmax=0;
            for(NaturalType i=0; i<F.size(); i++){
                if(F[i] > Fmax){
                    Fmax = F[i];
                    max = i;
                }
            }
            k--;
        }//main cycle
    }//else

    //update kmeans representatives
    typename std::list<sampleType>::iterator repIT = representatives.begin();
    for(NaturalType i=0; i < K; i++){
        representativeVector[i].Update(*repIT++);
    }

}

template<class DissimilarityType>
template <typename SamplesITType>
void DbcrimesInit<DissimilarityType>::getVector(const SamplesITType& itS, const SamplesITType& itE, VectorList& vlOUT) const{

    typedef typename SamplesITType::value_type sampleType;

    NaturalType Nsamples = std::distance(itS, itE), i=0, j;
    vlOUT.resize(Nsamples);
    DensityFunctionContainer f(Nsamples,0.0);
    F=f;
    SamplesITType samplesIT1 = itS, samplesIT2;

    RealType minDist=std::numeric_limits<RealType>::max(), maxDist=0;
    avgDist=0;

    if(Sigma == 0){ //sigma must be estimated
        //construct the vectorlist and extimate sigma
        while(samplesIT1 != itE){
            sampleType s_i = *samplesIT1++;
            j=0;
            samplesIT2 = itS;

            while(samplesIT2 != itE){
                sampleType s_j = *samplesIT2++;
                RealType d = mDiss.Diss(s_i, s_j);
                if(d > maxDist)
                    maxDist = d;
                if(d < minDist && d > 0)
                    minDist = d;
                avgDist += d;

                if(d <= t_d){
                    vlOUT[i].push_back(std::make_pair(d,j));
                }
                j++;
            }
            i++;
        }
        avgDist = avgDist/(Nsamples*(Nsamples-1));

        //build vector F
        Sigma =  0.4164421*pow(avgDist,3) - 0.7946562*pow(avgDist,2) + 0.6016253*avgDist + 0.0070494;
        for(i=0; i < Nsamples; i++){
            std::list<entryType> L_i = vlOUT[i];
            std::list<entryType>::iterator L_iIT = L_i.begin();
            while(L_iIT != L_i.end()){
                RealType dij = (*L_iIT++).first;
                F[i] += exp( -dij/(2*pow(Sigma,2)) ); //gaussian
                //F[i] += 1 - ( pow(dij,2) / (pow(dij,2)+0.0005) ); //rational quadratic
            }
        }

    }else{ //sigma is already given
        //construct the vectorlist and extimate sigma
        while(samplesIT1 != itE){
            sampleType s_i = *samplesIT1++;
            j=0;
            samplesIT2 = itS;

            while(samplesIT2 != itE){
                sampleType s_j = *samplesIT2++;
                RealType d = mDiss.Diss(s_i, s_j);
                avgDist += d;

                if(d <= t_d){
                    vlOUT[i].push_back(std::make_pair(d,j));
                }
                j++;
            }
            i++;
        }
        avgDist = avgDist/(Nsamples*(Nsamples-1));

        //build vector F
        for(i=0; i < Nsamples; i++){
            std::list<entryType> L_i = vlOUT[i];
            std::list<entryType>::iterator L_iIT = L_i.begin();
            while(L_iIT != L_i.end()){
                RealType dij = (*L_iIT++).first;
                F[i] += exp( -dij/(2*pow(Sigma,2)) ); //gaussian
                //F[i] += 1 - ( pow(dij,2) / (pow(dij,2)+0.001) ); //rational quadratic
            }
        }
    }
}

}

#endif /* DbcrimesInit_HPP_ */
