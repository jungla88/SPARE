//  Svm common functionalities and definitions, part of the SPARE library.
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

#ifndef _SvmCommon_h_
#define _SvmCommon_h_

// STD INCLUDES
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>

// BOOST INCLUDES
#include <boost/numeric/conversion/converter.hpp>

// SPARE INCLUDES
#include <spare/SpareExceptions.hpp>

// LIBSVM INCLUDES
#include <libsvm-3.23/svm.h>

// DEFINES
#ifdef _WIN32
#define SPARE_SVM_TMP_FILE       "SpareSvmTempFile"
#else
#define SPARE_SVM_TMP_FILE       "/tmp/SpareSvmTempFile"
#endif
#define SPARE_SVM_NO_MODEL_TAG   "no_svm_model"

namespace spare {  // Inclusione in namespace spare.

// Typedef uso interno.
typedef svm_model*        LibSvmModelPtr;
typedef const svm_model*  ConstLibSvmModelPtr;
typedef svm_node          LibSvmNode;
typedef svm_node*         LibSvmNodePtr;
typedef const svm_node*   ConstLibSvmNodePtr;
typedef svm_problem       LibSvmProblem;
typedef double            LibSvmLabel;

// Riproduzione struttura modello.
struct LibSvmModelStruct
{
//    struct svm_parameter   param; //no struct
//	int             nr_class;
//	int             l;
//    struct svm_node**      SV; //no struct
//	double**        sv_coef;
//	double*         rho;
//	double*         probA;
//	double*         probB;
//	int*            label;
//	int*            nSV;
//	int             free_sv;

    struct svm_parameter param;	/* parameter */
    int nr_class;		/* number of classes, = 2 in regression/one class svm */
    int l;			/* total #SV */
    struct svm_node **SV;		/* SVs (SV[l]) */
    double **sv_coef;	/* coefficients for SVs in decision functions (sv_coef[k-1][l]) */
    double *rho;		/* constants in decision functions (rho[k*(k-1)/2]) */
    double *probA;		/* pariwise probability information */
    double *probB;
    int *sv_indices;        /* sv_indices[0,...,nSV-1] are values in [1,...,num_traning_data] to indicate SVs in the training set */

    /* for classification only */

    int *label;		/* label of each class (label[k]) */
    int *nSV;		/* number of SVs for each class (nSV[k]) */
                /* nSV[0] + nSV[1] + ... + nSV[k-1] = l */
    /* XXX */
    int free_sv;		/* 1 if svm_model is created by svm_load_model*/
                /* 0 if svm_model is created by svm_train */


};

// Dati per costruzione parametri switch.
static const std::string SVMC_KTVAL[]= {"RBF", "Polynomial", "Sigmoid", "Linear"};
static const size_t      SVMC_KTVAL_SZ= 4;
static const std::string SVMC_SPVAL[]= {"Off", "On"};
static const size_t      SVMC_SPVAL_SZ= 2;

// Creazione vettore X libsvm da coppia di iteratori.
template <typename ForwardIterator>
LibSvmNodePtr           CreateSvmX(std::pair<ForwardIterator, ForwardIterator> aSample);

// Creazione vettore X da sequence container.
template <typename SequenceContainer>
LibSvmNodePtr           CreateSvmX(const SequenceContainer& rSample);

// Creazione vettore X da altro vettore X.
inline LibSvmNodePtr    CopySvmX(ConstLibSvmNodePtr pSample);

// Creazione problema libsvm.
template <typename ForwardIterator1, typename ForwardIterator2>
void                    CreateSvmProblem(
                                     ForwardIterator1   iSampleBegin,
                                     ForwardIterator1   iSampleEnd,
                                     ForwardIterator2   iLabelBegin,
                                     LibSvmProblem&     rProblem);


// Funzione uso interno, distruggi vettore x creato con CreateSvmX.
inline void             DestroySvmX(LibSvmNodePtr pX);

// Funzione uso interno, distruggi problema creato con CreateSvmProblem.
inline void             DestroySvmProblem(LibSvmProblem& rProblem);

// Funzione utilità / debugging, stampa problema svm.
inline void             PrintSvmProblem(
                                    std::ostream&          rStrm,
                                    const LibSvmProblem&   rProblem);

// Importante funzione per rendere autonomo un modello (copia interna degli SV).
inline void             FinalizeSvmModel(LibSvmModelPtr pModel);

// Estrazione numero SV da modello.
inline void             SvNumberFromSvmModel(
                                     ConstLibSvmModelPtr  pModel,
                                     int&                 rSvNumber);

// Genera rappresentazione testuale di modello svm.
inline void             SvmModelToText(
                                     ConstLibSvmModelPtr  pModel,
                                     std::string&         rText);

// Crea modello svm da rappresentazione testuale.
inline void             SvmModelFromText(
                                     LibSvmModelPtr&      rpModel,
                                     const std::string&   rText);

// Funzione dummy per disabilitare output su console di libsvm.
inline void             PrintNull(const char *s) { }

/******************************* TEMPLATE IMPLEMENTATION **********************************/

template <typename ForwardIterator>
LibSvmNodePtr
CreateSvmX(std::pair<ForwardIterator, ForwardIterator> aSample)
{
   // Typedef locali.
   typedef typename std::iterator_traits<ForwardIterator>::difference_type
                        SampleSizeType;

   // Dichiarazioni.
   SampleSizeType   D;         // Dimensione campioni.
   SampleSizeType   Dn;        // Numero caratteristiche non nulle.
   ForwardIterator  St;        // Iteratore singolo campione.
   LibSvmNodePtr    X= NULL;   // Vettore x.
   LibSvmNodePtr    Vt;        // Iteratore valori x.
   int              Cnt;       // Conteggio valori x.

   // Copia dati.
   D= std::distance(aSample.first, aSample.second);
   Dn= D - count(aSample.first, aSample.second, 0);
   X= (LibSvmNodePtr) malloc(sizeof(LibSvmNode) * (Dn + 1));

   St= aSample.first;
   Vt= X;
   Cnt= 0;
   while (aSample.second != St)
   {
      if (*St != 0)
      {
         Vt->index= ++Cnt;
         Vt->value= *St;
         ++St;
         ++Vt;
      }
      else
      {
         ++Cnt;
         ++St;
      }
   }
   Vt->index= -1;
   Vt->value= 0;

   return X;
}  // CreateSvmX (iterator pair)

template <typename SequenceContainer>
LibSvmNodePtr
CreateSvmX(const SequenceContainer& rSample)
{
   // Typedef locali.
   typedef typename SequenceContainer::size_type
                        SampleSizeType;

   typedef typename SequenceContainer::const_iterator
                        SampleIterator;

   // Dichiarazioni.
   SampleSizeType   D;         // Dimensione campioni.
   SampleSizeType   Dn;        // Numero caratteristiche non nulle.
   SampleIterator   St;        // Iteratore singolo campione.
   LibSvmNodePtr    X= NULL;   // Vettore x.
   LibSvmNodePtr    Vt;        // Iteratore valori x.
   int              Cnt;       // Conteggio valori x.

   // Copia dati.
   D= rSample.size();
   Dn= D - count(rSample.begin(), rSample.end(), 0);
   X= (LibSvmNodePtr) malloc(sizeof(LibSvmNode) * (Dn + 1));

   St= rSample.begin();
   Vt= X;
   Cnt= 0;
   while (rSample.end() != St)
   {
      if (*St != 0)
      {
         Vt->index= ++Cnt;
         Vt->value= *St;
         ++St;
         ++Vt;
      }
      else
      {
         ++Cnt;
         ++St;
      }
   }
   Vt->index= -1;
   Vt->value= 0;

   return X;
}  // CreateSvmX (SequenceContainer)

LibSvmNodePtr
CopySvmX(ConstLibSvmNodePtr pSample)
{
   size_t               Dn;       // Num. caratteristiche non nulle.
   LibSvmNodePtr        X= NULL;  // Puntatore aux.
   LibSvmNodePtr        Vt;       // Iteratore valori x.
   ConstLibSvmNodePtr   St;       // Iteratore valori x.

   St= pSample;
   Dn= 0;
   while (St->index != -1)
   {
      ++St;
      ++Dn;
   }

   X= (LibSvmNodePtr) malloc(sizeof(LibSvmNode) * (Dn + 1));

   St= pSample;
   Vt= X;
   while (St->index != -1)
   {
      Vt->index= St->index;
      Vt->value= St->value;
      ++St;
      ++Vt;
   }
   Vt->index= -1;
   Vt->value= 0;

   return X;
}  // CreateSvmX (LibSvmNodePtr)

template <typename ForwardIterator1, typename ForwardIterator2>
void
CreateSvmProblem(
             ForwardIterator1   iSampleBegin,
             ForwardIterator1   iSampleEnd,
             ForwardIterator2   iLabelBegin,
             LibSvmProblem&     rProblem)
{
   // Typedef locali.
   typedef typename std::iterator_traits<ForwardIterator1>::difference_type
                        SampleDiffType;

   // Dichiarazioni.
   SampleDiffType      S;    // Numero di campioni.
   ForwardIterator1    It;   // Iteratore campioni.
   ForwardIterator2    Ot;   // Iteratore etichette.
   LibSvmNodePtr*      Xt;   // Iteratore x.
   double*             Yt;   // Iteratore y.

   S= std::distance(iSampleBegin, iSampleEnd);
   if (S < 0)
   {
      throw SpareLogicError("SvmCommon, 0, Invalid sample iterators.");
   }

   // Inizializzazione.
   rProblem.l= 0;
   rProblem.y= NULL;
   rProblem.x= NULL;

   // Esco se non ho campioni.
   if (S < 1)
   {
      return;
   }

   // Allocazione problema.
   rProblem.l= boost::numeric::converter<int, SampleDiffType>::convert(S);
   rProblem.y= (double*) malloc(sizeof(double) * rProblem.l);
   rProblem.x= (LibSvmNodePtr*) malloc(sizeof(LibSvmNodePtr) * rProblem.l);
   std::fill(rProblem.x, rProblem.x + rProblem.l, (svm_node*) NULL);

   // Copia dati.
   It= iSampleBegin;
   Ot= iLabelBegin;
   Xt= rProblem.x;
   Yt= rProblem.y;
   while (It != iSampleEnd)
   {
      (*Xt++)= CreateSvmX(*It++);
      (*Yt++)= *Ot++;
   }
}  // CreateSvmProblem

void
DestroySvmX(LibSvmNodePtr pX)
{
   if (pX)
   {
      free(pX);
      pX= NULL;
   }
}  // DestroySvmX

void
DestroySvmProblem(LibSvmProblem& rProblem)
{
   int i;

   if (rProblem.y)
   {
      free(rProblem.y);
      rProblem.y= NULL;
   }

   if (rProblem.x)
   {
      for (i= 0; i < rProblem.l; ++i)
      {
         DestroySvmX(rProblem.x[i]);
      }

      free(rProblem.x);
      rProblem.x= NULL;
   }
}  // DestroySvmProblem

void
PrintSvmProblem(
            std::ostream&          rStrm,
            const LibSvmProblem&   rProblem)
{
   if (!rStrm.good())
   {
      throw SpareRuntimeError("SvmCommon, 1, Output stream error.");
   }

   LibSvmNodePtr*      Xt;  // Iteratore x.
   double*             Yt;  // Iteratore y.
   LibSvmNodePtr       Vt;  // Iteratore valori x.
   int                 i;   // Contatore.

   Xt= rProblem.x;
   Yt= rProblem.y;
   for (i= 0; i < rProblem.l; ++i)
   {
      rStrm << *Yt++;

      Vt= Xt[i];
      while (Vt->index != -1)
      {
         rStrm << ' ' << Vt->index << ':' << Vt->value;
         ++Vt;
      }
      rStrm << std::endl;

      if (!rStrm.good())
      {
         throw SpareRuntimeError("SvmCommon, 2, Output stream error.");
      }
   }
}  // PrintSvmProblem

void
FinalizeSvmModel(LibSvmModelPtr pModel)
{
   int                  i;
   LibSvmNodePtr        X;
   LibSvmModelStruct*   ModelPtr= (LibSvmModelStruct*) pModel;

   for (i= 0; i < ModelPtr->l; ++i)
   {
      X= CopySvmX(ModelPtr->SV[i]);
      ModelPtr->SV[i]= X;
   }

   ModelPtr->free_sv= 1;
}  // FinalizeSvmModel

void
SvNumberFromSvmModel(
           ConstLibSvmModelPtr  pModel,
           int&                 rSvNumber)
{
   const LibSvmModelStruct* ModelPtr= (const LibSvmModelStruct*) pModel;
   rSvNumber= ModelPtr->l;
}  // SvNumberFromSvmModel

void
SvmModelToText(
           ConstLibSvmModelPtr  pModel,
           std::string&         rText)
{
   svm_save_model(SPARE_SVM_TMP_FILE, pModel);
   std::ifstream ModelFile(SPARE_SVM_TMP_FILE);
   rText.assign(
            std::istreambuf_iterator<char>(ModelFile),
            std::istreambuf_iterator<char>());
}  // SvmModelToText

void
SvmModelFromText(
           LibSvmModelPtr&      rpModel,
           const std::string&   rText)
{
   std::ofstream ModelFile(SPARE_SVM_TMP_FILE);
   ModelFile << rText;
   ModelFile.close();
   rpModel= svm_load_model(SPARE_SVM_TMP_FILE);
}  // SvmModelFromText

}  // namespace spare

#endif // _SvmCommon_h_
