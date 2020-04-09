//  PiecewiseLinear class, part of the SPARE library.
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

/** @brief File PiecewiseLinear.hpp, containing the PiecewiseLinear class.
 *
 * The file contains the PiecewiseLinear class, implementing a scalar piecewise linear
 * function.
 *
 * @file PiecewiseLinear.hpp
 * @author Guido Del Vescovo
 */

#ifndef _PiecewiseLinear_h_
#define _PiecewiseLinear_h_

// STD INCLUDES
#include <iterator>
#include <limits>
#include <utility>
#include <vector>

// BOOST INCLUDES
#include <boost/numeric/conversion/converter.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusion in namespace spare.

/** @brief Piecewise linear function.
 *
 * The %PiecewiseLinear class models the @a Evaluator concept. The class implements a 
 * continuous piecewise linear function. The split point between two linear regions is called
 * node. The function setup is made by passing a sequence of real values defining the nodes.
 * If \f$N\f$ nodes have to be defined, the format of the \f$2N\f$ long sequence is \f$(x_0, 
 * y_0, x_1, y_1, ..., x_{N-1}, y_{N-1})\f$, where \f$x_i\f$ and \f$y_i\f$ denote the 
 * abscissa and the ordinate of the node, respectively. When performing the setup the passed 
 * nodes must be ordered such that the sequence of the \f$x_i\f$ values is non-decreasing.
 * For \f$x\f$ values falling between two nodes the returned value is the one defined by the
 * linear function joining the two nearest nodes. For \f$x<x_0\f$ the returned value is 
 * \f$y_0\f$, for \f$x>x_{N-1}\f$ the returned value is \f$y_{N-1}\f$. If the value for 
 * \f$x_i\f$ is defined more than once, the one returned for \f$x=x_i\f$ will be the last 
 * one. If no setup is done, a value of zero is always returned.
 */
class PiecewiseLinear
{
public:

   /** Default constructor.
    */
   PiecewiseLinear()
      : mNodes(1)
                           {
                              mNodes.front().first= 0.;
                              mNodes.front().second= 0.;
                           }

// OPERATIONS

   /** Function evaluation.
    *
    * @param[in] aX The input value.
    * @return The function value.
    */
   RealType             Eval(RealType aX) const;

// SETUP

   /** Piecewise linear function setup.
    *
    * A \f$2N\f$ long sequence ordered by non-decreasing values of \f$x_i\f$ must be passed.
    *
    * @param[in] rParams A reference to the container holding the node sequence.
    */
   template <typename SequenceContainer>
   void                 NodeSetup(const SequenceContainer& rParams);

   /** Piecewise linear function setup.
    *
    * A \f$2N\f$ long sequence ordered by non-decreasing values of \f$x_i\f$ must be passed.
    *
    * @param[in] aParams An iterator pair delimiting the node sequence.
    */
   template <typename ForwardIterator>
   void                 NodeSetup(std::pair<ForwardIterator, ForwardIterator> aParams);

private:

   // Tipo nodo.
   typedef std::pair<RealType, RealType>
                        Node;

   // Tipo vettore di nodi.
   typedef std::vector<Node>
                        NodeVector;

   // Nodi.
   NodeVector           mNodes;

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & BOOST_SERIALIZATION_NVP(mNodes);
   } // BOOST SERIALIZATION

}; // class PiecewiseLinear

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

template <typename ForwardIterator>
void
PiecewiseLinear::NodeSetup(std::pair<ForwardIterator, ForwardIterator> aParams)
{
   typedef typename std::iterator_traits<ForwardIterator>::difference_type ParamDiffType;

   ParamDiffType Psz= std::distance(aParams.first, aParams.second);

   #if SPARE_DEBUG
   if (Psz < 2)
   {
      throw SpareLogicError("PiecewiseLinear, 0, Invalid number of parameters.");
   }

   if (Psz % 2)
   {
      throw SpareLogicError("PiecewiseLinear, 1, Invalid number of parameters.");
   }
   #endif

   if (boost::numeric::converter<NodeVector::size_type, ParamDiffType>
       ::convert(Psz/2) > mNodes.max_size())
   {
      throw SpareLogicError("PiecewiseLinear, 2, Too many nodes.");
   }

   mNodes.resize(Psz/2);
   NodeVector::iterator Nit= mNodes.begin();

   while (mNodes.end() != Nit)
   {
      Nit->first= static_cast<RealType>(*aParams.first++);
      Nit->second= static_cast<RealType>(*aParams.first++);

      if (aParams.first != aParams.second)
      {
         if (*aParams.first < Nit->first)
         {
            throw SpareLogicError("PiecewiseLinear, 3, Invalid node position.");
         }
      }

      ++Nit;
   }
}

template <typename SequenceContainer>
void
PiecewiseLinear::NodeSetup(const SequenceContainer& rParams)
{
   typename SequenceContainer::size_type
                        Psz= rParams.size();

   #if SPARE_DEBUG
   if (Psz < 2)
   {
      throw SpareLogicError("PiecewiseLinear, 4, Invalid number of parameters.");
   }

   if (Psz % 2)
   {
      throw SpareLogicError("PiecewiseLinear, 5, Invalid number of parameters.");
   }
   #endif

   if ((Psz/2) > mNodes.max_size())
   {
      throw SpareLogicError("PiecewiseLinear, 6, Too many nodes.");
   }

   mNodes.resize(Psz/2);
   NodeVector::iterator                       Nit= mNodes.begin();
   typename SequenceContainer::const_iterator Sit= rParams.begin();

   while (mNodes.end() != Nit)
   {
      Nit->first= static_cast<RealType>(*Sit++);
      Nit->second= static_cast<RealType>(*Sit++);

      if (rParams.end() != Sit)
      {
         if (*Sit < Nit->first)
         {
            throw SpareLogicError("PiecewiseLinear, 7, Invalid node position.");
         }
      }

      ++Nit;
   }
}

inline RealType
PiecewiseLinear::Eval(RealType aX) const
{
   NodeVector::const_iterator Nit= mNodes.begin();
   while (mNodes.end() != Nit)
   {
      if (Nit->first > aX)
      {
         break;
      }
      else
      {
         ++Nit;
      }
   }

   if (Nit == mNodes.begin())
   {
      return Nit->second;
   }

   if (Nit == mNodes.end())
   {
      return (--Nit)->second;
   }

   RealType a, b, c, d;
   c= Nit->first;
   d= (*Nit--).second;
   a= Nit->first;
   b= Nit->second;

   if ((c-a) < std::numeric_limits<RealType>::min())
   {
      throw SpareLogicError("PiecewiseLinear, 8, Numeric trouble.");
   }

   return (b*(c-aX) + d*(aX-a)) / (c-a);
}

}  // namespace spare

#endif // _PiecewiseLinear_h_
