#ifndef MRKRun_h
#define MRKRun_h 1

#include <time.h>

#include "globals.hh"
#include "G4Run.hh"
#include "G4THitsMap.hh"
#include "G4UnitsTable.hh"

#include "TTree.h"
#include "TFile.h"
#include "TMacro.h"

#include "MRKHit.hh"
#include "MRKPrimaryGeneratorAction.hh"
#include "MRKConstants.hh"

class G4Event;

///Defines a run, a group of events, all at the same setting
class MRKRun: public G4Run
{
public:
	MRKRun(MRKPrimaryGeneratorAction* generator);
	virtual ~MRKRun();
	virtual void RecordEvent(const G4Event*);
	void setOutputFile(TString outputFileName);
	void closeAndWrite();
	void openOutputFile();
	inline int getBAPDHitCount() const { return BAPDHitCount;}
	inline bool getFileIsOpen() const { return fileIsOpen; }
    inline void setMacroFileName(TString inp){macroFileName=inp;}
    inline void setUseFluxTree(bool inp){useFluxTree=inp;}

private:
	bool fileIsOpen;
	G4double GetTotal(const G4THitsMap<G4double> &map) const;
	G4int SBDCollectionID;
	G4int BGOCollectionID[NUM_BGOS] = { };
	G4int BAPDCollectionID[NUM_BAPDS] = { };
	Double_t SBDEDepTotal, SBDKEFirst, SBDTimeFirst;
	Double_t BGOEDepTotal[NUM_BGOS] = { };
	Double_t BAPDEDepTotal[NUM_BAPDS] = { };
	Double_t BGOKEFirst[NUM_BGOS] = { };
	Double_t BAPDKEFirst[NUM_BAPDS] = { };
	Double_t SBDPosXFirst, SBDPosYFirst, SBDPosZFirst, SBDMomXFirst, SBDMomYFirst, SBDMomZFirst;
	Double_t MomX, MomY, MomZ, PosX, PosY, PosZ, Time, EDep, KE;
	Int_t Clocks;
	Int_t eventNumber;
	Char_t Particle, HitCode;
	TString outputFileName;
	TFile* geantResultsFile;
	TTree* geantResultsTree;
	TTree* geantSBDFluxTree;
	TTree* geantBGOFluxTree[NUM_BGOS] = { };
	TTree* geantBAPDFluxTree[NUM_BAPDS] = { };
	MRKPrimaryGeneratorAction* theGenerator;

	G4THitsMap<G4double>* evtMap;
	MRKHitsCollection* SBDHitsCollection;
	MRKHitsCollection* BGOHitsCollection[NUM_BGOS] = { };
	MRKHitsCollection* BAPDHitsCollection[NUM_BAPDS] = { };
	G4String currentParticleName;
	int BAPDHitCount;
	TString macroFileName;

	bool useSBDDetector, useBGODetectors, useBAPDDetectors;
	bool useFluxTree;

};

#endif

