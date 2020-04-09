//  RandomK class, part of the SPARE library.
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

/** @brief File RandomK.hpp
 *
 * The file contains the kmeans initialization that randomly selectes K input samples as representatives
 *
 * @file RandomK.hpp
 * @author Lorenzo Livi
 */

#ifndef RANDOMK_HPP_
#define RANDOMK_HPP_

//STD
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

/** @brief RandomK initialization algorithm for kmeans clustering algorithm.
 *
 * This kmeans initialization randomly selects, with uniform probability, K input samples as initial representatives.
 * The input patterns are fetched in the same order they are presented in input, and each one is selected with a user-defined selection probability (default is 0.5).
 */
class RandomK
{
public:

    /**
     * Default constructor
     */
    RandomK()
    {
        mSelectionProbability=0.5;
        mSeed=1;
    }


    /**
     * Read-write access to the selection probability
     * @return The reference to the selection probability
     */
    RealType& SelectionProbability() { return mSelectionProbability; }

    /**
     * Read-only access to the selection probability
     * @return The const reference to the selection probability
     */
    const RealType& SelectionProbability() const { return mSelectionProbability; }

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
     * @param[out] representativeVector Reference to the vector of the kmeans representatives. Note that this container must be assumed to be already resized to contain the K representatives
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

        SamplesITType it=itS;
        NaturalType count=0, pos=0;
        RealType p;
        SetType setOfPos;
        SetType::iterator setOfPosIT;

        do
        {
            p=uni();
            if(p<=mSelectionProbability)
            {
                //check if already selected
                if((setOfPosIT=setOfPos.find(pos))==setOfPos.end())
                {
                    representativeVector[count].Update(*it);
                    setOfPos.insert(pos);
                    count++;
                }
            }

            it++;
            pos++;

            if(count==K)
                break;

            //if not completed, restart (todo: avoid infinite loop here, and avoid selection of duplicate representatives)
            if(it==itE)
                it=itS;

        }while(true);
    }


private:

    /**
     * Selection probability
     */
    RealType mSelectionProbability;

    /**
     * Seed for boost uniform random number generator
     */
    NaturalType mSeed;
};

}

#endif
