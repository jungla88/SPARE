//  DissimilarityRepr class, part of the SPARE library.
//  Copyright (C) 2011 Lorenzo Livi
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

/** @brief File DissimilarityRepr.hpp, that contains the dissimilarity matrix representation class.
 *
 * Contains the declaration of the DissimilarityRepr class.
 *
 * @file DissimilarityRepr.hpp
 * @author Lorenzo Livi
 */

#ifndef DISSIMILARITYREPR_HPP
#define DISSIMILARITYREPR_HPP

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>


namespace spare {  // Inclusione in namespace spare.


/** @brief Dissimilarity matrix-based representation a set of samples.
 *
 * This class implements a @a Representation concept.
 * Given a dissimilarity measure, an input set, and a reference (representation) set of n and r samples, respectively, an n x r dissimilarity matrix D is computed.
 * The allocated matrix must be a boost-compliant (real-valued) matrix.
 */
template <class DissimilarityType>
class DissimilarityRepr {
public:

    /**
     * Build the dissimilarity representation matrix from the two sets of samples
     * @param[in] inputSet The input set of samples
     * @param[in] representationSet The representation set
     * @param[out] m The pre-allocated boost-compliant dissimilarity matrix
     */
    template <typename SamplesContainer1, typename SamplesContainer2, typename BoostMatrixType>
    void getMatrix(const SamplesContainer1& inputSet, const SamplesContainer2& representationSet,
            BoostMatrixType& m) const;

    /**
     * Embed an input set into a container of embedding vectors using the representation set
     * @param[in] inputSet The input set of samples
     * @param[in] representationSet The representation set
     * @param[out] v The container for embedding vectors (internally the method resizes this container to the inputSet size)
     */
    template <typename SamplesContainer1, typename SamplesContainer2, typename DissVectorContainerType>
    void getVectors(const SamplesContainer1& inputSet, const SamplesContainer2& representationSet,
            DissVectorContainerType& vectors) const;

    /**
     * Embed a single pattern into a vector using the representation set
     * @param[in] inputPattern The input pattern
     * @param[in] representationSet The representation set
     * @param[out] v The embedding vector for the input pattern (internally the method resizes this container to the inputSet size)
     */
    template <typename SampleType, typename SamplesContainer, typename DissVectorContainerType>
    void getVector(const SampleType& inputPattern, const SamplesContainer& representationSet,
            DissVectorContainerType& vectors) const;

// ACCESS

    /**
     * Read/Write access to the dissimilarity agent
     * @return A referecen to the dissimilarity
     */
    DissimilarityType& DissAgent() { return mDiss; }

    /**
     * Read-only access to the dissimilarity agent
     * @return A const reference to the dissimilarity
     */
    const DissimilarityType& DissAgent() const { return mDiss; }

private:

    /**
     * Dissimilarity measure
     */
    DissimilarityType mDiss;
};

//IMPL

template <class DissimilarityType>
template <typename SamplesContainer1, typename SamplesContainer2, typename BoostMatrixType>
void DissimilarityRepr<DissimilarityType>::getMatrix(const SamplesContainer1& inputSet, const SamplesContainer2& representationSet,
        BoostMatrixType& m) const
{
    //iterator types
    typedef typename SamplesContainer1::const_iterator samplesItType1;
    typedef typename SamplesContainer2::const_iterator samplesItType2;

    //var
    NaturalType i, j;
    samplesItType1 inputSetIt;
    samplesItType2 reprSetIt;

    //compute the dissimilarity matrix
    inputSetIt=inputSet.begin();

    i=0;
    while(inputSetIt!=inputSet.end())
    {
        reprSetIt=representationSet.begin();
        j=0;

        while(reprSetIt!=representationSet.end())
        {
            m(i, j)=mDiss.Diss(*inputSetIt, *reprSetIt);

            j++;
            reprSetIt++;
        }

        i++;
        inputSetIt++;
    }
}

template <class DissimilarityType>
template <typename SamplesContainer1, typename SamplesContainer2, typename DissVectorContainerType>
void DissimilarityRepr<DissimilarityType>::getVectors(const SamplesContainer1& inputSet, const SamplesContainer2& representationSet,
        DissVectorContainerType& vectors) const
{
    //types for the input patterns
    typedef typename SamplesContainer1::const_iterator samplesItType1;
    typedef typename SamplesContainer2::const_iterator samplesItType2;
    //types for the dissimilarity vectors
    typedef typename DissVectorContainerType::iterator vItType;
    typedef typename DissVectorContainerType::value_type containerType;

    //var
    samplesItType1 inputSetIt=inputSet.begin();
    samplesItType2 reprSetIt;

    //resize of the container for the embedding vectors to the size of the input set
    vectors.resize(inputSet.size());

    vItType vIt=vectors.begin();

    //compute the container of embedding vectors
    while(inputSetIt!=inputSet.end())
    {
        reprSetIt=representationSet.begin();

        containerType embeddingVector=*vIt;
        while(reprSetIt!=representationSet.end())
        {
            embeddingVector.insert(embeddingVector.end(), mDiss.Diss(*inputSetIt, *reprSetIt));

            reprSetIt++;
        }

        //update the modified embedding vector
        *vIt=embeddingVector;

        inputSetIt++;
        vIt++;
    }
}

template <class DissimilarityType>
template <typename SampleType, typename SamplesContainer, typename DissVectorContainerType>
void DissimilarityRepr<DissimilarityType>::getVector(const SampleType& inputPattern, const SamplesContainer& representationSet,
        DissVectorContainerType& vectors) const
{
    //iterator types
    typedef typename SamplesContainer::const_iterator samplesItType;

    //var
    samplesItType reprSetIt=representationSet.begin();

    vectors.resize(1);

    while(reprSetIt!=representationSet.end())
    {
        vectors.insert(vectors.end(), mDiss.Diss(inputPattern, *reprSetIt));

        reprSetIt++;
    }
}


}

#endif /* DISSIMILARITYREPR_HPP */
