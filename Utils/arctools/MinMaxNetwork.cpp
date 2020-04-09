//---------------------------------------------------------------------------
#include "MinMaxNetwork.h"

// HYPERBOX

THyperBox :: THyperBox(int N, TMembershipType MT)
{
 V.assign(N, 0);
 W.assign(N, 0);
 MembType = MT;

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

 Class = HybClass;
}

THyperBox :: THyperBox(const THyperBox& Source)
{
 V = Source.V;
 W = Source.W;
 Class = Source.Class;

 MembType = Source.MembType;
 MembPars = Source.MembPars;
}

THyperBox& THyperBox :: operator=(const THyperBox& Source)
{
 if (this != &Source)
    {
     V = Source.V;
     W = Source.W;
     Class = Source.Class;

     MembType = Source.MembType;
     MembPars = Source.MembPars;
    }

 return *this;
}

bool THyperBox :: operator==(const THyperBox& Other) const
{
 return (
          (V == Other.V) &&
          (W == Other.W) &&
          (Class == Other.Class) &&
          (MembType == Other.MembType) &&
          (MembPars == Other.MembPars)
        );
}

bool THyperBox :: operator<(const THyperBox& Other) const
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
             else return (MembPars < Other.MembPars);
            }
        }
    }
}

prlreal THyperBox :: Membership(const prlreal* X) const
{
 int N = V.size();
 prlreal Memb;

 switch (MembType)
    {
     case mtSimpson:

        Memb = 0;
        for(int i = 0; i < N; i++)
           Memb += prl_max( 0, 1-prl_max(0, MembPars[0]*prl_min(1, X[i] - W[i]) ) ) +
                   prl_max( 0, 1-prl_max(0, MembPars[0]*prl_min(1, V[i] - X[i]) ) );
        Memb /= (2*N);

        // correzione
        Memb -= 0.5;
        Memb *= 2;

     break;
     case mtTrapezoidal:

        Memb = 1;
        for(int i = 0; i < N; i++)
           Memb = prl_min( Memb, prl_min( prl_max( 0, 1-prl_max(0, MembPars[0]*prl_min(1, X[i] - W[i]) ) ),
                                          prl_max( 0, 1-prl_max(0, MembPars[0]*prl_min(1, V[i] - X[i]) ) ) ) );

     break;
     default: throw std::runtime_error("Unknown membership function");
    }

 return Memb;
}

void THyperBox :: WriteToFile(std::ofstream& Fp) const
{
 if ( !Fp.good() ) throw std::runtime_error("Invalid file stream");

 int N = GetInputSize();

 Fp << "HyperBox_v1.0\t" << N << std::endl;
 for(int i = 0; i < N; i++)
    if (i < N-1) Fp << V[i] << '\t';
    else Fp << V[i] << std::endl;
 for(int i = 0; i < N; i++)
    if (i < N-1) Fp << W[i] << '\t';
    else Fp << W[i] << std::endl;

 switch (MembType)
    {
     case mtSimpson: Fp << "Simpson" << std::endl;
     break;
     case mtTrapezoidal: Fp << "Trapezoidal" << std::endl;
     break;
     default: throw std::runtime_error("Unknown membership function");
    }

 int M = MembPars.size();

 for(int j = 0; j < M; j++)
    if (j < M-1) Fp << MembPars[j] << '\t';
    else Fp << MembPars[j] << std::endl;

 Fp << Class.c_str() << std::endl;

 if ( !Fp.good() ) throw std::runtime_error("Invalid file stream");
}

void THyperBox :: ReadFromFile(std::ifstream& Fp)
{
 if ( !Fp.good() ) throw std::runtime_error("Invalid file stream");

 // resetto informazioni
 V.clear();
 W.clear();
 MembPars.clear();

 char buf[256];
 Fp.width(256);
 Fp >> buf;

 std::string Id(buf);
 if (Id != "HyperBox_v1.0") throw std::runtime_error("Invalid file format");
 if ( !Fp.good() ) throw std::runtime_error("Invalid file stream");

 int N;
 Fp >> N;
 if ( !Fp.good() ) throw std::runtime_error("Invalid file stream");

 V.assign(N, 0);
 W.assign(N, 0);
 for(int i = 0; i < N; i++) Fp >> V[i];
 for(int i = 0; i < N; i++) Fp >> W[i];
 if ( !Fp.good() ) throw std::runtime_error("Invalid file stream");

 Fp.width(256);
 Fp >> buf;

 std::string MembId(buf);
 if (MembId == "Simpson")
    {
     MembType = mtSimpson;
     prlreal Gamma;
     Fp >> Gamma;
     MembPars.assign(1, Gamma);
    }
 else
    {
     if (MembId == "Trapezoidal")
        {
         MembType = mtTrapezoidal;
         prlreal Gamma;
         Fp >> Gamma;
         MembPars.assign(1, Gamma);
        }
     else throw std::runtime_error("Invalid file format");
    }

 if ( !Fp.good() ) throw std::runtime_error("Invalid file stream");

 Fp.width(256);
 Fp >> buf;

 std::string ClassId(buf);
 Class = ClassId;

 if ( !Fp.good() ) throw std::runtime_error("Invalid file stream");
}

// NETWORK

TMinMaxNetwork :: TMinMaxNetwork(std::string InitName,
                                 std::vector<THyperBox>& InitHBs,
                                 std::vector<int>& ProjMask,
                                 TVectorNormalizationTool& InitNT)
{
 Name = InitName;

 // controlli

 int NHB = InitHBs.size();
 // numero di HyperBox passati

 int HBISz = NHB ? InitHBs[0].GetInputSize() : -1;
 // Input Size primo HyperBox, se presente, se no -1

 int MSz = ProjMask.size();
 // dimensione Projection Mask

 int MSISz = -1;
 if (MSz)
    {
     MSISz = 0;
     for(int i = 0; i < MSz; i++)
        if (ProjMask[i]) MSISz++;
    }
 // MSISz contiene numero di feature selezionate dalla maschera, se questa è
 // non vuota, se no -1

 int NTISz = -1;
 if ( InitNT.Initialized() )
    NTISz = InitNT.GetTotalNumberOfComponents();
 // NTISz contiene Input Size del Normalization Tool se questo è inizializzato,
 // se no -1

 if (NHB)
    {
     // controllo che gli Hyperbox passati abbiano tutti lo stesso Input Size
     for(int i = 1; i < NHB; i++)
        if ( InitHBs[i].GetInputSize() != HBISz )
           throw std::runtime_error("Init HBs do not have the same input size");

     // controllo compatibilità tra maschera e hyperbox
     if (MSz)
        if (MSISz != HBISz)
           throw std::runtime_error("Init HBs and Projection Mask selection do not have the same input size");

     // controllo compatibilità tra normalization tool e maschera
     if ( MSz && InitNT.Initialized() )
        if (MSISz != NTISz)
           throw std::runtime_error("Normalization Tool and Projection Mask selection do not have the same input size");
    }
 else
    {
     // controllo compatibilità tra normalization tool e maschera
     if ( MSz && InitNT.Initialized() )
        if (MSISz != NTISz)
           throw std::runtime_error("Normalization Tool and Projection Mask selection do not have the same input size");
    }

 HyperBoxes = InitHBs;

 ProjectionMask = ProjMask;
 NormTool = InitNT;

 RepEnabled = false;
 RName = "MinMaxNetworkClassificationReport.txt";
}

TMinMaxNetwork :: TMinMaxNetwork(std::string InitName,
                                 std::vector<THyperBox>& InitHBs)
{
 Name = InitName;

 // controlli

 int NHB = InitHBs.size();
 // numero di HyperBox passati

 int HBISz = NHB ? InitHBs[0].GetInputSize() : -1;
 // Input Size primo HyperBox, se presente, se no -1

 if (NHB)
    {
     // controllo che gli Hyperbox passati abbiano tutti lo stesso Input Size
     for(int i = 1; i < NHB; i++)
        if ( InitHBs[i].GetInputSize() != HBISz )
           throw std::runtime_error("Init HBs do not have the same input size");
    }

 RepEnabled = false;
 RName = "MinMaxNetworkClassificationReport.txt";

 HyperBoxes = InitHBs;
}

TMinMaxNetwork :: TMinMaxNetwork(const TMinMaxNetwork& Source)
{
 Name = Source.Name;
 HyperBoxes = Source.HyperBoxes;

 ProjectionMask = Source.ProjectionMask;
 NormTool = Source.NormTool;

 SortData = Source.SortData;

 RepEnabled = Source.RepEnabled;
 RName = Source.RName;
}

TMinMaxNetwork& TMinMaxNetwork :: operator=(const TMinMaxNetwork& Source)
{
 if (this != &Source)
    {
     Name = Source.Name;
     HyperBoxes = Source.HyperBoxes;

     ProjectionMask = Source.ProjectionMask;
     NormTool = Source.NormTool;

     SortData = Source.SortData;

     RepEnabled = Source.RepEnabled;
     RName = Source.RName;
    }

 return *this;
}

bool TMinMaxNetwork :: operator<(const TMinMaxNetwork& Other) const
{
 if (SortData != Other.SortData) return (SortData < Other.SortData);
 else return (Name < Other.Name);
}

int TMinMaxNetwork :: GetCoreInputSize() const
{
 if ( !HyperBoxes.size() ) throw std::runtime_error("No Hyperboxes in MinMax Network");
 return HyperBoxes[0].GetInputSize();
}

int TMinMaxNetwork :: GetInterfaceInputSize() const
{
 if ( ProjectionMask.size() ) return ProjectionMask.size();
 else return GetCoreInputSize();
}

std::vector<std::string> TMinMaxNetwork :: Classify(const TVectorCollection* TestSet) const
{
 if ( !HyperBoxes.size() ) throw std::runtime_error("No Hyperboxes in MinMax Network");

 const TVectorCollection* PreprocTestSet;

 // preprocessing
 if ( ProjectionMask.size() || NormTool.Initialized() )
    {
     TVectorCollection* TempPointer = new TVectorCollection(*TestSet);

     // proiezione
     if ( ProjectionMask.size() )
        {
         if ( TempPointer->GetTotalNumberOfComponents() != GetInterfaceInputSize() )
            throw std::runtime_error("Input Size mismatch in MinMax Network Classify method");

         TempPointer->Projection( &ProjectionMask );
        }

     // normalizzazione
     if ( NormTool.Initialized() )
        {
         if ( TempPointer->GetTotalNumberOfComponents() != GetCoreInputSize() )
            throw std::runtime_error("Input Size mismatch in MinMax Network Classify method");

         NormTool.NormalizeCollection( TempPointer );
        }

     PreprocTestSet = TempPointer;
    }
 else PreprocTestSet = TestSet;

 if ( PreprocTestSet->GetTotalNumberOfComponents() != GetCoreInputSize() )
    throw std::runtime_error("Input Size mismatch in MinMax Network Classify method");

 int P = PreprocTestSet->GetNumberOfPatterns();
 std::vector<std::string> AssignedClasses(P, "NOT_ASSIGNED_YET");

 if (RepEnabled)
    {
     int CN = PreprocTestSet->GetNumberOfClasses();
     std::vector<std::string> ClassName;
     std::map<std::string, prlreal> ClassScore;
     for(int c = 0; c < CN; c++) ClassName.push_back( PreprocTestSet->GetClassName(c) );

     std::ofstream rep( RName.c_str() );
     if (!rep) throw std::runtime_error("Errore apertura file!");
     //rep.setf(ios_base::fixed, ios_base::floatfield);
     //rep.precision(6);

     rep << "MinMax Network Classification Report";
     rep << "\n\nTest Set: " << PreprocTestSet->GetCollectionName().c_str();
     rep << "\nNumber of patterns: " << PreprocTestSet->GetNumberOfPatterns();
     rep << "\n\nCode";
     for(int c = 0; c < CN; c++) rep << '\t' << ClassName[c].c_str();
     rep << "\tTarget\tOutput\tCR\tComment\n";

     prlreal ACCR = 0;
     prlreal AECR = 0;
     int Err = 0;
     int Ok = 0;
     int Undet = 0;

     for(int p = 0; p < P; p++)
        {
         AssignedClasses[p] = RawClassify( PreprocTestSet->GetDataPointer(p), ClassScore );
         rep << PreprocTestSet->GetCode(p).c_str();
         for(int c = 0; c < CN; c++) rep << '\t' << ClassScore[ ClassName[c] ];
         rep << '\t' << PreprocTestSet->GetClass(p).c_str();
         rep << '\t' << AssignedClasses[p].c_str();

         if (AssignedClasses[p] == "UNCLASSIFIABLE")
            {
             rep << "\tN.D.\tUNDETERMINED\n";
             Undet++;
            }
         else
            {
             // determino affidiabilità
             std::set<prlreal> Scores;
             std::map<std::string, prlreal>::iterator it_map;

             for( it_map = ClassScore.begin(); it_map != ClassScore.end(); it_map++ )
                Scores.insert(it_map->second);

             std::vector<prlreal> ScoreVector;
             std::set<prlreal>::iterator it_set;
             for( it_set = Scores.begin(); it_set != Scores.end(); it_set++ )
                ScoreVector.push_back(*it_set);

             prlreal CR = -1;
             int SVS = ScoreVector.size();
             if (SVS > 1) CR = 1 - ( ScoreVector[SVS-2] / ScoreVector[SVS-1] );
             else throw std::runtime_error("Trying to compute CR for an unclassifiable pattern");

             if ( AssignedClasses[p] != PreprocTestSet->GetClass(p) )
                {
                 rep << '\t' << CR << "\tERROR\n";
                 AECR += CR;
                 Err++;
                }
             else
                {
                 rep << '\t' << CR << "\tOK\n";
                 ACCR += CR;
                 Ok++;
                }
            }
        }

     if (Err) AECR /= Err;
     if (Ok) ACCR /= Ok;

     rep << "\nErrors: " << Err << "/" << P;
     rep << "\nUndetermined: " << Undet << "/" << P;
     rep << "\n\n(E + U) = " << 100 * ( (Err + Undet) / prlreal(P) ) << " %";
     if (Ok) rep << "\nACCR = " << ACCR;
     if (Err) rep << "\nAECR = " << AECR;
    }
 else
    {
     for(int p = 0; p < P; p++)
        AssignedClasses[p] = RawClassify( PreprocTestSet->GetDataPointer(p) );
    }

 if ( ProjectionMask.size() || NormTool.Initialized() ) delete PreprocTestSet;

 return AssignedClasses;
}

std::string TMinMaxNetwork :: RawClassify(const prlreal* X) const
{
 std::string AssignedClass;

 int H = HyperBoxes.size();
 std::map<std::string, prlreal> ClassScore;
 for(int i = 0; i < H; i++)
    {
     std::string CurrHBClass = HyperBoxes[i].GetClass();
     prlreal CurrHBMemb = HyperBoxes[i].Membership(X);
     if ( ClassScore.find( CurrHBClass ) != ClassScore.end() )
        ClassScore[ CurrHBClass ] = prl_max( ClassScore[ CurrHBClass ], CurrHBMemb );
     else ClassScore[ CurrHBClass ] = CurrHBMemb;
    }

 prlreal MaxScore = ClassScore.begin()->second;
 std::string MaxClass = ClassScore.begin()->first;
 std::map<std::string, prlreal>::iterator it_map;
 for( it_map = ClassScore.begin(); it_map != ClassScore.end(); it_map++ )
 if ( it_map->second > MaxScore )
    {
     MaxScore = it_map->second;
     MaxClass = it_map->first;
    }

 bool ExAequo = false;
 for( it_map = ClassScore.begin(); it_map != ClassScore.end(); it_map++ )
    if ( (it_map->first != MaxClass ) && (it_map->second == MaxScore) ) ExAequo = true;

 if (ExAequo) AssignedClass = "UNCLASSIFIABLE";
 else AssignedClass = MaxClass;

 return AssignedClass;
}

std::string TMinMaxNetwork :: RawClassify(const prlreal* X, std::map<std::string, prlreal>& ExtraData) const
{
 std::string AssignedClass;

 int H = HyperBoxes.size();
 std::map<std::string, prlreal> ClassScore;
 for(int i = 0; i < H; i++)
    {
     std::string CurrHBClass = HyperBoxes[i].GetClass();
     prlreal CurrHBMemb = HyperBoxes[i].Membership(X);
     if ( ClassScore.find( CurrHBClass ) != ClassScore.end() )
        ClassScore[ CurrHBClass ] = prl_max( ClassScore[ CurrHBClass ], CurrHBMemb );
     else ClassScore[ CurrHBClass ] = CurrHBMemb;
    }

 prlreal MaxScore = ClassScore.begin()->second;
 std::string MaxClass = ClassScore.begin()->first;
 std::map<std::string, prlreal>::iterator it_map;
 for( it_map = ClassScore.begin(); it_map != ClassScore.end(); it_map++ )
 if ( it_map->second > MaxScore )
    {
     MaxScore = it_map->second;
     MaxClass = it_map->first;
    }

 bool ExAequo = false;
 for( it_map = ClassScore.begin(); it_map != ClassScore.end(); it_map++ )
    if ( (it_map->first != MaxClass ) && (it_map->second == MaxScore) ) ExAequo = true;

 if (ExAequo) AssignedClass = "UNCLASSIFIABLE";
 else AssignedClass = MaxClass;

 ExtraData = ClassScore;
 return AssignedClass;
}

void TMinMaxNetwork :: WriteToFile(std::string NomeFile) const
{
 std::ofstream f_out(NomeFile.c_str());
 if (!f_out) throw std::runtime_error("File opening error");
 f_out.setf(std::ios_base::fixed, std::ios_base::floatfield);
 f_out.precision(6);

 f_out << "MinMaxNetwork_v1.0" << std::endl;
 f_out << Name.c_str() << std::endl;
 f_out << GetInterfaceInputSize() << std::endl;
 f_out << GetCoreInputSize() << std::endl << std::endl;

 int MS = ProjectionMask.size();
 f_out << "InputSizeReduction";
 if (MS) f_out << " ENABLED" << std::endl;
 else f_out << " DISABLED" << std::endl;
 if (MS)
    {
     f_out << MS << std::endl;
     f_out << '<';
     for(int i = 0; i < MS; i++)
        if (ProjectionMask[i]) f_out << 1;
        else f_out << 0;
     f_out << '>' << std::endl;
    }

 f_out << std::endl << "Normalization";
 if ( NormTool.Initialized() ) f_out << " ENABLED" << std::endl;
 else f_out << " DISABLED" << std::endl;
 if ( NormTool.Initialized() ) NormTool.WriteFile(f_out);

 int H = HyperBoxes.size();
 f_out << std::endl << "HyperBoxes" << std::endl;
 f_out << H << std::endl << std::endl;
 for(int j = 0; j < H; j++)
    {
     HyperBoxes[j].WriteToFile(f_out);
     if (j < H-1) f_out << std::endl;
    }
}

void TMinMaxNetwork :: LoadFromFile(std::string NomeFile)
{
 std::ifstream f_in( NomeFile.c_str() );
 if (!f_in) throw std::runtime_error("File opening error");

 char buf[256];
 std::string ASBuf;
 // buffers

 f_in.width(256);
 f_in >> buf;
 ASBuf = std::string(buf);
 if ( !f_in.good() ) throw std::runtime_error("Invalid file stream");
 if ( ASBuf != "MinMaxNetwork_v1.0" ) throw std::runtime_error("Invalid file format");

 f_in.width(256);
 f_in >> buf;
 ASBuf = std::string(buf);
 if ( !f_in.good() ) throw std::runtime_error("Invalid file stream");
 Name = ASBuf; // imposto Name

 int IFSz, CSz;
 f_in >> IFSz;
 f_in >> CSz;
 if ( !f_in.good() ) throw std::runtime_error("Invalid file stream");
 if ( (IFSz < 1) || (CSz < 1) || (CSz > IFSz) )
    throw std::runtime_error("Invalid file format");

 f_in.width(256);
 f_in >> buf;
 ASBuf = std::string(buf);
 if ( !f_in.good() ) throw std::runtime_error("Invalid file stream");
 if ( ASBuf != "InputSizeReduction" ) throw std::runtime_error("Invalid file format");

 f_in.width(256);
 f_in >> buf;
 ASBuf = std::string(buf);
 if ( !f_in.good() ) throw std::runtime_error("Invalid file stream");
 if ( ASBuf == "ENABLED" )
    {
     int MS;
     f_in >> MS;
     if ( !f_in.good() ) throw std::runtime_error("Invalid file stream");

     if (MS != IFSz) throw std::runtime_error("Invalid file format");

     std::vector<int> ReadPM(MS, 0);
     // vettore per leggere ProjectionMask

     char SCh; // buffer per lettura singolo carattere
     f_in >> SCh;
     while (SCh != '<') f_in >> SCh;

     int OneCount = 0;
     for(int i = 0; i < MS; i++)
        {
         f_in >> SCh;
         if (SCh == '1')
            {
             ReadPM[i] = 1;
             OneCount++;
            }
         else
            {
             if (SCh == '0') ReadPM[i] = 0;
             else throw std::runtime_error("Invalid file format");
            }
        }
     f_in >> SCh;
     if (SCh != '>') throw std::runtime_error("Invalid file format");

     if ( !f_in.good() ) throw std::runtime_error("Invalid file stream");

     if (OneCount != CSz) throw std::runtime_error("Invalid file format");

     ProjectionMask = ReadPM; // imposto ProjectionMask
    }
 else
    {
     if ( ASBuf == "DISABLED" )
        {
         ProjectionMask.clear(); // imposto ProjectionMask
        }
     else throw std::runtime_error("Invalid file format");
    }

 f_in.width(256);
 f_in >> buf;
 ASBuf = std::string(buf);
 if ( !f_in.good() ) throw std::runtime_error("Invalid file stream");
 if ( ASBuf != "Normalization" ) throw std::runtime_error("Invalid file format");

 f_in.width(256);
 f_in >> buf;
 ASBuf = std::string(buf);
 if ( !f_in.good() ) throw std::runtime_error("Invalid file stream");
 if ( ASBuf == "ENABLED" )
    {
     NormTool.LoadDataFromFile(f_in);
     // imposto NormTool
    }
 else
    {
     if ( ASBuf == "DISABLED" )
        {
         TVectorNormalizationTool Dummy; // non inizializzato
         NormTool = Dummy;
         // imposto NormTool
        }
     else throw std::runtime_error("Invalid file format");
    }

 f_in.width(256);
 f_in >> buf;
 ASBuf = std::string(buf);
 if ( !f_in.good() ) throw std::runtime_error("Invalid file stream");
 if ( ASBuf != "HyperBoxes" ) throw std::runtime_error("Invalid file format");

 HyperBoxes.clear();
 // cancello eventuali HyperBox

 int HN;
 f_in >> HN;
 if ( !f_in.good() ) throw std::runtime_error("Invalid file stream");
 if (HN < 0) throw std::runtime_error("Invalid file format");
 // consento anche il caricamento di reti prive di HyperBox

 for(int j = 0; j < HN; j++)
    {
     THyperBox ReadHB(CSz, mtSimpson); // solo per inizializzare, poi verrà letta
     ReadHB.ReadFromFile(f_in);

     if (ReadHB.GetInputSize() != CSz) throw std::runtime_error("Invalid file format");

     HyperBoxes.push_back(ReadHB);
    }

 // cancello SortData
 SortData.clear();
}

//---------------------------------------------------------------------------
