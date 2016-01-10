#ifndef RunActionMessenger_h
#define RunActionMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"
#include <string>
using namespace std;
using std::string;

class MRKRunAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithABool;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class RunActionMessenger: public G4UImessenger
{
  public:
    RunActionMessenger(MRKRunAction*);
   ~RunActionMessenger();

    void SetNewValue(G4UIcommand*, G4String);

  private:
    MRKRunAction* myRunAction;


    G4UIcmdWithAString* rootoutCmd;
    G4UIcmdWithABool* useFluxTreeCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


