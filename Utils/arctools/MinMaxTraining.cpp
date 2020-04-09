//---------------------------------------------------------------------------
#include "MinMaxTraining.h"

TTempHyperBox :: TTempHyperBox(const TVectorCollection* DS, TMembershipType MT)
   : THyperBox(DS->GetTotalNumberOfComponents(), MT)
{
 DataSet = DS;
 Status = hsEmpty;
}

TTempHyperBox :: TTempHyperBox(const TTempHyperBox& Source)
   : THyperBox(Source)
{
 DataSet = Source.DataSet;
 Status = Source.Status;
 Content = Source.Content;
}

TTempHyperBox& TTempHyperBox :: operator=(const TTempHyperBox& Source)
{
 if (this != &Source)
    {
     V = Source.V;
     W = Source.W;
     Class = Source.Class;

     MembType = Source.MembType;
     MembPars = Source.MembPars;

     DataSet = Source.DataSet;
     Status = Source.Status;
     Content = Source.Content;
    }

 return *this;
}

bool TTempHyperBox :: operator==(const TTempHyperBox& Other) const
{
 return (
          (V == Other.V) &&
          (W == Other.W) &&
          (Class == Other.Class) &&
          (MembType == Other.MembType) &&
          (MembPars == Other.MembPars) &&
          (DataSet == Other.DataSet) &&
          (Status == Other.Status) &&
          (Content == Other.Content)
        );
}

bool TTempHyperBox :: operator<(const TTempHyperBox& Other) const
{
 if (Status != Other.Status) return (Status < Other.Status);
 else
    {
     int NT = GetNumberOfPatterns();
     int NO = Other.GetNumberOfPatterns();
     if (NT != NO) return (NT > NO);
     else
        {
         if (Class != Other.Class) return (Class < Other.Class);
         else
            {
             if (V != Other.V) return (V < Other.V);
             else
                {
                 if (W != Other.W) return (W < Other.W);
                 else
                    {
                     if (MembType != Other.MembType) return (MembType < Other.MembType);
                     else
                        {
                         if (MembPars != Other.MembPars) return (MembPars < Other.MembPars);
                         else return (Content < Other.Content);
                        }
                    }
                }
            }
        }
    }
}

int TTempHyperBox :: GetNumberOfPatterns() const
{
 int P = 0;
 THyperBoxContent::const_iterator it;
 for(it = Content.begin(); it != Content.end(); it++)
    P += it->second.size();

 return P;
}

void TTempHyperBox :: UpdateVW(const prlreal* Patt)
{
 int N = V.size();
 for(int i = 0; i < N; i++)
    {
     if (Patt[i] < V[i]) V[i] = Patt[i];
     if (Patt[i] > W[i]) W[i] = Patt[i];
    }
}

void TTempHyperBox :: InitVW(const prlreal* Patt)
{
 int N = V.size();
 for(int i = 0; i < N; i++)
    {
     V[i] = Patt[i];
     W[i] = Patt[i];
    }
}

bool TTempHyperBox :: Includes(int PattInd)
{
 bool Res = true;

 const prlreal* Patt = DataSet->GetDataPointer(PattInd);
 int N = V.size();
 for(int i = 0; i < N; i++)
    if ( (Patt[i] < V[i]) || (Patt[i] > W[i]) )
       Res = false;

 return Res;
}

void TTempHyperBox :: AddPattern(int Index)
{
 Content[ DataSet->GetClass(Index) ].push_back( Index );

 if (Status == hsEmpty) InitVW( DataSet->GetDataPointer(Index) );
 else UpdateVW( DataSet->GetDataPointer(Index) );

 if ( Content.size() == 1 )
    {
     Status = hsPure;
     Class = Content.begin()->first;
    }
 else
    {
     Status = hsHybrid;
     Class = HybClass;
    }
}

void TTempHyperBox :: Affiliate(int Index)
{
 Content[ DataSet->GetClass(Index) ].push_back( Index );
}

int TTempHyperBox :: GetNumberOfIntruders() const
{
 int Res = 0;
 THyperBoxContent::const_iterator it;

 for(it = Content.begin(); it != Content.end(); it++)
    if(it->first != Class) Res += it->second.size();

 return Res;
}

void TTempHyperBox :: GetPatternIndexes(std::list<int>& PattInd) const
{
 if ( !Content.empty() )
    {
     THyperBoxContent::const_iterator it = Content.begin();
     for(it = Content.begin(); it != Content.end(); it++)
        PattInd.insert( PattInd.end(), it->second.begin(), it->second.end() );
    }
}

void TTempHyperBox :: Clear()
{
 int N = DataSet->GetTotalNumberOfComponents();
 V.assign(N, 0);
 W.assign(N, 0);
 Content.clear();
 Status = hsEmpty;
}

void TTempHyperBox :: Fill(std::list<int> PattInd)
{
 Clear();

 PattInd.sort();
 PattInd.unique();

 int P = DataSet->GetNumberOfPatterns();

 // controlli
 if ( PattInd.front() < 0 ) throw std::runtime_error("Invalid Index List");
 if ( PattInd.back() >= P ) throw std::runtime_error("Invalid Index List");

 std::list<int>::iterator it;
 for(it = PattInd.begin(); it != PattInd.end(); it++)
    AddPattern(*it);
}

void TTempHyperBox :: TotalFill()
{
 Clear();
 int P = DataSet->GetNumberOfPatterns();
 for(int i = 0; i < P; i++) AddPattern(i);
}

void TTempHyperBox :: Cut(TCutPoint CP, TTempHyperBox& Child1, TTempHyperBox& Child2) const
{
 // controllo m
 int N = V.size();
 if ( CP.m < 0 ) throw std::runtime_error("Invalid Cut Point");
 if ( CP.m >= N ) throw std::runtime_error("Invalid Cut Point");

 // controllo
 if (Child1.DataSet != DataSet)
    throw std::runtime_error("Trying to use HBs with different DataSet pointers together");
 if (Child2.DataSet != DataSet)
    throw std::runtime_error("Trying to use HBs with different DataSet pointers together");

 // svuoto Child1 e Child2
 Child1.Clear();
 Child2.Clear();

 // riempio Child1 e Child2
 THyperBoxContent::const_iterator it;
 for(it = Content.begin(); it != Content.end(); it++)
    {
     std::list<int>::const_iterator pt;
     for(pt = it->second.begin(); pt != it->second.end(); pt++)
        if ( DataSet->GetDataPointer(*pt)[CP.m] <= CP.alpha ) Child1.AddPattern(*pt);
        else Child2.AddPattern(*pt);
    }
}

TCutPoint TTempHyperBox :: CalcCutPoint(TCutMode CM) const
{
 if (Status != hsHybrid)
    throw std::runtime_error("Cannot calculate Cut Points for a Non-Hybrid HyperBox");

 if (Content.size() < 2)
    throw std::runtime_error("Found a Pure HyperBox labeled as Hybrid");

 TCutPoint CP;

 if (CM == cmCentroids)
    {
     THyperBoxContent::const_iterator it_f; // prima classe
     THyperBoxContent::const_iterator it_s; // seconda classe
     THyperBoxContent::const_iterator it_aux;

     it_f = Content.begin();
     for( it_aux = Content.begin(); it_aux != Content.end(); it_aux++ )
        if ( it_aux->second.size() > it_f->second.size() ) it_f = it_aux;

     it_s = Content.begin();
     if (it_s == it_f) it_s++;
     for( it_aux = Content.begin(); it_aux != Content.end(); it_aux++ )
        if ( ( it_aux->second.size() > it_s->second.size() ) &&
             ( it_aux != it_f ) ) it_s = it_aux;

     int SzF = it_f->second.size();
     int SzS = it_s->second.size();
     if ( (SzF == 0) || (SzS == 0) ) throw std::runtime_error("Unexpected empty list");

     int N = V.size();
     std::vector<prlreal> CentF(N, 0); // centroide prima classe
     std::vector<prlreal> CentS(N, 0); // centroide seconda classe

     std::list<int>::const_iterator it;
     for(it = it_f->second.begin(); it != it_f->second.end(); it++)
        {
         const prlreal* Patt = DataSet->GetDataPointer(*it);
         for(int i = 0; i < N; i++)
            CentF[i] += Patt[i];
        }
     for(int i = 0; i < N; i++) CentF[i] /= SzF;

     for(it = it_s->second.begin(); it != it_s->second.end(); it++)
        {
         const prlreal* Patt = DataSet->GetDataPointer(*it);
         for(int i = 0; i < N; i++)
            CentS[i] += Patt[i];
        }
     for(int i = 0; i < N; i++) CentS[i] /= SzS;

     std::vector<prlreal> CentDist(N, 0);
     for(int i = 0; i < N; i++) CentDist[i] = fabs(CentS[i] - CentF[i]);

     prlreal Dmax = CentDist[0];
     int DmaxIndex = 0;
     for(int i = 1; i < N; i++)
        if (CentDist[i] > Dmax)
           {
            Dmax = CentDist[i];
            DmaxIndex = i;
           }

     CP.m = DmaxIndex;
     CP.alpha = ( CentF[DmaxIndex] + CentS[DmaxIndex] ) / 2; // metà
     //CP.alpha = ( SzS * CentF[DmaxIndex] + SzF * CentS[DmaxIndex] ) / (SzS + SzF); // pesatura "inversa"
    }
 else throw std::runtime_error("Unknown Cut Mode");

 return CP;
}

void TTempHyperBox :: Fusion(const TTempHyperBox& Other, TTempHyperBox& Result) const
{
 if (Other.DataSet != DataSet)
    throw std::runtime_error("Trying to use HBs with different DataSet pointers together");

 if (Result.DataSet != DataSet)
    throw std::runtime_error("Trying to use HBs with different DataSet pointers together");

 int N = V.size();
 for(int i = 0; i < N; i++)
    {
     Result.V[i] = prl_min(V[i], Other.V[i]);
     Result.W[i] = prl_max(W[i], Other.W[i]);
    }

 Result.Content = Content;

 THyperBoxContent::const_iterator it;
 for(it = Other.Content.begin(); it != Other.Content.end(); it++)
    Result.Content[ it->first ].insert( Result.Content[ it->first ].end(),
                                        it->second.begin(), it->second.end() );

 THyperBoxContent::iterator it_r;
 for(it_r = Result.Content.begin(); it_r != Result.Content.end(); it_r++)
    {
     it_r->second.sort();
     it_r->second.unique();
    }

 if ( Result.Content.empty() )
    {
     Result.Status = hsEmpty;
     Result.Class = HybClass;
    }
 else
    {
     if ( (Status == hsPure) && (Other.Status == hsPure) && (Class == Other.Class) )
        {
         Result.Status = hsPure;
         Result.Class = Class;
        }
     else
        {
         Result.Status = hsHybrid;
         Result.Class = HybClass;
        }
    }
}

void TTempHyperBox :: WriteContentDebug(std::string NomeFile) const
{
 std::ofstream deb( NomeFile.c_str() );

 THyperBoxContent::const_iterator it;
 for(it = Content.begin(); it != Content.end(); it++)
    {
     deb << it->first.c_str() << std::endl;
     std::list<int>::const_iterator it_l;
     for(it_l = it->second.begin(); it_l != it->second.end(); it_l++)
        deb << *it_l << std::endl;
     deb << std::endl;
    }
}

bool TTempHyperBox :: SimpleOverlap(prlreal v1, prlreal w1, prlreal v2, prlreal w2) const
{
 if (v1 == v2) return true;
 else
    {
     if (v1 < v2) return (w1 >= v2);
     else return (w2 >= v1);
    }
}

bool TTempHyperBox :: OverlapsWith(const TTempHyperBox& Other) const
{
 int N = V.size();
 bool Res = true;
 for(int i = 0; i < N; i++)
    if ( !SimpleOverlap(V[i], W[i], Other.V[i], Other.W[i]) ) Res = false;

 return Res;
}

bool TTempHyperBox :: OverlapsWithFusionOf(const TTempHyperBox& Other1, const TTempHyperBox& Other2) const
{
 int N = V.size();
 std::vector<prlreal> FV(N, 0);
 std::vector<prlreal> FW(N, 0);
 for(int i = 0; i < N; i++)
    {
     FV[i] = prl_min(Other1.V[i], Other2.V[i]);
     FW[i] = prl_max(Other1.W[i], Other2.W[i]);
    }

 bool Res = true;
 for(int i = 0; i < N; i++)
    if ( !SimpleOverlap(V[i], W[i], FV[i], FW[i]) ) Res = false;

 return Res;
}

// funzione eliminazione incongruenti

int DeleteIncongruent(TVectorCollection* DataSet)
{
 std::vector< std::set<int> > IncongruentSets;
 int P = DataSet->GetNumberOfPatterns();
 int N = DataSet->GetTotalNumberOfComponents();

 for(int p = 0; p < P-1; p++)
    {
     bool Found = false;
     int TS = IncongruentSets.size();
     for(int s = 0; s < TS; s++)
        if ( IncongruentSets[s].find(p) != IncongruentSets[s].end() )
           Found = true;

     if (!Found)
        {
         std::set<int> NewSet;
         for(int q = p+1; q < P; q++)
            {
             prlreal diff = fabs( DataSet->ReadComponent(p, 0) - DataSet->ReadComponent(q, 0) );
             for(int c = 1; c < N; c++)
                {
                 prlreal aux = fabs( DataSet->ReadComponent(p, c) - DataSet->ReadComponent(q, c) );
                 diff = (aux > diff) ? aux : diff;
                }

             if (diff < ZT)
                {
                 if ( NewSet.empty() ) NewSet.insert(p);
                 NewSet.insert(q);
                }
            }

         if ( !NewSet.empty() )
            {
             std::set<int>::iterator it = NewSet.begin();
             std::string FirstClass = DataSet->GetClass(*it);
             bool MoreThanOne = false;
             for(it = NewSet.begin(); it != NewSet.end(); it++)
                if (DataSet->GetClass(*it) != FirstClass)
                   MoreThanOne = true;

             if (MoreThanOne) IncongruentSets.push_back(NewSet);
            }
        }
    }

 int S = IncongruentSets.size();

 // fondo tutti i set in un unico
 std::set<int> GlobalSet;
 for(int s = 0; s < S; s++)
    GlobalSet.insert( IncongruentSets[s].begin(), IncongruentSets[s].end() );

 if ( GlobalSet.empty() ) return 0;
 else
    {
     std::set<int>::reverse_iterator itr;
     for(itr = GlobalSet.rbegin(); itr != GlobalSet.rend(); itr++)
        DataSet->DeletePattern(*itr);

     return GlobalSet.size();
    }
}

// TArcParcTraining

TArcParcTraining :: TArcParcTraining()
{
 MembType = mtSimpson;
 MembPars.assign(1, 1);
 CutMode = cmCentroids;
 Lambda = 0.5;
 ValSpeed = true;
 HBDelMode = hdSingle;
 RepEnabled = true;
 RName = "MinMaxTrainingReport.txt";
}

void TArcParcTraining :: SetMembershipType(TMembershipType Val)
{
 MembType = Val;

 switch (MembType)
    {
     case mtSimpson:

        MembPars.assign(1, 1);

     break;
     case mtTrapezoidal:

        MembPars.assign(1, 1);

     break;
     default: break;
    }
}

std::string TArcParcTraining :: GetMembParName(int i) const
{
 std::string Name = "Unknown";

 switch (MembType)
    {
     case mtSimpson:

        if (i != 0) throw std::runtime_error("Index out of bound");
        Name = "Gamma";

     break;
     case mtTrapezoidal:

        if (i != 0) throw std::runtime_error("Index out of bound");
        Name = "Gamma";

     break;
     default: break;
    }

 return Name;
}

void TArcParcTraining :: PreprocessingSetup(std::vector<int>& PM, TVectorNormalizationTool& NT)
{
 // controlli
 if ( !PM.empty() )
    {
     int MaskSize = PM.size();
     int MaskCount = 0;
     for(int i = 0; i < MaskSize; i++)
        if (PM[i]) MaskCount++;

     if (!MaskCount) throw std::runtime_error("Invalid mask");

     if ( NT.Initialized() )
        if ( MaskCount != NT.GetTotalNumberOfComponents() )
           throw std::runtime_error("Mask and normalization tool do not match");
    }

 ProjectionMask = PM;
 NormTool = NT;
}

void TArcParcTraining :: ArcTrain(TVectorCollection* TrainingSet,
                                  bool AlreadyPreproc,
                                  bool ReturnAll,
                                  std::list<TMinMaxNetwork>& ReturnList)
{
 // controlli
 if (TrainingSet->GetNumberOfPatterns() < 2)
    throw std::runtime_error("Less than 2 patterns in training set");

 // preprocessing
 if (AlreadyPreproc)
    {
     // controllo solo compatibilità con il NormTool, se inizializzato
     if ( NormTool.Initialized() )
        if ( TrainingSet->GetTotalNumberOfComponents() != NormTool.GetTotalNumberOfComponents() )
           throw std::runtime_error("Training set and normalization tool do not match");
    }
 else
    {
     if ( !ProjectionMask.empty() )
        {
         // controllo compatibilità con la maschera ed effettuo proiezione
         if ( TrainingSet->GetTotalNumberOfComponents() != int( ProjectionMask.size() ) )
            throw std::runtime_error("Training set and mask do not match");

         TrainingSet->Projection(&ProjectionMask);
        }

     if ( NormTool.Initialized() )
        {
         // controllo compatibilità con il norm tool ed effettuo normalizzazione
         if ( TrainingSet->GetTotalNumberOfComponents() != NormTool.GetTotalNumberOfComponents() )
            throw std::runtime_error("Training set and normalization tool do not match");

         NormTool.NormalizeCollection(TrainingSet);
        }
    }

 // report
 std::ofstream rep;

 if (RepEnabled)
    {
     rep.open( RName.c_str() );
     if (!rep) throw std::runtime_error("Errore apertura file!");
     rep.setf(std::ios_base::fixed, std::ios_base::floatfield);
     rep.precision(6);

     rep << "Arc Training Report";
     rep << "\n\nTraining Set: " << TrainingSet->GetCollectionName().c_str();
     rep << "\nOriginal number of patterns: " << TrainingSet->GetNumberOfPatterns();
    }

 // incongruenti
 int IDN = DeleteIncongruent(TrainingSet);

 if (RepEnabled) rep << "\nIncongruents deleted: " << IDN;

 int AP = TrainingSet->GetNumberOfPatterns();
 // numero di pattern dopo cancellazione incongruenti

 // misura tempo di esecuzione
 /*TDateTime StartTime, EndTime, ElapsedTime;
 Word Hour, Min, Sec, MSec;
 StartTime = Time();*/

 if ( !AP ) ReturnList.clear(); // ritorno una lista vuota
 else
 {
  if (RepEnabled)
     {
      rep << "\n\nGenerated Networks";
      rep << "\n------------------";
      rep << "\nCov\tEtr\tHBN\tCost";
     }

 // training
 int MP = MembPars.size(); // numero parametri membership

 // lista modelli
 std::list<TMinMaxNetwork> NetworkList;

 // copetura di partenza
 TCoverage CurrCoverage;
 TTempHyperBox StartHB(TrainingSet, MembType);
 for(int i = 0; i < MP; i++) StartHB.SetMembPar(i, MembPars[i]);
 StartHB.TotalFill();
 CurrCoverage[StartHB.GetClass()].push_back(StartHB);

 int CoverageCount = 1;

 while ( !CurrCoverage[HybClass].empty() )
    {
     // taglio hyperbox
     std::list<TTempHyperBox>::iterator hbit = CurrCoverage[HybClass].begin();
     std::list<TTempHyperBox>::iterator to_cut = hbit;
     hbit++;
     while ( hbit != CurrCoverage[HybClass].end() )
        {
         if ( hbit->GetNumberOfPatterns() > to_cut->GetNumberOfPatterns() )
            to_cut = hbit;
         hbit++;
        }

     // calcolo punto di taglio
     TCutPoint CP = to_cut->CalcCutPoint(CutMode);

     // preparo due nuovi hyperbox
     TTempHyperBox Child1(TrainingSet, MembType);
     TTempHyperBox Child2(TrainingSet, MembType);

     to_cut->Cut(CP, Child1, Child2);

     // se uno dei due è vuoto faccio un taglio casuale
     while( (Child1.GetStatus() == hsEmpty) || (Child2.GetStatus() == hsEmpty) )
        {
         int D = to_cut->GetInputSize();
         CP.m = rand()%D;
         CP.alpha = to_cut->GetV(CP.m) +
                    ( to_cut->GetW(CP.m) - to_cut->GetV(CP.m) ) *
                    ( (rand()%1001) / prlreal(1000) );
         to_cut->Cut(CP, Child1, Child2);
        }

     // assegno parametri membership a nuovi hb
     for(int i = 0; i < MP; i++)
        {
         Child1.SetMembPar(i, MembPars[i]);
         Child2.SetMembPar(i, MembPars[i]);
        }

     // cancello hb tagliato
     CurrCoverage[HybClass].erase(to_cut);

     // aggiungo nuovi hb a coverage
     CurrCoverage[Child1.GetClass()].push_back(Child1);
     CurrCoverage[Child2.GetClass()].push_back(Child2);

     CoverageCount++;

     if ( (Child1.GetStatus() == hsPure) ||
          (Child2.GetStatus() == hsPure) )
        {
         // passo di fusione
         std::vector<std::string> CurrClasses; // classi presenti tra gli hb della copertura
         std::vector<int> FusionFlag;         // flag che indica se ci sono tentativi di fusione da fare in una data classe

         TCoverage::const_iterator it;
         for(it = CurrCoverage.begin(); it != CurrCoverage.end(); it++)
            {
             CurrClasses.push_back(it->first);
             if (it->first == HybClass) FusionFlag.push_back(0); // non fondo hb ibridi
             else FusionFlag.push_back(1);
            }

         bool StopFusion = false;
         while( !StopFusion )
            {
             int CC = CurrClasses.size();
             for(int c = 0; c < CC; c++)
                if( FusionFlag[c] && (CurrCoverage[ CurrClasses[c] ].size() > 1) )
                   {
                    std::list<TTempHyperBox>::iterator hbit1 = CurrCoverage[ CurrClasses[c] ].begin();
                    std::list<TTempHyperBox>::iterator hbit2 = hbit1;
                    hbit2++;

                    bool Stop = false;
                    while( !Stop )
                       {
                        if ( hbit2 == CurrCoverage[ CurrClasses[c] ].end() )
                           {
                            Stop = true;
                            FusionFlag[c] = 0;
                           }
                        else
                           {
                            bool FusionAdmissible = true;
                            // controllo se la fusione è ammissibile, ovvero se uno qualsiasi
                            // degli hb delle altre classi si sovrappone con l'ipotetica fusione
                            // degli hb puntati da hbit1 e hbit2

                            int k = 0;
                            while ( (k < CC) && FusionAdmissible )
                               {
                                if (k != c)
                                   {
                                    std::list<TTempHyperBox>::const_iterator hbit_oc = CurrCoverage[ CurrClasses[k] ].begin();
                                    // iteratore hb altre classi

                                    while ( (hbit_oc != CurrCoverage[ CurrClasses[k] ].end()) && FusionAdmissible )
                                       {
                                        if ( hbit_oc->OverlapsWithFusionOf(*hbit1, *hbit2) )
                                           FusionAdmissible = false;
                                        hbit_oc++;
                                       }
                                   }

                                k++;
                               }

                            if ( FusionAdmissible )
                              {
                               Stop = true;

                               // fusione
                               TTempHyperBox FusionHB(TrainingSet, MembType);
                               hbit1->Fusion(*hbit2, FusionHB);

                               // assegno parametri membership a nuovo hb
                               for(int i = 0; i < MP; i++) FusionHB.SetMembPar(i, MembPars[i]);

                               // cancello hb fusi
                               CurrCoverage[ CurrClasses[c] ].erase(hbit1);
                               CurrCoverage[ CurrClasses[c] ].erase(hbit2);

                               // aggiungo nuovo hb a coverage
                               CurrCoverage[ CurrClasses[c] ].push_back(FusionHB);

                               CoverageCount++;
                              }
                            else
                              {
                               hbit2++;
                               if ( hbit2 == CurrCoverage[ CurrClasses[c] ].end() )
                                  {
                                   hbit1++;
                                   hbit2 = hbit1;
                                   hbit2++;
                                  }
                              }
                           }
                       }
                   }
                else FusionFlag[c] = 0;

             bool AllFlagsDown = true;
             for(int c = 0; c < CC; c++)
                if (FusionFlag[c]) AllFlagsDown = false;

             StopFusion = AllFlagsDown;
            }

         // generazione rete

         // ottengo vector hb puri
         // e lista pattern in hb ibridi se validation speed-up è attivo
         std::vector<THyperBox> PureHB;
         std::list<int> EtrList;
         for(it = CurrCoverage.begin(); it != CurrCoverage.end(); it++)
            if ( it->first == HybClass )
               {
                if (ValSpeed)
                   {
                    std::list<TTempHyperBox>::const_iterator hbit;
                    for(hbit = it->second.begin(); hbit != it->second.end(); hbit++)
                       hbit->GetPatternIndexes(EtrList);
                   }
               }
            else
               {
                PureHB.insert( PureHB.end(), it->second.begin(), it->second.end() );
               }

         // se validation speed-up non è attivo, EtrList sarà la lista di tutti i pattern
         if (!ValSpeed)
            for(int p = 0; p < AP; p++)
               EtrList.push_back(p);

         // alloco rete
         TMinMaxNetwork NewNetwork("Network", PureHB, ProjectionMask, NormTool);
         std::vector<prlreal> NetworkData(2, 0);
         // primo valore: costo, secondo valore: complessità

         // DEBUG
         /*if (ValSpeed)
            {
             unsigned int HLsize = EtrList.size();
             EtrList.sort();
             EtrList.unique();
             if (EtrList.size() != HLsize)
                throw std::runtime_error("Found some duplicates");
            }*/
         // END DEBUG

         // calcolo Etr
         prlreal Etr = 0;
         std::list<int>::iterator etr_it;
         for(etr_it = EtrList.begin(); etr_it != EtrList.end(); etr_it++)
            if ( NewNetwork.RawClassify( TrainingSet->GetDataPointer(*etr_it) ) !=
                 TrainingSet->GetClass(*etr_it) ) Etr++;
         Etr += IDN;
         Etr /= (AP + IDN);

         // calcolo Comp
         prlreal Comp = NewNetwork.GetNumberOfHyperBoxes();
         Comp /= (AP + IDN);

         prlreal Cost = (1-Lambda)*Etr + Lambda*Comp;

         NetworkData[0] = Cost;
         NetworkData[1] = Comp;

         NewNetwork.SetSortData(NetworkData);

         NetworkList.push_back(NewNetwork);
         NetworkList.sort();

         if (RepEnabled)
            {
             rep << '\n' << CoverageCount << '\t' << Etr << '\t' << NewNetwork.GetNumberOfHyperBoxes()
                 << '\t' << Cost;
            }
        }
    }

 /*EndTime = Time();
 ElapsedTime = EndTime - StartTime;*/
 if (RepEnabled)
    {
     //DecodeTime(ElapsedTime, Hour, Min, Sec, MSec);
     //std::string TimeStr = "Elapsed time: " + IntToStr( Hour ) + "h " + IntToStr( Min ) + "m "
     //                     + IntToStr( Sec ) + "s " + IntToStr( MSec ) + "ms";

     //rep << "\n\n" << TimeStr.c_str();
    }

 if (ReturnAll) ReturnList = NetworkList;
 else
    {
     ReturnList.clear();
     if ( !NetworkList.empty() )
        ReturnList.push_back( NetworkList.front() );
    }
 }
}

void TArcParcTraining :: ArcTrainCopyTR(const TVectorCollection* TrainingSet,
                                        bool AlreadyPreproc,
                                        bool ReturnAll,
                                        std::list<TMinMaxNetwork>& ReturnList)
{
 TVectorCollection TrCopy(*TrainingSet);
 ArcTrain(&TrCopy, AlreadyPreproc, ReturnAll, ReturnList);
}

void TArcParcTraining :: ParcTrain(TVectorCollection* TrainingSet,
                                   bool AlreadyPreproc,
                                   bool ReturnAll,
                                   std::list<TMinMaxNetwork>& ReturnList)
{
 // controlli
 if (TrainingSet->GetNumberOfPatterns() < 2)
    throw std::runtime_error("Less than 2 patterns in training set");

 // preprocessing
 if (AlreadyPreproc)
    {
     // controllo solo compatibilità con il NormTool, se inizializzato
     if ( NormTool.Initialized() )
        if ( TrainingSet->GetTotalNumberOfComponents() != NormTool.GetTotalNumberOfComponents() )
           throw std::runtime_error("Training set and normalization tool do not match");
    }
 else
    {
     if ( !ProjectionMask.empty() )
        {
         // controllo compatibilità con la maschera ed effettuo proiezione
         if ( TrainingSet->GetTotalNumberOfComponents() != int( ProjectionMask.size() ) )
            throw std::runtime_error("Training set and mask do not match");

         TrainingSet->Projection(&ProjectionMask);
        }

     if ( NormTool.Initialized() )
        {
         // controllo compatibilità con il norm tool ed effettuo normalizzazione
         if ( TrainingSet->GetTotalNumberOfComponents() != NormTool.GetTotalNumberOfComponents() )
            throw std::runtime_error("Training set and normalization tool do not match");

         NormTool.NormalizeCollection(TrainingSet);
        }
    }

 // report
 std::ofstream rep;

 if (RepEnabled)
    {
     rep.open( RName.c_str() );
     if (!rep) throw std::runtime_error("Errore apertura file!");
     rep.setf(std::ios_base::fixed, std::ios_base::floatfield);
     rep.precision(6);

     rep << "Parc Training Report";
     rep << "\n\nTraining Set: " << TrainingSet->GetCollectionName().c_str();
     rep << "\nOriginal number of patterns: " << TrainingSet->GetNumberOfPatterns();
    }

 // incongruenti
 int IDN = DeleteIncongruent(TrainingSet);

 if (RepEnabled) rep << "\nIncongruents deleted: " << IDN;

 int AP = TrainingSet->GetNumberOfPatterns();
 // numero di pattern dopo cancellazione incongruenti

 // misura tempo di esecuzione
 //TDateTime StartTime, EndTime, ElapsedTime;
 //Word Hour, Min, Sec, MSec;
 //StartTime = Time();

 if ( !AP ) ReturnList.clear(); // ritorno una lista vuota
 else
 {
  if (RepEnabled)
     {
      rep << "\n\nGenerated Networks";
      rep << "\n------------------";
      rep << "\nCov\tEtr\tHBN\tCost";
     }

 // training
 int MP = MembPars.size(); // numero parametri membership

 // lista modelli
 std::list<TMinMaxNetwork> NetworkList;

 // copetura di partenza
 TCoverage CurrCoverage;
 TTempHyperBox StartHB(TrainingSet, MembType);
 for(int i = 0; i < MP; i++) StartHB.SetMembPar(i, MembPars[i]);
 StartHB.TotalFill();
 CurrCoverage[StartHB.GetClass()].push_back(StartHB);

 int CoverageCount = 1;

 // primo passo: raggiungimento copertura totale e pura
 while ( !CurrCoverage[HybClass].empty() )
    {
     // taglio hyperbox
     std::list<TTempHyperBox>::iterator hbit = CurrCoverage[HybClass].begin();
     std::list<TTempHyperBox>::iterator to_cut = hbit;
     hbit++;
     while ( hbit != CurrCoverage[HybClass].end() )
        {
         if ( hbit->GetNumberOfPatterns() > to_cut->GetNumberOfPatterns() )
            to_cut = hbit;
         hbit++;
        }

     // calcolo punto di taglio
     TCutPoint CP = to_cut->CalcCutPoint(CutMode);

     // preparo due nuovi hyperbox
     TTempHyperBox Child1(TrainingSet, MembType);
     TTempHyperBox Child2(TrainingSet, MembType);

     to_cut->Cut(CP, Child1, Child2);

     // se uno dei due è vuoto faccio un taglio casuale
     while( (Child1.GetStatus() == hsEmpty) || (Child2.GetStatus() == hsEmpty) )
        {
         int D = to_cut->GetInputSize();
         CP.m = std::rand()%D;
         CP.alpha = to_cut->GetV(CP.m) +
                    ( to_cut->GetW(CP.m) - to_cut->GetV(CP.m) ) *
                    ( (std::rand()%1001) / prlreal(1000) );
         to_cut->Cut(CP, Child1, Child2);
        }

     // assegno parametri membership a nuovi hb
     for(int i = 0; i < MP; i++)
        {
         Child1.SetMembPar(i, MembPars[i]);
         Child2.SetMembPar(i, MembPars[i]);
        }

     // cancello hb tagliato
     CurrCoverage[HybClass].erase(to_cut);

     // aggiungo nuovi hb a coverage
     CurrCoverage[Child1.GetClass()].push_back(Child1);
     CurrCoverage[Child2.GetClass()].push_back(Child2);

     CoverageCount++;
    }

 // secondo passo: fusioni, generazione reti, cancellazioni
 std::list<int> FreePatts; // pattern "orfani"
 bool Complete = true;
 while (Complete)
    {
     // passo di fusione

     std::vector<std::string> CurrClasses; // classi presenti tra gli hb della copertura
     std::vector<int> FusionFlag;         // flag che indica se ci sono tentativi di fusione da fare in una data classe

     TCoverage::const_iterator it;
     for(it = CurrCoverage.begin(); it != CurrCoverage.end(); it++)
        {
         CurrClasses.push_back(it->first);
         if (it->first == HybClass) FusionFlag.push_back(0); // non fondo hb ibridi
         else FusionFlag.push_back(1);
        }

     bool StopFusion = false;
     while( !StopFusion )
        {
         int CC = CurrClasses.size();
         for(int c = 0; c < CC; c++)
            if( FusionFlag[c] && (CurrCoverage[ CurrClasses[c] ].size() > 1) )
               {
                std::list<TTempHyperBox>::iterator hbit1 = CurrCoverage[ CurrClasses[c] ].begin();
                std::list<TTempHyperBox>::iterator hbit2 = hbit1;
                hbit2++;

                bool Stop = false;
                while( !Stop )
                   {
                    if ( hbit2 == CurrCoverage[ CurrClasses[c] ].end() )
                       {
                        Stop = true;
                        FusionFlag[c] = 0;
                       }
                    else
                       {
                        bool FusionAdmissible = true;
                        // controllo se la fusione è ammissibile, ovvero se uno qualsiasi
                        // degli hb delle altre classi si sovrappone con l'ipotetica fusione
                        // degli hb puntati da hbit1 e hbit2

                        int k = 0;
                        while ( (k < CC) && FusionAdmissible )
                           {
                            if (k != c)
                               {
                                std::list<TTempHyperBox>::const_iterator hbit_oc = CurrCoverage[ CurrClasses[k] ].begin();
                                // iteratore hb altre classi

                                while ( (hbit_oc != CurrCoverage[ CurrClasses[k] ].end()) && FusionAdmissible )
                                   {
                                    if ( hbit_oc->OverlapsWithFusionOf(*hbit1, *hbit2) )
                                       FusionAdmissible = false;
                                    hbit_oc++;
                                   }
                               }

                            k++;
                           }

                        if ( FusionAdmissible )
                          {
                           Stop = true;

                           // fusione
                           TTempHyperBox FusionHB(TrainingSet, MembType);
                           hbit1->Fusion(*hbit2, FusionHB);

                           // assegno parametri membership a nuovo hb
                           for(int i = 0; i < MP; i++) FusionHB.SetMembPar(i, MembPars[i]);

                           // affilio eventuali pattern orfani al nuovo hb
                           std::list<int>::iterator p_it = FreePatts.begin();
                           while ( p_it != FreePatts.end() )
                              if ( FusionHB.Includes(*p_it) )
                                 {
                                  FusionHB.Affiliate(*p_it);
                                  p_it = FreePatts.erase(p_it);
                                 }
                              else p_it++;

                           // cancello hb fusi
                           CurrCoverage[ CurrClasses[c] ].erase(hbit1);
                           CurrCoverage[ CurrClasses[c] ].erase(hbit2);

                           // aggiungo nuovo hb a coverage
                           CurrCoverage[ CurrClasses[c] ].push_back(FusionHB);

                           CoverageCount++;
                          }
                        else
                          {
                           hbit2++;
                           if ( hbit2 == CurrCoverage[ CurrClasses[c] ].end() )
                              {
                               hbit1++;
                               hbit2 = hbit1;
                               hbit2++;
                              }
                          }
                       }
                   }
               }
            else FusionFlag[c] = 0;

         bool AllFlagsDown = true;
         for(int c = 0; c < CC; c++)
            if (FusionFlag[c]) AllFlagsDown = false;

         StopFusion = AllFlagsDown;
        }

     // generazione rete

     // PATCH 25-4-2009
     CurrCoverage.erase(HybClass);

     // ottengo vector hb puri
     std::vector<THyperBox> PureHB;
     for(it = CurrCoverage.begin(); it != CurrCoverage.end(); it++)
        PureHB.insert( PureHB.end(), it->second.begin(), it->second.end() );

     // se validation speed-up non è attivo, EtrList sarà la lista di tutti i pattern,
     // altrimenti sarà la lista dei pattern orfani
     std::list<int> EtrList;
     int IntNum = 0; // numero totale di pattern intrusi (per caso ValSpeed)
     if (!ValSpeed) for(int p = 0; p < AP; p++) EtrList.push_back(p);
     else
        {
         EtrList = FreePatts;

         // conteggio intrusi
         std::list<TTempHyperBox>::const_iterator hbit;
         for(it = CurrCoverage.begin(); it != CurrCoverage.end(); it++)
            for(hbit = it->second.begin(); hbit != it->second.end(); hbit++)
               IntNum += hbit->GetNumberOfIntruders();
        }

     // alloco rete
     TMinMaxNetwork NewNetwork("Network", PureHB, ProjectionMask, NormTool);
     std::vector<prlreal> NetworkData(2, 0);
     // primo valore: costo, secondo valore: complessità

     // calcolo Etr
     prlreal Etr = 0;
     std::list<int>::iterator etr_it;
     for(etr_it = EtrList.begin(); etr_it != EtrList.end(); etr_it++)
        if ( NewNetwork.RawClassify( TrainingSet->GetDataPointer(*etr_it) ) !=
             TrainingSet->GetClass(*etr_it) ) Etr++;
     Etr += (IDN + IntNum);
     Etr /= (AP + IDN);

     // calcolo Comp
     prlreal Comp = NewNetwork.GetNumberOfHyperBoxes();
     Comp /= (AP + IDN);

     prlreal Cost = (1-Lambda)*Etr + Lambda*Comp;

     NetworkData[0] = Cost;
     NetworkData[1] = Comp;

     NewNetwork.SetSortData(NetworkData);

     NetworkList.push_back(NewNetwork);
     NetworkList.sort();

     if (RepEnabled)
        {
         rep << '\n' << CoverageCount << '\t' << Etr << '\t' << NewNetwork.GetNumberOfHyperBoxes()
             << '\t' << Cost;
        }

     // riduzione strutturale

     // trovo numero minimo di pattern in HB
     int MinPN = CurrCoverage.begin()->second.begin()->GetNumberOfPatterns();
     for(it = CurrCoverage.begin(); it != CurrCoverage.end(); it++)
        {
         std::list<TTempHyperBox>::const_iterator hbit = it->second.begin();
         while ( hbit != it->second.end() )
            {
             int CurrNP = hbit->GetNumberOfPatterns();
             if ( CurrNP < MinPN ) MinPN = CurrNP;
             hbit++;
            }
        }

     TCoverage::iterator er_it;
     bool Found;
     switch (HBDelMode)
        {
         case hdSingle:

            Found = false;
            er_it = CurrCoverage.begin();
            while( (er_it != CurrCoverage.end()) && (!Found) )
               {
                std::list<TTempHyperBox>::iterator hbit = er_it->second.begin();
                while ( (hbit != er_it->second.end()) && (!Found) )
                   if ( hbit->GetNumberOfPatterns() == MinPN ) Found = true;
                   else hbit++;

                if (Found)
                   {
                    hbit->GetPatternIndexes(FreePatts);
                    hbit = er_it->second.erase(hbit);
                   }
                else er_it++;
               }

         break;
         case hdBlock:

            for(er_it = CurrCoverage.begin(); er_it != CurrCoverage.end(); er_it++)
               {
                std::list<TTempHyperBox>::iterator hbit = er_it->second.begin();
                while ( hbit != er_it->second.end() )
                   {
                    int CurrNP = hbit->GetNumberOfPatterns();
                    if ( CurrNP == MinPN )
                       {
                        hbit->GetPatternIndexes(FreePatts);
                        hbit = er_it->second.erase(hbit);
                       }
                    else hbit++;
                   }
               }

         break;
        }

     CoverageCount++;

     // controllo se la copertura corrente è ancora completa
     for(it = CurrCoverage.begin(); it != CurrCoverage.end(); it++)
        if ( ( it->first != HybClass ) && ( it->second.empty() ) )
           Complete = false;
    }

 //EndTime = Time();
 //ElapsedTime = EndTime - StartTime;
 if (RepEnabled)
    {
     //DecodeTime(ElapsedTime, Hour, Min, Sec, MSec);
     //std::string TimeStr = "Elapsed time: " + IntToStr( Hour ) + "h " + IntToStr( Min ) + "m "
       //                   + IntToStr( Sec ) + "s " + IntToStr( MSec ) + "ms";

    // rep << "\n\n" << TimeStr.c_str();
    }

 if (ReturnAll) ReturnList = NetworkList;
 else
    {
     ReturnList.clear();
     if ( !NetworkList.empty() )
        ReturnList.push_back( NetworkList.front() );
    }
 }
}

void TArcParcTraining :: ParcTrainCopyTR(const TVectorCollection* TrainingSet,
                                         bool AlreadyPreproc,
                                         bool ReturnAll,
                                         std::list<TMinMaxNetwork>& ReturnList)
{
 TVectorCollection TrCopy(*TrainingSet);
 ParcTrain(&TrCopy, AlreadyPreproc, ReturnAll, ReturnList);
}

void TArcParcTraining :: ArcTrainLazyInc(TVectorCollection* TrainingSet,
                                         bool AlreadyPreproc,
                                         bool ReturnAll,
                                         std::list<TMinMaxNetwork>& ReturnList)
{
 // controlli
 if (TrainingSet->GetNumberOfPatterns() < 2)
    throw std::runtime_error("Less than 2 patterns in training set");

 // preprocessing
 if (AlreadyPreproc)
    {
     // controllo solo compatibilità con il NormTool, se inizializzato
     if ( NormTool.Initialized() )
        if ( TrainingSet->GetTotalNumberOfComponents() != NormTool.GetTotalNumberOfComponents() )
           throw std::runtime_error("Training set and normalization tool do not match");
    }
 else
    {
     if ( !ProjectionMask.empty() )
        {
         // controllo compatibilità con la maschera ed effettuo proiezione
         if ( TrainingSet->GetTotalNumberOfComponents() != int( ProjectionMask.size() ) )
            throw std::runtime_error("Training set and mask do not match");

         TrainingSet->Projection(&ProjectionMask);
        }

     if ( NormTool.Initialized() )
        {
         // controllo compatibilità con il norm tool ed effettuo normalizzazione
         if ( TrainingSet->GetTotalNumberOfComponents() != NormTool.GetTotalNumberOfComponents() )
            throw std::runtime_error("Training set and normalization tool do not match");

         NormTool.NormalizeCollection(TrainingSet);
        }
    }

 // report
 std::ofstream rep;

 if (RepEnabled)
    {
     rep.open( RName.c_str() );
     if (!rep) throw std::runtime_error("Errore apertura file!");
     rep.setf(std::ios_base::fixed, std::ios_base::floatfield);
     rep.precision(6);

     rep << "Arc Training Report";
     rep << "\n\nTraining Set: " << TrainingSet->GetCollectionName().c_str();
     rep << "\nOriginal number of patterns: " << TrainingSet->GetNumberOfPatterns();
    }

 // incongruenti 
 int IDN = 0; // LAZY_INC
 int AP = TrainingSet->GetNumberOfPatterns();

 std::vector<bool> Incongruent(AP, false);
 std::list<int>    TempIncList;
 
 // misura tempo di esecuzione
 /*TDateTime StartTime, EndTime, ElapsedTime;
 Word Hour, Min, Sec, MSec;

 StartTime = Time();*/

 if ( !AP ) ReturnList.clear(); // ritorno una lista vuota
 else
 {
  if (RepEnabled)
     {
      rep << "\n\nGenerated Networks";
      rep << "\n------------------";
      rep << "\nCov\tEtr\tHBN\tCost";
     }

 // training
 int MP = MembPars.size(); // numero parametri membership

 // lista modelli
 std::list<TMinMaxNetwork> NetworkList;

 // copetura di partenza
 TCoverage CurrCoverage;
 TTempHyperBox StartHB(TrainingSet, MembType);
 for(int i = 0; i < MP; i++) StartHB.SetMembPar(i, MembPars[i]);
 StartHB.TotalFill();
 CurrCoverage[StartHB.GetClass()].push_back(StartHB);

 int CoverageCount = 1;
 int InputSize = TrainingSet->GetTotalNumberOfComponents();
 bool Degenerate;

 while ( !CurrCoverage[HybClass].empty() )
    {
     // taglio hyperbox
     std::list<TTempHyperBox>::iterator hbit = CurrCoverage[HybClass].begin();
     std::list<TTempHyperBox>::iterator to_cut = hbit;
     hbit++;
     while ( hbit != CurrCoverage[HybClass].end() )
        {
         if ( hbit->GetNumberOfPatterns() > to_cut->GetNumberOfPatterns() )
            to_cut = hbit;
         hbit++;
        }

     // calcolo punto di taglio
     TCutPoint CP = to_cut->CalcCutPoint(CutMode);

     // preparo due nuovi hyperbox
     TTempHyperBox Child1(TrainingSet, MembType);
     TTempHyperBox Child2(TrainingSet, MembType);

     to_cut->Cut(CP, Child1, Child2);

     // se uno dei due è vuoto faccio un taglio casuale
     while( (Child1.GetStatus() == hsEmpty) || (Child2.GetStatus() == hsEmpty) )
        {
         int D = to_cut->GetInputSize();
         CP.m = rand()%D;
         CP.alpha = to_cut->GetV(CP.m) +
                    ( to_cut->GetW(CP.m) - to_cut->GetV(CP.m) ) *
                    ( (rand()%1001) / prlreal(1000) );
         to_cut->Cut(CP, Child1, Child2);
        }

     // assegno parametri membership a nuovi hb
     for(int i = 0; i < MP; i++)
        {
         Child1.SetMembPar(i, MembPars[i]);
         Child2.SetMembPar(i, MembPars[i]);
        }

     // cancello hb tagliato
     CurrCoverage[HybClass].erase(to_cut);

     // aggiungo nuovi hb a coverage
     CurrCoverage[Child1.GetClass()].push_back(Child1);
     CurrCoverage[Child2.GetClass()].push_back(Child2);

     CoverageCount++;

     if ( (Child1.GetStatus() == hsPure) ||
          (Child2.GetStatus() == hsPure) )
        {
         // passo di fusione
         std::vector<std::string> CurrClasses; // classi presenti tra gli hb della copertura
         std::vector<int> FusionFlag;         // flag che indica se ci sono tentativi di fusione da fare in una data classe

         TCoverage::const_iterator it;
         for(it = CurrCoverage.begin(); it != CurrCoverage.end(); it++)
            {
             CurrClasses.push_back(it->first);
             if (it->first == HybClass) FusionFlag.push_back(0); // non fondo hb ibridi
             else FusionFlag.push_back(1);
            }

         bool StopFusion = false;
         while( !StopFusion )
            {
             int CC = CurrClasses.size();
             for(int c = 0; c < CC; c++)
                if( FusionFlag[c] && (CurrCoverage[ CurrClasses[c] ].size() > 1) )
                   {
                    std::list<TTempHyperBox>::iterator hbit1 = CurrCoverage[ CurrClasses[c] ].begin();
                    std::list<TTempHyperBox>::iterator hbit2 = hbit1;
                    hbit2++;

                    bool Stop = false;
                    while( !Stop )
                       {
                        if ( hbit2 == CurrCoverage[ CurrClasses[c] ].end() )
                           {
                            Stop = true;
                            FusionFlag[c] = 0;
                           }
                        else
                           {
                            bool FusionAdmissible = true;
                            // controllo se la fusione è ammissibile, ovvero se uno qualsiasi
                            // degli hb delle altre classi si sovrappone con l'ipotetica fusione
                            // degli hb puntati da hbit1 e hbit2

                            int k = 0;
                            while ( (k < CC) && FusionAdmissible )
                               {
                                if (k != c)
                                   {
                                    std::list<TTempHyperBox>::const_iterator hbit_oc = CurrCoverage[ CurrClasses[k] ].begin();
                                    // iteratore hb altre classi

                                    while ( (hbit_oc != CurrCoverage[ CurrClasses[k] ].end()) && FusionAdmissible )
                                       {
                                        if ( hbit_oc->OverlapsWithFusionOf(*hbit1, *hbit2) )
                                           FusionAdmissible = false;
                                        hbit_oc++;
                                       }
                                   }

                                k++;
                               }

                            if ( FusionAdmissible )
                              {
                               Stop = true;

                               // fusione
                               TTempHyperBox FusionHB(TrainingSet, MembType);
                               hbit1->Fusion(*hbit2, FusionHB);

                               // assegno parametri membership a nuovo hb
                               for(int i = 0; i < MP; i++) FusionHB.SetMembPar(i, MembPars[i]);

                               // cancello hb fusi
                               CurrCoverage[ CurrClasses[c] ].erase(hbit1);
                               CurrCoverage[ CurrClasses[c] ].erase(hbit2);

                               // aggiungo nuovo hb a coverage
                               CurrCoverage[ CurrClasses[c] ].push_back(FusionHB);

                               CoverageCount++;
                              }
                            else
                              {
                               hbit2++;
                               if ( hbit2 == CurrCoverage[ CurrClasses[c] ].end() )
                                  {
                                   hbit1++;
                                   hbit2 = hbit1;
                                   hbit2++;
                                  }
                              }
                           }
                       }
                   }
                else FusionFlag[c] = 0;

             bool AllFlagsDown = true;
             for(int c = 0; c < CC; c++)
                if (FusionFlag[c]) AllFlagsDown = false;

             StopFusion = AllFlagsDown;
            }

     // LAZY_INC ciclo per eliminare hb ibridi a dimensione nulla, ogni qualvolta N
     // pattern vengono eliminati IDN = IDN + N, AP = AP - N
     hbit = CurrCoverage[HybClass].begin();
     while ( hbit != CurrCoverage[HybClass].end() )
        {
           Degenerate = true;

           for (int i = 0; i < InputSize; ++i)
           {
              if (hbit->GetV(i) != hbit->GetW(i))
              {
                 Degenerate = false;
              }
           }
           
           if (Degenerate)
           {
               if (RepEnabled)
               {
                  rep << '\n' << "Erasing " << hbit->GetNumberOfPatterns() << " incongruents";
               }           
           
               IDN += hbit->GetNumberOfPatterns();
               AP -= hbit->GetNumberOfPatterns();
               
               hbit->GetPatternIndexes(TempIncList);
               
               hbit= CurrCoverage[HybClass].erase(hbit);
           }
           else
           {
               ++hbit;
           }
        } 

         // generazione rete

         // ottengo vector hb puri
         // e lista pattern in hb ibridi se validation speed-up è attivo
         std::vector<THyperBox> PureHB;
         std::list<int> EtrList;
         for(it = CurrCoverage.begin(); it != CurrCoverage.end(); it++)
            if ( it->first == HybClass )
               {
                if (ValSpeed)
                   {
                    std::list<TTempHyperBox>::const_iterator hbit;
                    for(hbit = it->second.begin(); hbit != it->second.end(); hbit++)
                       hbit->GetPatternIndexes(EtrList);
                   }
               }
            else
               {
                PureHB.insert( PureHB.end(), it->second.begin(), it->second.end() );
               }

         // se validation speed-up non è attivo, EtrList sarà la lista di tutti i pattern
         if (!ValSpeed)
            for(int p = 0; p < AP; p++)
               EtrList.push_back(p);

         // alloco rete
         TMinMaxNetwork NewNetwork("Network", PureHB, ProjectionMask, NormTool);
         std::vector<prlreal> NetworkData(2, 0);
         // primo valore: costo, secondo valore: complessità

         // DEBUG
         /*if (ValSpeed)
            {

             unsigned int HLsize = EtrList.size();
             EtrList.sort();
             EtrList.unique();
             if (EtrList.size() != HLsize)
                throw std::runtime_error("Found some duplicates");
            }*/
         // END DEBUG

         // calcolo Etr
         prlreal Etr = 0;
         std::list<int>::iterator etr_it;
         
         for(etr_it = TempIncList.begin(); etr_it != TempIncList.end(); etr_it++)
            Incongruent[*etr_it]= true; 
         
         TempIncList.clear();
         
         for(etr_it = EtrList.begin(); etr_it != EtrList.end(); etr_it++)
            if (!Incongruent[*etr_it])
               if ( NewNetwork.RawClassify( TrainingSet->GetDataPointer(*etr_it) ) != 
                    TrainingSet->GetClass(*etr_it) ) Etr++;
                 
         Etr += IDN;
         Etr /= (AP + IDN);

         // calcolo Comp
         prlreal Comp = NewNetwork.GetNumberOfHyperBoxes();
         Comp /= (AP + IDN);

         prlreal Cost = (1-Lambda)*Etr + Lambda*Comp;

         NetworkData[0] = Cost;
         NetworkData[1] = Comp;

         NewNetwork.SetSortData(NetworkData);

         NetworkList.push_back(NewNetwork);
         NetworkList.sort();

         if (RepEnabled)
            {
             rep << '\n' << CoverageCount << '\t' << Etr << '\t' << NewNetwork.GetNumberOfHyperBoxes()
                 << '\t' << Cost;
            }
        }
    }

 /*EndTime = Time();
 ElapsedTime = EndTime - StartTime;*/
 if (RepEnabled)
    {
     //DecodeTime(ElapsedTime, Hour, Min, Sec, MSec);
     //std::string TimeStr = "Elapsed time: " + IntToStr( Hour ) + "h " + IntToStr( Min ) + "m "
     //                     + IntToStr( Sec ) + "s " + IntToStr( MSec ) + "ms";

     //rep << "\n\n" << TimeStr.c_str();
    }

 if (ReturnAll) ReturnList = NetworkList;
 else
    {
     ReturnList.clear();
     if ( !NetworkList.empty() )
        ReturnList.push_back( NetworkList.front() );
    }
 }
}

void TArcParcTraining :: ParcTrainLazyInc(TVectorCollection* TrainingSet,
                                          bool AlreadyPreproc,
                                          bool ReturnAll,
                                          std::list<TMinMaxNetwork>& ReturnList)
{
 // controlli
 if (TrainingSet->GetNumberOfPatterns() < 2)
    throw std::runtime_error("Less than 2 patterns in training set");

 // preprocessing
 if (AlreadyPreproc)
    {
     // controllo solo compatibilità con il NormTool, se inizializzato
     if ( NormTool.Initialized() )
        if ( TrainingSet->GetTotalNumberOfComponents() != NormTool.GetTotalNumberOfComponents() )
           throw std::runtime_error("Training set and normalization tool do not match");
    }
 else
    {
     if ( !ProjectionMask.empty() )
        {
         // controllo compatibilità con la maschera ed effettuo proiezione
         if ( TrainingSet->GetTotalNumberOfComponents() != int( ProjectionMask.size() ) )
            throw std::runtime_error("Training set and mask do not match");

         TrainingSet->Projection(&ProjectionMask);
        }

     if ( NormTool.Initialized() )
        {
         // controllo compatibilità con il norm tool ed effettuo normalizzazione
         if ( TrainingSet->GetTotalNumberOfComponents() != NormTool.GetTotalNumberOfComponents() )
            throw std::runtime_error("Training set and normalization tool do not match");

         NormTool.NormalizeCollection(TrainingSet);
        }
    }

 // report
 std::ofstream rep;

 if (RepEnabled)
    {
     rep.open( RName.c_str() );
     if (!rep) throw std::runtime_error("Errore apertura file!");
     rep.setf(std::ios_base::fixed, std::ios_base::floatfield);
     rep.precision(6);

     rep << "Parc Training Report";
     rep << "\n\nTraining Set: " << TrainingSet->GetCollectionName().c_str();
     rep << "\nOriginal number of patterns: " << TrainingSet->GetNumberOfPatterns();
    }

 // incongruenti
 int IDN = 0;
 int AP = TrainingSet->GetNumberOfPatterns();

 std::vector<bool> Incongruent(AP, false);
 std::list<int>    TempIncList;

 int InputSize = TrainingSet->GetTotalNumberOfComponents();
 bool Degenerate;


 // misura tempo di esecuzione
 //TDateTime StartTime, EndTime, ElapsedTime;
 //Word Hour, Min, Sec, MSec;
 //StartTime = Time();

 if ( !AP ) ReturnList.clear(); // ritorno una lista vuota
 else
 {
  if (RepEnabled)
     {
      rep << "\n\nGenerated Networks";
      rep << "\n------------------";
      rep << "\nCov\tEtr\tHBN\tCost";
     }

 // training
 int MP = MembPars.size(); // numero parametri membership

 // lista modelli
 std::list<TMinMaxNetwork> NetworkList;

 // copetura di partenza
 TCoverage CurrCoverage;
 TTempHyperBox StartHB(TrainingSet, MembType);
 for(int i = 0; i < MP; i++) StartHB.SetMembPar(i, MembPars[i]);
 StartHB.TotalFill();
 CurrCoverage[StartHB.GetClass()].push_back(StartHB);

 int CoverageCount = 1;

 // primo passo: raggiungimento copertura totale e pura
 while ( !CurrCoverage[HybClass].empty() )
    {
     // taglio hyperbox
     std::list<TTempHyperBox>::iterator hbit = CurrCoverage[HybClass].begin();
     std::list<TTempHyperBox>::iterator to_cut = hbit;
     hbit++;
     while ( hbit != CurrCoverage[HybClass].end() )
        {
         if ( hbit->GetNumberOfPatterns() > to_cut->GetNumberOfPatterns() )
            to_cut = hbit;
         hbit++;
        }

     // calcolo punto di taglio
     TCutPoint CP = to_cut->CalcCutPoint(CutMode);

     // preparo due nuovi hyperbox
     TTempHyperBox Child1(TrainingSet, MembType);
     TTempHyperBox Child2(TrainingSet, MembType);

     to_cut->Cut(CP, Child1, Child2);

     // se uno dei due è vuoto faccio un taglio casuale
     while( (Child1.GetStatus() == hsEmpty) || (Child2.GetStatus() == hsEmpty) )
        {
         int D = to_cut->GetInputSize();
         CP.m = rand()%D;
         CP.alpha = to_cut->GetV(CP.m) +
                    ( to_cut->GetW(CP.m) - to_cut->GetV(CP.m) ) *
                    ( (rand()%1001) / prlreal(1000) );
         to_cut->Cut(CP, Child1, Child2);
        }

     // assegno parametri membership a nuovi hb
     for(int i = 0; i < MP; i++)
        {
         Child1.SetMembPar(i, MembPars[i]);
         Child2.SetMembPar(i, MembPars[i]);
        }

     // cancello hb tagliato
     CurrCoverage[HybClass].erase(to_cut);

     // aggiungo nuovi hb a coverage
     CurrCoverage[Child1.GetClass()].push_back(Child1);
     CurrCoverage[Child2.GetClass()].push_back(Child2);

     CoverageCount++;
     
     // LAZY_INC ciclo per eliminare hb ibridi a dimensione nulla, ogni qualvolta N
     // pattern vengono eliminati IDN = IDN + N, AP = AP - N
     hbit = CurrCoverage[HybClass].begin();
     while ( hbit != CurrCoverage[HybClass].end() )
        {
           Degenerate = true;

           for (int i = 0; i < InputSize; ++i)
           {
              if (hbit->GetV(i) != hbit->GetW(i))
              {
                 Degenerate = false;
              }
           }
           
           if (Degenerate)
           {
               if (RepEnabled)
               {
                  rep << '\n' << "Erasing " << hbit->GetNumberOfPatterns() << " incongruents";
               }           
           
               IDN += hbit->GetNumberOfPatterns();
               AP -= hbit->GetNumberOfPatterns();
               
               hbit->GetPatternIndexes(TempIncList);
               
               hbit= CurrCoverage[HybClass].erase(hbit);
           }
           else
           {
               ++hbit;
           }
        }     
    }

 if (!AP)
 {
     return;
 }

 // secondo passo: fusioni, generazione reti, cancellazioni
 std::list<int> FreePatts; // pattern "orfani"
 bool Complete = true;
 while (Complete)
    {
     // passo di fusione

     std::vector<std::string> CurrClasses; // classi presenti tra gli hb della copertura
     std::vector<int> FusionFlag;         // flag che indica se ci sono tentativi di fusione da fare in una data classe

     TCoverage::const_iterator it;
     for(it = CurrCoverage.begin(); it != CurrCoverage.end(); it++)
        {
         CurrClasses.push_back(it->first);
         if (it->first == HybClass) FusionFlag.push_back(0); // non fondo hb ibridi
         else FusionFlag.push_back(1);
        }

     bool StopFusion = false;
     while( !StopFusion )
        {
         int CC = CurrClasses.size();
         for(int c = 0; c < CC; c++)
            if( FusionFlag[c] && (CurrCoverage[ CurrClasses[c] ].size() > 1) )
               {
                std::list<TTempHyperBox>::iterator hbit1 = CurrCoverage[ CurrClasses[c] ].begin();
                std::list<TTempHyperBox>::iterator hbit2 = hbit1;
                hbit2++;

                bool Stop = false;
                while( !Stop )
                   {
                    if ( hbit2 == CurrCoverage[ CurrClasses[c] ].end() )
                       {
                        Stop = true;
                        FusionFlag[c] = 0;
                       }
                    else
                       {
                        bool FusionAdmissible = true;
                        // controllo se la fusione è ammissibile, ovvero se uno qualsiasi
                        // degli hb delle altre classi si sovrappone con l'ipotetica fusione
                        // degli hb puntati da hbit1 e hbit2

                        int k = 0;
                        while ( (k < CC) && FusionAdmissible )
                           {
                            if (k != c)
                               {
                                std::list<TTempHyperBox>::const_iterator hbit_oc = CurrCoverage[ CurrClasses[k] ].begin();
                                // iteratore hb altre classi

                                while ( (hbit_oc != CurrCoverage[ CurrClasses[k] ].end()) && FusionAdmissible )
                                   {
                                    if ( hbit_oc->OverlapsWithFusionOf(*hbit1, *hbit2) )
                                       FusionAdmissible = false;
                                    hbit_oc++;
                                   }
                               }

                            k++;
                           }

                        if ( FusionAdmissible )
                          {
                           Stop = true;

                           // fusione
                           TTempHyperBox FusionHB(TrainingSet, MembType);
                           hbit1->Fusion(*hbit2, FusionHB);

                           // assegno parametri membership a nuovo hb
                           for(int i = 0; i < MP; i++) FusionHB.SetMembPar(i, MembPars[i]);

                           // affilio eventuali pattern orfani al nuovo hb
                           std::list<int>::iterator p_it = FreePatts.begin();
                           while ( p_it != FreePatts.end() )
                              if ( FusionHB.Includes(*p_it) )
                                 {
                                  FusionHB.Affiliate(*p_it);
                                  p_it = FreePatts.erase(p_it);
                                 }
                              else p_it++;

                           // cancello hb fusi
                           CurrCoverage[ CurrClasses[c] ].erase(hbit1);
                           CurrCoverage[ CurrClasses[c] ].erase(hbit2);

                           // aggiungo nuovo hb a coverage
                           CurrCoverage[ CurrClasses[c] ].push_back(FusionHB);

                           CoverageCount++;
                          }
                        else
                          {
                           hbit2++;
                           if ( hbit2 == CurrCoverage[ CurrClasses[c] ].end() )
                              {
                               hbit1++;
                               hbit2 = hbit1;
                               hbit2++;
                              }
                          }
                       }
                   }
               }
            else FusionFlag[c] = 0;

         bool AllFlagsDown = true;
         for(int c = 0; c < CC; c++)
            if (FusionFlag[c]) AllFlagsDown = false;

         StopFusion = AllFlagsDown;
        }

     // generazione rete

     // PATCH 25-4-2009
     CurrCoverage.erase(HybClass);

     // ottengo vector hb puri
     std::vector<THyperBox> PureHB;
     for(it = CurrCoverage.begin(); it != CurrCoverage.end(); it++)
        PureHB.insert( PureHB.end(), it->second.begin(), it->second.end() );

     // se validation speed-up non è attivo, EtrList sarà la lista di tutti i pattern,
     // altrimenti sarà la lista dei pattern orfani
     std::list<int> EtrList;
     int IntNum = 0; // numero totale di pattern intrusi (per caso ValSpeed)
     if (!ValSpeed) for(int p = 0; p < AP; p++) EtrList.push_back(p);
     else
        {
         EtrList = FreePatts;

         // conteggio intrusi
         std::list<TTempHyperBox>::const_iterator hbit;
         for(it = CurrCoverage.begin(); it != CurrCoverage.end(); it++)
            for(hbit = it->second.begin(); hbit != it->second.end(); hbit++)
               IntNum += hbit->GetNumberOfIntruders();
        }

     // alloco rete
     TMinMaxNetwork NewNetwork("Network", PureHB, ProjectionMask, NormTool);
     std::vector<prlreal> NetworkData(2, 0);
     // primo valore: costo, secondo valore: complessità

     // calcolo Etr
     prlreal Etr = 0;
     std::list<int>::iterator etr_it;
         
     for(etr_it = TempIncList.begin(); etr_it != TempIncList.end(); etr_it++)
        Incongruent[*etr_it]= true; 
         
     TempIncList.clear();
         
     for(etr_it = EtrList.begin(); etr_it != EtrList.end(); etr_it++)
        if (!Incongruent[*etr_it])
           if ( NewNetwork.RawClassify( TrainingSet->GetDataPointer(*etr_it) ) != 
                TrainingSet->GetClass(*etr_it) ) Etr++;

     Etr += (IDN + IntNum);
     Etr /= (AP + IDN);

     // calcolo Comp
     prlreal Comp = NewNetwork.GetNumberOfHyperBoxes();
     Comp /= (AP + IDN);

     prlreal Cost = (1-Lambda)*Etr + Lambda*Comp;

     NetworkData[0] = Cost;
     NetworkData[1] = Comp;

     NewNetwork.SetSortData(NetworkData);

     NetworkList.push_back(NewNetwork);
     NetworkList.sort();

     if (RepEnabled)
        {
         rep << '\n' << CoverageCount << '\t' << Etr << '\t' << NewNetwork.GetNumberOfHyperBoxes()
             << '\t' << Cost;
        }

     // riduzione strutturale

     // trovo numero minimo di pattern in HB
     int MinPN = CurrCoverage.begin()->second.begin()->GetNumberOfPatterns();
     for(it = CurrCoverage.begin(); it != CurrCoverage.end(); it++)
        {
         std::list<TTempHyperBox>::const_iterator hbit = it->second.begin();
         while ( hbit != it->second.end() )
            {
             int CurrNP = hbit->GetNumberOfPatterns();
             if ( CurrNP < MinPN ) MinPN = CurrNP;
             hbit++;
            }
        }

     TCoverage::iterator er_it;
     bool Found;
     switch (HBDelMode)
        {
         case hdSingle:

            Found = false;
            er_it = CurrCoverage.begin();
            while( (er_it != CurrCoverage.end()) && (!Found) )
               {
                std::list<TTempHyperBox>::iterator hbit = er_it->second.begin();
                while ( (hbit != er_it->second.end()) && (!Found) )
                   if ( hbit->GetNumberOfPatterns() == MinPN ) Found = true;
                   else hbit++;

                if (Found)
                   {
                    hbit->GetPatternIndexes(FreePatts);
                    hbit = er_it->second.erase(hbit);
                   }
                else er_it++;
               }

         break;
         case hdBlock:

            for(er_it = CurrCoverage.begin(); er_it != CurrCoverage.end(); er_it++)
               {
                std::list<TTempHyperBox>::iterator hbit = er_it->second.begin();
                while ( hbit != er_it->second.end() )
                   {
                    int CurrNP = hbit->GetNumberOfPatterns();
                    if ( CurrNP == MinPN )
                       {
                        hbit->GetPatternIndexes(FreePatts);
                        hbit = er_it->second.erase(hbit);
                       }
                    else hbit++;
                   }
               }

         break;
        }

     CoverageCount++;

     // controllo se la copertura corrente è ancora completa
     for(it = CurrCoverage.begin(); it != CurrCoverage.end(); it++)
        if ( ( it->first != HybClass ) && ( it->second.empty() ) )
           Complete = false;
    }

 //EndTime = Time();
 //ElapsedTime = EndTime - StartTime;
 if (RepEnabled)
    {
     //DecodeTime(ElapsedTime, Hour, Min, Sec, MSec);
     //std::string TimeStr = "Elapsed time: " + IntToStr( Hour ) + "h " + IntToStr( Min ) + "m "
       //                   + IntToStr( Sec ) + "s " + IntToStr( MSec ) + "ms";

    // rep << "\n\n" << TimeStr.c_str();
    }

 if (ReturnAll) ReturnList = NetworkList;
 else
    {
     ReturnList.clear();
     if ( !NetworkList.empty() )
        ReturnList.push_back( NetworkList.front() );
    }
 }
}

//---------------------------------------------------------------------------
