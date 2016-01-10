#ifndef GeneratorMessenger_h
#define GeneratorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class MRKPrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIcmdWith3VectorAndUnit;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class GeneratorMessenger: public G4UImessenger
{
  public:
    GeneratorMessenger(MRKPrimaryGeneratorAction*);
   ~GeneratorMessenger();

    void SetNewValue(G4UIcommand*, G4String);



  private:

    MRKPrimaryGeneratorAction* myGenerator;

    G4UIdirectory*             fileDir;
    G4UIcmdWithAString*        genInpCmd;
    G4UIcmdWithAString*        fluxMapCmd;
    G4UIcmdWithABool* eventByEventVerbose;
    G4UIcmdWithAnInteger* genModeCmd;
    G4UIcmdWithAnInteger* eventSetNumberCmd;
    G4UIcmdWithAnInteger* eventStartCmd;
    G4UIcmdWithAnInteger* printRandomNumbersCmd;
    G4UIcmdWithAnInteger* setNumberOfEventsToMakeCmd;
    G4UIcmdWith3VectorAndUnit* setBeamOffsetCmd;

    G4UIcmdWithABool* useManualDirCmd;
    G4UIcmdWithABool* useManualPosCmd;
    G4UIcmdWithABool* useManualEnergyCmd;


};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
