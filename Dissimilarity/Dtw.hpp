//  Dtw class, part of the SPARE library.
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

/** @brief File Dtw.hpp, containing the Dtw template class.
 *
 * The file contains the Dtw template class, implementing the Dynamic Time Warping algorithm
 * for computing a dissimilarity measure between two generic sequences of objects.
 *
 * @file Dtw.hpp
 * @author Guido Del Vescovo
 */

#ifndef _Dtw_h_
#define _Dtw_h_

// STD INCLUDES
#include <algorithm>
#include <iterator>
#include <limits>
#include <utility>
#include <vector>

// BOOST INCLUDES
#include <boost/numeric/conversion/converter.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/access.hpp>

// SPARE INCLUDES
#include <spare/BoundedParameter.hpp>
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>
#include <spare/SwitchParameter.hpp>

namespace spare {  // Inclusion in namespace spare.

// Dati per costruzione parametro switch.
static const std::string LVSTH_NSVAL[]= {"Off", "On"};
static const size_t      LVSTH_NSVAL_SZ= 2;


/** @brief Dynamic Time Warping (DTW) class.
 *
 * This class models the @a Dissimilarity concept. Two sequences of objects (called nodes) 
 * are compared using a dynamic programming approach. The algorithm needs a dissimilarity 
 * measure between two single nodes to be defined. The node dissimilarity measure is the 
 * template argument. Once the template has been specialized for a certain dissimilarity 
 * measure, it can compute the DTW dissimilarity between two sequences of objects, which must 
 * meet the requirement of being acceptable arguments for the Diss methods of the chosen 
 * dissimilariy agent.
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
 *     <td class="indexvalue">W</td>
 *     <td class="indexvalue">[0, inf)</td>
 *     <td class="indexvalue">Locality constraint window.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">0</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">Normalization</td>
 *     <td class="indexvalue">{On, Off}</td>
 *     <td class="indexvalue">If set to "On", the evaluated distance is normalized in the
 *                            range [0, 1].</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">Off</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">MaxDissiValue</td>
 *     <td class="indexvalue">Reals</td>
 *     <td class="indexvalue">User-defined real valued maximum dissimilarity value.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">1</td>
 *  </tr>
 *  </table>  
 */
template <typename Dissimilarity>
class Dtw
{
public:

// PUBLIC TYPES

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
   Dtw()
      : mW(0, std::numeric_limits<NaturalType>::max()),
        mNormalization(LVSTH_NSVAL,
                               LVSTH_NSVAL + LVSTH_NSVAL_SZ)
                           {
                              mW= 0;
                              mNormalization= "Off";
                              mMaxDissValue= 1;
                           }

// OPERATIONS

   /** Dissimilarity evaluation with iterator interface.
    *
    * @param[in] aA A pair of iterators delimiting the first sequence.
    * @param[in] aB A pair of iterators delimiting the second sequence.
    * @return The dissimilarity value.
    */
   template <typename ForwardIterator1, typename ForwardIterator2>
   RealType             Diss(
                           std::pair<ForwardIterator1, ForwardIterator1> aA,
                           std::pair<ForwardIterator2, ForwardIterator2> aB) const;

   /** Dissimilarity evaluation with container interface.
    *
    * @param[in] rA A reference to the first sequence container.
    * @param[in] rB A reference to the second sequence container.
    * @return The dissimilarity value.
    */
   template <typename SequenceContainer1, typename SequenceContainer2>
   RealType             Diss(
                           const SequenceContainer1& rA,
                           const SequenceContainer2& rB) const
                           {
                              return Diss(
                                        std::make_pair( rA.begin(), rA.end() ),
                                        std::make_pair( rB.begin(), rB.end() ) );
                           }

// ACCESS

   /** Read/Write access to the value of W, the locality constraint window.
    *
    * @return A reference to the value of W.
    */
   NaturalParam&        W()                        { return mW; }

   /** Read access to the value of W, the locality constraint window.
    *
    * @return A reference to the value of W.
    */
   const NaturalParam&  W() const                  { return mW; }

   /** Read/write access to the dissimilarity agent instance.
    *
    * @return A reference to the dissimilarity agent instance.
    */
   Dissimilarity&       DissAgent()                { return mDissAgent; }

   /** Read only access to the dissimilarity agent instance.
    *
    * @return A const reference to the dissimilarity agent instance.
    */
   const Dissimilarity& DissAgent() const          { return mDissAgent; }

   /** Read/Write access to the normalization flag.
    *
    * @return A reference to the Normalization value.
    */
   StringParam&         Normalization()            { return mNormalization; }

   /** Read access to the normalization flag.
    *
    * @return A reference to the Normalization flag value.
    */
   const StringParam&   Normalization() const      { return mNormalization; }

   /** Read/Write access to the used-defined maximum dissimilarity value.
    *
    * @return A reference to the used-defined maximum dissimilarity value.
    */
   RealType&             MaxDissValue()             { return mMaxDissValue; }

   /** Read access to the used-defined maximum dissimilarity value.
    *
    * @return A reference to the used-defined maximum dissimilarity value.
    */
   const RealType&       MaxDissValue() const       { return mMaxDissValue; }

private:

   // Typedef.
   typedef
      std::vector<std::vector<NaturalType> >::size_type
         RowIndexType;

   typedef
      std::vector<NaturalType>::size_type
         ColIndexType;

   // Finestra per locality constraint.
   NaturalParam         mW;

   // User-defined maximum dissimilarity value
   RealType                mMaxDissValue;

   // Switch normalizzazione.
   StringParam          mNormalization;

   // Istanza classe misuratrice di dissimilarità.
   Dissimilarity        mDissAgent;

   // Matrice per calcolo interno.
   mutable std::vector<std::vector<RealType> >
                        mMat;

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & BOOST_SERIALIZATION_NVP(mW);
      ar & BOOST_SERIALIZATION_NVP(mDissAgent);
   } // BOOST SERIALIZATION

}; // class Dtw

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename Dissimilarity>
template <typename ForwardIterator1, typename ForwardIterator2>
RealType
Dtw<Dissimilarity>::Diss(
                     std::pair<ForwardIterator1, ForwardIterator1> aA,
                     std::pair<ForwardIterator2, ForwardIterator2> aB) const
{
   // Variabili.
   ForwardIterator1  Ait;
   ForwardIterator2  Bit;
   RealType          Cost;
   RowIndexType      M;
   RealType          M_;
   ColIndexType      N;
   RealType          N_;
   RowIndexType      i;
   ColIndexType      j;
   NaturalType       s;

   // Controllo.
   #if SPARE_DEBUG
   if ( std::distance(aA.first, aA.second) < 0 )
   {
      throw SpareLogicError("Dtw, 0, Invalid range.");
   }

   if ( std::distance(aB.first, aB.second) < 0 )
   {
      throw SpareLogicError("Dtw, 1, Invalid range.");
   }
   #endif

   // Typedef.
   typedef
      typename std::iterator_traits<ForwardIterator1>::difference_type
         DiffType1;

   typedef
      typename std::iterator_traits<ForwardIterator2>::difference_type
         DiffType2;

   M= boost::numeric::converter<RowIndexType, DiffType1>::convert(
      std::distance(aA.first, aA.second) );

   N= boost::numeric::converter<ColIndexType, DiffType2>::convert(
      std::distance(aB.first, aB.second) );

   M_= boost::numeric::converter<RealType, RowIndexType>::convert(M);
   N_= boost::numeric::converter<RealType, ColIndexType>::convert(N);

   if ((M == 0) && (N == 0))
   {
      return RealType(0.);
   }

   // Dimensiono e inizializzo matrice.
   mMat.resize(M+1);

   for (i= 0; i <= M; i++)
   {
      mMat[i].resize(N+1);
   }

   for (i= 0; i <= M; i++)
   {
      for (j= 0; j <= N; j++)
      {
         mMat[i][j]= std::numeric_limits<RealType>::max();
      }
   }

   mMat[0][0]= 0.;

   // Calcolo distanza.
   i= 1;
   for (Ait= aA.first; Ait != aA.second; Ait++)
   {
      j= 1;
      for (Bit= aB.first; Bit != aB.second; Bit++)
      {
         // FIXME: si potrebbe rendere più efficiente agendo direttamente sul ciclo
         if (i > j)
         {
            s= i - j;
         }
         else
         {
            s= j - i;
         }

         if ((s <= mW) || !(mW))
         {
            Cost= mDissAgent.Diss(*Ait, *Bit);
            mMat[i][j]= Cost + std::min(
                                       mMat[i-1][j],
                                       std::min(
                                               mMat[i][j-1],
                                               mMat[i-1][j-1]) );
         }

         ++j;
      }
      ++i;
   }


   if (mNormalization == "On")
   {
      return mMat[M][N] / (mMaxDissValue*std::max(M_, N_));
   }
   else
      return mMat[M][N];

}  // Diss

}  // namespace spare

#endif  // _Dtw_h_
