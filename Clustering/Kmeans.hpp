//  Kmeans class, part of the SPARE library.
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

/** @brief File Kmeans.hpp, containing the %Kmeans template class.
 *
 * The file contains the %Kmeans template class, implementing a clustering algorithm known as
 * K-means or C-means.
 *
 * @file Kmeans.hpp
 * @author Guido Del Vescovo (updated by Lorenzo Livi, Filippo Maria Bianchi, Antonio Di Noia)
 */

#ifndef _Kmeans_h_
#define _Kmeans_h_

// STD INCLUDES
#include <algorithm>
#include <iterator>
#include <limits>
#include <vector>

// BOOST INCLUDES
#include <boost/numeric/conversion/converter.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

// SPARE INCLUDES
#include <spare/BoundedParameter.hpp>
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>

namespace spare {  // Inclusion in namespace spare.

/** @brief K-means clustering algorithm.
 *
 * %Kmeans is a template class which models the @a Clustering concept. Its first template argument
 * is a class modeling the @a Representative concept. The selected Representative class is in
 * charge to build the abstract representations of the clusters during their aggregation.
 * The second template argument instead implements the Kmeans initialization scheme, which is in charge of defining the initial K representatives.
 * The main method is Process, which performs the clustering procedure. The sample type (i.e. the
 * input data type) must be a type which can be passed as argument to the Update and Diss
 * methods of the selected Representative class. The output LabelType is an integer marking
 * the different clusters. Each new cluster is initialized with an internal prototype of
 * Representative, called @a representative @a initializer. The cluster object initializer is usually an
 * empty representative object which can be edited by the user with the aim of customising the
 * behavior of the cluster models in some way, for example setting some dissimilarity measure
 * parameters.
 * The implemented stop condition is a logical OR combining the a maximum number of iterations, and a (dynamic) check veryfing if the partition has sifficiently changed during the last iterations.
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
 *     <td class="indexvalue">K</td>
 *     <td class="indexvalue">[1, inf)</td>
 *     <td class="indexvalue">Tentative number of cluster to generate (less clusters could be
                              found).</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">2</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">MaxIter</td>
 *     <td class="indexvalue">[1, inf)</td>
 *     <td class="indexvalue">Maximum number of iterations.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">100</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">MinChangeThreshold</td>
 *     <td class="indexvalue">[0, inf)</td>
 *     <td class="indexvalue">Minimum amount of variation observed among two consecutive partitions to stop algorithm iteration.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">0</td>
 *  </tr>
 *  </table>
 */
template <typename Representative, typename Initialization>
class Kmeans
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

   /** Integer parameter.
    */
   typedef BoundedParameter<NaturalType>
                        NaturalParam;

// LIFECYCLE

   /** Default constructor
    */
   Kmeans()
      : mK( 1, std::numeric_limits<NaturalType>::max() ),
        mMaxIter( 1, std::numeric_limits<NaturalType>::max() )
                           {
                              mK= 2;
                              mMaxIter= 100;
                              mNumPerformedIterations=0;
                              mMinChangeThreshold=0;
                              mMinChange=0;
							  mDynThreshold=false; // ** june 2013 - DNA
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
                           ForwardIterator2  iLabelBegin);

// ACCESS

   /** Read/write access to the K parameter.
    *
    * @return A reference to the K parameter.
    */
   NaturalParam&        K()                        { return mK; }

   /** Read only access to the K parameter.
    *
    * @return A const reference to the K parameter.
    */
   const NaturalParam&  K() const                  { return mK; }

   /** Read/write access to the MaxIter parameter.
    *
    * @return A reference to the MaxIter parameter.
    */
   NaturalParam&        MaxIter()                  { return mMaxIter; }

   /** Read only access to the MaxIter parameter.
    *
    * @return A const reference to the MaxIter parameter.
    */
   const NaturalParam&  MaxIter() const            { return mMaxIter; }

   /**
    * Read-only access to the number of effectively performed iterations.
    *
    * @return A const reference to the number of performed iterations.
    */
   const NaturalType& NumPerformedIterations() const { return mNumPerformedIterations; }

   /** Read/write access to the initialization algorithm for the first K representatives.
    *
    * @return A reference to the instance.
    */
   Initialization& Init() { return mInit; }

   /** Read only access to the initialization algorithm for the first K representatives.
    *
    * @return A const reference to the instance.
    */
   const Initialization& Init() const { return mInit; }

   /** Read/write access to the representative initialization object.
    *
    * @return A reference to the instance.
    */
   Representative&      RepInit()                  { return mRepInit; }

   /** Read only access to the representative intialization object.
    *
    * @return A const reference to the instance.
    */
   const Representative&
                        RepInit() const            { return mRepInit; }

   /** Read access to the container holding the defined output labels (labels pertaining the clustering).
    *
    * The container holds the valid labels defined during the last call of the Process
    * method. In other words, in the container we find the IDs of the generated clusters.
    * We also know how many clusters have been found just looking at the size of the
    * container.
    *
    * @return A const reference to the container of the computed cluster labels.
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

// ** New Implementation may 2013 - DNA **//
   /** Read only access to the mMinChange variable.
    *
    * @return a const reference to the mMinChange variable.
    */
   const RealType&  GetMinChange() const{ return mMinChange; }
   
	/**
    * Read-only access to vector SMG (vector containing the mean dispacements of the cluster representatives calculated over the iterations)
    *
    * @return A const reference to the vector
	*/
   const std::vector<RealType>& GetVSMG() const{return vSMG;}

   /**
    * Read-write access to the threshold that quantifies the minimium amount of variation observed in two consecutive partitions under which the algorithm must be stopped
    *
    * @return A reference to the threshold variable
    */
   RealType& MinChangeThreshold(){ return mMinChangeThreshold; }

   /**
    * Read-only access to the threshold that quantifies the minimium amount of variation observed in two consecutive partitions under which the algorithm must be stopped
    *
    * @return A const reference to the threshold variable
    */
   const RealType& MinChangeThreshold() const{ return mMinChangeThreshold; }
   
   /**
    * Read / Write access to boolean variable for dynamic stop criterion
    *
    * @return A reference to the boolean variable
	*/
   bool& DynThreshold() {return mDynThreshold;}

private:

   // Numero di cluster.
   NaturalParam mK;

   /**
    * Number of effectively-performed iterations by the kmeans algorithm
    */
   NaturalType mNumPerformedIterations;

   /**
    * Minimum allowed changing in the representatives
    */
   RealType mMinChangeThreshold;

   // Massimo numero di iterazioni.
   NaturalParam mMaxIter;

   // Inizializzatore cluster.
   Representative mRepInit;

   // Initializer for the kmeans
   Initialization mInit;

   // Rappresentanti memorizzati internamente.
   RepVector mRepresentatives;

   // Lista etichette rappresentanti.
   LabelVector mLabels;

   // Vettore dissimilarita'.
   std::vector<RealType> mRepDiss;
   
   // Contiene il minimo valore dello scostamento medio globale - may 2013 DNA
   RealType	mMinChange;

   // Vettore degli Scostamenti Medi Globali - may 2013 DNA
   std::vector<RealType> vSMG;

   // Variabile che indica se eseguire il calcolo della soglia dinamica - june 2013 - DNA
   bool mDynThreshold;


   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & mK;
      ar & mMaxIter;
      ar & mNumPerformedIterations;
      ar & mMinChangeThreshold;
      ar & mRepInit;
      ar & mRepresentatives;
      ar & mLabels;

	  ar & mMinChange; // ** may-2013 - DNA
	  ar & vSMG; // ** june-2013 - DNA
	  ar & mDynThreshold; // ** june-2013 - DNA
   } // BOOST SERIALIZATION

}; // class Kmeans

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename Representative, typename Initialization>
template <typename ForwardIterator1, typename ForwardIterator2>
void
Kmeans<Representative, Initialization>::Process(
                            ForwardIterator1  iSampleBegin,
                            ForwardIterator1  iSampleEnd,
                            ForwardIterator2  iLabelBegin)
{
   // Typedef locali.
   typedef typename RepVector::iterator
                        RepVectorIterator;

   typedef std::iterator_traits<std::vector<RealType>::iterator>::difference_type
                        RealDiffType;

   typedef std::vector<RealType>::size_type
                        DissSizeType;

   typedef std::vector<RealType>::iterator
                        DissIterator;

   typedef typename LabelVector::iterator
                        LabelVectorIterator;

   typedef typename LabelVector::size_type
                        LabelVectorSizeType;

   typedef typename std::iterator_traits<ForwardIterator1>::difference_type
                        SampleDiffType;

   // Variabili.
   ForwardIterator1        It;           // Iteratore principale dati.
   ForwardIterator2        Ot;           // Iteratore principale etichette.
   DissIterator            Dit;          // Iteratore distanze campione-cluster.
   DissIterator            MinIt;        // Iteratore distanze campione-cluster.
   LabelVectorSizeType     i;            // Contatore.
   LabelType               j;            // Contatore.
   NaturalType             Iter;         // Contatore iterazioni.
   bool                    Stop;         // Flag di stop.
   bool                    Del;          // Flag svuotamento cluster.
   RepVectorIterator       RepIt;        // Iteratore rappresentanti.
   LabelVectorIterator     LabIt;        // Iteratore etichette.
   LabelType               ClosestRep;   // Indice rappresentante più simile.
   LabelType               K_;           // Valore K.
   LabelVectorSizeType     K__;          // Valore K.
   LabelVectorSizeType     S;            // Numero di campioni.
   RealType				   Change;		 // Valore dello scostamento medio temporaneo.

   // ** may-2013 - DNA
   if (!vSMG.empty()) // ** se il vettore degli scostamenti medi globali non è vuoto lo svuoto. - june 2013 DNA
	   vSMG.clear();
   
   // Ricavo il numero S di campioni.
   S= boost::numeric::converter<LabelVectorSizeType, SampleDiffType>::convert(
                                                                       std::distance(
                                                                              iSampleBegin,
                                                                              iSampleEnd) );
   // Converto il valore di mK.
   K_= boost::numeric::converter<LabelType, NaturalType>::convert(mK);
   K__= boost::numeric::converter<LabelVectorSizeType, NaturalType>::convert(mK);

   // Controllo se ho almeno K campioni.
   if (S < K__)
   {
      throw SpareLogicError("Kmeans, 0, Invalid sample range.");
   }

   // Svuoto contenitori temporanei.
   mRepresentatives.clear();
   mLabels.clear();

   // I contenitori ce la possono fare ?
   if ( ( K_ > mRepresentatives.max_size() ) || ( K__ > mLabels.max_size() ) )
   {
      throw SpareLogicError("Kmeans, 1, Too large sample number or K.");
   }

   // Alloco la memoria e inizializzo le etichette.
   mRepresentatives.resize(K_, mRepInit);
   mRepDiss.resize( boost::numeric::converter<DissSizeType, LabelType>::convert(K_) );

   // Initialization of the K representatives
   It=iSampleBegin;
   mInit.Initialize(K_, It, iSampleEnd, mRepresentatives);

   // Eseguo l'iterazione zero.
   It= iSampleBegin;
   Ot= iLabelBegin;
   while (iSampleEnd != It)
   {
      Dit= mRepDiss.begin();
      RepIt= mRepresentatives.begin();
      while (mRepDiss.end() != Dit)
      {
         (*Dit++)= (*RepIt++).Diss(*It);
      }

      MinIt= std::min_element(
                          mRepDiss.begin(),
                          mRepDiss.end() );

      (*Ot++)= boost::numeric::converter<LabelType, RealDiffType>
               ::convert( std::distance(mRepDiss.begin(), MinIt) );
      It++;
   }

   Iter= 0;
   Stop= false;
   RepVector oldRep;
   oldRep.resize(K_);

   // ** Avvio iterazioni successive alla prima.
   while((Iter < mMaxIter))
   {
      // ** Si verifica se dobbiamo considerare un qualunque criterio di soglia: statico o dinamico.
	  // ** Inoltre si verifica anche se si tratta della prima iterazione del while
	  // ** Se le due condizioni sono verificate si salvano i centroidi iniziali. - june 2013 - DNA
      if ( (Iter == 0) && ((mMinChangeThreshold > 0)||(mDynThreshold)) )
	  {
         for(j= 0; j < K_; j++)
            oldRep[j]=mRepresentatives[j];
	  }

	  // Calcolo nuovi centroidi.
      for(j= 0; j < K_; j++)
      {
         mRepresentatives[j]=mRepInit;
      }

      It= iSampleBegin;
      Ot= iLabelBegin;
	  while (iSampleEnd != It)
      {
         mRepresentatives[*Ot++].Update(*It++);
      }

      // ** Si verifica se siamo in un caso di controllo con soglia statica o dinamica e quindi
	  // ** controllo di quanto si sono spostati i rappresentanti dall'ultima iterazione 
	  // ** ed a seconda del caso setto stop=true. june 2013 - DNA
      if ((mMinChangeThreshold > 0) || (mDynThreshold))// ** valore di soglia statico o dinamico? - DNA
	  {
		  Change=0;
		  for(j= 0; j < K_; j++){
			  Change += oldRep[j].Diss(mRepresentatives[j]);// ** Calcolo della distanza tra i nuovi centroidi ed i vecchi. may 2013 - DNA
		  }
		  Change = Change / (RealType)K_;
		  vSMG.push_back(Change); // ** Memorizzazione dello scostamento medio nell'appropriato vettore. - june 2013 - DNA

		  if (Iter == 0) // ** prima iterazione del while (seconda iterazione globale). may-2013 - DNA   
			  mMinChange = Change; 
		  else // ** dalla iterazione successiva.
		  {
			  if (Change < mMinChange) // ** se e' stato trovato un valore Scostamento Medio Globale minore di quello gia' trovato lo salviamo. - may-2013 DNA
				  mMinChange = Change;
			  else // ** se non è < potrà essere = o >. june 2013 - DNA
			  {
				  if (mDynThreshold) // ** se stiamo utilizzando il criterio di stop dinamico. june 2013 - DNA
					  if (Change == mMinChange) // ** verifichiamo se abbiamo trovato nuovamente un valore del minimo uguale ad uno gia' trovato. june 2013 - DNA
						  mMinChangeThreshold = Change; // ** se e' cosi' ci fermiamo. june 2013 - DNA
			  }
		  }

		  // ** Nel caso di criterio di stop statico la variabile mMinChangeThreshold sara' stata valorizzata prima della chiamata di Process(...).
		  // ** Nel caso di soglia dinamica la variabile mMinChangeThreshold viene valorizzata dalla stessa funzione Process(...) (istruzione precedente).
		  // ** Caso limite per la soglia dinamica quando Change=0. june 2013 - DNA
		  if ( (Change <= mMinChangeThreshold) )// ** deve essere <= e non solo < - may-2013 DNA
		  { 
			  Stop=true;
		  }
		  else // ** Se non ci si ferma si va avanti salvando i centroidi. - DNA
		  {
			  for(j= 0; j < K_; j++){
				  oldRep[j] = mRepresentatives[j];
			  }
		  }
      }

      // Elimino eventuali cluster vuoti.
      Del= false;
      RepIt= mRepresentatives.begin();
      Dit= mRepDiss.begin();
      while (mRepresentatives.end() != RepIt)
      {
         if (RepIt->GetCount() == 0)
         {
            Del= true;
            RepIt= mRepresentatives.erase(RepIt);
            Dit= mRepDiss.erase(Dit);
         }
         else
         {
            RepIt++;
            Dit++;
         }
      }


      K_= mRepresentatives.size();

      if (!Del)
      {
         Iter++;
      }

      if (( Del || (Iter < mMaxIter) ) && !Stop)
      {
         // Eseguo nuova iterazione.
         It= iSampleBegin;
         Ot= iLabelBegin;
         Stop= !Del;
         while (iSampleEnd != It)
         {
            Dit= mRepDiss.begin();
            RepIt= mRepresentatives.begin();
            while (mRepDiss.end() != Dit)
            {
               (*Dit++)= (*RepIt++).Diss(*It);
            }

            MinIt= std::min_element(
                                mRepDiss.begin(),
                                mRepDiss.end() );

            ClosestRep= boost::numeric::converter<LabelType, RealDiffType>
                        ::convert( std::distance(mRepDiss.begin(), MinIt) );

            //controlla se le etichette sono rimaste invariate dall'ultima assegnazione
            if (*Ot != ClosestRep)
            {
               Stop= false;
            }

            (*Ot++)= ClosestRep;
            It++;
         }
      }

      //again no changes
      if(Stop==true)
          break;

   } //while (Iter < mMaxIter)

   // ** set the number of effectively performed iterations
   if (Iter == mMaxIter) // ** ossia abbiamo eseguito tutte le iterazioni impostate - june 2013 - DNA
	   mNumPerformedIterations = Iter;
   else
	   mNumPerformedIterations = Iter + 1; // ** va aggiunta l'iterazione 0 iniziale - june 2013 - DNA

   // Genero etichette
   K__= boost::numeric::converter<LabelVectorSizeType, LabelType>::convert(K_);
   mLabels.resize(K__);

   for(i= 0; i < K__; i++)
   {
      mLabels[i]= boost::numeric::converter<LabelType, LabelVectorSizeType>::convert(i);
   }

}  // End Process()

}  // namespace spare

#endif  // _Kmeans_h_

