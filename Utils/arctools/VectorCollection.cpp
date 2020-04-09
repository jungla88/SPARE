//---------------------------------------------------------------------------
#include "VectorCollection.h"

TVectorCollection :: TVectorCollection(const int S, const std::string* Names,
                                       const TSectionType* Types, const int* Sizes)
{
 Name = "VectorCollection";

 for(int i = 0; i < S; i++)
    {
     TSectionDescriptor NewSection;
     NewSection.Name = BlanksToUnderscores( Names[i] );
     NewSection.Type = Types[i];
     NewSection.Size = Sizes[i];
     Structure.push_back( NewSection );
    }
}

TVectorCollection :: TVectorCollection(const TVectorCollection& Source)
{
 Name = Source.Name;
 Structure = Source.Structure;
 Codes = Source.Codes;
 Classes = Source.Classes;
 Patterns = Source.Patterns;
}

TVectorCollection& TVectorCollection :: operator=(const TVectorCollection& Source)
{
 if (this != &Source)
    {
     Name = Source.Name;
     Structure = Source.Structure;
     Codes = Source.Codes;
     Classes = Source.Classes;
     Patterns = Source.Patterns;
    }

 return *this;
}

TVectorCollection :: ~TVectorCollection()
{
 Patterns.clear();
 Codes.clear();
 Classes.clear();
}

TVectorCollection :: TVectorCollection(const TVectorCollection* Source)
{
 Name = "VectorCollection";
 Structure = Source->Structure;
}

TVectorCollection :: TVectorCollection(const TVectorCollection* Source, const std::vector<int>* Mask)
{
 Name = Source->Name + "_reduced";
 int P = Source->GetNumberOfPatterns();
 int N = Source->GetTotalNumberOfComponents();
 int M = Mask->size();
 int Scan = (M < N) ? M : N;
 int NewDim = 0;
 for(int j = 0; j < Scan; j++) if ( (*Mask)[j] ) NewDim++;

 TSectionDescriptor NewSection;
 NewSection.Name = "Sect";
 NewSection.Type = stReal;
 NewSection.Size = NewDim;
 Structure.push_back( NewSection );

 Initialize(P);
 for(int i = 0; i < P; i++)
    {
     int c = 0;
     for(int j = 0; j < Scan; j++)
        {
         if ( (*Mask)[j] )
            {
             Patterns[i].Data[c] = Source->Patterns[i].Data[j];
             c++;
            }
        }
     Codes[i] = Source->Codes[i];
     Classes[i] = Source->Classes[i];
    }
}

TVectorCollection :: TVectorCollection(const TVectorCollection* Source, int Sec,
                                       const std::vector<int>* Mask)
{
 Name = Source->Name;
 int P = Source->GetNumberOfPatterns();
 int Tot = Source->GetTotalNumberOfComponents();
 int S = Source->GetNumberOfSections();
 if (Sec >= S) throw std::runtime_error("Construction Error");
 if (Sec < 0) throw std::runtime_error("Construction Error");
 int N = Source->GetSectionSize(Sec);
 int M = Mask->size();
 int Scan = (M < N) ? M : N;
 int NewDim = 0;
 for(int j = 0; j < Scan; j++) if ( (*Mask)[j] ) NewDim++;

 Structure = Source->Structure;
 Structure[Sec].Size = NewDim;

 Initialize(P);

 // sezioni antecedenti quella da ridurre
 int C = 0;
 for(int s = 0; s < Sec; s++) C += Source->GetSectionSize(s);
 for(int p = 0; p < P; p++)
    for(int c = 0; c < C; c++)
       Patterns[p].Data[c] = Source->Patterns[p].Data[c];

 // sezione da ridurre
 int Offset = Source->GetSectionStart(Sec);
 if (GetSectionStart(Sec) != Offset) throw std::runtime_error("Construction Error");
 for(int p = 0; p < P; p++)
    {
     int c = 0;
     for(int j = 0; j < Scan; j++)
        {
         if ( (*Mask)[j] )
            {
             Patterns[p].Data[Offset+c] = Source->Patterns[p].Data[Offset+j];
             c++;
            }
        }
    }

 // sezioni successive a quella da ridurre
 if (Sec < S-1)
    {
     int SourceOs = Source->GetSectionStart(Sec+1);
     int Os = GetSectionStart(Sec+1);
     int Diff = SourceOs - Os;
     if ( Diff != (Source->GetSectionSize(Sec)-GetSectionSize(Sec)) )
        throw std::runtime_error("Construction Error");

     for(int p = 0; p < P; p++)
        for(int c = SourceOs; c < Tot; c++)
           Patterns[p].Data[c-Diff] = Source->Patterns[p].Data[c];
    }

 Codes = Source->Codes;
 Classes = Source->Classes;
}

void TVectorCollection :: AddPattern(std::string Code, std::string Class)
{
 TSectionedArray NewPattern( Structure );
 Patterns.push_back( NewPattern );
 Codes.push_back( BlanksToUnderscores(Code) );
 Classes.push_back( BlanksToUnderscores(Class) );
}

int TVectorCollection :: GetTotalNumberOfComponents() const
{
 int S = Structure.size();
 int Comps = 0;
 for(int i = 0; i < S; i++) Comps += Structure[i].Size;
 return Comps;
}

prlreal* TVectorCollection :: GetMeanArray(TCrispPartition* Partition, int Id) const
{
 int C = GetTotalNumberOfComponents();
 int P = Patterns.size();

 if ( Partition->GetNumberOfPatterns() != P )
    throw std::runtime_error("Partizione non valida!");

 prlreal* Mean = new prlreal[C];
 for(int c = 0; c < C; c++) Mean[c] = 0;

 int ClCard = Partition->GetClassCardinality(Id);
 for(int i = 0; i < ClCard; i++)
    for(int c = 0; c < C; c++) Mean[c] += Patterns[Partition->GetPattern(Id, i)].Data[c];

 for(int c = 0; c < C; c++) Mean[c] /= ClCard;

 return Mean;
}

void TVectorCollection :: WriteFile(std::string NomeFile) const
{
 std::ofstream f_out(NomeFile.c_str());
 if (!f_out) throw std::runtime_error("Errore apertura file!");
 f_out.setf(std::ios_base::fixed, std::ios_base::floatfield);
 f_out.precision(6);
 int P = Patterns.size();
 int S = Structure.size();

 int Comps = GetTotalNumberOfComponents();

 f_out << "Collezione testo versione 4.0" << std::endl << std::endl;
 f_out << Name.c_str() << std::endl;
 f_out << P << std::endl << std::endl;
 f_out << S << std::endl;

 for(int s = 0; s < S; s++)
    {
     f_out << Structure[s].Name.c_str() << '\t';
     f_out << Structure[s].Size << '\t';

     std::string temp;
     switch (Structure[s].Type)
        {
         case stSwitch: temp = "SWITCH";
         break;
         case stInteger: temp = "INTEGER";
         break;
         case stReal: temp = "REAL";
         break;
         case stComplex: temp = "COMPLEX";
         break;
         case stControlledCircular: temp = "CONTROLLED_CIRCULAR";
        }

     f_out << temp.c_str() << std::endl;
    }

 f_out << std::endl;

 for(int p = 0; p < P; p++)
    {
     f_out << Codes[p].c_str() << '\t';
     for(int c = 0; c < Comps; c++) f_out << Patterns[p].Data[c] << '\t';
     f_out << Classes[p].c_str() << std::endl;
    }
}

int TVectorCollection :: GetSectionStart(int Index) const
{
 int Offset = 0;
 for(int i = 0; i < Index; i++) Offset += Structure[i].Size;
 return Offset;
}

void TVectorCollection :: LoadDataFromFile(const std::string& NomeFile)
{
 std::ifstream f_in( NomeFile.c_str() );
 if (!f_in) throw std::runtime_error("Errore apertura file!");

 char buf[256];
 f_in.getline(buf, 256, '\n');
 std::string FileId(buf);
 if (FileId != "Collezione testo versione 4.0") throw std::runtime_error("Formato file scorretto!");

 f_in.width(256);
 f_in >> buf;
 if ( !f_in.good() ) throw std::runtime_error("Formato file scorretto!");
 std::string NewName(buf);

 int Npatt;
 f_in >> Npatt;
 if ( !f_in.good() ) throw std::runtime_error("Formato file scorretto!");

 int Nsez;
 f_in >> Nsez;
 if ( !f_in.good() ) throw std::runtime_error("Formato file scorretto!");

 TSectioning FileSectioning;
 for(int s = 0; s < Nsez; s++)
    {
     f_in.width(256);
     f_in >> buf;
     std::string Name(buf);

     int Size;
     f_in >> Size;

     f_in.width(256);
     f_in >> buf;
     std::string sType(buf);
     if ( !f_in.good() ) throw std::runtime_error("Formato file scorretto!");

     if ( (sType != "REAL") && (sType != "COMPLEX") &&
          (sType != "INTEGER") && (sType != "SWITCH")  && (sType != "CONTROLLED_CIRCULAR") )
        throw std::runtime_error("Formato file scorretto!");

     TSectionType Type;
     if ( sType == "REAL" ) Type = stReal;
     else
        {
         if ( sType == "COMPLEX" ) Type = stComplex;
         else
            {
             if ( sType == "INTEGER" ) Type = stInteger;
             else
                {
                 if ( sType == "SWITCH" ) Type = stSwitch;
                 else Type = stControlledCircular;
                }
            }
        }

     TSectionDescriptor* CurrSection = new TSectionDescriptor;
     CurrSection->Name = Name;
     CurrSection->Type = Type;
     CurrSection->Size = Size;

     FileSectioning.push_back( *CurrSection );
     delete CurrSection;
    }

 // carico nuovi dati cancellando quelli vecchi
 ClearPatterns();
 Name = NewName;
 Structure = FileSectioning;

 for(int p = 0; p < Npatt; p++)
    {
     int Nc = GetTotalNumberOfComponents();

     f_in.width(256);
     f_in >> buf;
     std::string NewCode(buf);
     if ( !f_in.good() ) throw std::runtime_error("Formato file scorretto!");

     TSectionedArray* NewPatt = new TSectionedArray(Structure);
     for(int c = 0; c < Nc; c++) f_in >> NewPatt->Data[c];

     if ( !f_in.good() )
        {
         delete NewPatt;
         throw std::runtime_error("Formato file scorretto!");
        }

     f_in.width(256);
     f_in >> buf;
     std::string NewClass(buf);
     if ( !f_in.good() && !f_in.eof() )
     {
         delete NewPatt;
         throw std::runtime_error("Formato file scorretto!");
     }

     Patterns.push_back( *NewPatt );
     Codes.push_back( NewCode );
     Classes.push_back( NewClass );

     delete NewPatt;
    }
}

void TVectorCollection :: Initialize(int N)
{
 TSectionedArray ZeroPattern( Structure );
 Patterns.assign( N, ZeroPattern );
 Codes.assign( N, "Pattern" );
 Classes.assign( N, "NoClass" );
}

void TVectorCollection :: MergeWith(const TVectorCollection* OtherCollection)
{
 if (Structure != OtherCollection->Structure)
    throw std::runtime_error("Trying to merge differently sectioned vector collections!");

 std::vector<std::string>::iterator A = Codes.end();
 std::vector<std::string>::const_iterator B = OtherCollection->Codes.begin();
 std::vector<std::string>::const_iterator C = OtherCollection->Codes.end();
 Codes.insert(A, B, C);

 A = Classes.end();
 B = OtherCollection->Classes.begin();
 C = OtherCollection->Classes.end();
 Classes.insert(A, B, C);

 std::vector<TSectionedArray>::iterator D = Patterns.end();
 std::vector<TSectionedArray>::const_iterator E = OtherCollection->Patterns.begin();
 std::vector<TSectionedArray>::const_iterator F = OtherCollection->Patterns.end();
 Patterns.insert(D, E, F);

 // controlli
 bool Ok = true;
 if (Codes.size() != Classes.size()) Ok = false;
 if (Codes.size() != Patterns.size()) Ok = false;

 if (!Ok) throw std::runtime_error("Something weird happened merging vector collections!");
}

void TVectorCollection :: CopyAndPastePatternTo(int P, TVectorCollection* OtherCollection)
{
 if (Structure != OtherCollection->Structure)
    throw std::runtime_error("Trying to copy pattern to differently sectioned vector collection!");

 if (P < 0) throw std::runtime_error("Index out of bound!");
 if (unsigned(P) >= Patterns.size())
    throw std::runtime_error("Index out of bound!");

 OtherCollection->Patterns.push_back(Patterns[P]);
 OtherCollection->Codes.push_back(Codes[P]);
 OtherCollection->Classes.push_back(Classes[P]);
}

void TVectorCollection :: CutAndPastePatternTo(int P, TVectorCollection* OtherCollection)
{
 if (Structure != OtherCollection->Structure)
    throw std::runtime_error("Trying to copy pattern to differently sectioned vector collection!");

 if (P < 0) throw std::runtime_error("Index out of bound!");
 if (unsigned(P) >= Patterns.size())
    throw std::runtime_error("Index out of bound!");

 OtherCollection->Patterns.push_back(Patterns[P]);
 OtherCollection->Codes.push_back(Codes[P]);
 OtherCollection->Classes.push_back(Classes[P]);

 DeletePattern(P);
}

void TVectorCollection :: DeletePattern(int Index)
{
 if (Patterns.empty()) throw std::runtime_error("Empty Data Set");
 if ( Index < 0 || Index >= int( Patterns.size() ) ) throw std::runtime_error("Index Out Of Range");

 std::vector<TSectionedArray>::iterator itp = Patterns.begin();
 std::vector<std::string>::iterator itcl = Classes.begin();
 std::vector<std::string>::iterator itcod = Codes.begin();

 for(int i = 0; i < Index; i++) {itp++; itcl++; itcod++;}

 Patterns.erase( itp );
 Classes.erase( itcl );
 Codes.erase( itcod );
}

void TVectorCollection :: SwapPatterns(int P1, int P2)
{
 if (Patterns.empty()) throw std::runtime_error("Empty Data Set");
 if ( P1 < 0 || P1 >= int( Patterns.size() ) ) throw std::runtime_error("Index Out Of Range");
 if ( P2 < 0 || P2 >= int( Patterns.size() ) ) throw std::runtime_error("Index Out Of Range");
 if (P1 != P2)
    {
     TSectionedArray AuxSa = Patterns[P1];
     Patterns[P1] = Patterns[P2];
     Patterns[P2] = AuxSa;

     std::string AuxCode = Codes[P1];
     Codes[P1] = Codes[P2];
     Codes[P2] = AuxCode;

     std::string AuxClass = Classes[P1];
     Classes[P1] = Classes[P2];
     Classes[P2] = AuxClass;
    }
}

int TVectorCollection :: GetNumberOfClasses() const
{
 std::set<std::string> DiffClasses( Classes.begin(), Classes.end() );
 return DiffClasses.size();
}

std::string TVectorCollection :: GetClassName(int Index) const
{
 std::set<std::string> DiffClasses( Classes.begin(), Classes.end() );
 std::vector<std::string> UniqueClasses( DiffClasses.begin(), DiffClasses.end() );
 return UniqueClasses.at(Index);
}

void TVectorCollection :: Projection(const std::vector<int>* Mask)
{
 // controllo maschera
 int S = Mask->size();
 if ( S != GetTotalNumberOfComponents() )
    throw std::runtime_error("Mask of invalid dimension");
 bool AllZero = true;
 for(int i = 0; i < S; i++) if ((*Mask)[i])
    AllZero = false;
 if (AllZero) throw std::runtime_error("All zero Mask");

 int Secs = GetNumberOfSections();

 // controllo validità maschera in corrispondenza a sezioni complesse
 bool ComplexOk = true;
 for(int s = 0; s < Secs; s++)
    if (GetSectionType(s) == stComplex)
       {
        int St = GetSectionStart(s);
        int Sz = GetSectionSize(s);

        for(int c = St; c < (St+Sz); c = c+2)
           {
            int Re = (*Mask)[c];
            int Im = (*Mask)[c+1];
            if ( (Re && (!Im)) || ((!Re) && Im) ) ComplexOk = false;
           }
       }

 if (!ComplexOk) throw std::runtime_error("Incorrect Complex Mask");

 // aggiorno info sezionamento
 std::vector<int> NewSectSizes(Secs, 0);
 for(int s = 0; s < Secs; s++)
    {
     int St = GetSectionStart(s);
     int Sz = GetSectionSize(s);

     for(int c = St; c < (St+Sz); c++)
        if ((*Mask)[c]) NewSectSizes[s]++;
    }

 for(int s = 0; s < Secs; s++) Structure[s].Size = NewSectSizes[s];
 std::vector<TSectionDescriptor>::iterator it = Structure.begin();
 while (it != Structure.end())
    {
     if ((*it).Size) it++;
     else it = Structure.erase(it);
    }

 // pattern
 int N = Patterns.size();
 TSectionedArray ZeroPattern( Structure );
 std::vector<TSectionedArray> NewPatterns( N, ZeroPattern );

 int count = 0;
 for(int c = 0; c < S; c++)
    {
     if ((*Mask)[c])
        {
         for(int p = 0; p < N; p++)
            NewPatterns[p].Data[count] = Patterns[p].Data[c];
         count++;
        }
    }

 Patterns = NewPatterns;
}
//---------------------------------------------------------------------------
