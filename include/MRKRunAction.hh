#ifndef MRKRunAction_h
#define MRKRunAction_h 1

#include <time.h>

#include "G4UserRunAction.hh"
#include "globals.hh"

#include "MRKPrimaryGeneratorAction.hh"
#include "MRKRunActionMessenger.hh"

class G4Run;

class MRKRunAction: public G4UserRunAction
{
public:
	MRKRunAction(MRKPrimaryGeneratorAction* generator);
	~MRKRunAction();
	void BeginOfRunAction(const G4Run*);
	void EndOfRunAction(const G4Run*);
	void setOutputFile(TString outputFileName);
	G4Run* GenerateRun();
	inline void setMacroFileName(TString inp){macroFileName=inp;}
	inline void setUseFluxTree(bool inp){useFluxTree=inp;}

private:
	MRKPrimaryGeneratorAction* theGenerator;
	TString outputFileName;
	MRKRunActionMessenger* rAMess;
	time_t startTime, endTime;
	TString macroFileName;
	bool useFluxTree;

};

#endif
