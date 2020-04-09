//  SvmApprox class, part of the SPARE library.
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

/** @brief File SvmApprox.hpp, that contains the SvmApprox.
 *
 * This file contains the declaration of the SvmApprox class.
 *
 * @file SvmApprox.hpp
 * @author Guido Del Vescovo
 */

#ifndef _SvmApprox_h_
#define _SvmApprox_h_

// STD INCLUDES
#include <limits>
#include <string>

// BOOST INCLUDES
#include <boost/numeric/conversion/converter.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>

// SPARE INCLUDES
#include <spare/BoundedParameter.hpp>
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>
#include <spare/SwitchParameter.hpp>
#include <spare/Supervised/SvmCommon.hpp>

namespace spare {  // Inclusione in namespace spare.

// Dati per costruzione parametri switch.
static const std::string SVMR_STVAL[]= {"epsilon-SVR", "nu-SVR"};
static const size_t      SVMR_STVAL_SZ= 2;

/** @brief Svm Function Approximator.
 *
 * This class models the @a Supervised concept. It's a wrapper to the libsvm library, made
 * with the aim of taking advantage of that well known implementation using the incapsulated 
 * SPARE interface. For a complete information about libsvm visit 
 * @a http://www.csie.ntu.edu.tw/~cjlin/libsvm/
 */
class SvmApprox
{
public:

   /** Real parameter.
    */
   typedef BoundedParameter<RealType>
                        RealParam;

   /** Integer parameter.
    */
   typedef BoundedParameter<NaturalType>
                        NaturalParam;

   /** Switch parameter.
    */
   typedef SwitchParameter<std::string>
                        StringParam;

// LIFECYCLE

   /** Default constructor.
    */
   SvmApprox()
      : mSvmType(SVMR_STVAL, SVMR_STVAL + SVMR_STVAL_SZ),
        mKernelType(SVMC_KTVAL, SVMC_KTVAL + SVMC_KTVAL_SZ),
        mDegree( 0, std::numeric_limits<NaturalType>::max() ),
        mGamma( 0, std::numeric_limits<RealType>::max() ),
        mCoef0( -std::numeric_limits<RealType>::max(),
                 std::numeric_limits<RealType>::max() ),
        mCacheSize( std::numeric_limits<RealType>::epsilon(),
                    std::numeric_limits<RealType>::max() ),
        mEps( std::numeric_limits<RealType>::epsilon(),
              std::numeric_limits<RealType>::max() ),
        mC( std::numeric_limits<RealType>::epsilon(),
              std::numeric_limits<RealType>::max() ),
        mNu(std::numeric_limits<RealType>::epsilon(), 1),
        mP( 0, std::numeric_limits<RealType>::max() ),
        mShrinking(SVMC_SPVAL, SVMC_SPVAL + SVMC_SPVAL_SZ)
                           {
                              mpSvmModel= NULL;
                              mSvmType= "epsilon-SVR";
                              mKernelType= "RBF";
                              mDegree= 3;
                              mGamma= 0.5;
                              mCoef0= 0;
                              mCacheSize= 100;
                              mEps= 0.001;
                              mC= 1;
                              mNu= 0.5;
                              mP= 0.1;
                              mShrinking= "On";
                           }

   /** Destructor.
    */
   ~SvmApprox()
                           {
                              if (mpSvmModel)
                              {
                                 //svm_destroy_model(mpSvmModel);
                                 svm_free_model_content(mpSvmModel);
                                 mpSvmModel= NULL;
                              }
                           }

// OPERATIONS

   /** Learning operation from a batch of samples.
    *
    * @param[in] iSampleBegin Iterator pointing to the first sample.
    * @param[in] iSampleEnd Iterator pointing to the first position after the last sample.
    * @param[in] iLabelBegin Iterator pointing to the first label.
    */
   template <typename ForwardIterator1, typename ForwardIterator2>
   void                 Learn(
                           ForwardIterator1    iSampleBegin,
                           ForwardIterator1    iSampleEnd,
                           ForwardIterator2    iLabelBegin);

   /** Classify a single sample.
    *
    * @param[in] rSample Reference to the sample.
    * @param[out] rLabel Reference to the label.
    */
   template <typename SampleType, typename LabelType>
   void                 Process(
                           const SampleType&   rSample,
                           LabelType&          rLabel) const;

   /** Classify a batch of samples.
    *
    * @param[in] iSampleBegin Iterator pointing to the first sample.
    * @param[in] iSampleEnd Iterator pointing to the first position after the last sample.
    * @param[out] iLabelBegin Iterator pointing to the first label. The container of labels must be initialized with a proper user-defined value.
    */
   template <typename ForwardIterator1, typename ForwardIterator2>
   void                 Process(
                           ForwardIterator1    iSampleBegin,
                           ForwardIterator1    iSampleEnd,
                           ForwardIterator2    iLabelBegin) const;

// ACCESS

   /** Read/Write access to the SvmType param.
    * Possible values are: "C_SVC", "NU_SVC", "ONE_CLASS", "EPSILON_SVR", "NU_SVR".
    * Default value: "EPSILON_SVR"
    *
    * @return A reference to the value of SvmType.
    */
   StringParam&         SvmType()                  { return mSvmType; }

   /** Read access to the SvmType param.
    *
    * @return A reference to the value of SvmType.
    */
   const StringParam&   SvmType() const            { return mSvmType; }

   /** Read/Write access to the value of KernelType parameter.
    * Accepted values are: "LINEAR", "POLY", "RBF", "SIGMOID", "PRECOMPUTED".
    * Default: "RBF".
    *
    * @return A reference to the value of KernelType.
    */
   StringParam&         KernelType()               { return mKernelType; }

   /** Read access to the value of KernelType parameter.
    *
    * @return A reference to the value of KernelType.
    */
   const StringParam&   KernelType() const         { return mKernelType; }

   /** Read/Write access to the value of Degree (for polynomial kernel type).
    *
    * @return A reference to the value of Degree.
    */
   NaturalParam&        Degree()                   { return mDegree; }

   /** Read access to the value of Degree (for polynomial kernel type).
    *
    * @return A reference to the value of Degree.
    */
   const NaturalParam&  Degree() const             { return mDegree; }

   /** Read/Write access to the value of the parameter Gamma.
    *
    * @return A reference to the value of Gamma.
    */
   RealParam&           Gamma()                    { return mGamma; }

   /** Read access to the value of the parameter Gamma.
    *
    * @return A reference to the value Gamma.
    */
   const RealParam&     Gamma() const              { return mGamma; }

   /** Read/Write access to the value of the paramter Coef0.
    *
    * @return A reference to the value of Coef0.
    */
   RealParam&           Coef0()                    { return mCoef0; }

   /** Read access to the value of the paramter Coef0.
    *
    * @return A reference to the value of Coef0.
    */
   const RealParam&     Coef0() const              { return mCoef0; }

   /** Read/Write access to the training paramter CacheSize.
    *
    * @return A reference to the value of CacheSize.
    */
   RealParam&           CacheSize()                { return mCacheSize; }

   /** Read access to the training paramter CacheSize.
    *
    * @return A reference to the value of CacheSize.
    */
   const RealParam&     CacheSize() const          { return mCacheSize; }

   /** Read/Write access to the value of Eps, parameter for the stop criterion.
    *
    * @return A reference to the value of Eps.
    */
   RealParam&           Eps()                      { return mEps; }

   /** Read access to the value of Eps, parameter for the stop criterion.
    *
    * @return A referene to the value of Eps.
    */
   const RealParam&     Eps() const                { return mEps; }

   /** Read/Write access to the parameter C for types epsilon-SVR e nu-SVR.
    *
    * @return A reference to the value of C.
    */
   RealParam&           C()                        { return mC; }

   /** Read access to the parameter C for types epsilon-SVR e nu-SVR
    *
    * @return A reference to the value of C.
    */
   const RealParam&     C() const                  { return mC; }

   /** Read/Write access to the value of Nu, parameter of nu-SVC.
    *
    * @return A reference to the value of Nu.
    */
   RealParam&           Nu()                       { return mNu; }

   /** Read access to the value of Nu, parameter of nu-SVC.
    *
    * @return A reference to the value of Nu.
    */
   const RealParam&     Nu() const                 { return mNu; }

   /** Read/Write access to the parameter P for the epsilon-SVR type.
    *
    * @return A reference to the value of P.
    */
   RealParam&           P()                        { return mP; }

   /** Read access to the parameter P for the epsilon-SVR type.
    *
    * @return A reference to the value of P.
    */
   const RealParam&     P() const                  { return mP; }

   /** Read/Write access to the shrinking heuristic.
    *
    * @return A reference to the value of Shrinking.
    */
   StringParam&         Shrinking()                { return mShrinking; }

   /** Read access to the shrinking heuristic.
    *
    * @return A reference to the value of Shrinking.
    */
   const StringParam&   Shrinking() const          { return mShrinking; }

   /** Read access to the number of support vectors.
    *
    * @return The number of support vectors.
    */
   NaturalType          GetSvNumber() const
                           {
                              // Controllo esistenza modello.
                              if (!mpSvmModel)
                              {
                                 throw SpareLogicError("SvmApprox, 0, Model not present.");
                              }

                              // Estraggo numero SV.
                              int SvNumber;
                              SvNumberFromSvmModel(mpSvmModel, SvNumber);

                              return boost::numeric::converter<NaturalType, int>
                                     ::convert(SvNumber);
                           }

private:

   // Modello libsvm.
   LibSvmModelPtr       mpSvmModel;

   // Tipo Svm.
   StringParam          mSvmType;

   // Tipo Kernel.
   StringParam          mKernelType;

   // Grado kernel (Polynomial).
   NaturalParam         mDegree;

   // Gamma kernel (Rbf, Polynomial, Sigmoid).
   RealParam            mGamma;

   // Coef0 kernel (Polynomial, Sigmoid).
   RealParam            mCoef0;

   // Cache size (MB).
   RealParam            mCacheSize;

   // Criterio di stop.
   RealParam            mEps;

   // Parametro C (epsilon-SVR, nu-SVR).
   RealParam            mC;

   // Parametro nu (nu-SVR).
   RealParam            mNu;

   // Parametro p (epsilon-SVR).
   RealParam            mP;

   // Parametro shrinking.
   StringParam          mShrinking;

   // Funzione ausiliaria.
   inline void          SetParams(svm_parameter& rParams);

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void save(Archive & ar, const unsigned int version) const
   {
      std::string ModelText= SPARE_SVM_NO_MODEL_TAG;
      if (mpSvmModel)
      {
         SvmModelToText(mpSvmModel, ModelText);
      }
      ar & ModelText;

      ar & mSvmType;
      ar & mKernelType;
      ar & mDegree;
      ar & mGamma;
      ar & mCoef0;
      ar & mCacheSize;
      ar & mEps;
      ar & mC;
      ar & mNu;
      ar & mP;
      ar & mShrinking;
   }

   template<class Archive>
   void load(Archive & ar, const unsigned int version)
   {
      std::string ModelText;
      ar & ModelText;
      if (mpSvmModel)
      {
         //svm_destroy_model(mpSvmModel);
         svm_free_model_content(mpSvmModel);
         mpSvmModel= NULL;
      }
      if (ModelText != SPARE_SVM_NO_MODEL_TAG)
      {
         SvmModelFromText(mpSvmModel, ModelText);
      }

      ar & mSvmType;
      ar & mKernelType;
      ar & mDegree;
      ar & mGamma;
      ar & mCoef0;
      ar & mCacheSize;
      ar & mEps;
      ar & mC;
      ar & mNu;
      ar & mP;
      ar & mShrinking;
   }

   BOOST_SERIALIZATION_SPLIT_MEMBER() // BOOST SERIALIZATION

}; // class SvmApprox

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename ForwardIterator1, typename ForwardIterator2>
void
SvmApprox::Learn(
             ForwardIterator1  iSampleBegin,
             ForwardIterator1  iSampleEnd,
             ForwardIterator2  iLabelBegin)
{
   // Dichiarazioni.
   svm_problem                     Prob;
   svm_parameter                   Params;
   std::string                     CheckRes;

   // Eventuale eliminazione vecchio modello.
   if (mpSvmModel)
   {
      //svm_destroy_model(mpSvmModel);
      svm_free_model_content(mpSvmModel);
      mpSvmModel= NULL;
   }

   // Traduzione formato problema.
   CreateSvmProblem(
                iSampleBegin,
                iSampleEnd,
                iLabelBegin,
                Prob);

   // Impostazione parametri.
   SetParams(Params);

   // Disabilito logging su console.
   //svm_print_string = &PrintNull;
   svm_set_print_string_function(&PrintNull);

   // Check parametri.
   if (svm_check_parameter(&Prob, &Params))
   {
      CheckRes= svm_check_parameter(&Prob, &Params);
      throw SpareLogicError("SvmApprox, 1, " + CheckRes);
   }

   // Training.
   mpSvmModel= svm_train(&Prob, &Params);
   FinalizeSvmModel(mpSvmModel);

   DestroySvmProblem(Prob);
   svm_destroy_param(&Params);
}  // Learn

template <typename SampleType, typename LabelType>
void
SvmApprox::Process(
               const SampleType&   rSample,
               LabelType&          rLabel) const
{
   // Controllo esistenza modello.
   if (!mpSvmModel)
   {
      throw SpareLogicError("SvmApprox, 2, Model not present.");
   }

   LibSvmNodePtr pX= CreateSvmX(rSample);
   rLabel= svm_predict(mpSvmModel, pX); // TODO: conversione di tipo rigorosa
   DestroySvmX(pX);
}  // Process

template <typename ForwardIterator1, typename ForwardIterator2>
void
SvmApprox::Process(
               ForwardIterator1  iSampleBegin,
               ForwardIterator1  iSampleEnd,
               ForwardIterator2  iLabelBegin) const
{
   // Controllo esistenza modello.
   if (!mpSvmModel)
   {
      throw SpareLogicError("SvmApprox, 3, Model not present.");
   }

   // Dichiarazioni.
   ForwardIterator1   It;
   ForwardIterator2   Ot;
   LibSvmNodePtr      pX;

   // Decisione etichette.
   It= iSampleBegin;
   Ot= iLabelBegin;
   while (It != iSampleEnd)
   {
      pX= CreateSvmX(*It++);
      (*Ot++)= svm_predict(mpSvmModel, pX); // TODO: conversione di tipo rigorosa
      DestroySvmX(pX);
   }
}  // Process

////////////////////////////////////// PRIVATE /////////////////////////////////////////////

// Funzione ausiliaria.
void
SvmApprox::SetParams(svm_parameter& rParams)
{
   if (mSvmType == "epsilon-SVR")
   {
      rParams.svm_type= EPSILON_SVR;
   }
   else
   {
      if (mSvmType == "nu-SVR")
      {
         rParams.svm_type= NU_SVR;
      }
      else
      {
         throw SpareLogicError("SvmApprox, 4, Unknown svm type.");
      }
   }

   if (mKernelType == "RBF")
   {
      rParams.kernel_type= RBF;
   }
   else
   {
      if (mKernelType == "Polynomial")
      {
         rParams.kernel_type= POLY;
      }
      else
      {
         if (mKernelType == "Sigmoid")
         {
            rParams.kernel_type= SIGMOID;
         }
         else
         {
            if (mKernelType == "Linear")
            {
               rParams.kernel_type= LINEAR;
            }
            else
            {
               throw SpareLogicError("SvmApprox, 5, Unknown kernel type.");
            }
         }
      }
   }

   rParams.degree= mDegree;
   rParams.gamma= mGamma;
   rParams.coef0= mCoef0;
   rParams.nu= mNu;
   rParams.p= mP;
   rParams.cache_size= mCacheSize;
   rParams.C= mC;
   rParams.eps= mEps;

   if (mShrinking == "On")
   {
      rParams.shrinking= 1;
   }
   else
   {
      if (mShrinking == "Off")
      {
         rParams.shrinking= 0;
      }
      else
      {
         throw SpareLogicError("SvmApprox, 6, Unknown shrinking switch value.");
      }
   }

   rParams.probability= 0;
   rParams.nr_weight= 0;
   rParams.weight_label= NULL;
   rParams.weight= NULL;
}  // SetParams

}  // namespace spare

#endif // _SvmApprox_h_
