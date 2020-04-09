//---------------------------------------------------------------------------
#include "VectorNormalizationTool.h"

TVectorNormalizationTool :: TVectorNormalizationTool()
{
 Name = "VectorNormalizationTool";

 RealDest.Min = 0;
 RealDest.Max = 1;

 ComplexDest.R = 1;

 ClippingEnabled = true;
}

TVectorNormalizationTool :: TVectorNormalizationTool(const TVectorCollection* DataSet)
{
 Name = DataSet->GetCollectionName() + "_NormTool";

 RefStructure = *DataSet;
 RefStructure.ClearPatterns();
 RefStructure.SetCollectionName("ReferenceStructure");

 int Sz = RefStructure.GetTotalNumberOfComponents();

 PairMap.assign(Sz, -1);

 RealDest.Min = 0;
 RealDest.Max = 1;

 ComplexDest.R = 1;

 ClippingEnabled = true;
}

TVectorNormalizationTool :: TVectorNormalizationTool(const TVectorNormalizationTool& Source)
{
 Name = Source.Name;

 RefStructure = Source.RefStructure;

 RealPairs = Source.RealPairs;
 ComplexPairs = Source.ComplexPairs;

 PairMap = Source.PairMap;

 RealDest = Source.RealDest;
 ComplexDest = Source.ComplexDest;

 ClippingEnabled = Source.ClippingEnabled;
}

TVectorNormalizationTool& TVectorNormalizationTool :: operator=(const TVectorNormalizationTool& Source)
{
 if (this != &Source)
    {
     Name = Source.Name;

     RefStructure = Source.RefStructure;

     RealPairs = Source.RealPairs;
     ComplexPairs = Source.ComplexPairs;

     PairMap = Source.PairMap;

     RealDest = Source.RealDest;
     ComplexDest = Source.ComplexDest;

     ClippingEnabled = Source.ClippingEnabled;
    }

 return *this;
}

void TVectorNormalizationTool :: Projection(std::vector<int>* Mask)
{
 if ( !RefStructure.GetNumberOfSections() )
    throw std::runtime_error("VectorNormalizationTool non inizializzato!");

 RefStructure.Projection(Mask);

 if (Mask->size() != PairMap.size())
    throw std::runtime_error("Something weird happened");

 // riduco pair map
 int oldN = Mask->size();
 int newN = RefStructure.GetTotalNumberOfComponents();
 std::vector<int> NewPairMap(newN, -1);

 int count = 0;
 for(int c = 0; c < oldN; c++)
    {
     if ((*Mask)[c])
        {
         NewPairMap[count] = PairMap[c];
         count++;
        }
    }

 PairMap = NewPairMap;

 if (RefStructure.GetTotalNumberOfComponents() != int(PairMap.size()))
    throw std::runtime_error("Something weird happened");

 RemoveUnusedPairs();
}

void TVectorNormalizationTool :: RemoveUnusedPairs()
{
 if ( !RefStructure.GetNumberOfSections() )
    throw std::runtime_error("VectorNormalizationTool non inizializzato!");

 std::vector<int> RemapReal(RealPairs.size(), 0);
 std::vector<int> RemapComplex(ComplexPairs.size(), 0);

 int Secs = RefStructure.GetNumberOfSections();

 for(int s = 0; s < Secs; s++)
    switch (RefStructure.GetSectionType(s))
    {
     int St, Sz;
     case stReal:

        St = RefStructure.GetSectionStart(s);
        Sz = RefStructure.GetSectionSize(s);

        for(int c = St; c < (St+Sz); c++)
           if (PairMap[c] != -1)
              RemapReal[ PairMap[c] ]++;

     break;
     case stComplex:

        St = RefStructure.GetSectionStart(s);
        Sz = RefStructure.GetSectionSize(s);

        for(int c = St; c < (St+Sz); c = c+2)
           if ( (PairMap[c] != -1) && (PairMap[c+1] != -1) )
              {
               RemapComplex[ PairMap[c] ]++;
               RemapComplex[ PairMap[c+1] ]++;
              }

     break;
     default: break;
    }

 int count = 0;
 for(unsigned int i = 0; i < RemapReal.size(); i++)
    {
     if (RemapReal[i]) {RemapReal[i] = count; count++;}
     else RemapReal[i] = -2;
    }

 count = 0;
 for(unsigned int i = 0; i < RemapComplex.size(); i++)
    {
     if (RemapComplex[i]) {RemapComplex[i] = count; count++;}
     else RemapComplex[i] = -2;
    }

 for(int s = 0; s < Secs; s++)
    switch (RefStructure.GetSectionType(s))
    {
     int St, Sz;
     case stReal:

        St = RefStructure.GetSectionStart(s);
        Sz = RefStructure.GetSectionSize(s);

        for(int c = St; c < (St+Sz); c++)
           if (PairMap[c] != -1)
              PairMap[c] = RemapReal[ PairMap[c] ];

     break;
     case stComplex:

        St = RefStructure.GetSectionStart(s);
        Sz = RefStructure.GetSectionSize(s);

        for(int c = St; c < (St+Sz); c = c+2)
           if ( (PairMap[c] != -1) && (PairMap[c+1] != -1) )
              {
               PairMap[c] = RemapComplex[ PairMap[c] ];
               PairMap[c+1] = RemapComplex[ PairMap[c+1] ];
              }

     break;
     default: break;
    }

 std::vector<TRealFeatureNormalizationPair>::iterator rp_it = RealPairs.begin();
 std::vector<int>::iterator rmr_it = RemapReal.begin();

 while( rmr_it != RemapReal.end() )
    {
     if ( (*rmr_it) == -2 )
        {
         RemapReal.erase(rmr_it);
         RealPairs.erase(rp_it);
        }
     else
        {
         rmr_it++;
         rp_it++;
        }
    }

 std::vector<TComplexFeatureNormalizationPair>::iterator cp_it = ComplexPairs.begin();
 std::vector<int>::iterator rmc_it = RemapComplex.begin();

 while( rmc_it != RemapComplex.end() )
    {
     if ( (*rmc_it) == -2 )
        {
         RemapComplex.erase(rmc_it);
         ComplexPairs.erase(cp_it);
        }
     else
        {
         rmc_it++;
         cp_it++;
        }
    }

 // check
 bool Ok = true;
 for(unsigned int i = 0; i < RemapReal.size(); i++)
    if (RemapReal[i] != int(i)) Ok = false;

 for(unsigned int i = 0; i < RemapComplex.size(); i++)
    if (RemapComplex[i] != int(i)) Ok = false;

 if (!Ok) throw std::runtime_error("Something weird happened");
}

void TVectorNormalizationTool :: NormalizeCollection(TVectorCollection* DataSet) const
{
 if ( !RefStructure.GetNumberOfSections() )
    throw std::runtime_error("VectorNormalizationTool non inizializzato!");

 // controllo struttura
 if (!DataSet->HasSameStructure(&RefStructure))
    throw std::runtime_error("Struttura non compatibile!");

 int P = DataSet->GetNumberOfPatterns();

 if (!P) throw std::runtime_error("Collezione vuota!");

 int SN = DataSet->GetNumberOfSections();
 for(int s = 0; s < SN; s++)
    switch ( DataSet->GetSectionType(s) )
       {
        int St, Sz;
        case stReal:

           St = DataSet->GetSectionStart(s);
           Sz = DataSet->GetSectionSize(s);

           for(int c = St; c < (St+Sz); c++)
              if (PairMap[c] != -1)
                 {
                  TRealFeatureNormalizationPair CurrPair = RealPairs[ PairMap[c] ];

                  for(int p = 0; p < P; p++)
                     {
                      prlreal aux = DataSet->ReadComponent(p, c);

                      // trasformazione affine verso [0, 1]
                      aux -= CurrPair.S.Min;
                      if ( (CurrPair.S.Max - CurrPair.S.Min) > ZT )
                         aux /= (CurrPair.S.Max - CurrPair.S.Min);

                      // trasformazione affine verso destinazione
                      aux = aux * (CurrPair.D.Max - CurrPair.D.Min) + CurrPair.D.Min;

                      // clipping
                      if (ClippingEnabled)
                         {
                          if (aux < CurrPair.D.Min) aux = CurrPair.D.Min;
                          if (aux > CurrPair.D.Max) aux = CurrPair.D.Max;
                         }

                      DataSet->WriteComponent(p, c, aux);
                     }
                 }

        break;
        case stComplex:

           St = DataSet->GetSectionStart(s);
           Sz = DataSet->GetSectionSize(s);

           for(int c = St; c < (St+Sz); c = c+2)
              if (PairMap[c] != -1)
                 {
                  TComplexFeatureNormalizationPair CurrPair = ComplexPairs[ PairMap[c] ];

                  for(int p = 0; p < P; p++)
                     {
                      prlreal aux_re = DataSet->ReadComponent(p, c);
                      prlreal aux_im = DataSet->ReadComponent(p, c+1);

                      // trasformazione verso cerchio unitario
                      if (CurrPair.S.R > ZT) aux_re /= CurrPair.S.R;
                      if (CurrPair.S.R > ZT) aux_im /= CurrPair.S.R;

                      // clipping modulo
                      if (ClippingEnabled)
                         {
                          prlreal mag = sqrt( std::pow(aux_re, 2) +
                                              std::pow(aux_im, 2) );

                          if (mag > 1)
                             {
                              aux_re /= mag;
                              aux_im /= mag;
                             }
                         }

                      // trasformazione verso destinazione
                      aux_re = aux_re * CurrPair.D.R;
                      aux_im = aux_im * CurrPair.D.R;

                      DataSet->WriteComponent(p, c, aux_re);
                      DataSet->WriteComponent(p, c+1, aux_im);
                     }
                 }

        break;
        default: break;
       }
}

void TVectorNormalizationTool :: DenormalizeCollection(TVectorCollection* DataSet) const
{
 if ( !RefStructure.GetNumberOfSections() )
    throw std::runtime_error("VectorNormalizationTool non inizializzato!");

 // controllo struttura
 if (!DataSet->HasSameStructure(&RefStructure))
    throw std::runtime_error("Struttura non compatibile!");

 int P = DataSet->GetNumberOfPatterns();

 if (!P) throw std::runtime_error("Collezione vuota!");

 // nodi
 int SN = DataSet->GetNumberOfSections();
 for(int s = 0; s < SN; s++)
    switch ( DataSet->GetSectionType(s) )
       {
        int St, Sz;
        case stReal:

           St = DataSet->GetSectionStart(s);
           Sz = DataSet->GetSectionSize(s);

           for(int c = St; c < (St+Sz); c++)
              if (PairMap[c] != -1)
                 {
                  TRealFeatureNormalizationPair CurrPair = RealPairs[ PairMap[c] ];

                  for(int p = 0; p < P; p++)
                     {
                      prlreal aux = DataSet->ReadComponent(p, c);

                      // da destinazione a [0, 1]
                      aux -= CurrPair.D.Min;
                      if ( (CurrPair.D.Max - CurrPair.D.Min) > ZT )
                         aux /= (CurrPair.D.Max - CurrPair.D.Min);

                      // da [0, 1] a sorgente
                      aux = aux * (CurrPair.S.Max - CurrPair.S.Min) + CurrPair.S.Min;

                      DataSet->WriteComponent(p, c, aux);
                     }
                 }

        break;
        case stComplex:

           St = DataSet->GetSectionStart(s);
           Sz = DataSet->GetSectionSize(s);

           for(int c = St; c < (St+Sz); c = c+2)
              if (PairMap[c] != -1)
                 {
                  TComplexFeatureNormalizationPair CurrPair = ComplexPairs[ PairMap[c] ];

                  for(int p = 0; p < P; p++)
                     {
                      prlreal aux_re = DataSet->ReadComponent(p, c);
                      prlreal aux_im = DataSet->ReadComponent(p, c+1);

                      // da destinazione a cerchio unitario
                      if (CurrPair.D.R > ZT) aux_re /= CurrPair.D.R;
                      if (CurrPair.D.R > ZT) aux_im /= CurrPair.D.R;

                      // da cerchio unitario a sorgente
                      aux_re *= CurrPair.S.R;
                      aux_im *= CurrPair.S.R;

                      DataSet->WriteComponent(p, c, aux_re);
                      DataSet->WriteComponent(p, c+1, aux_im);
                     }
                 }

        break;
        default: break;
       }
}

void TVectorNormalizationTool :: RealSectionManualSetup(const TVectorCollection* DataSet, int S,
                                                        TNormalizationMode NormMode, prlreal Headroom)
{
 if ( !RefStructure.GetNumberOfSections() )
    throw std::runtime_error("VectorNormalizationTool non inizializzato!");

 // controllo struttura
 if (!DataSet->HasSameStructure(&RefStructure))
    throw std::runtime_error("Struttura non compatibile!");

 // controllo S
 if ( S >= DataSet->GetNumberOfSections() )
    throw std::runtime_error("S fuori range!");
 if ( S < 0 )
    throw std::runtime_error("S fuori range!");

 // controllo tipo
 if ( DataSet->GetSectionType(S) != stReal )
    throw std::runtime_error("Sezione non reale!");

 int St = DataSet->GetSectionStart(S);
 int Sz = DataSet->GetSectionSize(S);

 int P = DataSet->GetNumberOfPatterns();

 switch (NormMode)
  {
   case nmNone: break;
   case nmFeatureWise:

      for(int c = St; c < (St+Sz); c++)
         {
          TRealFeatureNormalizationPair NewPair;
          NewPair.D = RealDest;

          prlreal Min = DataSet->ReadComponent(0, c);
          prlreal Max = DataSet->ReadComponent(0, c);
          for(int p = 0; p < P; p++)
             {
              prlreal CurrVal = DataSet->ReadComponent(p, c);
              if (CurrVal < Min) Min = CurrVal;
              if (CurrVal > Max) Max = CurrVal;
             }

          prlreal Delta = fabs(Max - Min) * Headroom;

          NewPair.S.Min = Min - (Delta / 2);
          NewPair.S.Max = Max + (Delta / 2);

          RealPairs.push_back(NewPair);
          PairMap[c] = RealPairs.size() - 1;
         }

   break;
   case nmSectionWise:

      TRealFeatureNormalizationPair NewPair;
      NewPair.D = RealDest;

      prlreal Min = DataSet->ReadComponent(0, St);
      prlreal Max = DataSet->ReadComponent(0, St);
      for(int c = St; c < (St+Sz); c++)
         for(int p = 0; p < P; p++)
            {
             prlreal CurrVal = DataSet->ReadComponent(p, c);
             if (CurrVal < Min) Min = CurrVal;
             if (CurrVal > Max) Max = CurrVal;
            }

      prlreal Delta = fabs(Max - Min) * Headroom;

      NewPair.S.Min = Min - (Delta / 2);
      NewPair.S.Max = Max + (Delta / 2);

      RealPairs.push_back(NewPair);
      for(int c = St; c < (St+Sz); c++)
         PairMap[c] = RealPairs.size() - 1;

   break;
  }
}

void TVectorNormalizationTool :: ComplexSectionManualSetup(const TVectorCollection* DataSet, int S,
                                                           TNormalizationMode NormMode, prlreal Headroom)
{
 if ( !RefStructure.GetNumberOfSections() )
    throw std::runtime_error("VectorNormalizationTool non inizializzato!");

 // controllo struttura
 if (!DataSet->HasSameStructure(&RefStructure))
    throw std::runtime_error("Struttura non compatibile!");

 // controllo S
 if ( S >= DataSet->GetNumberOfSections() )
    throw std::runtime_error("S fuori range!");
 if ( S < 0 )
    throw std::runtime_error("S fuori range!");

 // controllo tipo
 if ( DataSet->GetSectionType(S) != stComplex )
    throw std::runtime_error("Sezione non complessa!");

 int St = DataSet->GetSectionStart(S);
 int Sz = DataSet->GetSectionSize(S);

 int P = DataSet->GetNumberOfPatterns();

 switch (NormMode)
  {
   case nmNone: break;
   case nmFeatureWise:

      for(int c = St; c < (St+Sz); c = c+2)
         {
          TComplexFeatureNormalizationPair NewPair;
          NewPair.D = ComplexDest;

          prlreal R = sqrt( std::pow(DataSet->ReadComponent(0, c), 2) +
                            std::pow(DataSet->ReadComponent(0, c+1), 2) );

          for(int p = 0; p < P; p++)
             {
              prlreal CurrVal = sqrt( std::pow(DataSet->ReadComponent(p, c), 2) +
                                      std::pow(DataSet->ReadComponent(p, c+1), 2) );

              if (CurrVal > R) R = CurrVal;
             }

          prlreal Delta = R * Headroom;

          NewPair.S.R = R + Delta;

          ComplexPairs.push_back(NewPair);
          PairMap[c] = ComplexPairs.size() - 1;
          PairMap[c+1] = ComplexPairs.size() - 1;
         }

   break;
   case nmSectionWise:

      TComplexFeatureNormalizationPair NewPair;
      NewPair.D = ComplexDest;

      prlreal R = sqrt( std::pow(DataSet->ReadComponent(0, St), 2) +
                        std::pow(DataSet->ReadComponent(0, St+1), 2) );

      for(int c = St; c < (St+Sz); c = c+2)
         for(int p = 0; p < P; p++)
            {
             prlreal CurrVal = sqrt( std::pow(DataSet->ReadComponent(p, c), 2) +
                                     std::pow(DataSet->ReadComponent(p, c+1), 2) );

             if (CurrVal > R) R = CurrVal;
            }

      prlreal Delta = R * Headroom;

      NewPair.S.R = R + Delta;

      ComplexPairs.push_back(NewPair);
      for(int c = St; c < (St+Sz); c = c+2)
         {
          PairMap[c] = ComplexPairs.size() - 1;
          PairMap[c+1] = ComplexPairs.size() - 1;
         }

   break;
  }
}

void TVectorNormalizationTool :: AutoSetup(const TVectorCollection* DataSet,
                                           TNormalizationMode NormMode, prlreal Headroom)
{
 if ( !RefStructure.GetNumberOfSections() )
    throw std::runtime_error("VectorNormalizationTool non inizializzato!");

 // controllo struttura
 if (!DataSet->HasSameStructure(&RefStructure))
    throw std::runtime_error("Struttura non compatibile!");

 if (Headroom < -0.5) throw std::runtime_error("Headroom negativo troppo grande!");

 Reset();
 Name = DataSet->GetCollectionName() + "_NormTool";

 int SN = DataSet->GetNumberOfSections();

 for(int s = 0; s < SN; s++)
    {
     switch ( DataSet->GetSectionType(s) )
      {
       case stReal: RealSectionManualSetup(DataSet, s, NormMode, Headroom);
       break;
       case stComplex: ComplexSectionManualSetup(DataSet, s, NormMode, Headroom);
       break;
       default: break;
      }
    }
}

void TVectorNormalizationTool :: AutoSetup(const TVectorCollection* DataSet,
                                           std::vector<TNormalizationMode> SectModes,
                                           std::vector<prlreal> SectHeadrooms)
{
 if ( !RefStructure.GetNumberOfSections() )
    throw std::runtime_error("VectorNormalizationTool non inizializzato!");

 // controllo struttura
 if (!DataSet->HasSameStructure(&RefStructure))
    throw std::runtime_error("Struttura non compatibile!");

 int SN = DataSet->GetNumberOfSections();

 // controllo dati
 if (int(SectModes.size()) != SN)
    throw std::runtime_error("Dati non compatibili!");
 if (int(SectHeadrooms.size()) != SN)
    throw std::runtime_error("Dati non compatibili!");

 for(int s = 0; s < SN; s++)
    if (SectHeadrooms[s] < -0.5)
       throw std::runtime_error("Headroom negativo troppo grande!");

 Reset();
 Name = DataSet->GetCollectionName() + "_NormTool";

 for(int s = 0; s < SN; s++)
    {
     switch ( DataSet->GetSectionType(s) )
      {
       case stReal: RealSectionManualSetup(DataSet, s, SectModes[s], SectHeadrooms[s]);
       break;
       case stComplex: ComplexSectionManualSetup(DataSet, s, SectModes[s], SectHeadrooms[s]);
       break;
       default: break;
      }
    }
}

/*void TVectorNormalizationTool :: AutoSetupGUI(const TVectorCollection* DataSet)
{
 if ( !RefStructure.GetNumberOfSections() )
    throw std::runtime_error("VectorNormalizationTool non inizializzato!");

 // controllo struttura
 if (!DataSet->HasSameStructure(&RefStructure))
    throw std::runtime_error("Struttura non compatibile!");

 int SN = DataSet->GetNumberOfSections();

 std::vector<TNormalizationMode> SectModes(SN, nmNone);
 std::vector<prlreal> SectHeadrooms(SN, 0);

 VNTParForm->SetReferences(DataSet, &SectModes, &SectHeadrooms);
 VNTParForm->ShowModal();

 AutoSetup(DataSet, SectModes, SectHeadrooms);
}*/

void TVectorNormalizationTool :: Reset()
{
 if ( !RefStructure.GetNumberOfSections() )
    throw std::runtime_error("VectorNormalizationTool non inizializzato!");

 RealPairs.clear();
 ComplexPairs.clear();

 int Sz = PairMap.size();

 for(int i = 0; i < Sz; i++) PairMap[i] = -1;
}

void TVectorNormalizationTool :: WriteFile(std::string FileName) const
{
 if ( !RefStructure.GetNumberOfSections() )
    throw std::runtime_error("VectorNormalizationTool non inizializzato!");

 std::ofstream f_out(FileName.c_str());
 if (!f_out) throw std::runtime_error("Errore apertura file!");
 f_out.setf(std::ios_base::fixed, std::ios_base::floatfield);
 f_out.precision(6);

 int SN = RefStructure.GetNumberOfSections();

 f_out << "Vector Normalization Tool 1.0" << std::endl;
 f_out << Name.c_str() << std::endl << std::endl;

 // formato vero e proprio

 f_out << SN << std::endl;

 for(int s = 0; s < SN; s++)
    {
     f_out << RefStructure.GetSectionName(s).c_str() << '\t';
     f_out << RefStructure.GetSectionSize(s) << '\t';

     std::string temp;
     switch (RefStructure.GetSectionType(s))
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

 f_out << RealDest.Min << '\t' << RealDest.Max << std::endl;
 f_out << ComplexDest.R << std::endl;
 f_out << int(ClippingEnabled) << std::endl << std::endl;

 f_out << RealPairs.size() << std::endl;
 for(unsigned int i = 0; i < RealPairs.size(); i++)
    f_out << RealPairs[i].S.Min << '\t' << RealPairs[i].S.Max << '\t'
          << RealPairs[i].D.Min << '\t' << RealPairs[i].D.Max << std::endl;

 f_out << std::endl << ComplexPairs.size() << std::endl;
 for(unsigned int i = 0; i < ComplexPairs.size(); i++)
    f_out << ComplexPairs[i].S.R << '\t' << ComplexPairs[i].D.R << std::endl;

 f_out << std::endl;
 for(unsigned int i = 0; i < PairMap.size(); i++)
    f_out << PairMap[i] << std::endl;
}

void TVectorNormalizationTool :: WriteUserFriendlyFile(std::string FileName) const
{
 if ( !RefStructure.GetNumberOfSections() )
    throw std::runtime_error("VectorNormalizationTool non inizializzato!");

 std::ofstream f_out(FileName.c_str());
 if (!f_out) throw std::runtime_error("Errore apertura file!");
 f_out.setf(std::ios_base::fixed, std::ios_base::floatfield);
 f_out.precision(6);

 int SN = RefStructure.GetNumberOfSections();

 f_out << "Vector Normalization Tool 1.0 - User Fristd::endly" << std::endl;
 f_out << Name.c_str() << std::endl << std::endl;

 f_out << "Clipping mode";
 if (ClippingEnabled) f_out << " enabled" << std::endl << std::endl;
 else f_out << " disabled" << std::endl << std::endl;

 f_out << "Real Sections:";
 bool Found = false;
 for(int s = 0; s < SN; s++)
    if (RefStructure.GetSectionType(s) == stReal)
       {
        Found = true;
        f_out << std::endl << RefStructure.GetSectionName(s).c_str() << std::endl;

        int St = 0;
        for(int ps = 0; ps < s; ps++) St += RefStructure.GetSectionSize(ps);
        int Sz = RefStructure.GetSectionSize(s);

        for(int c = St; c < (St+Sz); c++)
           {
            if (PairMap[c] != -1)
               {
                TRealFeatureNormalizationPair CurrPair = RealPairs[ PairMap[c] ];
                f_out << "(" << c << "): " << "[" << CurrPair.S.Min << ", "
                      << CurrPair.S.Max << "] -> [" << CurrPair.D.Min << ", "
                      << CurrPair.D.Max << "]" << std::endl;
               }
            else f_out << "(" << c << "): " << "no normalization" << std::endl;
           }
       }
 if (!Found) f_out << " absent" << std::endl << std::endl;
 else f_out << std::endl;

 f_out << "Complex Sections:";
 Found = false;
 for(int s = 0; s < SN; s++)
    if (RefStructure.GetSectionType(s) == stComplex)
       {
        Found = true;
        f_out << std::endl << RefStructure.GetSectionName(s).c_str() << std::endl;

        int St = 0;
        for(int ps = 0; ps < s; ps++) St += RefStructure.GetSectionSize(ps);
        int Sz = RefStructure.GetSectionSize(s);

        for(int c = St; c < (St+Sz); c = c+2)
           {
            if (PairMap[c+1] != PairMap[c])
               throw std::runtime_error("Something Weird Happened");

            if (PairMap[c] != -1)
               {
                TComplexFeatureNormalizationPair CurrPair = ComplexPairs[ PairMap[c] ];
                f_out << "(" << c << ", " << c+1 << "): " << "R=" << CurrPair.S.R
                      << " -> R=" << CurrPair.D.R << std::endl;
               }
            else f_out << "(" << c << ", " << c+1 << "): " << "no normalization" << std::endl;
           }
       }
 if (!Found) f_out << " absent" << std::endl << std::endl;
 else f_out << std::endl;
}

void TVectorNormalizationTool :: WriteFile(std::ofstream& Fp) const
{
 if ( !RefStructure.GetNumberOfSections() )
    throw std::runtime_error("VectorNormalizationTool non inizializzato!");

 if (!Fp) throw std::runtime_error("Errore apertura file!");

 int SN = RefStructure.GetNumberOfSections();

 // formato vero e proprio

 Fp << SN << std::endl;

 for(int s = 0; s < SN; s++)
    {
     Fp << RefStructure.GetSectionName(s).c_str() << '\t';
     Fp << RefStructure.GetSectionSize(s) << '\t';

     std::string temp;
     switch (RefStructure.GetSectionType(s))
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

     Fp << temp.c_str() << std::endl;
    }

 Fp << std::endl;

 Fp << RealDest.Min << '\t' << RealDest.Max << std::endl;
 Fp << ComplexDest.R << std::endl;
 Fp << int(ClippingEnabled) << std::endl << std::endl;

 Fp << RealPairs.size() << std::endl;
 for(unsigned int i = 0; i < RealPairs.size(); i++)
    Fp << RealPairs[i].S.Min << '\t' << RealPairs[i].S.Max << '\t'
          << RealPairs[i].D.Min << '\t' << RealPairs[i].D.Max << std::endl;

 Fp << std::endl << ComplexPairs.size() << std::endl;
 for(unsigned int i = 0; i < ComplexPairs.size(); i++)
    Fp << ComplexPairs[i].S.R << '\t' << ComplexPairs[i].D.R << std::endl;

 Fp << std::endl;
 for(unsigned int i = 0; i < PairMap.size(); i++)
    Fp << PairMap[i] << std::endl;
}

void TVectorNormalizationTool :: LoadDataFromFile(std::string FileName)
{
 // clear dati precedenti
 TVectorCollection DummyColl;
 RefStructure = DummyColl; // priva info sezionamento

 RealPairs.clear();
 ComplexPairs.clear();
 PairMap.clear();

 std::ifstream f_in( FileName.c_str() );
 if (!f_in) throw std::runtime_error("Errore apertura file!");

 char buf[256];
 f_in.getline(buf, 256, '\n');
 std::string FileId(buf);
 if (FileId != "Vector Normalization Tool 1.0") throw std::runtime_error("Formato file scorretto!");

 f_in.width(256);
 f_in >> buf;
 std::string NormToolName(buf);
 Name = NormToolName;

 int Nsez;
 f_in >> Nsez;
 if ( !Nsez ) throw std::runtime_error("Formato file scorretto!");
 if ( !f_in.good() ) throw std::runtime_error("Formato file scorretto!");

 std::vector<std::string> SecNames;
 std::vector<TSectionType> SecTypes;
 std::vector<int> SecSizes;
 for(int s = 0; s < Nsez; s++)
    {
     f_in.width(256);
     f_in >> buf;
     std::string SectName(buf);

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

     SecNames.push_back(SectName);
     SecTypes.push_back(Type);
     SecSizes.push_back(Size);
    }

 TVectorCollection TempColl(Nsez, &SecNames[0], &SecTypes[0], &SecSizes[0]);
 RefStructure = TempColl;

 f_in >> RealDest.Min;
 f_in >> RealDest.Max;
 f_in >> ComplexDest.R;
 f_in >> ClippingEnabled;
 if ( !f_in.good() ) throw std::runtime_error("Formato file scorretto!");

 int RPnum;
 f_in >> RPnum;
 if (RPnum < 0) throw std::runtime_error("Formato file scorretto!");
 if ( !f_in.good() ) throw std::runtime_error("Formato file scorretto!");

 for(int i = 0; i < RPnum; i++)
    {
     TRealFeatureNormalizationPair NewPair;
     f_in >> NewPair.S.Min;
     f_in >> NewPair.S.Max;
     f_in >> NewPair.D.Min;
     f_in >> NewPair.D.Max;
     if (NewPair.S.Min > NewPair.S.Max) throw std::runtime_error("Formato file scorretto!");
     if (NewPair.D.Min >= NewPair.D.Max) throw std::runtime_error("Formato file scorretto!");
     RealPairs.push_back(NewPair);
    }

 int CPnum;
 f_in >> CPnum;
 if (CPnum < 0) throw std::runtime_error("Formato file scorretto!");
 if ( !f_in.good() ) throw std::runtime_error("Formato file scorretto!");

 for(int i = 0; i < CPnum; i++)
    {
     TComplexFeatureNormalizationPair NewPair;
     f_in >> NewPair.S.R;
     f_in >> NewPair.D.R;
     if (NewPair.S.R < 0) throw std::runtime_error("Formato file scorretto!");
     if (NewPair.D.R <= 0) throw std::runtime_error("Formato file scorretto!");
     ComplexPairs.push_back(NewPair);
    }

 if ( !f_in.good() ) throw std::runtime_error("Formato file scorretto!");

 int NF = RefStructure.GetTotalNumberOfComponents();
 PairMap.assign(NF, -2);
 for(int i = 0; i < NF; i++)
    {
     int MapComp;
     f_in >> MapComp;
     PairMap[i] = MapComp;
    }

 if ( !f_in.good() ) throw std::runtime_error("Formato file scorretto!");

 // check pair map
 bool PairMapOk = true;
 for(int s = 0; s < Nsez; s++)
    switch (RefStructure.GetSectionType(s))
    {
     int St, Sz;
     case stReal:

        St = RefStructure.GetSectionStart(s);
        Sz = RefStructure.GetSectionSize(s);

        for(int c = St; c < (St+Sz); c++)
           if ( (PairMap[c] < -1) || (PairMap[c] >= RPnum) ) PairMapOk = false;

     break;
     case stComplex:

        St = RefStructure.GetSectionStart(s);
        Sz = RefStructure.GetSectionSize(s);

        for(int c = St; c < (St+Sz); c = c+2)
           {
            if ( PairMap[c] != PairMap[c+1] ) PairMapOk = false;
            if ( (PairMap[c] < -1) || (PairMap[c] >= CPnum) ) PairMapOk = false;
           }

     break;
     default: break;
    }

 if ( !PairMapOk ) throw std::runtime_error("Formato file scorretto!");
}

void TVectorNormalizationTool :: LoadDataFromFile(std::ifstream& Fp)
{
 // clear dati precedenti
 TVectorCollection DummyColl;
 RefStructure = DummyColl; // priva info sezionamento

 RealPairs.clear();
 ComplexPairs.clear();
 PairMap.clear();

 char buf[256];

 int Nsez;
 Fp >> Nsez;
 if ( !Nsez ) throw std::runtime_error("Formato file scorretto!");
 if ( !Fp.good() ) throw std::runtime_error("Formato file scorretto!");

 std::vector<std::string> SecNames;
 std::vector<TSectionType> SecTypes;
 std::vector<int> SecSizes;
 for(int s = 0; s < Nsez; s++)
    {
     Fp.width(256);
     Fp >> buf;
     std::string SectName(buf);

     int Size;
     Fp >> Size;

     Fp.width(256);
     Fp >> buf;
     std::string sType(buf);
     if ( !Fp.good() ) throw std::runtime_error("Formato file scorretto!");

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

     SecNames.push_back(SectName);
     SecTypes.push_back(Type);
     SecSizes.push_back(Size);
    }

 TVectorCollection TempColl(Nsez, &SecNames[0], &SecTypes[0], &SecSizes[0]);
 RefStructure = TempColl;

 Fp >> RealDest.Min;
 Fp >> RealDest.Max;
 Fp >> ComplexDest.R;
 Fp >> ClippingEnabled;
 if ( !Fp.good() ) throw std::runtime_error("Formato file scorretto!");

 int RPnum;
 Fp >> RPnum;
 if (RPnum < 0) throw std::runtime_error("Formato file scorretto!");
 if ( !Fp.good() ) throw std::runtime_error("Formato file scorretto!");

 for(int i = 0; i < RPnum; i++)
    {
     TRealFeatureNormalizationPair NewPair;
     Fp >> NewPair.S.Min;
     Fp >> NewPair.S.Max;
     Fp >> NewPair.D.Min;
     Fp >> NewPair.D.Max;
     if (NewPair.S.Min > NewPair.S.Max) throw std::runtime_error("Formato file scorretto!");
     if (NewPair.D.Min >= NewPair.D.Max) throw std::runtime_error("Formato file scorretto!");
     RealPairs.push_back(NewPair);
    }

 int CPnum;
 Fp >> CPnum;
 if (CPnum < 0) throw std::runtime_error("Formato file scorretto!");
 if ( !Fp.good() ) throw std::runtime_error("Formato file scorretto!");

 for(int i = 0; i < CPnum; i++)
    {
     TComplexFeatureNormalizationPair NewPair;
     Fp >> NewPair.S.R;
     Fp >> NewPair.D.R;
     if (NewPair.S.R < 0) throw std::runtime_error("Formato file scorretto!");
     if (NewPair.D.R <= 0) throw std::runtime_error("Formato file scorretto!");
     ComplexPairs.push_back(NewPair);
    }

 if ( !Fp.good() ) throw std::runtime_error("Formato file scorretto!");

 int NF = RefStructure.GetTotalNumberOfComponents();
 PairMap.assign(NF, -2);
 for(int i = 0; i < NF; i++)
    {
     int MapComp;
     Fp >> MapComp;
     PairMap[i] = MapComp;
    }

 if ( !Fp.good() ) throw std::runtime_error("Formato file scorretto!");

 // check pair map
 bool PairMapOk = true;
 for(int s = 0; s < Nsez; s++)
    switch (RefStructure.GetSectionType(s))
    {
     int St, Sz;
     case stReal:

        St = RefStructure.GetSectionStart(s);
        Sz = RefStructure.GetSectionSize(s);

        for(int c = St; c < (St+Sz); c++)
           if ( (PairMap[c] < -1) || (PairMap[c] >= RPnum) ) PairMapOk = false;

     break;
     case stComplex:

        St = RefStructure.GetSectionStart(s);
        Sz = RefStructure.GetSectionSize(s);

        for(int c = St; c < (St+Sz); c = c+2)
           {
            if ( PairMap[c] != PairMap[c+1] ) PairMapOk = false;
            if ( (PairMap[c] < -1) || (PairMap[c] >= CPnum) ) PairMapOk = false;
           }

     break;
     default: break;
    }

 if ( !PairMapOk ) throw std::runtime_error("Formato file scorretto!");
}

int TVectorNormalizationTool :: GetTotalNumberOfComponents() const
{
 if ( Initialized() ) return RefStructure.GetTotalNumberOfComponents();
 else throw std::runtime_error("VectorNormalizationTool non inizializzato!");
}

std::string TVectorNormalizationTool :: GetDetailedInfo() const
{
 if ( !RefStructure.GetNumberOfSections() )
    throw std::runtime_error("VectorNormalizationTool non inizializzato!");

 std::string Info = "";

 int SN = RefStructure.GetNumberOfSections();

 //Info += "Vector Normalization Tool 1.0 Detailed Info\n";
 Info += (Name + "\n\n");

 Info += "Clipping mode";
 if (ClippingEnabled) Info += " enabled\n\n";
 else Info += " disabled\n\n";

 Info += "Real Sections:";
 bool Found = false;
 for(int s = 0; s < SN; s++)
    if (RefStructure.GetSectionType(s) == stReal)
       {
        Found = true;
        Info += ("\n" + RefStructure.GetSectionName(s) + "\n");

        int St = 0;
        for(int ps = 0; ps < s; ps++) St += RefStructure.GetSectionSize(ps);
        int Sz = RefStructure.GetSectionSize(s);

        for(int c = St; c < (St+Sz); c++)
           {
            if (PairMap[c] != -1)
               {
                TRealFeatureNormalizationPair CurrPair = RealPairs[ PairMap[c] ];
                Info += ( "(" + boost::lexical_cast<std::string>(c) + "): " + "[" + boost::lexical_cast<std::string>(CurrPair.S.Min) + ", "
                          + boost::lexical_cast<std::string>(CurrPair.S.Max) + "] -> [" + boost::lexical_cast<std::string>(CurrPair.D.Min) + ", "
                          + boost::lexical_cast<std::string>(CurrPair.D.Max) + "]\n" );
               }
            else Info += ( "(" + boost::lexical_cast<std::string>(c) + "): no normalization\n" );
           }
       }
 if (!Found) Info += " absent\n\n";
 else Info += "\n";

 Info += "Complex Sections:";
 Found = false;
 for(int s = 0; s < SN; s++)
    if (RefStructure.GetSectionType(s) == stComplex)
       {
        Found = true;
        Info += ("\n" + RefStructure.GetSectionName(s) + "\n");

        int St = 0;
        for(int ps = 0; ps < s; ps++) St += RefStructure.GetSectionSize(ps);
        int Sz = RefStructure.GetSectionSize(s);

        for(int c = St; c < (St+Sz); c = c+2)
           {
            if (PairMap[c+1] != PairMap[c])
               throw std::runtime_error("Something Weird Happened");

            if (PairMap[c] != -1)
               {
                TComplexFeatureNormalizationPair CurrPair = ComplexPairs[ PairMap[c] ];
                Info += ( "(" + boost::lexical_cast<std::string>(c) + ", " + boost::lexical_cast<std::string>(c+1) + "): R=" + boost::lexical_cast<std::string>(CurrPair.S.R)
                          + " -> R=" + boost::lexical_cast<std::string>(CurrPair.D.R) + "\n" );
               }
            else Info += ( "(" + boost::lexical_cast<std::string>(c) + ", " + boost::lexical_cast<std::string>(c+1) + "): no normalization\n" );
           }
       }
 if (!Found) Info += " absent\n\n";
 else Info += "\n";

 return Info;
}
//---------------------------------------------------------------------------
