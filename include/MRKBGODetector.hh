#ifndef MRKBGODetector_h
#define MRKBGODetector_h 1

#include "G4VSensitiveDetector.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#include "TRandom3.h"

#include "MRKText.hh"
#include "MRKHit.hh"
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class MRKBGODetector: public G4VSensitiveDetector
{

public:

	MRKBGODetector(G4String name, int inpDetNum, G4ThreeVector inpGammaDetOffset, bool inpusePositionalLightOutput = true, bool inpuseNormalizedlightOutput = true);
	~MRKBGODetector();

	void Initialize(G4HCofThisEvent* HCE);
	G4bool ProcessHits(G4Step*aStep, G4TouchableHistory* ROhist);
	void EndOfEvent(G4HCofThisEvent *HCE);
	void clear();
	void DrawAll();
	void PrintAll();

private:
	MRKHitsCollection* mrkCollection;
	G4ParticleDefinition* protonParDef;
	G4ParticleDefinition* electronParDef;
	G4ParticleDefinition* photonParDef;
	double getBGOPosGain(G4double z);
	double kk0[12];
	double kk1[12];
	double kk2[12];
	double kk3[12];
	double kk4[12];
	double kk5[12];
	double kk6[12];
	double norm_func[12];
	bool usePositionalLightOutput, useNormalizedLightOutput;
	double bgoCenterZPosition;
	int HCID;
	int DetNum;

};

#endif
