//  Levenshtein class, part of the SPARE library.
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

/** @brief File Levenshtein.hpp, that contains the Levenshtein distance.
 *
 * Contains the declaration of the Levenshtein class.
 *
 * @file Levenshtein.hpp
 * @author Guido Del Vescovo
 */

#ifndef _Levenshtein_h_
#define _Levenshtein_h_

// STD INCLUDES
#include <algorithm>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

// BOOST INCLUDES
#include <boost/numeric/conversion/converter.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>
#include <spare/SwitchParameter.hpp>

namespace spare {  // Inclusione in namespace spare.

// Dati per costruzione parametro switch.
static const std::string LVSTH_NSVAL[]= {"Off", "On"};
static const size_t      LVSTH_NSVAL_SZ= 2;

/** @brief %Levenshtein (edit) distance.
 *
 * This class implements the @a Dissimilarity concept.
 * This class implements the particular type of edit distance called Levenshtein distance,
 * generalized to a generic input space where is defined a distance between patterns.
 * The calculated distance could be normalized in [0, 1] setting the proper parameter.
 * This implementation uses a static set of edit costs.
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
 *     <td class="indexvalue">Normalization</td>
 *     <td class="indexvalue">{On, Off}</td>
 *     <td class="indexvalue">If set to "On", the evaluated distance is normalized in the
 *                            range [0, 1].</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">Off</td>
 *  </tr>
 *  </table>
 */
class Levenshtein
{
public:

// PUBLIC TYPES

   /** Switch parameter.
    */
   typedef SwitchParameter<std::string>
                        StringParam;

// LIFECYCLE

   /** Default constructor.
    */
   Levenshtein()
      : mNormalization(LVSTH_NSVAL,
                       LVSTH_NSVAL + LVSTH_NSVAL_SZ)
                                                   { 
                                                      mNormalization= "Off"; 
                                                   }

// OPERATIONS

   /** Levenshtein distance computation.
    *
    * @param[in] aA A pair of iterator of the first vector.
    * @param[in] aB A pair of iterator of the second vector.
    * @return The value of the distance.
    */
   template <typename ForwardIterator1, typename ForwardIterator2>
   RealType             Diss(
                           std::pair<ForwardIterator1, ForwardIterator1> aA,
                           std::pair<ForwardIterator2, ForwardIterator2> aB) const;

   /** Levenshtein distance computation.
    *
    * @param[in] rA A reference to the first container.
    * @param[in] rB A reference to the second container.
    * @return Il valore della distanza.
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

   /** Read/Write access to the normalization flag.
    *
    * @return A reference to the Normalization value.
    */
   StringParam&         Normalization()            { return mNormalization; }

   /** Read access to the normalization flag.
    *
    * @return A reference to the Normalization value.
    */
   const StringParam&   Normalization() const      { return mNormalization; }

private:

   // Typedef.
   typedef
      std::vector<std::vector<NaturalType> >::size_type
         RowIndexType;

   typedef
      std::vector<NaturalType>::size_type
         ColIndexType;

   // Switch normalizzazione.
   StringParam          mNormalization;

   // Matrice per calcolo interno.
   mutable std::vector<std::vector<NaturalType> >
                        mMat;

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & BOOST_SERIALIZATION_NVP(mNormalization);
   } // BOOST SERIALIZATION

}; // class Levenshtein

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename ForwardIterator1, typename ForwardIterator2>
RealType
Levenshtein::Diss(
                 std::pair<ForwardIterator1, ForwardIterator1> aA,
                 std::pair<ForwardIterator2, ForwardIterator2> aB) const
{
   // Variabili.
   ForwardIterator1  Ait;
   ForwardIterator2  Bit;
   NaturalType       Cost;
   RowIndexType      M;
   RealType          M_;
   ColIndexType      N;
   RealType          N_;
   RowIndexType      i;
   ColIndexType      j;

   // Controllo.
   #if SPARE_DEBUG
   if ( std::distance(aA.first, aA.second) < 0 )
   {
      throw SpareLogicError("Levenshtein, 0, Invalid range.");
   }

   if ( std::distance(aB.first, aB.second) < 0 )
   {
      throw SpareLogicError("Levenshtein, 1, Invalid range.");
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

   if (M == 0)
   {
      if (mNormalization == "On")
      {
         return RealType(1.);
      }
      else
      {
         return N_;
      }
   }

   if (N == 0)
   {
      if (mNormalization == "On")
      {
         return RealType(1.);
      }
      else
      {
         return M_;
      }
   }

   // Dimensiono e inizializzo matrice.
   mMat.resize(M+1);

   for (i= 0; i <= M; i++)
   {
      mMat[i].resize(N+1);
   }

   for (i= 0; i <= M; i++)
   {
      mMat[i][0]= boost::numeric::converter<NaturalType, RowIndexType>::convert(i);
   }

   for (j= 0; j <= N; j++)
   {
      mMat[0][j]= boost::numeric::converter<NaturalType, ColIndexType>::convert(j);
   }

   // Calcolo distanza.
   i= 1;
   for (Ait= aA.first; Ait != aA.second; Ait++)
   {
      j= 1;
      for (Bit= aB.first; Bit != aB.second; Bit++)
      {
         if (*Ait == *Bit)
         {
            Cost= 0;
         }
         else
         {
            Cost= 1;
         }

         mMat[i][j]= std::min(
                             mMat[i-1][j]+1,
                             std::min(
                                     mMat[i][j-1]+1,
                                     mMat[i-1][j-1]+Cost) );

         j++;
      }

      i++;
   }

   if (mNormalization == "On")
   {
      return NaturalToReal::convert( mMat[M][N] ) / std::max(M_, N_);
   }
   else
   {
      return NaturalToReal::convert( mMat[M][N] );
   }
}  // Diss

}  // namespace spare

#endif  // _Levenshtein_h_
