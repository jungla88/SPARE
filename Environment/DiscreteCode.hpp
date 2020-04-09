//  DiscreteCode class, part of the SPARE library.
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

/** @brief File DiscreteCode.hpp, containing the DiscreteCode template class.
 *
 * The file contains the DiscreteCode template class, implementing the @a Environment
 * concept, mainly for the genetic algorithm implemented by the Genetic class. In this
 * environment, the solution code is a discrete vector, i.e. a fixed length vector whose
 * components are integers.
 *
 * @file DiscreteCode.hpp
 * @author Guido Del Vescovo
 */

#ifndef _DiscreteCode_h_
#define _DiscreteCode_h_

// STD INCLUDES
#include <vector>

// BOOST INCLUDES
#include <boost/random.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

// SPARE INCLUDES
#include <spare/BoundedParameter.hpp>
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

#include <math.h>

namespace spare {  // Inclusion in namespace spare.

/** @brief Environment for the Genetic class, the code is a fixed length integer vector.
 *
 * The DiscreteCode class models the @a Environment concept. The code is a fixed lenght
 * integer vector, the specific integer type is defined by the GeneType template parameter.
 * The fitness evaluation is demanded from a fitness agent which models the Evaluator
 * concept, the fitness agent type is defined by the Evaluator template parameter. The Eval
 * method of the fitness agent must accept as input argument a solution code of CodeType and
 * return a non-negative real value. Higher is the value, better is the solution.
 *
 * <b>Parameter summary</b>
 *  <table class="contents">
 *  <tr>
 *     <td class="indexkey"><b>Name</b></td>
 *     <td class="indexkey"><b>Domain</b></td>
 *     <td class="indexkey"><b>Description</b></td>
 *     <td class="indexkey"><b>Const</b></td>
 *     <td class="indexkey"><b>Default</b></td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">CodeSize</td>
 *     <td class="indexvalue">[1, inf)</td>
 *     <td class="indexvalue">Number of genes of the solution code.</td>
 *     <td class="indexvalue">Yes</td>
 *     <td class="indexvalue">16</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">GeneMin</td>
 *     <td class="indexvalue">(-inf, inf)</td>
 *     <td class="indexvalue">Minimum value of gene, must be less than GeneMax.</td>
 *     <td class="indexvalue">Yes</td>
 *     <td class="indexvalue">0</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">GeneMax</td>
 *     <td class="indexvalue">(-inf, inf)</td>
 *     <td class="indexvalue">Maximum value of gene, must be more than GeneMin.</td>
 *     <td class="indexvalue">Yes</td>
 *     <td class="indexvalue">1</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">UniformRate</td>
 *     <td class="indexvalue">[0, 1]</td>
 *     <td class="indexvalue">Uniform crossover rate, the probability of performing a uniform
                              crossover instead of a cut based crossover.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">0.33</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">TwoPointRate</td>
 *     <td class="indexvalue">[0, 1]</td>
 *     <td class="indexvalue">Two point crossover rate, in a cut based crossover, the
                              probability of performing a two point crossover instead of a
                              single point crossover.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">0.5</td>
 *  </tr>
 *  </table>
 *
 * <b>Advanced setup</b>
 *  <table class="contents">
 *  <tr>
 *     <td class="indexkey"><b>Method</b></td>
 *     <td class="indexkey"><b>Comment</b></td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">RandSeedSetup</td>
 *     <td class="indexvalue">Setup of the randomization seeds, two non-negative integers.
                              </td>
 *  </tr>
 *  </table>
 */
template <typename GeneType, typename Evaluator>
class DiscreteCode
{
public:

// PUBLIC TYPES

   /** Solution code type.
    */
   typedef std::vector<GeneType>
                        CodeType;

   /** Code size type.
    */
   typedef typename CodeType::size_type
                        CodeSizeType;

   /** Real parameter type.
    */
   typedef BoundedParameter<RealType>
                        RealParam;

// LIFECYCLE

   /** Default constructor.
    *
    * The gene range will be [0, 1] (binary code). The code size will be 16. These properties
    * cannot be changed after construction, so this constructor will be rarely useful.
    */
   DiscreteCode()
      : mUniformRate(0, 1),
        mTwoPointRate(0, 1),
        mRealDist(mRng)
                                                   { Init(
                                                        16,
                                                        0,
                                                        1); }

   /** Constructor taking code size and gene range.
    *
    * @param[in] aCodeSize Solution code size.
    * @param[in] aGeneMin Minimum value of gene.
    * @param[in] aGeneMax Maximum value of gene.
    */
   DiscreteCode(
            CodeSizeType   aCodeSize,
            GeneType       aGeneMin,
            GeneType       aGeneMax)
      : mUniformRate(0, 1),
        mTwoPointRate(0, 1),
        mRealDist(mRng)
                                                   { Init(
                                                        aCodeSize,
                                                        aGeneMin,
                                                        aGeneMax); }

// OPERATIONS

   /** Random generation of a code.
    *
    * @return A const reference to the generated code.
    */
   const CodeType&      Breed() const;

   /** Solution fitness evaluation.
    *
    * @return The fitness value.
    */
   RealType             Fitness(const CodeType& rCode) const
                           {
                              return mFitnessAgent.Eval(rCode);
                           }

   /** Code mutation.
    */
   void                 Mutate(CodeType& rCode) const;

   /** Crossover between two codes.
    */
   void                 Crossover(
                           CodeType& rCodeA,
                           CodeType& rCodeB) const;

// ACCESS

   /** Read only access to the CodeSize parameter.
    *
    * @return The CodeSize value.
    */
   CodeSizeType         CodeSize() const           { return mCodeSize; }

   /** Read only access to the GeneMin parameter.
    *
    * @return The GeneMin value.
    */
   GeneType             GeneMin() const            { return mGeneMin; }

   /** Read only access to the GeneMax parameter.
    *
    * @return The GeneMax value.
    */
   GeneType             GeneMax() const            { return mGeneMax; }

   /** Read/write access to the internal fitness evaluation agent instance.
    *
    * @return A reference to the FitnessAgent object.
    */
   Evaluator&           FitnessAgent()             { return mFitnessAgent; }

   /** Read only access to the internal fitness evaluation agent instance.
    *
    * @return A const reference to the FitnessAgent object.
    */
   const Evaluator&     FitnessAgent() const       { return mFitnessAgent; }

   /** Read/write access to the UniformRate parameter.
    *
    * @return A reference to the UniformRate parameter.
    */
   RealParam&           UniformRate()              { return mUniformRate; }

   /** Read only access to the UniformRate parameter.
    *
    * @return A const reference to the UniformRate parameter.
    */
   const RealParam&     UniformRate() const        { return mUniformRate; }

   /** Read/write access to the TwoPointRate parameter.
    *
    * @return A reference to the TwoPointRate parameter.
    */
   RealParam&           TwoPointRate()             { return mTwoPointRate; }

   /** Read only access to the TwoPointRate parameter.
    *
    * @return A const reference to the TwoPointRate parameter.
    */
   const RealParam&     TwoPointRate() const       { return mTwoPointRate; }

// SETUP

   /** Randomization seeds setup.
    *
    * @param[in] aSeed1 First seed.
    * @param[in] aSeed2 Second seed.
    */
   void                 RandSeedSetup(
                           NaturalType aSeed1,
                           NaturalType aSeed2)
                           {
                              mRng.seed(aSeed1);
                              mRealDist.base().seed(aSeed2);
                           }

   // Maximum number of different individuals given mCodeSize
      NaturalType 		MaxPopulation() const;

private:

   // Typedef privati.
   typedef typename CodeType::iterator
                        CodeIterator;

   // Dimensione codice.
   CodeSizeType         mCodeSize;

   // Valore minimo componente codice (gene).
   GeneType             mGeneMin;

   // Valore massimo componente codice (gene).
   GeneType             mGeneMax;

   // Valutatore fitness.
   Evaluator            mFitnessAgent;

   // Probabilità di crossover uniforme rispetto a quello basato su cut point.
   RealParam            mUniformRate;

   // Probabilità di crossover two point rispetto a quello single point.
   RealParam            mTwoPointRate;

   // BOOST RANDOM
   // Generatore.
   mutable boost::mt19937
                        mRng;

   // Distribuzione per estrazione gene.
   mutable boost::uniform_int<GeneType>
                        mGeneDist;

   // Distribuzione per estrazione cut point.
   mutable boost::uniform_int<CodeSizeType>
                        mCutDist;

   // Distribuzione per estrazione gene da mutare.
   mutable boost::uniform_int<CodeSizeType>
                        mMuteDist;

   // Distribuzione per crossover uniforme.
   mutable boost::uniform_int<char>
                        mBoolDist;

   // Distribuzione per selezione tipo crossover.
   mutable boost::uniform_01<boost::mt19937, RealType>
                        mRealDist;
   // BOOST RANDOM

   // Buffer codice.
   mutable CodeType     mCodeBuffer;

   // Inizializzazione.
   void                 Init(
                           CodeSizeType   aSize,
                           GeneType       aMin,
                           GeneType       aMax);

   // Single point crossover.
   void                 SinglePointCrossover(
                           CodeType&       rCodeA,
                           CodeType&       rCodeB) const;

   // Two point crossover.
   void                 TwoPointCrossover(
                           CodeType&       rCodeA,
                           CodeType&       rCodeB) const;

   // Uniform crossover.
   void                 UniformCrossover(
                           CodeType&       rCodeA,
                           CodeType&       rCodeB) const;

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      CodeSizeType    CodeSize_= mCodeSize;
      GeneType        GeneMin_= mGeneMin;
      GeneType        GeneMax_= mGeneMax;

      ar & mCodeSize;
      ar & mGeneMin;
      ar & mGeneMax;

      if ( (mCodeSize != CodeSize_) || (mGeneMin != GeneMin_) || (mGeneMax != GeneMax_) )
      {
         Init(
            mCodeSize,
            mGeneMin,
            mGeneMax);
      }

      ar & mFitnessAgent;
      ar & mUniformRate;
      ar & mTwoPointRate;
   } // BOOST SERIALIZATION
}; // class DiscreteCode

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename GeneType, typename Evaluator>
const typename DiscreteCode<GeneType, Evaluator>::CodeType&
DiscreteCode<GeneType, Evaluator>::Breed() const
{
   CodeIterator Cit= mCodeBuffer.begin();
   while (mCodeBuffer.end() != Cit)
   {
      (*Cit++)= mGeneDist(mRng);
   }

   return mCodeBuffer;
}  // Breed

template <typename GeneType, typename Evaluator>
void
DiscreteCode<GeneType, Evaluator>::Crossover(
                                         CodeType& rCodeA,
                                         CodeType& rCodeB) const
{
   if (mRealDist() < mUniformRate)
   {
      UniformCrossover(rCodeA, rCodeB);
   }
   else
   {
      if (mRealDist() < mTwoPointRate)
      {
         TwoPointCrossover(rCodeA, rCodeB);
      }
      else
      {
         SinglePointCrossover(rCodeA, rCodeB);
      }
   }
}  // Crossover

template <typename GeneType, typename Evaluator>
void
DiscreteCode<GeneType, Evaluator>::Mutate(CodeType& rCode) const
{
   CodeSizeType MutePoint= mMuteDist(mRng);

   // Random Mutation
   rCode[MutePoint] = mGeneDist(mRng);

   // Small Mutation (+-1)
   /*if (rCode[MutePoint] == mGeneMax)
   {
      --rCode[MutePoint];
   }
   else
   {
      if (rCode[MutePoint] == mGeneMin)
      {
         ++rCode[MutePoint];
      }
      else
      {
         if ( mBoolDist(mRng) )
         {
            --rCode[MutePoint];
         }
         else
         {
            ++rCode[MutePoint];
         }
      }
   }*/
}  // Mutate

////////////////////////////////////// PRIVATE /////////////////////////////////////////////

template <typename GeneType, typename Evaluator>
void
DiscreteCode<GeneType, Evaluator>::Init(
                                    CodeSizeType   aCodeSize,
                                    GeneType       aGeneMin,
                                    GeneType       aGeneMax)
{
   if (aCodeSize < 1)
   {
      throw SpareLogicError("DiscreteCode, 0, CodeSize cannot be zero.");
   }

   if (aGeneMin >= aGeneMax)
   {
      throw SpareLogicError("DiscreteCode, 1, GeneMin must be less than GeneMax.");
   }

   mCodeSize= aCodeSize;
   mGeneMin= aGeneMin;
   mGeneMax= aGeneMax;

   mCodeBuffer.resize(mCodeSize);

   boost::uniform_int<GeneType>       GeneDist(mGeneMin, mGeneMax);
   mGeneDist= GeneDist;

   boost::uniform_int<CodeSizeType>   CutDist(1, mCodeSize-1);
   mCutDist= CutDist;

   boost::uniform_int<CodeSizeType>   MuteDist(0, mCodeSize-1);
   mMuteDist= MuteDist;

   boost::uniform_int<char>           BoolDist(0, 1);
   mBoolDist= BoolDist;

   mUniformRate= 0.33;
   mTwoPointRate= 0.5;
}  // Init

template <typename GeneType, typename Evaluator>
void
DiscreteCode<GeneType, Evaluator>::SinglePointCrossover(
                                                    CodeType& rCodeA,
                                                    CodeType& rCodeB) const
{
   CodeSizeType                   CutPoint;
   CodeSizeType                   i;
   GeneType                       Aux;

   if (mCodeSize > 1)
   {
      CutPoint= mCutDist(mRng);

      for(i= CutPoint; i < mCodeSize; i++)
      {
         Aux= rCodeA[i];
         rCodeA[i]= rCodeB[i];
         rCodeB[i]= Aux;
      }
   }
}  // SinglePointCrossover

template <typename GeneType, typename Evaluator>
void
DiscreteCode<GeneType, Evaluator>::TwoPointCrossover(
                                                 CodeType& rCodeA,
                                                 CodeType& rCodeB) const
{
   CodeSizeType                   CutPoint1;
   CodeSizeType                   CutPoint2;
   CodeSizeType                   i;
   GeneType                       Aux;

   if (mCodeSize > 2)
   {
      CutPoint1= mCutDist(mRng);

      do
      {
         CutPoint2= mCutDist(mRng);
      }
      while (CutPoint1 == CutPoint2);

      if (CutPoint1 > CutPoint2)
      {
         i= CutPoint1;
         CutPoint1= CutPoint2;
         CutPoint2= i;
      }

      for(i= CutPoint1; i < CutPoint2; i++)
      {
         Aux= rCodeA[i];
         rCodeA[i]= rCodeB[i];
         rCodeB[i]= Aux;
      }
   }
   else
   {
      SinglePointCrossover(rCodeA, rCodeB);
   }
}  // TwoPointCrossover

template <typename GeneType, typename Evaluator>
void
DiscreteCode<GeneType, Evaluator>::UniformCrossover(
                                                CodeType& rCodeA,
                                                CodeType& rCodeB) const
{
   CodeSizeType                   i;
   GeneType                       Aux;

   if (mCodeSize > 1)
   {
      for(i= 0; i < mCodeSize; i++)
      {
         if ( mBoolDist(mRng) )
         {
            Aux= rCodeA[i];
            rCodeA[i]= rCodeB[i];
            rCodeB[i]= Aux;
         }
      }
   }
}  // UniformCrossover

template <typename GeneType, typename Evaluator>
NaturalType
DiscreteCode<GeneType, Evaluator>::MaxPopulation() const
{
   return std::min((double)std::numeric_limits<NaturalType>::max(),pow(mGeneMax - mGeneMin + 1,mCodeSize));
}  // MaxPopulation


}  // namespace spare

#endif  // _DiscreteCode_h_
