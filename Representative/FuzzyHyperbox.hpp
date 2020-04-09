#ifndef _FuzzyHyperbox_h_
#define _FuzzyHyperbox_h_

// STD INCLUDES
#include <algorithm>
#include <iterator>
#include <limits>
#include <vector>
#include <utility>

// BOOST INCLUDES
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>

// SPARE INCLUDES
#include <spare/BoundedParameter.hpp>
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

#define FHB_MT_SIMPSON     0
#define FHB_MT_TRAPEZOIDAL 1

namespace spare {  // Inclusion in namespace spare.

class FuzzyHyperbox
{
public:

// PUBLIC TYPES

   /** Hyperbox vertex type.
    */
   typedef std::vector<RealType>
                        VertexVector;

   /** Real parameter.
    */
   typedef BoundedParameter<RealType>
                        RealParam;

   /** Membership switch parameter.
    */
   typedef BoundedParameter<NaturalType>
                        MembershipParam;

// LIFECYCLE

   /** Default constructor.
    */
   FuzzyHyperbox()
      : mMembType(FHB_MT_SIMPSON, FHB_MT_TRAPEZOIDAL),
        mGamma(RealType(0), std::numeric_limits<RealType>::max()),
        mCount(0)
   {
      mMembType= FHB_MT_SIMPSON;
      mGamma= RealType(1);
   }

// OPERATIONS

   /** Function evaluation.
    *
    * @param[in] rInput Container holding the input vector.
    * @return The function value.
    */
   template <typename SequenceContainer>
   RealType             Eval(const SequenceContainer& rSample) const;

   /** Function evaluation.
    *
    * @param[in] aInput Pair of iterators delimiting the input vector.
    * @return The function value.
    */
   template <typename ForwardIterator>
   RealType             Eval(std::pair<ForwardIterator, ForwardIterator> aSample) const;

   /** Hyperbox update routine.
    *
    * @param[in] aSample Pair of iterators delimiting the sample.
    */
   template <typename ForwardIterator>
   void                 Update(std::pair<ForwardIterator, ForwardIterator> aSample);

   /** Hyperbox update routine.
    *
    * @param[in] rSample Reference to the container storing the sample.
    */
   template <typename SequenceContainer>
   void                 Update(const SequenceContainer& rSample);

   /** Calculates the dissimilarity between the sample and the fuzzy hyperbox.
    *
    * @param[in] aSample Pair of iterators that delimit the sample.
    * @return The calculated dissimilarity value.
    */
   template <typename ForwardIterator>
   RealType             Diss(std::pair<ForwardIterator, ForwardIterator> aSample) const
                           {
                              if (!mCount)
                              {
                                 throw SpareLogicError("FuzzyHyperbox, 0, Uninitialized "
                                                       "object.");
                              }

                              return RealType(1) - Eval(aSample);
                           }

   /** Calculates the dissimilarity between the sample and the fuzzy hyperbox.
    *
    * @param[in] rSample Reference to the container that store the sample.
    * @return The calculated dissimilarity value.
    */
   template <typename SequenceContainer>
   RealType             Diss(const SequenceContainer& rSample) const
                           {
                              if (!mCount)
                              {
                                 throw SpareLogicError("FuzzyHyperbox, 1, Uninitialized "
                                                       "object.");
                              }

                              return RealType(1) - Eval(rSample);
                           }

// ACCESS

   /** Read/write access to the MembType parameter.
    *
    * @return A reference to the MembType parameter.
    */
   MembershipParam&     MembType()                 { return mMembType; }

   /** Read only access to the MembType parameter.
    *
    * @return A const reference to the MembType parameter.
    */
   const MembershipParam&   
                        MembType() const           { return mMembType; }

   /** Read/write access to the Gamma parameter.
    *
    * @return A reference to the Gamma parameter.
    */
   RealParam&           Gamma()                    { return mGamma; }

   /** Read only access to the Gamma parameter.
    *
    * @return A const reference to the Gamma parameter.
    */
   const RealParam&     Gamma() const              { return mGamma; }

   /** Read access to the min vertex.
    *
    * @return A reference to the min vertex.
    */
   const VertexVector&  GetV() const { return mV; }

   /** Read access to the max vertex.
    *
    * @return A reference to the max vertex.
    */
   const VertexVector&  GetW() const { return mW; }

   /** Read access to the number of samples inserted so far.
    *
    * @return The number of samples.
    */
   NaturalType          GetCount() const           { return mCount; }

private:

   VertexVector         mV;
   VertexVector         mW;

   NaturalType          mCount;

   MembershipParam      mMembType;
   RealParam            mGamma;
   
   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & BOOST_SERIALIZATION_NVP(mV);
      ar & BOOST_SERIALIZATION_NVP(mW);
      ar & BOOST_SERIALIZATION_NVP(mCount);
      ar & BOOST_SERIALIZATION_NVP(mMembType);
      ar & BOOST_SERIALIZATION_NVP(mGamma);
   } // BOOST SERIALIZATION
      
};

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename ForwardIterator>
void
FuzzyHyperbox::Update(std::pair<ForwardIterator, ForwardIterator> aSample)
{
   // Local typedefs.
   typedef VertexVector::size_type
                        VertexSizeType;

   typedef typename std::iterator_traits<ForwardIterator>::difference_type
                        SampleDiffType;

   // Variables.
   VertexVector::iterator Vit, Wit;

   // On first update, initialize vertices.
   if (!mCount)
   {
      mV.assign(aSample.first, aSample.second);
      mW.assign(aSample.first, aSample.second);
      
      ++mCount;
   }
   else
   {
      // Dimension check.
      #if SPARE_DEBUG
      if (static_cast<SampleDiffType>(mV.size()) != 
          std::distance(aSample.first, aSample.second))
      {
         throw SpareLogicError("FuzzyHyperbox, 2, Different lenghts.");
      }
      #endif
   
      Vit= mV.begin();
      Wit= mW.begin();
      while (mV.end() != Vit)
      {
         if (*aSample.first < *Vit)
         {
            *Vit= *aSample.first;
         }
         else
         {
            if (*aSample.first > *Wit)
            {
               *Wit= *aSample.first;
            }
         }
         
         ++aSample.first;
         ++Vit;
         ++Wit;
      }
      
      ++mCount;
   }
}  // Update

template <typename SequenceContainer>
void
FuzzyHyperbox::Update(const SequenceContainer& rSample)
{
   Update(std::make_pair(rSample.begin(), rSample.end())); 
   // FIXME: Si può migliorare efficienza controlli, con duplicazione di codice
}  // Update

template <typename SequenceContainer>
RealType
FuzzyHyperbox::Eval(const SequenceContainer& rSample) const
{
   return Eval(std::make_pair(rSample.begin(), rSample.end())); 
   // FIXME: Si può migliorare efficienza controlli, con duplicazione di codice
}

template <typename ForwardIterator>
RealType
FuzzyHyperbox::Eval(std::pair<ForwardIterator, ForwardIterator> aSample) const
{
   // Local typedefs.
   typedef typename std::iterator_traits<ForwardIterator>::difference_type
                        SampleDiffType;

   // Variables.
   VertexVector::const_iterator Vit, Wit;   
   RealType Memb;

   #if SPARE_DEBUG
   if (static_cast<SampleDiffType>(mV.size()) != 
       std::distance(aSample.first, aSample.second))
   {
      throw SpareLogicError("FuzzyHyperbox, 3, Different lenghts.");
   }
   #endif

   switch (mMembType)
   {
      case FHB_MT_SIMPSON:

         Memb = RealType(0);

         Vit= mV.begin();
         Wit= mW.begin();
         while (aSample.first != aSample.second)
         {
             Memb+= std::max(
                             RealType(0), 
                             RealType(1) - std::max(
                                                    RealType(0),
                                                    mGamma * std::min(
                                                                   RealType(1),
                                                                   *Vit - *aSample.first))) +
                    std::max(
                             RealType(0), 
                             RealType(1) - std::max(
                                                    RealType(0),
                                                    mGamma * std::min(
                                                                   RealType(1),
                                                                   *aSample.first - *Wit)));
 
            ++aSample.first;
            ++Vit;
            ++Wit; 
         }
                  
         Memb /= (2 * mV.size());

         // correzione
         Memb -= RealType(0.5);
         Memb *= RealType(2);

     break;
     
     case FHB_MT_TRAPEZOIDAL:

         Memb = RealType(1);
          
         Vit= mV.begin();
         Wit= mW.begin();
         while (aSample.first != aSample.second)
         {
            Memb= 
             std::min(
                  Memb,
                  std::min(
                       std::max(
                            RealType(0), 
                            RealType(1) - std::max(
                                                   RealType(0),
                                                   mGamma * std::min(
                                                                  RealType(1),
                                                                  *Vit - *aSample.first))),
                       std::max(
                            RealType(0), 
                            RealType(1) - std::max(
                                                   RealType(0),
                                                   mGamma * std::min(
                                                                  RealType(1),
                                                                  *aSample.first - *Wit)))));
                  
            ++aSample.first;
            ++Vit;
            ++Wit; 
         }

     break;

     default:

        throw SpareLogicError("FuzzyHyperbox, 4, Unknown membership function.");

     break;
   }

   return Memb;
}

}  // namespace spare

#endif
