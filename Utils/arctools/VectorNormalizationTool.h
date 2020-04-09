//---------------------------------------------------------------------------
#ifndef VectorNormalizationToolH
#define VectorNormalizationToolH

#include "VectorCollection.h"
#include "PrlDefinitions.h"
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>

class TVectorNormalizationTool
{
 private:

    std::string Name;
    // nome normtool

    TVectorCollection RefStructure;
    // collezione vuota con struttura di riferimento

    std::vector<TRealFeatureNormalizationPair> RealPairs;
    std::vector<TComplexFeatureNormalizationPair> ComplexPairs;

    std::vector<int> PairMap;
    // per ciascuna caratteristica reale indica il real pair associato
    // per ciascuna caratteristca complessa di nodo indica il complex pair associato
    // se troviamo -1 vuol dire che non è una caratteristica reale o complessa o
    // che non subisce normalizzazione

    TRealAxisInterval RealDest;        // intervallo destinazione
    TComplexPlaneCircle ComplexDest;   // cerchio destinazione

    bool ClippingEnabled;

    void RealSectionManualSetup(const TVectorCollection* DataSet, int S,
                                TNormalizationMode NormMode, prlreal Headroom);

    void ComplexSectionManualSetup(const TVectorCollection* DataSet, int S,
                                   TNormalizationMode NormMode, prlreal Headroom);

    void RemoveUnusedPairs();

 public:

    TVectorNormalizationTool();
    // non inizializzato, occorre farlo tramite operatore =, o tramite caricamento
    // da file

    TVectorNormalizationTool(const TVectorCollection*);

    TVectorNormalizationTool(const TVectorNormalizationTool&);
    TVectorNormalizationTool& operator=(const TVectorNormalizationTool&);

    bool Initialized() const {return (bool) RefStructure.GetNumberOfSections();};
    // contiene le informazioni e può essere usato

    std::string GetName() const {return Name;};
    void SetName(std::string Val){Name = Val;};

    int GetNumberOfSections() const {return RefStructure.GetNumberOfSections();};
    std::string GetSectionName(int Index) const {return RefStructure.GetSectionName(Index);};
    TSectionType GetSectionType(int Index) const {return RefStructure.GetSectionType(Index);};
    int GetSectionSize(int Index) const {return RefStructure.GetSectionSize(Index);};

    int GetTotalNumberOfComponents() const;
    // dimensione totale vettore

    void Projection(std::vector<int>* Mask);
    // vedi TVectorCollection

    void Reset();
    // resetta le info di normalizzazione, non la struttura di sezionamento

    bool HasSameStructure(const TVectorCollection* Collection) const
       {return (RefStructure.Structure == Collection->Structure);};
    // controlla se hanno stessa struttura

    void AutoSetup(const TVectorCollection* DataSet,
                   TNormalizationMode NormMode, prlreal Headroom);

    void AutoSetup(const TVectorCollection* DataSet,
                   std::vector<TNormalizationMode> SectModes,
                   std::vector<prlreal> SectHeadrooms);

    //void AutoSetupGUI(const TVectorCollection* DataSet);

    void NormalizeCollection(TVectorCollection*) const;
    void DenormalizeCollection(TVectorCollection*) const;

    void WriteFile(std::string FileName) const;
    void WriteFile(std::ofstream& Fp) const; // append
    void LoadDataFromFile(std::string FileName);
    void LoadDataFromFile(std::ifstream& Fp); // da file aperto
    void WriteUserFriendlyFile(std::string FileName) const;
    std::string GetDetailedInfo() const;
};
//---------------------------------------------------------------------------
#endif
