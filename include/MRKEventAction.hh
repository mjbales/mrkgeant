#ifndef MRKEventAction_h
#define MRKEventAction_h 1

#include "G4UserEventAction.hh"

class G4Event;

class MRKEventAction: public G4UserEventAction
{
public:
	MRKEventAction();
	~MRKEventAction();

public:
	void BeginOfEventAction(const G4Event*);
	void EndOfEventAction(const G4Event*);
};

#endif

