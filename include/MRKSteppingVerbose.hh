class MRKSteppingVerbose;

#ifndef MRKSteppingVerbose_h
#define MRKSteppingVerbose_h 1

#include "G4SteppingVerbose.hh"

class MRKSteppingVerbose: public G4SteppingVerbose
{
public:

	MRKSteppingVerbose();
	~MRKSteppingVerbose();

	void StepInfo();
	void TrackingStarted();

};

#endif
