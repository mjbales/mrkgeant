#ifndef MRKPHYSICSLIST_H_INCLUDED
#define MRKPHYSICSLIST_H_INCLUDED 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

class MRKDetectorConstruction;
class MRKPhysicsListMessenger;
class G4VPhysicsConstructor;
class MRKStepMax;

class MRKPhysicsList: public G4VModularPhysicsList
{
public:
	MRKPhysicsList(MRKDetectorConstruction*);
	~MRKPhysicsList();

	void ConstructProcess();
	void ConstructParticle();
	void AddPhysicsList(const G4String& name);

	void AddDecay();
	void AddStepMax();
    MRKStepMax* GetStepMaxProcess() {return stepMaxProcess;};

	void SetCuts();
	void SetCutForGamma(G4double);
	void SetCutForProton(G4double);
	void SetCutForElectron(G4double);
	void SetCutForPositron(G4double);

private:
	G4double cutForGamma;
	G4double cutForProton;
	G4double cutForElectron;
	G4double cutForPositron;
	G4double currentDefaultCut;

	G4VPhysicsConstructor* emPhysicsList;
	G4VPhysicsConstructor* decPhysicsList;
	G4VPhysicsConstructor* raddecayList;
	G4String emName;

	MRKStepMax* stepMaxProcess;

	MRKDetectorConstruction* pDet;
	MRKPhysicsListMessenger* pMessenger;
};

#endif

