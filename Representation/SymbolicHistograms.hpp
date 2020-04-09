//  SymbolicHistograms class, part of the SPARE library.
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

/** @brief File SymbolicHistograms.hpp
 *
 * Contains the declaration of the SymbolicHistograms class.
 * This class implements a @a Representation concept.
 * This class is used for representing graphs with symbolic histograms
 * accoring to a given alphabet of symbols.
 *
 * @file SymbolicHistograms.hpp
 * @author Filippo Maria Bianchi
 */

#ifndef SYMBOLICHISTOGRAMS_HPP
#define SYMBOLICHISTOGRAMS_HPP

//STD INCLUDES
#include <vector>
#include <cmath>

//SPARE INCLUDES
#include <spare/SpareTypes.hpp>
#include <spare/SpareExceptions.hpp>


namespace spare {  // Inclusion in namespace spare.


/** @brief Symbolic Histograms representation.
 *
 * This class implements a @a Representation concept.
 * The explicit representation is computed using a set of representatives symbols.
 * Each symbol must be associated to a local metric threshold.
 * The input set is assumed to be expanded, and consequently a container of mappings ids (structure <--> substructure) must be supplied.
 */
template <class Dissimilarity>
class SymbolicHistograms {
public:

    /**
     * Build the dissimilarity representation matrix from the two sets of samples
     * @param[in] inputSet the container of all the subgraphs of the graphs that need to be converted
     * @param[in] representationSet The representation set, i.e. the alphabet of symbols
     * @param[out] M The allocated dissimilarity matrix
     */
    template <typename SamplesContainer, typename SymbolsContainer, typename BoostMatrix>
    void getMatrix(const SamplesContainer& inputSet, const SymbolsContainer& representationSet,
            BoostMatrix& m);

    /**
     * Embed an input set into a container of embedding vectors using the representation set
     * @param[in] inputSet The input set of samples
     * @param[in] representationSet The representation set
     * @param[out] v The container of embedding vectors
     */
    template <typename SamplesContainer1, typename SamplesContainer2, typename SequenceContainer>
    void getVectors(const SamplesContainer1& inputSet, const SamplesContainer2& representationSet,
            SequenceContainer& v);

    /**
     * Embed a single pattern into a vector
     * @param[in] inputPattern The input pattern
     * @param[in] representationSet The representation set
     * @param[out] v The embedding vector
     */
    template <typename SampleType, typename SamplesContainer, typename SequenceContainer>
    void getVector(const SampleType& inputPattern, const SamplesContainer& representationSet,
            SequenceContainer& v);

// CONSTRUCTOR

    SymbolicHistograms(){
        Hard = 1;
        smThreshold = 0.5;
        numberOfStruct = 0;
    }

// ACCESS

    /**
     * Read/Write access to the dissimilarity agent
     */
    Dissimilarity& DissAgent() { return mDiss; }

    /**
     * Read-only access to the dissimilarity agent
     */
    const Dissimilarity& DissAgent() const { return mDiss; }

    /**
     * Read/Write access to the threshold of soft version
     */
    spare::RealType& softThreshold() { return smThreshold; }

    /**
     * Read-only access to the threshold of soft version
     */
    const spare::RealType& softThreshold() const { return smThreshold; }

    /**
     * Read/Write access to the substructures-structure mapping ids
     */
    std::vector<spare::NaturalType>& SubstructuresIds() { return mSubstructuresIds; }

    /**
     * Read-only access to the substructures-structure mapping ids
     */
    const std::vector<spare::NaturalType>& SubstructuresIds() const { return mSubstructuresIds; }

    /*
     * Read/Write access to the flag for soft or hard recognition threshold
     */
    bool& recognitionStrategy() { return Hard; }

    /*
     * Read/Write access to the flag for soft or hard recognition threshold
     */
    const bool& recognitionStrategy() const { return Hard; }

    /*
     * R/W access to the number of the original struct
     */
    spare::NaturalType& NumberOfStruct(){
        return numberOfStruct;
    }

    /*
     * R_Only access to the number of the original struct
     */
    const spare::NaturalType& NumberOfStruct() const{
        return numberOfStruct;
    }


private:


    /**
     * Dissimilarity measure
     */
    Dissimilarity mDiss;

    /*
     * threshold for soft recognition
     */
    spare::RealType smThreshold;

    /**
     * Substructure <--> structure mapping
     * IDs must be sequential and they must start from 0
     */
    std::vector<spare::NaturalType> mSubstructuresIds;

    /*
     * hard or soft threshold recognition recognition (set to Hard by default)
     */
    bool Hard;

    /*
     * number of original structures (initialization is optional)
     */
    spare::NaturalType numberOfStruct;

};

//IMPL

template <class Dissimilarity>
template <typename SamplesContainer1, typename SamplesContainer2, typename BoostMatrix>
void SymbolicHistograms<Dissimilarity>::getMatrix(const SamplesContainer1& inputSet, const SamplesContainer2& representationSet, BoostMatrix& m)
{
    spare::NaturalType inputSize = inputSet.size();
    typedef typename SamplesContainer1::value_type sample;
    typedef typename SamplesContainer2::value_type symbol;
    typename SamplesContainer1::const_iterator samplesIT;
    std::vector<spare::NaturalType>::iterator sIDIT;
    typename SamplesContainer2::const_iterator symbolsIT;


    //DEBUG***********************
//    cout << "ID size: " << mSubstructuresIds.size() <<endl;
//    cout << "sottografi: "<<inputSet.size()<<endl;
//    cout << "simboli: "<<representationSet.size() <<endl;
    //****************************

    //consistency check
    if(mSubstructuresIds.size() != inputSize){
#if SPARE_DEBUG
        throw SpareLogicError("Error: structure size doesn't match with number of ID.");
#endif
    }

    //initialize matrix m if is empty
    if(m.begin1() == m.end1()){
        //spare::NaturalType nrows = mSubstructuresIds[inputSize -1] + 1; //xL: che dici? questa è una zozzata, ma più veloce
        spare::NaturalType nrows;
        if(numberOfStruct > 0){
            nrows = numberOfStruct;
        }
        else{
            nrows = 0;
            for(spare::NaturalType i = 0; i < mSubstructuresIds.size(); i++){
                if(mSubstructuresIds[i] > nrows){
                    nrows = mSubstructuresIds[i];
                }
            }
            nrows++;
        }
        spare::NaturalType ncols = representationSet.size();
        m.resize(nrows, ncols, false);
        for(spare::NaturalType i = 0; i < nrows; i++){
            for(spare::NaturalType j = 0; j < ncols; j++){
                m.insert_element(i,j,0);
            }
        }
        //cout << "Instantiated a zero matrix of "<<nrows<<" rows and "<<ncols<<" columns."<<endl;
    }


    spare::NaturalType row; //row

    sIDIT = mSubstructuresIds.begin();
    samplesIT = inputSet.begin();
    while (samplesIT != inputSet.end())
    {
        sample subgraph = *samplesIT;
        row = *sIDIT;

        //DEBUG
        //cout<<"GRAPH ID: "<<row<<endl;

        spare::RealType distance;
        spare::NaturalType column = 0;

        symbolsIT = representationSet.begin();
        while(symbolsIT != representationSet.end())
        {
            symbol sym = *symbolsIT;
            //recognition threshold
            spare::RealType threshold=sym.getDissMetric();
            sample symSubgraph = sym.getSubstructure();

            distance = mDiss.Diss(subgraph, symSubgraph);


            if(Hard){
                //hard threshold
                if(distance <= threshold){
                    m(row, column)++;
                }
            }
            else{
                //soft threshold
                spare::RealType ins = 1 - 1/( 1 + exp(-smThreshold*(distance - threshold)) );
                m(row, column)+=ins;
            }

            column++;
            symbolsIT++;
        }

        sIDIT++;
        samplesIT++;
    }
}

template <class Dissimilarity>
template <typename SamplesContainer1, typename SamplesContainer2, typename SequenceContainer>
void SymbolicHistograms<Dissimilarity>::getVectors(const SamplesContainer1& inputSet, const SamplesContainer2& representationSet,
        SequenceContainer& v)
{
//    std::cout << "Inside Symbolic Histograms" << "\n";
    spare::NaturalType inputSize = inputSet.size();
    typedef typename SamplesContainer1::value_type sample;
    typedef typename SamplesContainer2::value_type symbol;
    typename SamplesContainer1::const_iterator samplesIT;
    std::vector<spare::NaturalType>::iterator sIDIT;
    typename SamplesContainer2::const_iterator symbolsIT;
    typedef typename SequenceContainer::value_type containerType;
    typedef typename containerType::iterator internalContainerType;
    typedef typename SequenceContainer::iterator vItType;


//    std::cout << "DEBUG getVectors()***********************" << "\n";
#if SPARE_DEBUG
    std::cout << "ID size: " << mSubstructuresIds.size() <<std::endl;
    std::cout << "sottografi: "<<inputSet.size()<<std::endl;
    std::cout << "simboli: "<<representationSet.size() <<std::endl;
#endif
    //std::cout << "****************************" << "\n";

//    std::cout << "consistency check ****************************" << "\n";
    if(mSubstructuresIds.size() != inputSize){
#if SPARE_DEBUG
        throw SpareLogicError("Error: structure size doesn't match with number of ID");
#endif
    }

//    std::cout << "****************************" << "\n";
    //INIT with 0s
    if(v.begin() == v.end()){
        //spare::NaturalType nrows = mSubstructuresIds[inputSize -1] + 1; //zozzata
        spare::NaturalType nrows;
        if(numberOfStruct > 0){
            nrows = numberOfStruct;
        }
        else{
            nrows = 0;
            for(spare::NaturalType i = 0; i < mSubstructuresIds.size(); i++){
                if(mSubstructuresIds[i] > nrows){
                    nrows = mSubstructuresIds[i];
                }
            }
            nrows++;
        }
        for (spare::NaturalType i = 0; i < nrows; i++){
            containerType c;
            for (spare::NaturalType j = 0; j < representationSet.size(); j++){
                c.insert(c.end(), 0);
            }
            v.insert(v.end(), c);
        }
        //cout << "Instantiated a vector of "<<v.size()<<" vectors of "<<(*v.begin()).size()<<" zeros."<<endl;
    }

    spare::NaturalType row; //row
//int y = 0;
    sIDIT = mSubstructuresIds.begin();
    samplesIT = inputSet.begin();
    while (samplesIT != inputSet.end())
    {
        sample subgraph = *samplesIT;
        row = *sIDIT;

        spare::RealType distance;
        spare::NaturalType column = 0;

        symbolsIT = representationSet.begin();
        while(symbolsIT != representationSet.end())
        {
            symbol sym = *symbolsIT;

            //recognition threshold
            spare::RealType threshold=sym.getDissMetric();
            sample symSubgraph = sym.getSubstructure();

            distance = mDiss.Diss(subgraph, symSubgraph);


            if(distance <= threshold){
//cout << "-"<<y <<endl;
                vItType vIt = v.begin();
                std::advance(vIt, row);
                containerType& c = *vIt;
                internalContainerType cIt = c.begin();
                std::advance(cIt, column);

                if(Hard){
                    //hard threshold
                    *cIt += 1;
                }
                else{
                    //soft threshold
                    spare::RealType ins = 1 - 1/( 1 + exp(-smThreshold*distance) );
                    *cIt += ins;
                }
            }

            column++;
            symbolsIT++;
        }
//y++;
        sIDIT++;
        samplesIT++;
    }
}

template <class Dissimilarity>
template <typename SampleType, typename SamplesContainer, typename SequenceContainer>
void SymbolicHistograms<Dissimilarity>::getVector(const SampleType& inputPattern, const SamplesContainer& representationSet,
        SequenceContainer& v)
{
    typedef typename SampleType::value_type sample;
    typedef typename SamplesContainer::value_type symbol;
    typename SampleType::const_iterator samplesIT;
    typename SamplesContainer::const_iterator symbolsIT;
    typename SequenceContainer::iterator sequenceIT;

    //initialize vector with zero values if empty
    if(v.begin() == v.end()){
        for(spare::NaturalType i = 0; i < representationSet.size(); i++){
            v.insert(v.end(), 0);
        }
        //cout << "Instantiated a vector of "<<v.size()<<" elements."<<endl;
    }


    //update
    samplesIT = inputPattern.begin();
    while (samplesIT != inputPattern.end()){

        sample subgraph = *samplesIT;
        spare::RealType distance;

        symbolsIT = representationSet.begin();
        sequenceIT = v.begin();
        while(symbolsIT != representationSet.end())
        {
            symbol sym = *symbolsIT;
            sample symSubgraph = sym.getSubstructure();
            spare::RealType threshold=sym.getDissMetric();

            distance = mDiss.Diss(subgraph, symSubgraph);

            if(Hard){
                //hard threshold
                if(distance <= threshold){
                    (*sequenceIT)++;
                }
            }
            else{
                //soft threshold
                if(distance <= threshold){
                    spare::RealType ins = 1 - 1/( 1 + exp(-smThreshold*distance) );
                    (*sequenceIT) += ins;
                }

            }

            symbolsIT++;
            sequenceIT++;
        }

        samplesIT++;
    }

}

}


#endif /* SYMBOLICHISTOGRAMS_HPP */
