#ifndef MRKSteppingAction_h
#define MRKSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

class MRKSteppingAction: public G4UserSteppingAction
{
public:
	MRKSteppingAction();
	~MRKSteppingAction()
	{
	}
	;

	void UserSteppingAction(const G4Step* aStep);
protected:
	G4ParticleDefinition* protonParDef;
	G4ParticleDefinition* electronParDef;
	G4ParticleDefinition* photonParDef;
	G4double emaxTime, pmaxTime;
};

#endif
