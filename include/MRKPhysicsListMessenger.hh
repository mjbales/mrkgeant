#ifndef PhysicsListMessenger_h
#define PhysicsListMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class MRKPhysicsList;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;

//Defines macro commands for the physics list
class MRKPhysicsListMessenger: public G4UImessenger
{
public:

	MRKPhysicsListMessenger(MRKPhysicsList*);
	~MRKPhysicsListMessenger();

	void SetNewValue(G4UIcommand*, G4String);

private:

	MRKPhysicsList* pPhysicsList;

	G4UIdirectory* physDir;
	G4UIcmdWithADoubleAndUnit* gammaCutCmd;
	G4UIcmdWithADoubleAndUnit* electCutCmd;
	G4UIcmdWithADoubleAndUnit* protoCutCmd;
	G4UIcmdWithADoubleAndUnit* allCutCmd;
	G4UIcmdWithADoubleAndUnit* rCmd;
	G4UIcmdWithAString* pListCmd;

};

#endif

