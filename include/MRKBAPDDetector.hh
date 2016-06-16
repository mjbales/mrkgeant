#ifndef MRKBAPDDetector_h
#define MRKBAPDDetector_h 1

#include "G4VSensitiveDetector.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"

#include "TH1.h"

#include "MRKHit.hh"
#include "MRKText.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class MRKBAPDDetector: public G4VSensitiveDetector
{

public:

	MRKBAPDDetector(G4String name, TH1D* inpSio2Vector, int inpDetNum, G4ThreeVector inpGammaDetOffset, bool inpUseCollectionEfficiencyModel = true);
	~MRKBAPDDetector();

	void Initialize(G4HCofThisEvent* HCE);
	G4bool ProcessHits(G4Step*aStep, G4TouchableHistory* ROhist);
	void EndOfEvent(G4HCofThisEvent *HCE);
	void clear();
	void DrawAll();
	void PrintAll();
	double BAPDCollectionEfficiencyModel(double inpEnergy, double depth);

private:

	MRKHitsCollection* mrkCollection;
	G4ParticleDefinition* protonParDef;
	G4ParticleDefinition* electronParDef;
	G4ParticleDefinition* photonParDef;

	int HCID;
	TH1D* sio2Hist;
	double absorbDataStart, absorbDataSpacing, thicknessMultiplier;
	G4ThreeVector gMomentum;
	double bapdFrontFacePosition;
	double gKE;
	int DetNum;
	bool useCollectionEfficiencyModel;

public:
};

TH1D* getSiO2Hist();

#endif
