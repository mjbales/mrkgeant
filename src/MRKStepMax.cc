#include "MRKStepMax.hh"
#include "MRKStepMaxMessenger.hh"

MRKStepMax::MRKStepMax(const G4String& processName) :
	G4VDiscreteProcess(processName), MaxChargedStep(DBL_MAX)
{
	pMess = new MRKStepMaxMessenger(this);
}

MRKStepMax::~MRKStepMax()
{
	delete pMess;
}

G4bool MRKStepMax::IsApplicable(const G4ParticleDefinition& particle)
{
	return (particle.GetPDGCharge() != 0. && !particle.IsShortLived());
}

void MRKStepMax::SetMaxStep(G4double step)
{
	MaxChargedStep = step;
}

G4double MRKStepMax::PostStepGetPhysicalInteractionLength(const G4Track&, G4double, G4ForceCondition* condition)
{
	// condition is set to "Not Forced"
	*condition = NotForced;

	return MaxChargedStep;
}

G4VParticleChange* MRKStepMax::PostStepDoIt(const G4Track& aTrack, const G4Step&)
{
	// do nothing
	aParticleChange.Initialize(aTrack);
	return &aParticleChange;
}

