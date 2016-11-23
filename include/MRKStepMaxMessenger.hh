#ifndef StepMaxMessenger_h
#define StepMaxMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class MRKStepMax;
class G4UIcmdWithADoubleAndUnit;

/// Defines macro commands related the the MRKStepMax process
class MRKStepMaxMessenger: public G4UImessenger
{
public:
	MRKStepMaxMessenger(MRKStepMax*);
	~MRKStepMaxMessenger();

	void SetNewValue(G4UIcommand*, G4String);

private:
	MRKStepMax* stepMax;
	G4UIcmdWithADoubleAndUnit* StepMaxCmd;
};

#endif
