#ifndef MRKEventAction_h
#define MRKEventAction_h 1

#include "G4UserEventAction.hh"

class G4Event;

///Defines user event actions which activate at the beginning or end of events.
///Currently only used for printing every 100 events
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

