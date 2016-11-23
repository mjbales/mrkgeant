class MRKSteppingVerbose;

#ifndef MRKSteppingVerbose_h
#define MRKSteppingVerbose_h 1

#include "G4SteppingVerbose.hh"

/// Allows for control of how much should be outputted to stdout during the simulation.
class MRKSteppingVerbose: public G4SteppingVerbose
{
public:

	MRKSteppingVerbose();
	~MRKSteppingVerbose();

	void StepInfo();
	void TrackingStarted();

};

#endif
