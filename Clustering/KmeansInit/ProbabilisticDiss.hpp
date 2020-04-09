//  ProbabilisticDiss class, part of the SPARE library.
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

/** @brief File ProbabilisticDiss.hpp
 *
 * The file contains the kmeans initialization that selectes K input samples as representatives according to probabilities inversely proportional to the pattern dissimilarity values.
 *
 * @file ProbabilisticDiss.hpp
 * @author Lorenzo Livi
 */

#ifndef PROBABILISTICDISS_HPP_
#define PROBABILISTICDISS_HPP_

//STD
#include <vector>
#include <set>

//BOOST
#include <boost/random.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>

//SPARE
#include <spare/SpareTypes.hpp>


namespace spare {

/** @brief ProbabilisticDiss initialization algorithm for kmeans clustering algorithm.
 *
 *   This kmeans initialization randomly selects K input patterns as representatives according to probabilities that are inversely proportional to the dissimilarity values of the patterns.
 *   That is, patterns with lower sum-of-distances are more likely to be selected as initial representatives.
 *   This initialization method has a quadratic computational complexity in the input data size.
 *   The class takes a template argument that implements the dissimilarity measure for the patterns.
 */
template <class DissimilarityType>
class ProbabilisticDiss
{
public:

    /**
     * Default constructor
     */
    ProbabilisticDiss()
    {
        mSeed=1;
    }


    /**
     * Read-write access to the dissimilarity measure
     * @return The reference to the dissimilarity measure
     */
    DissimilarityType& DissAgent() { return mDiss; }

    /**
     * Read-only access to the dissimilarity measure
     * @return The const reference to the dissimilarity measure
     */
    const DissimilarityType& DissAgent() const { return mDiss; }

    /**
     * Read-write access to the seed
     * @return The reference to the seed
     */
    NaturalType& Seed() { return mSeed; }

    /**
     * Read-only access to the seed
     * @return The const reference to the seed
     */
    const NaturalType& Seed() const { return mSeed; }

    /**
     * Main representatives initialization method.
     * @param[in] K The K parameter of the kmeans algorithm
     * @param[in] itS Iterator pointing at the beginning of the samples container
     * @param[in] itE Iterator pointing at the end of the samples container
     * @param[out] representativeVector Reference to the vector of the kmeans representatives. Note that this container must be assumed to be already resized to contain the K representatives.
     */
    template <typename SamplesITType, typename RepVectorType>
    void Initialize(const NaturalType& K, const SamplesITType& itS, const SamplesITType& itE, RepVectorType& representativeVector) const
    {
        typedef boost::minstd_rand BaseGeneratorType;
        typedef boost::variate_generator<BaseGeneratorType&, boost::uniform_real<> > UniformGeneratorType;
        typedef std::set<NaturalType> SetType;

        BaseGeneratorType random(mSeed);
        boost::uniform_real<> uni_dist(0, 1);
        UniformGeneratorType uni(random, uni_dist);

        SamplesITType it=itS, it2;
        NaturalType count=0, pos=0;
        RealType p, SOD=0, SSOD=0, SOP=0;
        std::vector<RealType> probs, sods;
        SetType setOfPos;
        SetType::iterator setOfPosIT;

        //dissimilarity is not assumed symmetric
        while(it!=itE)
        {
            it2=itS;
            SOD=0.0;
            while(it2!=itE)
            {
                SOD+=mDiss.Diss(*it, *it2);
                it2++;
            }
    
            sods.push_back(SOD);
            SSOD+=SOD;

            it++;
        }

        it=itS;
        count=0;
        while(it!=itE)
        {
            //add the probability of being selected
            probs.push_back(1.0-(sods[count]/SSOD));

            count++;
            it++;
        }


        count=0;
        //select K representatives with probability inversely proportional to their SODs
        do
        {
            p=uni();

            SOP=0, pos=0;
            it=itS;

            while(it!=itE)
            {
                SOP+=probs[pos];
                //ok it is its turn
                if(SOP>=p)
                {
                    //check if already selected
                    if((setOfPosIT=setOfPos.find(pos))==setOfPos.end())
                    {
                        representativeVector[count].Update(*it);
                        setOfPos.insert(pos);
                        count++;
                        break;
                    }
                }

                it++;
                pos++;
            }

            if(count==K)
                break;

        }while(true);
    }

private:

    /**
     * Seed for boost uniform random number generator
     */
    NaturalType mSeed;

    /**
     * The pattern dissimilarity
     */
    DissimilarityType mDiss;
};

}

#endif
