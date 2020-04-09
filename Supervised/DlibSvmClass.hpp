//  DliSvmClass class, part of the SPARE library.
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

/** @brief File DliSvmClass.hpp, containing the template class DliSvmClass.
 *
 * The file contains the template class DliSvmClass, a wrapper of the kernelized SVM implementation of Dlib (
 *
 * @file DliSvmClass.hpp
 * @author Lorenzo Livi
 */


#ifndef _DliSvmClass_h
#define _DliSvmClass_h_

// STD INCLUDES
#include <vector>

// SPARE INCLUDES
#include <spare/SpareTypes.hpp>

//local include for the Dlib shit
#include <dlib/svm.h>



namespace spare {


/**
 * @brief Dlib wrapper of the user-define data type
 */
template <typename DataType>
struct DlibDataTypeWrap
{
    typedef spare::RealType type;

    typedef dlib::default_memory_manager mem_manager_type;

    DataType mData;
};


/**
 * @brief Dlib compliant kernel function to be customized with any user-defined SPARE kernel function
 */
template <typename SampleType, typename SPAREKernelType>
struct DlibKernel
{
    //some types required by the Dlib library
    typedef typename SampleType::type scalar_type;
    typedef SampleType sample_type;
    typedef typename dlib::default_memory_manager mem_manager_type;

    /**
     * Read-write access to the SPARE kernel
     * @return A reference to the internal SPARE kernel
     */
    SPAREKernelType& SPAREKernel() { return mSPAREKernel; }

    /**
     * Read-only access to the SPARE kernel
     * @return A read-only reference to the internal SPARE kernel
     */
    const SPAREKernelType& SPAREKernel() const { return mSPAREKernel; }


    /**
     * Evaluation method for the internal kernel function
     * @a[in] The first argument of the kernel
     * @b[in] The second argument of the kernel
     * @return The similarity degree according to the internal SPARE kernel function
     */
    scalar_type operator() (const sample_type& a, const sample_type& b) const
    {
        return mSPAREKernel.Sim(a.mData, b.mData);
    }

    /**
     * Implementation of the == operator (required by Dlib)
     */
    bool operator== (const DlibKernel& k) const
    {
        return false;
    }

private:

    /**
     * The SPARE kernel function
     */
    SPAREKernelType mSPAREKernel;
};



/** @brief Kernelized SVM implementation provided by Dlib (C-SVM and Nu-SVM)
 *
 * This class implements a @a Supervised concept.
 * It is a wrapper of the kernelized SVM implementation provided by the Dlib library (http://dlib.net).
 * It is a template class which accepts three arguments.
 * The first one is the sample type which is defined by the user (e.g., a vector of real numbers, a labeled graphs, a sequence etc).
 * The second one is the suited positive definite kernel function which is assumed to be consistent with the defined sample type.
 * Finally, the third argument is the type of container used by user to handle the input test set labels. The input labels are assumed to be in some string format, like "0" and "1"; however the user can chose also "3" and "4" as class labels.
 * The user is required to convert the input data labels according to this binary format. This is also the format of the labels provided as output during the classification.
 * This class takes into account a macro called SPARE_DLIB_NUSVM. By setting this macro during the compilation stage, the Dlib wrapper will use the Nu-SVM version (C-SVM is the default).
 */
template <class SampleType, class KernelType, class ContainerTestLabelsType>
class DlibSvmClass
{
public:

    /**
     * Dlib data type
     */
    typedef DlibDataTypeWrap<SampleType> DlibDataType;

    /**
     * Dlib dataset type
     */
    typedef std::vector<DlibDataType> DlibDSType;

    /**
     * Dlib type for set of labels
     */
    typedef std::vector<RealType> DlibDSLabelsType;

    /**
     * The normalized decision function
     */
    typedef dlib::normalized_function<dlib::decision_function<KernelType> > DecisionFunctionType;


    /**
     * Read-write access to the Dlib decision function
     * @return The reference to the decision function
     */
    DecisionFunctionType& DecisionFunction() { return mDecisionFunction; }

    /**
     * Read-only access to the Dlib decision function
     * @return The const reference to the decision function
     */
    const DecisionFunctionType& DecisionFunction() const { return mDecisionFunction; }

    /**
     * Read-write access to the user-defined set of test labels
     * @return The reference to the user-defined test labels
     */
    ContainerTestLabelsType& TsSetLabels() { return mTestSetLabels; }

    /**
     * Read-only access to the user-defined set of test labels
     * @return The const reference to the user-defined test labels
     */
    const ContainerTestLabelsType& TsSetLabels() const { return mTestSetLabels; }

    /**
     * Read-write access to the Dlib converted set of test labels (Dlib uses -1 and +1 notation, casted as double)
     * @return The reference to the converted dblib test labels
     */
    DlibDSLabelsType& TsLabels() { return mTsLabels; }

    /**
     * Read-only access to the Dlib converted set of test labels (Dlib uses -1 and +1 notation, casted as double)
     * @return The const reference to the converted dblib test labels
     */
    const DlibDSLabelsType& TsLabels() const { return mTsLabels; }

    /**
     * Read-write access to the kernel function
     * @return The reference to the kernel function
     */
    KernelType& KernelFunction() { return mKernelFunction; }

    /**
     * Read-only access to the kernel function
     * @return The const reference to the kernel function
     */
    const KernelType& KernelFunction() const { return mKernelFunction; }

    /**
     * Read-write access to the Nu parameter of the Nu-SVM (defaul 0.1560)
     * @return The reference
     */
    RealType& Nu() { return mNu; }

    /**
     * Read-only access to the Nu parameter of the Nu-SVM (defaul 0.1560)
     * @return The const reference
     */
    const RealType& Nu() const { return mNu; }

    /**
     * Read-write access to the C parameter of the C-SVM (defaul 5)
     * @return The reference
     */
    RealType& C() { return mC; }

    /**
     * Read-only access to the C parameter of the C-SVM (defaul 5)
     * @return The const reference
     */
    const RealType& C() const { return mC; }

    /**
     * Read-write access to the user-defined label denoting the class 0 patterns
     * @return The reference to the label for class 0 patterns
     */
    std::string& Label0() { return mLabel0; }

    /**
     * Read-only access to the user-defined label denoting the class 0 patterns
     * @return The const reference to the label for class 0 patterns
     */
    const std::string& Label0() const { return mLabel0; }

    /**
     * Read-write access to the user-defined label denoting the class 1 patterns
     * @return The reference to the label for class 1 patterns
     */
    std::string& Label1() { return mLabel1; }

    /**
     * Read-only access to the user-defined label denoting the class 1 patterns
     * @return The const reference to the label for class 1 patterns
     */
    const std::string& Label1() const { return mLabel1; }

    /**
     * Read-only access to the number of support vectors used after the training of the model
     * @return The const reference to the number of support vectors
     */
    const NaturalType& GetSVNumber() const { return mSVNumber; }


    /**
     * Default constructor
     */
    DlibSvmClass()
    {
        mNu=0.1560;
        mC=5.0;
        mLabel0="0";
        mLabel1="1";
        mSVNumber=0;
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
     * User-defined string label denoting the class 0 patterns
     */
    std::string mLabel0;

    /**
     * User-defined string label denoting the class 1 patterns
     */
    std::string mLabel1;

    /**
     * A Dlib-compliant kernel function which implements the Sim(,) method proper of SPARE kernels
     */
    KernelType mKernelFunction;

    /**
     * A Dlib-compliant decision function
     */
    DecisionFunctionType mDecisionFunction;

    /**
     * Set of user-defined test labels
     */
    ContainerTestLabelsType mTestSetLabels;

    /**
     * The training set converted according to the Dlib format
     */
    DlibDSType mTrSet;

    /**
     * The training set labels in the Dlib format
     */
    DlibDSLabelsType mTrLabels;

    /**
     * The test set according to the Dlib format
     */
    mutable DlibDSType mTsSet;

    /**
     * The test set labels according to the Dlib format
     */
    mutable DlibDSLabelsType mTsLabels;

    /**
     * The C parameter for the C-SVM
     */
    RealType mC;

    /**
     * The Nu parameter for the Nu-SVM
     */
    RealType mNu;

    /**
     * The number of used SVs
     */
    NaturalType mSVNumber;
};


template <class SampleType, class KernelType, class ContainerTestLabelsType>
template <typename ForwardIterator1, typename ForwardIterator2>
void DlibSvmClass<SampleType, KernelType, ContainerTestLabelsType>::Learn(ForwardIterator1 iSampleBegin, ForwardIterator1 iSampleEnd,
        ForwardIterator2 iLabelBegin)
{
    //convert the training set according to the Dlib standard
    while(iSampleBegin!=iSampleEnd)
    {
        SampleType g=*iSampleBegin;
        std::string l=*iLabelBegin;
        DlibDataType st;

        st.mData=g;

        mTrSet.push_back(st);

        //discriminate using the user-defined labels
        if(*iLabelBegin==mLabel1)
            mTrLabels.push_back(1.0);
        else
            mTrLabels.push_back(-1.0);

        iSampleBegin++;
        iLabelBegin++;
    }

    //train a C-SVM or a Nu-SVM (default is C-SVM)
#ifdef SPARE_DLIB_NUSVM
    dlib::svm_nu_trainer<KernelType> trainer;
    trainer.set_nu(mNu);
#else
    dlib::svm_c_trainer<KernelType> trainer;
    trainer.set_c(mC);
#endif

    //setup the kernel function
    trainer.set_kernel(mKernelFunction);

    //train the classifier
    mDecisionFunction.function=trainer.train(mTrSet, mTrLabels);

    //set the number of SVs
    mSVNumber=mDecisionFunction.function.basis_vectors.size();
}


template <class SampleType, class KernelType, class ContainerTestLabelsType>
template <typename ForwardIterator1, typename ForwardIterator2>
void DlibSvmClass<SampleType, KernelType, ContainerTestLabelsType>::Process(ForwardIterator1 iSampleBegin, ForwardIterator1 iSampleEnd,
        ForwardIterator2 iLabelBegin) const
{
    typedef typename ContainerTestLabelsType::const_iterator lItType;

    lItType lIT=mTestSetLabels.begin();

    //for test set
    while(iSampleBegin!=iSampleEnd)
    {
        SampleType g=*iSampleBegin;
        std::string l=*lIT;
        DlibDataType st;

        st.mData=g;

        //Dlib compliant test set
        mTsSet.push_back(st);

        //test the pattern
        if(mDecisionFunction.function(st)>=0)
            *iLabelBegin=mLabel1;
        else
            *iLabelBegin=mLabel0;

        //generate also the Dlib compliant set of test labels
        if(*lIT==mLabel1)
            mTsLabels.push_back(1.0);
        else
            mTsLabels.push_back(-1.0);

        iSampleBegin++;
        iLabelBegin++;
        lIT++;
    }
}

}

#endif
