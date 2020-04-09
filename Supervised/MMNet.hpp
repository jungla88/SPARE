//  MMNet class, part of the SPARE library.
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

/** @brief File MMNet.hpp, containing the template class MMNet.
 *
 * The file contains the template class MMNet, a neuro-fuzzy min-max network trained with the ARC/PARC algorithms.
 * The details can be retrieved from the paper "Adaptive Resolution Min-Max Classifiers. Antonello Rizzi, Massimo Panella, and Fabio Massimo Frattale Mascioli, IEEE TRANSACTIONS ON NEURAL NETWORKS, VOL. 13, NO. 2, MARCH 2002".
 *
 * @file MMNet.hpp
 * @author Lorenzo Livi
 */


#ifndef MMNet_h
#define MMNet_h

// STD INCLUDES
#include <list>
#include <map>
#include <stdexcept>

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>

//local include for the ARC/PARC tool (by Guido Del Vescovo)
#include <spare/Utils/arctools/MinMaxNetwork.h>
#include <spare/Utils/arctools/MinMaxTraining.h>


namespace spare {

/** @brief Neurofuzzy Min-Max (MM) classifier trained with ARC/PARC algorithm.
 *
 * This class models the @a Supervised concept.
 * It provides an implementation of a min-max neural network trained with the ARC/PARC algorithm.
 * It is defined by a template argument defining the container (usually a STL container such as the vector) for the test set labels.
 * By default it uses the ARC training algorithm to synthesize the MM model, single hyperboxes deletion mode, the Simpson membership function, and the centroid hyperbox cut mode.
 * The details can be retrieved from the paper "Adaptive Resolution Min-Max Classifiers. Antonello Rizzi, Massimo Panella, and Fabio Massimo Frattale Mascioli, IEEE TRANSACTIONS ON NEURAL NETWORKS, VOL. 13, NO. 2, MARCH 2002".
 */
template <class LabelContainerType>
class MMNet
{
public:

    /**
     * Type for the list of synthesized MM network models
     */
    typedef std::list<TMinMaxNetwork> NetListType;


    /**
     * Read-write access to the training algo (ARC/PARC) mode
     */
    bool& IsArcTrainingAlgo() { return mIsArcTrainingAlgo; }

    /**
     * Read-only accesso to the training algo (ARC/PARC) mode
     */
    const bool& IsArcTrainingAlgo() const { return mIsArcTrainingAlgo; }

    /**
     * Read-write access to the internal synthesized Min-Max model
     */
    TMinMaxNetwork& MMModel() { return mMMModel; }

    /**
     * Read-only access to the internal synthesized Min-Max model
     */
    const TMinMaxNetwork& MMModel() const { return mMMModel; }

    /**
     * Read-write access to the converted training set
     */
    TVectorCollection* TrainSet() { return mpTrainSet; }

    /**
     * Read-only access to the converted training set
     */
    const TVectorCollection* TrainSet() const { return mpTrainSet; }

    /**
     * Read-write access to the converted test set
     */
    TVectorCollection* TestSet() { return mpTestSet; }

    /**
     * Read-only access to the converted test set
     */
    const TVectorCollection* TestSet() const { return mpTestSet; }

    /**
     * Read-write access to the size of the training set
     */
    NaturalType& SizeTRSet() { return mSizeTRSet; }

    /**
     * Read-only access to the size of the training set
     */
    const NaturalType& SizeTRSet() const { return mSizeTRSet; }

    /**
     * Read-write access to the size of the test set
     */
    NaturalType& SizeTSSet() { return mSizeTSSet; }

    /**
     * Read-only access to the size of the test set
     */
    const NaturalType& SizeTSSet() const { return mSizeTSSet; }

    /**
     * Read-write access to the dimensions of the embedding space
     */
    NaturalType& DimEmbeddingSpace() { return mDimEmbeddingSpace; }

    /**
     * Read-only access to the dimensions of the embedding space
     */
    const NaturalType& DimEmbeddingSpace() const { return mDimEmbeddingSpace; }

    /**
     * Read-write access to the value of the lambda parameter (tradeoff among the error and structural complexity). The lower the value, the lower is the importance of the structural cost during the syntheis.
     */
    RealType& Lambda() { return mLambda; }

    /**
     * Read-only access to the value of the lambda parameter (tradeoff among the error and structural complexity). The lower the value, the lower is the importance of the structural cost during the syntheis.
     */
    const RealType& Lambda() const { return mLambda; }

    /**
     * Read-write access to the test set labels
     */
    LabelContainerType& TestSetLabels() { return mTestSetLabels; }

    /**
     * Read-only access to the test set labels
     */
    const LabelContainerType& TestSetLabels() const { return mTestSetLabels; }

    /**
     * Set the single hyperboxes deletion mode
     */
    void SetSingleHBDeletionMode() { mHBDeletionMode=hdSingle; }

    /**
     * Set the block hyperboxes deletion mode
     */
    void SetBlockHBDeletionMode() { mHBDeletionMode=hdBlock; }

    /**
     * Set the Simpson membership function
     */
    void SetSimpsonMF() { mMembershipType=mtSimpson; }

    /**
     * Set the trapezoidal membership function
     */
    void SetTrapezoidalMF() { mMembershipType=mtTrapezoidal; }

    /** Luca Hack
     * Set member parameter
     */
    RealType& SetMembPar(){return mMembPar;}


    /**
     * Default constructor
     */
    MMNet()
    {
        mIsArcTrainingAlgo=true;
        mSizeTRSet=100;
        mSizeTSSet=100;
        mDimEmbeddingSpace=50;
        mLambda=0.5;
        mMembPar=1.0;
        mHBDeletionMode=hdSingle;
        mMembershipType=mtSimpson;
        mCutMode=cmCentroids;
        //fix uninitialized pointer
        mpTestSet = nullptr;
        mpTrainSet = nullptr;
    }

    /**
     * Standard destructor
     */
    ~MMNet()
    {
        delete mpTestSet;
        delete mpTrainSet;
    }


    /**
     * Write the synthesized MM model to the specified output file
     * @param[in] outputFile The full output file name (path and name)
     */
    void WriteMMModel(const std::string& outputFile)
    {
        mMMModel.WriteToFile(outputFile);
    }

    /**
     * Write the converted training set (ct4 format) to the disk.
     * @param[in] outputFile The full output file name (path and name)
     */
    void WriteTRSet(const std::string& outputFile)
    {
        mpTrainSet->WriteFile(outputFile);
    }

    /**
     * Write the converted test set (ct4 format) to the disk.
     * @param[in] outputFile The full output file name (path and name)
     */
    void WriteTSSet(const std::string& outputFile)
    {
        mpTestSet->WriteFile(outputFile);
    }

    /**
     * Get the number of HBs of the synthesized MM model
     * @return The number of HBs in the MM model
     */
    int getNumHyperboxes()
    {
        return mMMModel.GetNumberOfHyperBoxes();
    }


    /** Learning from a batch of training samples.
     *
     * @param[in] iSampleBegin Iterator pointing to the first input sample.
     * @param[in] iSampleEnd Iterator pointing to one position after the last input sample.
     * @param[in] iLabelBegin Iterator pointing to the first input label.
     */
    template <typename ForwardIterator1, typename ForwardIterator2>
    void Learn(ForwardIterator1 iSampleBegin, ForwardIterator1 iSampleEnd, ForwardIterator2 iLabelBegin);

    /** Classification of a batch of samples.
     *
     * @param[in] iSampleBegin Iterator pointing to the first input sample.
     * @param[in] iSampleEnd Iterator pointing to one position after the last input sample.
     * @param[out] iLabelBegin Iterator pointing to the first output label.
     */
    template <typename ForwardIterator1, typename ForwardIterator2>
    void Process(ForwardIterator1 iSampleBegin, ForwardIterator1 iSampleEnd, ForwardIterator2 iLabelBegin) const;

private:

    /**
     * Hyperboxes deletion mode
     */
    THBDeletionMode mHBDeletionMode;

    /**
     * Membership function type
     */
    TMembershipType mMembershipType;

    /**
     * Hyperbox cut mode
     */
    TCutMode mCutMode;

    /**
     * If true uses ARC to synthesize the Min-Max Network, otherwise it uses PARC
     */
    bool mIsArcTrainingAlgo;

    /**
     * The synthesized MM Model
     */
    TMinMaxNetwork mMMModel;

    /**
     * List of MM network models
     */
    NetListType NetList;

    /**
     * The converted training set
     */
    mutable TVectorCollection* mpTrainSet;

    /**
     * The converted test set
     */
    mutable TVectorCollection* mpTestSet;

    /**
     * Size del training set
     */
    NaturalType mSizeTRSet;

    /**
     * Size del test set
     */
    NaturalType mSizeTSSet;

    /**
     * Dimensions embedding space
     */
    NaturalType mDimEmbeddingSpace;

    /**
     * Lambda parameter for tradeoff among the training error and structural complexity of the MM network. The lower the value, the lower is the importance of the structural cost during the syntheis.
     */
    RealType mLambda;

    /**
     * MembPar parameter
     */
    RealType mMembPar;

    /**
     * Labels of the test set
     */
    LabelContainerType mTestSetLabels;
};


template <class LabelContainerType>
template <typename ForwardIterator1, typename ForwardIterator2>
void MMNet<LabelContainerType>::Learn(ForwardIterator1 iSampleBegin, ForwardIterator1 iSampleEnd,
        ForwardIterator2 iLabelBegin)
{
    TArcParcTraining TrainAlgo;

    ///////
    //convert the training set
    std::string my_names[1]= {"my_section"};
    TSectionType my_sections[1]= {stReal};
    int my_sizes[1]= {mDimEmbeddingSpace};

    mpTrainSet=new TVectorCollection(1, my_names, my_sections, my_sizes);
    //set the number of training patterns
    mpTrainSet->Initialize(mSizeTRSet);

    NaturalType i=0;
    //generate the dataset according to the arc/parc tool
    while(iSampleBegin!=iSampleEnd)
    {
        //set the class of the i-th pattern
        mpTrainSet->SetClass(i, *iLabelBegin);

        //construct the vector
        std::vector<RealType> v=*iSampleBegin;
        for(NaturalType j=0;j<mDimEmbeddingSpace;j++)
            mpTrainSet->WriteComponent(i, j, v[j]);

        i++;
        iSampleBegin++;
        iLabelBegin++;
    }
    /////

    //Some setup of the training algorithm
    TrainAlgo.DisableReport();
    TrainAlgo.SetLambda(mLambda);
    TrainAlgo.SetMembershipType(mMembershipType);
    TrainAlgo.SetMembPar(0, mMembPar);
    TrainAlgo.SetCutMode(mCutMode);
    TrainAlgo.SetDeletionMode(mHBDeletionMode);

    //train with ARC or PARC
    if(mIsArcTrainingAlgo)
        TrainAlgo.ArcTrain(mpTrainSet, true, false, NetList);
    else
        TrainAlgo.ParcTrain(mpTrainSet, true, false, NetList);

    //save the MM model to be used during the test stage
    if(NetList.empty()){ //NetList might be empty
        throw std::invalid_argument("Empty netlist");
    }
    mMMModel=NetList.front();

}


template <class LabelContainerType>
template <typename ForwardIterator1, typename ForwardIterator2>
void MMNet<LabelContainerType>::Process(ForwardIterator1 iSampleBegin, ForwardIterator1 iSampleEnd,
        ForwardIterator2 iLabelBegin) const
{
    NaturalType i=0;
    std::map<std::string, RealType> MembData;

    ///////
    //convert the test set
    std::string my_names[1]= {"my_section"};
    TSectionType my_sections[1]= {stReal};
    int my_sizes[1]= {mDimEmbeddingSpace};

    mpTestSet=new TVectorCollection(1, my_names, my_sections, my_sizes);
    //set the number of test patterns
    mpTestSet->Initialize(mSizeTSSet);

    i=0;
    typename LabelContainerType::const_iterator lIT=mTestSetLabels.begin();
    //generate the dataset according to the arc/parc tool
    while(iSampleBegin!=iSampleEnd)
    {
        //set the class of the i-th pattern
        mpTestSet->SetClass(i, *lIT);

        //construct the vector
        std::vector<RealType> v=*iSampleBegin;
        for(NaturalType j=0;j<mDimEmbeddingSpace;j++)
            mpTestSet->WriteComponent(i, j, v[j]);

        i++;
        lIT++;
        iSampleBegin++;
    }
    ///////

    i=0;
    //classify the entire test set
    while(i<mSizeTSSet)
    {
        std::string StrBuf=mMMModel.RawClassify(mpTestSet->GetDataPointer(i), MembData);

        if(StrBuf=="UNCLASSIFIABLE")
            StrBuf="UNDETERMINED";

        //set the output label
        *iLabelBegin=StrBuf;

        iLabelBegin++;
        i++;
    }
}

}

#endif
