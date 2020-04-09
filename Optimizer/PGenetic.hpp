//  PGenetic class, part of the SPARE library.
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

/** @brief File PGenetic.hpp, containing the PGenetic template class.
 *
 * The file contains the PGenetic template class, implementing a parallel genetic algorithm.
 *
 * @file PGenetic.hpp
 * @author Lorenzo Livi
 */

#ifndef _PGenetic_h_
#define _PGenetic_h_

// STD INCLUDES
#include <limits>
#include <map>
#include <set>
#include <string>
#include <utility>

// BOOST INCLUDES
#include <boost/numeric/conversion/converter.hpp>
#include <boost/random.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

// SPARE INCLUDES
#include <spare/BoundedParameter.hpp>
#include <spare/SpareExceptions.hpp>
#include <spare/SpareTypes.hpp>
#include <spare/SwitchParameter.hpp>

namespace spare {  // Inclusion in namespace spare.

// Data for Registry switch parameter construction.
static const std::string GENETIC_RSVAL[]= {"Off", "On"};
static const size_t      GENETIC_RSVAL_SZ= 2;

// Stagnation Detection Factor.
static const NaturalType GENETIC_SDF= 100;

/** @brief %PGenetic algorithm.
 *
 * The PGenetic class models the @a Optimizer concept. It is a parallel genetic algorithm with optional
 * registry function. The number of threads are selected by the user at compile-time through the suited template parameter.
 * The registry is a container holding every generated solution code along
 * with its fitness value. The solution code manipulation functions such as Breed, Crossover,
 * Mutation and Fitness evaluation are demanded from an environment agent of the type
 * established by the template argument. The environment agent must model the @a Environment
 * concept. The actual form of the solution code is actually defined by the environment type.
 * The non deterministic behaviour of the algorithm is controlled by a user-defined random seed.
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
 *     <td class="indexvalue">Registry</td>
 *     <td class="indexvalue">{On, Off}</td>
 *     <td class="indexvalue">Registry enabling switch.</td>
 *     <td class="indexvalue">Yes</td>
 *     <td class="indexvalue">Off</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">PopSize</td>
 *     <td class="indexvalue">[1, inf)</td>
 *     <td class="indexvalue">Number of individuals in the population.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">20</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">Elitism</td>
 *     <td class="indexvalue">[0, 1]</td>
 *     <td class="indexvalue">Number of elitary individuals as a fraction of the population 
                              size.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">0.1</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">CrossRate</td>
 *     <td class="indexvalue">[0, 1]</td>
 *     <td class="indexvalue">Crossover rate, the probability of performing a crossover 
                              operation on a couple of selected individuals.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">0.8</td>
 *  </tr>
 *  <tr>
 *     <td class="indexvalue">MuteRate</td>
 *     <td class="indexvalue">[0, 1]</td>
 *     <td class="indexvalue">Mutation rate, the probability of performing a mutation 
                              operation on the newly generated individuals.</td>
 *     <td class="indexvalue">No</td>
 *     <td class="indexvalue">0.3</td>
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
 *     <td class="indexvalue">Setup of the randomization seed, a non-negative integer.</td>
 *  </tr>
 *  </table>
 */
template <typename Environment, int NThreads>
class PGenetic
{
public:

// PUBLIC TYPES

   /** Solution code type.
    */
   typedef typename Environment::CodeType
                        CodeType;

   /** Registry type.
    */
   typedef std::map<CodeType, RealType>
                        RegistryMap;

   /** Real parameter type.
    */
   typedef BoundedParameter<RealType>
                        RealParam;

   /** Unsigned integer parameter type.
    */
   typedef BoundedParameter<NaturalType>
                        NaturalParam;

   /** Switch parameter type.
    */
   typedef SwitchParameter<std::string>
                        StringParam;

// LIFECYCLE

   /** Default constructor. If used, the registry will be disabled.
    */
   PGenetic()
      : mPopSize( 1, std::numeric_limits<NaturalType>::max() ),
        mElitism(0, 1),
        mCrossRate(0, 1),
        mMuteRate(0, 1),
        mRegistry(GENETIC_RSVAL, GENETIC_RSVAL + GENETIC_RSVAL_SZ),
        mRealDist( boost::mt19937() )
                           {
                              mPopSize= 20;
                              mElitism= 0.1;
                              mCrossRate= 0.8;
                              mMuteRate= 0.3;
                              mRegistry= "Off";
                           }

   /** Constructor taking the registry enabling parameter.
    *
    * @param[in] rRegistry Registry enabling switch. Possible values: @a On and @a Off.
    */
   PGenetic(const std::string& rRegistry)
      : mPopSize( 1, std::numeric_limits<NaturalType>::max() ),
        mElitism(0, 1),
        mCrossRate(0, 1),
        mMuteRate(0, 1),
        mRegistry(GENETIC_RSVAL, GENETIC_RSVAL + GENETIC_RSVAL_SZ),
        mRealDist( boost::mt19937() )
                           {
                              mPopSize= 20;
                              mElitism= 0.1;
                              mCrossRate= 0.8;
                              mMuteRate= 0.3;
                              mRegistry= rRegistry;
                           }

// OPERATIONS

   /** Algorithm initialization.
    *
    * The first generation is entirely created by the Breed function of the environment
    * agent.
    */
   void                 Initialize();

   /** Algorithm initialization with seeded solutions.
    *
    * Some custom solutions are inserted in the first generation. If the number of passed
    * solutions is less than the population size, other solutions will be generated by the
    * Breed function of the environment agent. If the number of passed solutions is more than
    * the population size, the solutions with lower fitness will be discarded.
    *
    * @param[in] iCodeBegin Iterator pointing at the first solution.
    * @param[in] iCodeEnd Iterator pointing at one past the end of the solution batch.
    */
   template <typename ForwardIterator>
   void                 Initialize(
                               ForwardIterator   iCodeBegin,
                               ForwardIterator   iCodeEnd);

   /** Execution of one step, the next generation is created.
    *
    * The elitary individuals, the number of which is established by the Elitism parameter
    * as a fraction of the population size, are directly passed to the new generation. The
    * other individuals are created by means of the Mutate and Crossover functions of the
    * environment agent, starting from the individuals of the current population selected by
    * means of a roulette wheel method. Initialize must be called after the first call of 
    * StepUp.
    */
   void                 StepUp()
                           {
                              if ( mPopBuffB.empty() )
                              {
                                 NextGeneration(mPopBuffA, mPopBuffB);
                              }
                              else
                              {
                                 NextGeneration(mPopBuffB, mPopBuffA);
                              }
                           }

// ACCESS

   /** Read/write access to the PopSize parameter.
    *
    * @return A reference to the PopSize parameter.
    */
   NaturalParam&        PopSize()                  { return mPopSize; }

   /** Read only access to the PopSize parameter.
    *
    * @return A const reference to the PopSize parameter.
    */
   const NaturalParam&  PopSize() const            { return mPopSize; }

   /** Read/write access to the Elitism parameter.
    *
    * @return A reference to the Elitism parameter.
    */
   RealParam&           Elitism()                  { return mElitism; }

   /** Read only access to the Elitism parameter.
    *
    * @return A const reference to the Elitism parameter.
    */
   const RealParam&     Elitism() const            { return mElitism; }

   /** Read/write access to the CrossRate parameter.
    *
    * @return A reference to the CrossRate parameter.
    */
   RealParam&           CrossRate()                { return mCrossRate; }

   /** Read only access to the CrossRate parameter.
    *
    * @return A const reference to the CrossRate parameter.
    */
   const RealParam&     CrossRate() const          { return mCrossRate; }

   /** Read/write access to the MuteRate parameter.
    *
    * @return A reference to the MuteRate parameter.
    */
   RealParam&           MuteRate()                 { return mMuteRate; }

   /** Read only access to the MuteRate parameter.
    *
    * @return A const reference to the MuteRate parameter.
    */
   const RealParam&     MuteRate() const           { return mMuteRate; }

   /** Read/write access to the internal environment agent instance.
    *
    * @return A reference to the EnvAgent object.
    */
   Environment&         EnvAgent()                 { return mEnvAgent; }

   /** Read only access to the internal environment agent instance.
    *
    * @return A const reference to the EnvAgent object.
    */
   const Environment&   EnvAgent() const           { return mEnvAgent; }

   /** Read only access to the Registry parameter.
    *
    * @return A const reference to the Registry parameter.
    */
   const StringParam&   Registry() const           { return mRegistry; }

   /** Read only access to the fitness of an individual in the current population.
    *
    * @param[in] aIndex Index of the individual (base 0). Pass 0 or nothing for the best one.
    * @return The fitness value.
    */
   RealType             GetPerformance(NaturalType aIndex= 0) const;

   /** Read only access to the code of an individual in the current population.
    *
    * @param[in] aIndex Index of the individual (base 0). Pass 0 or nothing for the best one.
    * @return A const reference to the individual code.
    */
   const CodeType&      GetSolution(NaturalType aIndex= 0) const;

   /** Read only access to the registry. If the registry is disabled, it will be empty.
    *
    * @return A const reference to the registry.
    */
   const RegistryMap&   GetRegistryData() const    { return mRegistryData; }

   /** Read/write access to the internal parameter MaxEvolutions.
     *
     * @return A reference to the MaxEvolutions parameter.
     */
    NaturalType&         MaxEvolutions()            { return mMaxEvolutions; }

    /** Read only access to the internal parameter MaxEvolutions.
     *
     * @return A const reference to the MaxEvolutions parameter.
     */
    const NaturalType&         MaxEvolutions() const  { return mMaxEvolutions; }

    /** Read/write access to the internal parameter MaxEvolutions.
 	*
 	* @return A reference to the StallEvolutions parameter.
 	*/
    NaturalType&         StallEvolutions()            { return mStallEvolutions; }

    /** Read only access to the internal parameter MaxEvolutions.
     *
     * @return A const reference to the StallEvolutions parameter.
     */
    const NaturalType&         StallEvolutions() const { return mStallEvolutions; }

    std::vector<RealType>	  Execute()
 					   {
 						   // The main feature selection loop.
 							RealType performance;
 							NaturalType stallCounter=0;
 							std::vector<RealType> fitlist(mMaxEvolutions, 0.);
 							for (NaturalType i= 0; i < mMaxEvolutions && stallCounter < mStallEvolutions && GetPerformance() < 1; i++)
 							{
 								StepUp();
 								if(stallCounter==0)
 								{
 									performance=GetPerformance();
 									stallCounter++;
 								}
 								else
 								{
 										   if(performance==GetPerformance())
 										   {
 											   stallCounter++;
 										   }
 										   else
 										   {
 												   performance=GetPerformance();
 												   stallCounter=1;
 										   }
 								}
 								fitlist[i] = performance;
 								std::cout << i<<std::endl;
 							}

 							return fitlist;
 					    }

// SETUP

   /** Randomization seed setup.
    *
    * @param[in] aSeed The seed.
    */
   void                 RandSeedSetup(NaturalType aSeed)
                           {
                              mRealDist.base().seed(aSeed);
                           }

private:

   // Typedef privati.

   //pair of fitness value -- code
   typedef std::pair<RealType, CodeType>
                        Individual;

   typedef std::set<Individual>
                        Population;

   typedef typename Population::size_type
                        PopulationSizeType;

   typedef typename Population::reverse_iterator
                        PopulationRevIterator;

   typedef typename Population::iterator
                        PopulationIterator;

   typedef typename RegistryMap::const_iterator
                        RegistryMapIterator;

   // Dimensione popolazione.
   NaturalParam         mPopSize;

   // Elitismo.
   RealParam            mElitism;

   // Crossover rate.
   RealParam            mCrossRate;

   // Mutation rate.
   RealParam            mMuteRate;

   // Ambiente.
   Environment          mEnvAgent;

   // Abilitazione anagrafe.
   StringParam          mRegistry;

   // Primo buffer popolazione.
   Population           mPopBuffA;

   // Secondo buffer popolazione.
   Population           mPopBuffB;

   // Anagrafe.
   RegistryMap          mRegistryData;

   // Numero massimo di evoluzioni prima dello stop.
   NaturalType			mMaxEvolutions;

   // Numero di evoluzioni in cui la fitness non cambia dopo il quale fermare l'algoritmo.
   NaturalType			mStallEvolutions;

   //mutex for individuals iterator
   mutable boost::mutex itMutex;

   //mutex for registry
   mutable boost::mutex regMutex;

   // BOOST RANDOM
   // Distribuzione [0, 1).
   mutable boost::uniform_01<boost::mt19937, RealType>
                        mRealDist;
   // BOOST RANDOM

   // Passaggio a nuova popolazione.
   void                 NextGeneration(
                           Population& OldPop,
                           Population& NewPop);

   // Selezione di un individuo con meccanismo roulette wheel.
   PopulationRevIterator RouletteWheelSelection(Population& Pop);

   // Calcolo fitness.
   RealType GetFitness(const CodeType& rCode);

   // wrapper fitness function for threads
   void GetFitness(Population* population, Population* np, PopulationIterator* it);

   // BOOST SERIALIZATION
   friend class boost::serialization::access;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & mPopSize;
      ar & mElitism;
      ar & mCrossRate;
      ar & mMuteRate;
      ar & mEnvAgent;
      ar & mRegistry;
      ar & mPopBuffA;
      ar & mPopBuffB;
      ar & mRegistryData;
   } // BOOST SERIALIZATION

}; // class Genetic

/******************************* TEMPLATE IMPLEMENTATION **********************************/

////////////////////////////////////// PUBLIC //////////////////////////////////////////////

//==================================== OPERATIONS ==========================================

template <typename Environment, int NThreads>
void
PGenetic<Environment, NThreads>::Initialize()
{
   PopulationSizeType      PopSize_;
   Individual              IndBuff;
   NaturalType             TrialCounter;
   //threads
   boost::thread_group threadGroup;

   PopSize_= boost::numeric::converter<PopulationSizeType, NaturalType>::convert(mPopSize);

   mPopBuffA.clear();
   mPopBuffB.clear();

   // Cerco di inizializzare la popolazione con PopSize individui generati dall'ambiente.
   TrialCounter= 0;
   while (mPopBuffA.size() < PopSize_)
   {
      // Eccezione se non si riescono a generare PopSize indivdui diversi.
      if (std::numeric_limits<NaturalType>::max() == TrialCounter)
      {
         throw SpareLogicError("Genetic, 0, Unable to get PopSize different individuals.");
      }

      IndBuff.second= mEnvAgent.Breed();
      IndBuff.first=0.0;
      mPopBuffA.insert(IndBuff);

      TrialCounter++;
   }

   //multi-threaded evaluation of new population fitness...
   Population np;
   PopulationIterator it=mPopBuffA.begin();

   for(NaturalType i=0;i<NThreads;i++)
       threadGroup.add_thread(new boost::thread(
               boost::bind(&PGenetic::GetFitness, this, &mPopBuffA, &np, &it)));

   threadGroup.join_all();

   mPopBuffA.clear();
   mPopBuffA.insert(np.begin(), np.end());
}

template <typename Environment, int NThreads>
template <typename ForwardIterator>
void
PGenetic<Environment, NThreads>::Initialize(
                             ForwardIterator iCodeBegin,
                             ForwardIterator iCodeEnd)
{
   PopulationSizeType      PopSize_;
   Individual              IndBuff;
   NaturalType             TrialCounter;
   //threads
   boost::thread_group threadGroup;

   PopSize_= boost::numeric::converter<PopulationSizeType, NaturalType>::convert(mPopSize);

   mPopBuffA.clear();
   mPopBuffB.clear();

   while (iCodeBegin != iCodeEnd)
   {
      IndBuff.second= *iCodeBegin++;
      IndBuff.first=0.0;
      mPopBuffA.insert(IndBuff);
   }

   // Se serve, elimino gli individui peggiori dalla popolazione.
   while (mPopBuffA.size() > PopSize_)
   {
      mPopBuffA.erase( mPopBuffA.begin() );
   }

   // Se serve, cerco di completare la popolazione con individui generati dall'ambiente.
   TrialCounter= 0;
   while (mPopBuffA.size() < PopSize_)
   {
      // Eccezione se non si riescono a generare PopSize indivdui diversi.
      if (std::numeric_limits<NaturalType>::max() == TrialCounter)
      {
         throw SpareLogicError("Genetic, 1, Unable to get PopSize different individuals.");
      }

      IndBuff.second= mEnvAgent.Breed();
      IndBuff.first=0.0;
      mPopBuffA.insert(IndBuff);

      TrialCounter++;
   }

   //multi-threaded evaluation of new population fitness...
   Population np;
   PopulationIterator it=mPopBuffA.begin();

   for(NaturalType i=0;i<NThreads;i++)
       threadGroup.add_thread(new boost::thread(
               boost::bind(&PGenetic::GetFitness, this, &mPopBuffA, &np, &it)));

   threadGroup.join_all();

   mPopBuffA.clear();
   mPopBuffA.insert(np.begin(), np.end());
}

//==================================== ACCESS ==============================================

template <typename Environment, int NThreads>
RealType
PGenetic<Environment, NThreads>::GetPerformance(NaturalType aIndex) const
{
   PopulationRevIterator   Pit;
   NaturalType             i;

   if ( mPopBuffA.empty() && mPopBuffB.empty() )
   {
      throw SpareLogicError("Genetic, 2, Uninitialized object: mPopBuffA.");
   }

   if (mPopSize <= aIndex)
   {
      throw SpareLogicError("Genetic, 3, Index out of range.");
   }

   if ( mPopBuffB.empty() )
   {
      Pit= mPopBuffA.rbegin();

      for (i= 0; i < aIndex; i++)
      {
         Pit++;
      }

      return Pit->first;
   }
   else
   {
      Pit= mPopBuffB.rbegin();

      for (i= 0; i < aIndex; i++)
      {
         Pit++;
      }

      return Pit->first;
   }
}

template <typename Environment, int NThreads>
const typename PGenetic<Environment, NThreads>::CodeType&
PGenetic<Environment, NThreads>::GetSolution(NaturalType aIndex) const
{
   PopulationRevIterator   Pit;
   NaturalType             i;

   if ( mPopBuffA.empty() && mPopBuffB.empty() )
   {
      throw SpareLogicError("Genetic, 4, Uninitialized object: mPopBuffA.");
   }

   if (mPopSize <= aIndex)
   {
      throw SpareLogicError("Genetic, 5, Index out of range.");
   }

   if ( mPopBuffB.empty() )
   {
      Pit= mPopBuffA.rbegin();

      for (i= 0; i < aIndex; i++)
      {
         Pit++;
      }

      return Pit->second;
   }
   else
   {
      Pit= mPopBuffB.rbegin();

      for (i= 0; i < aIndex; i++)
      {
         Pit++;
      }

      return Pit->second;
   }
}

////////////////////////////////////// PRIVATE /////////////////////////////////////////////

template <typename Environment, int NThreads>
void
PGenetic<Environment, NThreads>::NextGeneration(
                                 Population& OldPop,
                                 Population& NewPop)
{
   PopulationSizeType        PopSize_;
   PopulationSizeType        ElCount;    // Contatore elitismo.
   PopulationRevIterator     PopRit;
   Individual                IndBuffA;
   Individual                IndBuffB;
   NaturalType               TrialCounter;
   //threads
   boost::thread_group threadGroup;

   if ( OldPop.empty() )
   {
      throw SpareLogicError("Genetic, 6, Uninitialized object: OldPop.");
   }

   if ( !NewPop.empty() )
   {
      throw SpareLogicError("Genetic, 7, Unexpected condition: NewPop empty.");
   }

   PopSize_= boost::numeric::converter<PopulationSizeType, NaturalType>::convert(mPopSize);
   ElCount= (PopulationSizeType) PopSize_ * mElitism;

   // Elitismo.
   PopRit= OldPop.rbegin();
   while (ElCount--)
   {
       Individual ind=*PopRit;
       ind.first=0.0;
       NewPop.insert(ind);
       PopRit++;
   }

   // Ciclo principale.
   TrialCounter= 0;
   while (NewPop.size() < PopSize_)
   {
      // Eccezione se proprio non si riescono a generare PopSize indivdui diversi.
      if (std::numeric_limits<NaturalType>::max() == TrialCounter)
      {
         throw SpareLogicError("Genetic, 8, Unable to get PopSize different individuals.");
      }

      PopRit= RouletteWheelSelection(OldPop);
      IndBuffA.second= PopRit->second;
      IndBuffA.first=0.0;

      PopRit= RouletteWheelSelection(OldPop);
      IndBuffB.second= PopRit->second;
      IndBuffB.first=0.0;

      if (mRealDist() < mCrossRate)
      {
         mEnvAgent.Crossover(IndBuffA.second, IndBuffB.second);
      }

      if (mRealDist() < mMuteRate)
      {
         mEnvAgent.Mutate(IndBuffA.second);
      }

      if (mRealDist() < mMuteRate)
      {
         mEnvAgent.Mutate(IndBuffB.second);
      }

      //add unevaluated new individuals
      NewPop.insert(IndBuffA);
      NewPop.insert(IndBuffB);

      // Faccio anche un Breed se sto avendo difficoltà a generare PopSize individui diversi.
      if (TrialCounter > GENETIC_SDF * PopSize_)
      {
         IndBuffA.second= mEnvAgent.Breed();
         IndBuffA.first=0.0;
         NewPop.insert(IndBuffA);
      }

      TrialCounter++;
   }

   OldPop.clear();

   //multi-threaded evaluation of new population fitness...
   Population np;
   PopulationIterator it=NewPop.begin();

   for(NaturalType i=0;i<NThreads;i++)
       threadGroup.add_thread(new boost::thread(
               boost::bind(&PGenetic::GetFitness, this, &NewPop, &np, &it)));

   threadGroup.join_all();

   NewPop.clear();
   NewPop.insert(np.begin(), np.end());

   // Riduzione popolazione a valore fisso (Problema con il multithread. Infatti si calcolano le fitness anche di individui che poi potrei eliminare. Il problema sta nel fatto che non possiamo eliminare a priori gli individui solo sulla base del numero, perche' non conosciamo la loro fitness)
   while (NewPop.size() > PopSize_)
   {
      NewPop.erase( NewPop.begin() );
   }
}

template <typename Environment, int NThreads>
void
PGenetic<Environment, NThreads>::GetFitness(Population* population, Population* np,
        PopulationIterator* it)
{
    //local variables
    Individual ind;
    PopulationIterator lIt;

    do
    {
        itMutex.lock();

        //stop condition
        if((*it) == (*population).end())
        {
            itMutex.unlock();
            break;
        }

        lIt=*it;
        ind=*lIt;
        (*it)++;

        itMutex.unlock();

        //parallel evaluation of the fitness
        ind.first=GetFitness(ind.second);

        itMutex.lock();

        //insert of the individual
        (*np).insert(ind);

        itMutex.unlock();

    }while(true);
}

template <typename Environment, int NThreads>
typename PGenetic<Environment, NThreads>::PopulationRevIterator
PGenetic<Environment, NThreads>::RouletteWheelSelection(Population& Pop)
{
   RealType                  FitnessAcc= 0;
   RealType                  AuxAcc;
   RealType                  Ball;
   PopulationRevIterator     PopRit;
   PopulationRevIterator	 PopREndIt;

   int size=Pop.size();
   int i=0;
   PopRit= Pop.rbegin();
   PopREndIt=Pop.rend();
   for(;i<size;i++)
   {
      FitnessAcc+= (*PopRit++).first;
   }

   Ball= FitnessAcc * mRealDist();

   PopRit= Pop.rbegin();
   AuxAcc= PopRit->first;
   i=0;
   while ( (AuxAcc < Ball) && (i<size) )
   {
	   i++;
	   PopRit++;
	   AuxAcc+= PopRit->first;
   }

   if (PopREndIt == PopRit)
   {
      //throw SpareLogicError("Genetic, 9, Unexpected condition.");
      return Pop.rbegin();
   }

   return PopRit;
}

template <typename Environment, int NThreads>
RealType
PGenetic<Environment, NThreads>::GetFitness(const CodeType& rCode)
{
    if (mRegistry == "On")
    {
        regMutex.lock();
        RegistryMapIterator Rit= mRegistryData.find(rCode);

        //if found
        if (mRegistryData.end() != Rit)
        {
            regMutex.unlock();
            return Rit->second;
        }
        else
        {
            regMutex.unlock();

            RealType FitVal= mEnvAgent.Fitness(rCode);

            if ( mRegistryData.size() < mRegistryData.max_size() )
            {
                regMutex.lock();
                mRegistryData[rCode]= FitVal;
                regMutex.unlock();
            }

            return FitVal;
        }
    }
    else
    {
        return mEnvAgent.Fitness(rCode);
    }
}  // GetFitness

}  // namespace spare

#endif  // _PGenetic_h_
