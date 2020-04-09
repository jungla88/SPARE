//  SamplingSeeding class, part of the SPARE library.
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

/** @brief File SamplingSeeding.hpp
 *
 * The file contains the kmeans initialization that selectes K input samples as initial representatives such that the input set is covered as much as possible.
 * The details can be retrieved from the paper "The Effectiveness of Lloyd-Type Methods for the k-Means Problem. Ostrovsky et al., 2006".
 *
 * @file SamplingSeeding.hpp
 * @author Filippo Maria Bianchi
 */

#ifndef SAMPLINGSEEDING_HPP_
#define SAMPLINGSEEDING_HPP_

//STD
#include <vector>
#include <list>
#include <limits>

//BOOST
#include <boost/random.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>

//SPARE
#include <spare/SpareTypes.hpp>
#include <spare/Representative/MinSod.hpp>


namespace spare{

/** @brief SamplingSeeding initialization algorithm for kmeans clustering algorithm.
 *
 * This kmeans initialization selects K input samples as initial representatives with the aim of covering the input set as much as possible.
 * The details can be retrieved from the paper "The Effectiveness of Lloyd-Type Methods for the k-Means Problem. Ostrovsky et al., 2006".
 */
template<class DissimilarityType>
class SamplingSeeding{

private:

    DissimilarityType mDiss;

    NaturalType mSeed;

    RealType alpha;

    NaturalType mCacheSize;

public:

    /**
     * Read-write access to the dissimilarity measure
     * @return The reference to the dissimilarity measure
     */
    DissimilarityType& DissAgent(){
        return mDiss;
    }

    /**
     * Read-only access to the dissimilarity measure
     * @return The const reference to the dissimilarity measure
     */
    const DissimilarityType& DissAgent() const{
        return mDiss;
    }

    /**
     * Read-write access to the random seed
     * @return The reference
     */
    NaturalType& Seed() { return mSeed; }

    /**
     * Read-only access to the random seed
     * @return The const reference
     */
    const NaturalType& Seed() const { return mSeed; }

    /**
     * Read-write access to the cache size of the MinSOD
     * @return The reference
     */
    NaturalType& CacheSize() { return mCacheSize; }

    /**
     * Read-only access to the cache size of the MinSOD
     * @return The const reference
     */
    const NaturalType& CacheSize() const { return mCacheSize; }

    /**
     * Read-write access to the alpha parameter
     * @return The reference
     */
    RealType& Alpha() { return alpha; }

    /**
     * Read-only access to the alpha parameter
     * @return The const reference
     */
    const RealType& Alpha() const { return alpha; }


    /**
     * Default constructor
     */
    SamplingSeeding(){
        alpha = 0.1;
        mSeed = 0;
        mCacheSize = 100;
    }


    /**
     * Main representatives initialization method.
     * @param[in] K The K parameter of the kmeans algorithm
     * @param[in] itS Iterator pointing at the beginning of the samples container
     * @param[in] itE Iterator pointing at the end of the samples container
     * @param[out] representativeVector Reference to the vector of the kmeans representatives. Note that this container must be assumed to be already resized to contain the K representatives
     */
    template <typename SamplesITType, typename RepVectorType>
    void Initialize(const NaturalType& K, const SamplesITType& itS, const SamplesITType& itE,
            RepVectorType& representativeVector) const{

        typedef typename SamplesITType::value_type SampleType;
        typedef boost::minstd_rand BaseGeneratorType;
        typedef boost::variate_generator<BaseGeneratorType&, boost::uniform_real<> > UniformGeneratorType;
        typedef MinSod<SampleType,DissimilarityType> RepresentativeType;

        BaseGeneratorType random(mSeed);
        boost::uniform_real<> uni_dist(0, 1);
        UniformGeneratorType uni(random, uni_dist);

        std::vector<SampleType> tempRep;

        //select the first 2 temporary representatives with a probability proportional to their distance
        RealType prob = 0, repDist=0;
        SampleType c1,c2,s1,s2;
        SamplesITType sIT1 = itS;
        while(sIT1 != itE){
            s1 = *sIT1++;
            SamplesITType sIT2 = itS;
            while(sIT2 != itE){
                s2 = *sIT2++;
                RealType d = mDiss.Diss(s1,s2);
                RealType temp_p = alpha*uni() + (1-alpha)*pow(d,2);
                if(temp_p > prob){
                    prob = temp_p;
                    c1 = s1;
                    c2 = s2;
                    repDist = d/3;
                }
            }
        }
        tempRep.push_back(c1);
        tempRep.push_back(c2);
        RealType minRepDist = repDist;


        //select the remaining temporary representatives
        NaturalType i=0;
        while(i < K-2){
            prob=0;

            SampleType new_c;
            SamplesITType sIT = itS;
            while(sIT != itE){
                SampleType s = *sIT++;
                RealType dmin=std::numeric_limits<RealType>::max();
                typename std::vector<SampleType>::iterator rIT = tempRep.begin();
                while(rIT != tempRep.end()){
                    SampleType r = *rIT++;
                    RealType dtemp = mDiss.Diss(s,r);
                    if(dtemp < dmin)
                        dmin = dtemp;
                }
                RealType temp_p = alpha*uni() + (1-alpha)*pow(dmin,2);
                if(temp_p > prob){
                    prob = temp_p;
                    new_c = s;
                    repDist = dmin/3;
                }
            }
            tempRep.push_back(new_c);
            if(minRepDist > repDist)
                minRepDist = repDist;
            i++;
        }

        //extract the final representatives as the centers of a sphere centered in the temporary representatives
        typename std::vector<SampleType>::iterator tempRepIT = tempRep.begin();
        i=0;
        while(tempRepIT != tempRep.end()){
            SampleType tempRep_i = *tempRepIT++;
            RepresentativeType mSod(mCacheSize);
            SamplesITType samplesIT = itS;
            while(samplesIT != itE){
                SampleType s_i = *samplesIT++;
                RealType d_i = mDiss.Diss(s_i,tempRep_i);
                if(d_i <= minRepDist){
                    mSod.Update(s_i);
                }
            }
            SampleType new_rep = mSod.getRepresentativeSample();
            representativeVector[i++].Update(new_rep);
        }
    }//end Process

};//end class

}//end namespace spare

#endif
