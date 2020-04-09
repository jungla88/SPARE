//  MTBsas class, part of the SPARE library.
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

/** @brief File MTBsas.hpp, containing the %Bsas template class.
 *
 * The file contains the %Bsas template class, implementing the multi-threaded version of a clustering algorithm known as
 * Basic Sequential Algorithmic Scheme. The algorithm is provided in two versions, basic and
 * modified.
 *
 * @file MTBsas.hpp
 * @author Lorenzo Livi
 */

#ifndef _MTBsas_h_
#define _MTBsas_h_

// STD INCLUDES
#include <algorithm>
#include <iterator>
#include <limits>
#include <string>
#include <vector>

// BOOST INCLUDES
#include <boost/numeric/conversion/converter.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

// SPARE INCLUDES
#include <spare/BoundedParameter.hpp>
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>
#include <spare/SwitchParameter.hpp>

namespace spare {  // Inclusion in namespace spare.

// Data for switch parameter construction.
static const std::string BSAS_SCVAL[]= {"Basic", "Modified"};
static const size_t      BSAS_SCVAL_SZ= 2;

/** @brief %Bsas clustering algorithm.
 *
 * %MTBsas is a template class which models the @a Clustering concept. Its template argument
 * is a class modeling the @a Representative concept. It is a multithreaded implementation that
 * computes the representatives/test sample dissimilarity in parallel, using a used-defined
 * number of threads.
 * The selected Representative class is in
 * charge to build the abstract representations of the clusters during their aggregation. The
 * main method is Process, which performs the clustering procedure. The sample type (i.e. the
 * input data type) must be a type which can be passed as argument to the Update and Diss
 * methods of the selected Representative class. The output LabelType is an integer marking
 * the different clusters. Each new cluster is initialized with an internal prototype of
 * Representative, called @a representative @a initializer. The initializer is usually an
 * empty representative which can be edited by the user with the aim of customising the
 * behavior of the cluster models in some way, for example setting some dissimilarity measure
 * parameters.
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
 *     <td class="indexvalue">Scheme</td>
 *     <td class="indexvalue">{Basic, Modified}</td>
 *     <td class="indexvalue">Choice of the algoithm variant to use.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">Modified</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">Theta</td>
 *     <td class="indexvalue">[0, inf)</td>
 *     <td class="indexvalue">Clustering threshold.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">0.5</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">Q</td>
 *     <td class="indexvalue">[1, inf)</td>
 *     <td class="indexvalue">Maximum number of clusters to be generated.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">100</td>
 *  </tr>
 *  </table>  
 */
template <typename Representative, NaturalType NThreads>
class MTBsas
{
public:

// PUBLIC TYPES

   /** Type of container storing the generated representatives.
    */
   typedef std::vector<Representative>
                        RepVector;

   /** Type of label type assigned to the processed samples.
    */
   typedef typename RepVector::size_type
                        LabelType;

   /** Container of the defined output labels.
    */
   typedef std::vector<LabelType>
                        LabelVector;

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
   MTBsas()
      : mScheme(BSAS_SCVAL, BSAS_SCVAL + BSAS_SCVAL_SZ),
        mTheta( 0, std::numeric_limits<RealType>::max() ),
        mQ( 1, std::numeric_limits<NaturalType>::max() )
                           {
                              mScheme= "Modified";
                              mTheta= 0.5;
                              mQ= 100;
                           }

// OPERATIONS

   /** Cluster analysis execution.
    *
    * @param[in] iSampleBegin Iterator pointing to the first sample.
    * @param[in] iSampleEnd Iterator pointing to the first position after the last sample.
    * @param[out] iLabelBegin Iterator pointing to the first label.
    */
   template <typename ForwardIterator1, typename ForwardIterator2>
   void                 Process(
                           ForwardIterator1  iSampleBegin,
                           ForwardIterator1  iSampleEnd,
                           ForwardIterator2  iLabelBegin)
                                                   {
                                                      if (mScheme == "Basic")
                                                      {
                                                         BasicClusterAnalysis(
                                                           iSampleBegin,
                                                           iSampleEnd,
                                                           iLabelBegin);
                                                      }
                                                      else
                                                      {
                                                         ModifiedClusterAnalysis(
                                                           iSampleBegin,
                                                           iSampleEnd,
                                                           iLabelBegin);
                                                      }
                                                   }

// ACCESS

   /** Read/write access to the Scheme parameter.
    *
    * @return A reference to the Scheme parameter.
    */
   StringParam&         Scheme()                   { return mScheme; }

   /** Read only access to the Scheme parameter.
    *
    * @return A const reference to the Scheme parameter.
    */
   const StringParam&   Scheme() const             { return mScheme; }

   /** Read/write access to the Theta parameter.
    *
    * @return A reference to the Theta parameter.
    */
   RealParam&           Theta()                    { return mTheta; }

   /** Read only access to the Theta parameter.
    *
    * @return A const reference to the Theta parameter.
    */
   const RealParam&     Theta() const              { return mTheta; }

   /** Read/write access to the Q parameter.
    *
    * @return A reference to the Q parameter.
    */
   NaturalParam&        Q()                        { return mQ; }

   /** Read only access to the Q parameter.
    *
    * @return A const reference to the Q parameter.
    */
   const NaturalParam&  Q() const                  { return mQ; }

   /** Read/write access to the representative intializer.
    *
    * @return A reference to the instance.
    */
   Representative&      RepInit()                  { return mRepInit; }

   /** Read only access to the representative intializer.
    *
    * @return A const reference to the instance.
    */
   const Representative&
                        RepInit() const            { return mRepInit; }

   /** Read access to the container holding the defined output labels.
    *
    * The container holds the valid labels defined during the last call of the Process
    * method. In other words, in the container we find the id's of the generated clusters.
    * We also know how many clusters have been found just looking at the size of the
    * container.
    *
    * @return A const reference to the container of the defined labels.
    */
   const LabelVector&   GetLabels() const          { return mLabels; }

   /** Read access to the container holding the generated representatives.
    *
    * The container holds the representatives associated with the labels returned by the
    * GetLabels method, in the same order.
    *
    * @return A const reference to the container of the generated representatives.
    */
   const RepVector&     GetRepresentatives() const { return mRepresentatives; }

private:

   // Typedef privati.
   typedef typename RepVector::const_iterator
                        RepresentativeIterator;

   typedef std::iterator_traits<std::vector<RealType>::iterator>::difference_type
                        RealDiffType;

   typedef std::vector<RealType>::size_type
                        DissSizeType;

   typedef std::vector<RealType>::iterator
                        DissIterator;

   typedef typename LabelVector::size_type
                        LabelVectorSizeType;

   // Schema algoritmico (base o modificato).
   StringParam          mScheme;

   // Soglia di clustering.
   RealParam            mTheta;

   // Massimo numero di cluster.
   NaturalParam         mQ;

   // Inizializzatore cluster.
   Representative       mRepInit;

   // Rappresentanti memorizzati internamente.
   RepVector            mRepresentatives;

   // Lista etichette rappresentanti.
   LabelVector          mLabels;

   // Cluster analysis con schema di base.
   template <typename ForwardIterator1, typename ForwardIterator2>
   void                 BasicClusterAnalysis(
                           ForwardIterator1  iSampleBegin,
                           ForwardIterator1  iSampleEnd,
                           ForwardIterator2  iLabelBegin);

   // Cluster analysis con schema modificato.
   template <typename ForwardIterator1, typename ForwardIterator2>
   void                 ModifiedClusterAnalysis(
                           ForwardIterator1  iSampleBegin,
                           ForwardIterator1  iSampleEnd,
                           ForwardIterator2  iLabelBegin);

   /**
    * Threads function
    */
   template <typename ForwardIterator1>
   void ThreadFunction(RepresentativeIterator* RepIt, ForwardIterator1* SampleIt);


   // Funzione ausiliaria.
   void                 AlgoInit();

   // Variabili ad uso di calcolo interno.

   // Indice rappresentante più simile.
   LabelType            ClosestRep;

   // Max numero di cluster (= Q).
   LabelType            Q_;

   // Numero di campioni.
   LabelVectorSizeType  S;

   // Numero max etichette (= Q).
   LabelVectorSizeType  Q__;

   // Iteratore rappresentanti.
   RepresentativeIterator
                        RepIt;

   // Iteratore dissimilarità.
   DissIterator         MinIt;

   // Minima dissimilarità.
   RealType             MinDiss;

   // Vettore dissimilarità.
   std::vector<RealType>
                        RepDiss;

   //mutex
   boost::mutex mMutex;
   boost::mutex mAddMutex;


   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & mScheme;
      ar & mTheta;
      ar & mQ;
      ar & mRepInit;
      ar & mRepresentatives;
      ar & mLabels;
   } // BOOST SERIALIZATION

}; // class MTBsas

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename Representative, NaturalType NThreads>
template <typename ForwardIterator1, typename ForwardIterator2>
void
MTBsas<Representative, NThreads>::BasicClusterAnalysis(
                         ForwardIterator1  iSampleBegin,
                         ForwardIterator1  iSampleEnd,
                         ForwardIterator2  iLabelBegin)
{
   // Typedef locali.
   typedef typename std::iterator_traits<ForwardIterator1>::difference_type
                        SampleDiffType;

   // Iteratore principale dati.
   ForwardIterator1     It;
   // Iteratore principale etichette.
   ForwardIterator2     Ot;


   // Ricavo il numero S di campioni.
   S= boost::numeric::converter<LabelVectorSizeType, SampleDiffType>::convert(
                                                                       std::distance(
                                                                          iSampleBegin,
                                                                          iSampleEnd) );

   // Inizializzo.
   AlgoInit();

   // Punto al primo campione e alla prima etichetta.
   It= iSampleBegin;
   Ot= iLabelBegin;

   // Inizializzo il primo rappresentante con il primo campione.
   mLabels.push_back( mRepresentatives.size() );
   (*Ot++)= mLabels.back();
   mRepresentatives.push_back( mRepInit );
   mRepresentatives.back().Update( *It++ );

   // Ciclo principale.
   while (iSampleEnd != It)
   {
      boost::thread_group threadGroup;
      RepDiss.clear();
      RepIt= mRepresentatives.begin();


      for(NaturalType i=0;i<NThreads;i++)
          threadGroup.add_thread(new boost::thread(
                  boost::bind(&MTBsas::ThreadFunction<ForwardIterator1>, this, &RepIt, &It)) );

      threadGroup.join_all();


      MinIt= std::min_element(
                RepDiss.begin(),
                RepDiss.end() );

      MinDiss= *MinIt;
      ClosestRep= boost::numeric::converter<LabelType, RealDiffType>
                  ::convert( std::distance(RepDiss.begin(), MinIt) );

      //new representative
      if ( (MinDiss > mTheta) && (mRepresentatives.size() < Q_) )
      {
         mLabels.push_back( mRepresentatives.size() );
         (*Ot++)= mLabels.back();
         mRepresentatives.push_back( mRepInit );
         mRepresentatives.back().Update( *It );
      }
      else
      {
         (*Ot++)= ClosestRep;
         mRepresentatives[ClosestRep].Update(*It);
      }

      It++;
   } // ciclo principale

}  // BasicClusterAnalysis

template <typename Representative, NaturalType NThreads>
template <typename ForwardIterator1, typename ForwardIterator2>
void
MTBsas<Representative, NThreads>::ModifiedClusterAnalysis(
                         ForwardIterator1  iSampleBegin,
                         ForwardIterator1  iSampleEnd,
                         ForwardIterator2  iLabelBegin)
{
   // Typedef locali.
   typedef typename std::iterator_traits<ForwardIterator1>::difference_type
                        SampleDiffType;

   // Iteratore principale dati.
   ForwardIterator1     It;
   // Iteratore principale etichette.
   ForwardIterator2     Ot;

   // Ricavo il numero S di campioni.
   S= boost::numeric::converter<LabelVectorSizeType, SampleDiffType>::convert(
                                                                       std::distance(
                                                                          iSampleBegin,
                                                                          iSampleEnd) );

   // Inizializzo.
   AlgoInit();

   // Punto al primo campione e alla prima etichetta.
   It= iSampleBegin;
   Ot= iLabelBegin;

   // Inizializzo il primo rappresentante con il primo campione.
   mLabels.push_back( mRepresentatives.size() );
   (*Ot++)= mLabels.back();
   mRepresentatives.push_back( mRepInit );
   mRepresentatives.back().Update( *It++ );


   // Prima passata.
   while (iSampleEnd != It)
   {
      boost::thread_group threadGroup;
      RepDiss.clear();
      RepIt= mRepresentatives.begin();

      //threads
      for(NaturalType i=0;i<NThreads;i++)
          threadGroup.add_thread(new boost::thread(
                          boost::bind(&MTBsas::ThreadFunction<ForwardIterator1>, this, &RepIt, &It)) );

      threadGroup.join_all();

      MinIt= std::min_element(
                RepDiss.begin(),
                RepDiss.end() );

      MinDiss= *MinIt;
      ClosestRep= boost::numeric::converter<LabelType, RealDiffType>
                  ::convert( std::distance(RepDiss.begin(), MinIt) );

      //new representative
      if ( (MinDiss > mTheta) && (mRepresentatives.size() < Q_) )
      {
         mLabels.push_back( mRepresentatives.size() );
         (*Ot++)= mLabels.back();
         mRepresentatives.push_back( mRepInit );
         mRepresentatives.back().Update( *It );
      }
      else
      {
         (*Ot++)= Q_; // Placeholder.
      }

      It++;
   } // prima passata

   // Seconda passata.

   // Punto di nuovo al primo campione e alla prima etichetta.
   It= iSampleBegin;
   Ot= iLabelBegin;

   // Seconda passata.
   while (iSampleEnd != It)
   {
      boost::thread_group threadGroup;
      if (*Ot == Q_) // Non ancora assegnato.
      {
         RepDiss.clear();
         RepIt= mRepresentatives.begin();

         for(NaturalType i=0;i<NThreads;i++)
             threadGroup.add_thread(new boost::thread(
                     boost::bind(&MTBsas::ThreadFunction<ForwardIterator1>, this, &RepIt, &It)) );

         threadGroup.join_all();

         MinIt= std::min_element(
                   RepDiss.begin(),
                   RepDiss.end() );

         MinDiss= *MinIt;
         ClosestRep= boost::numeric::converter<LabelType, RealDiffType>
                     ::convert( std::distance(RepDiss.begin(), MinIt) );

         (*Ot)= ClosestRep;
         mRepresentatives[ClosestRep].Update( *It );
      }

      Ot++;
      It++;
   } // seconda passata

}  // ModifiedClusterAnalysis


template <class Representative, NaturalType NThreads>
template <typename ForwardIterator1>
void MTBsas<Representative, NThreads>::ThreadFunction(RepresentativeIterator* RepIt, ForwardIterator1* SampleIt)
{
    Representative localRep;
    RealType localDiss=0;

    do
    {
        mMutex.lock();
        if(mRepresentatives.end()==(*RepIt))
        {
            mMutex.unlock();
            break;
        }
        else
        {
            localRep=*(*RepIt);
            (*RepIt)++;
        }
        mMutex.unlock();

        //parallel computation of the Representative<->Sample dissimilarity
        localDiss=localRep.Diss(*(*SampleIt));

        mAddMutex.lock();
        RepDiss.push_back(localDiss);
        mAddMutex.unlock();

    }while(true);
}


////////////////////////////////////// PRIVATE /////////////////////////////////////////////

// Funzione ausiliaria.
template <typename Representative, NaturalType NThreads>
void
MTBsas<Representative, NThreads>::AlgoInit()
{
   // Converto il valore di mQ.
   Q_= boost::numeric::converter<LabelType, NaturalType>::convert(mQ);
   Q__= boost::numeric::converter<LabelVectorSizeType, NaturalType>::convert(mQ);

   // Controllo se ho almeno 1 campione.
   if (S < 1)
   {
      throw SpareLogicError("Bsas, 0, Invalid sample range.");
   }

   // Cancello vecchi rappresentanti.
   mRepresentatives.clear();
   mLabels.clear();

   // I vector ce la possono fare ?
   if ( ( Q_ > mRepresentatives.max_size() ) || ( Q__ > mLabels.max_size() ) )
   {
      throw SpareLogicError("Bsas, 1, Too large sample number or Q.");
   }

   // Riservo spazio sufficiente in memoria, per non ri-allocare dinamicamente.
   mRepresentatives.reserve(Q_);
   mLabels.reserve(Q__);
   RepDiss.reserve( boost::numeric::converter<DissSizeType, LabelType>::convert(Q_) );
}  // AlgoInit

}  // namespace spare

#endif  // _MTBsas_h_
