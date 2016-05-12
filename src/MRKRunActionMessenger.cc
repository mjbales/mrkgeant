#include "MRKRunAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithABool.hh"
#include "globals.hh"
#include "../include/MRKRunActionMessenger.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MRKRunActionMessenger::MRKRunActionMessenger(MRKRunAction* myRunA)
:myRunAction(myRunA)
{


  rootoutCmd = new G4UIcmdWithAString("/MRK/file/rootout",this);
  rootoutCmd->SetGuidance("Choose output root file.");
  rootoutCmd->SetParameterName("choice",false);
  rootoutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  useFluxTreeCmd = new G4UIcmdWithABool("/MRK/det/useFluxTree",this);
  useFluxTreeCmd->SetGuidance("Sets true/false to create and use flux trees for detectors");
  useFluxTreeCmd->SetParameterName("useFluxTree",false);
  useFluxTreeCmd->AvailableForStates(G4State_PreInit);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MRKRunActionMessenger::~MRKRunActionMessenger()
{

  delete rootoutCmd;
  delete useFluxTreeCmd;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MRKRunActionMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  if( command == rootoutCmd )
   { myRunAction->setOutputFile(newValue);}
    else if( command == useFluxTreeCmd )
   { myRunAction->setUseFluxTree(useFluxTreeCmd->GetNewBoolValue(newValue));}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
