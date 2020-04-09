//---------------------------------------------------------------------------
#ifndef VectorCollectionH
#define VectorCollectionH

#include "SectionedArray.h"
#include "CrispPartition.h"
#include "StringUtilities.h"
#include <fstream>
#include <set>
#include <vector>

class TVectorCollection
{
 friend class TVectorDistanceTool;
 friend class TVectorNormalizationTool;
 friend class TVectorCentroidTool;
 friend class TMinMaxNetwork;

 private:

    std::string Name;

    // data set
    TSectioning Structure;

    std::vector<std::string> Codes;
    std::vector<std::string> Classes;

    std::vector<TSectionedArray> Patterns;
    // Patterns viene distrutto prima perché è dichiarato per ultimo !!!

 public:

    TVectorCollection(){Name = "VectorCollection";};
    TVectorCollection(const int, const std::string*, const TSectionType*, const int*);
    TVectorCollection(const TVectorCollection&); // copia intera collezione sorgente
    TVectorCollection& operator=(const TVectorCollection&);

    TVectorCollection(const TVectorCollection*);
    // crea collezione vuota con stessa struttura sorgente

    TVectorCollection(const TVectorCollection* Source, const std::vector<int>* Mask);
    // crea collezione selezionando un sottoinsieme di colonne della collezione
    // sorgente; le colonne selezionate sono quelle con valore mask diverso da zero;
    // la collezione creata avrà una sola sezione reale, dunque ogni informazione
    // di sezionamento della sorgente verrà distrutta. Mask dovrebbe avere dimensione
    // pari al numero totale di componenti della sorgente; se è minore, le colonne
    // successive vengono comunque scartate (come fosse 0), se è maggiore, solo
    // i primi N valori (se N è il numero di componenti) vengono usati.
    // Il sezionamento viene distrutto, il nuovo dataset ha una sola sezione chiamata
    // "Sect"

    TVectorCollection(const TVectorCollection* Source, int Sec, const std::vector<int>* Mask);
    // come il precedente, ma la maschera agisce solo sulla sezione Sec, mentre
    // le altre vengono lasciate invariate. Il sezionamento viene conservato.

    ~TVectorCollection();

    int GetNumberOfSections() const {return Structure.size();};
    std::string GetSectionName(int Index) const {return Structure.at(Index).Name;};
    TSectionType GetSectionType(int Index) const {return Structure.at(Index).Type;};
    int GetSectionSize(int Index) const {return Structure.at(Index).Size;};
    int GetSectionStart(int Index) const;

    std::string GetCollectionName() const {return Name;};
    void SetCollectionName(std::string Val){Name = Val;};

    void Projection(const std::vector<int>* Mask);
    // la maschera deve avere dimensione pari a GetTotalNumberOfComponents(),
    // le componenti con un valore di maschera diverso da zero vengono mantenute,
    // le altre scartate. L'informazione di sezionamento viene aggiornata.
    // La maschera deve contenere almeno un valore diverso da zero.

    // pattern

    int GetNumberOfPatterns() const {return Patterns.size();};
    int GetTotalNumberOfComponents() const;
    std::string GetClass(int Index) const {return Classes[Index];};
    std::string GetCode(int Index) const {return Codes[Index];};
    void SetClass(int Index, std::string Val) {Classes[Index] = Val;};
    void SetCode(int Index, std::string Val) {Codes[Index] = Val;};

    void AddPattern(std::string Code = "Pattern", std::string Class = "NoClass");
    void DeletePattern(int Index);
    void ClearPatterns(){Patterns.clear(); Codes.clear(); Classes.clear();};
    void SwapPatterns(int P1, int P2);

    prlreal ReadComponent(int P, int C) const {return Patterns[P].Data[C];};
    void WriteComponent(int P, int C, prlreal Val) {Patterns[P].Data[C] = Val;};

    const prlreal* GetDataPointer(int P) const {return Patterns[P].Data;};
    // resta valido finché non modifico la collezione

    prlreal* GetMeanArray(TCrispPartition* Partition, int Id) const;
    // media delle componenti dei vettori di classe Id;
    // alloca un array di dimensioni pari al numero totale di componenti
    // e torna il puntatore ad esso

    bool HasSameStructure(const TVectorCollection* OtherCollection) const {return (Structure == OtherCollection->Structure);};
    // controlla se hanno stessa struttura

    void MergeWith(const TVectorCollection* OtherCollection);
    // appende alla collezione presente tutti i pattern di OtherCollection,
    // OtherCollection non viene modificata,
    // se le due collezioni hanno diverso sezionamento lancia un'eccezione.

    void CopyAndPastePatternTo(int P, TVectorCollection* OtherCollection);
    // il P-esimo pattern viene appeso a OtherCollection, senza che venga eliminato
    // dalla collezione presente,
    // se le due collezioni hanno diverso sezionamento lancia un'eccezione.

    void CutAndPastePatternTo(int P, TVectorCollection* OtherCollection);
    // il P-esimo pattern viene appeso a OtherCollection, venendo eliminato
    // dalla collezione presente,
    // se le due collezioni hanno diverso sezionamento lancia un'eccezione.

    void Initialize(int N);
    // alloca N pattern di zeri con codice Pattern e classe NoClass
    // il precedente contenuto viene distrutto

    int GetNumberOfClasses() const;
    std::string GetClassName(int Index) const;

    void LoadDataFromFile(const std::string& NomeFile);
    void WriteFile(std::string NomeFile) const;

    //void WritePartitionedComponents(std::vector<int> Comps, TCrispPartition* Partition, std::string NomeFile);

    // metodi inseriti per compatibilità con un template

    int GetNumberOfPatterns(int L, int S) const {return Patterns.size();};
    // L ed S vengono ignorati
};

//---------------------------------------------------------------------------
#endif
