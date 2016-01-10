//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
//
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef MRKFieldMessenger_h
#define MRKFieldMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class MRKGlobalField;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;
class G4UIcmdWith3VectorAndUnit;

class MRKFieldMessenger: public G4UImessenger
{
  public:
    MRKFieldMessenger(MRKGlobalField* );
   ~MRKFieldMessenger();

    void SetNewValue(G4UIcommand*, G4String);
    void SetNewValue(G4UIcommand*, G4int);

  private:

    MRKGlobalField*            fGlobalField;

    G4UIdirectory*             detDir;

    G4UIcmdWithAnInteger*      fStepperCMD;
    G4UIcmdWithADoubleAndUnit* fMinStepCMD;
    G4UIcmdWithADoubleAndUnit* fDeltaChordCMD;
    G4UIcmdWithADoubleAndUnit* fDeltaOneStepCMD;
    G4UIcmdWithADoubleAndUnit* fDeltaIntersectionCMD;
    G4UIcmdWithADoubleAndUnit* fEpsMinCMD;
    G4UIcmdWithADoubleAndUnit* fEpsMaxCMD;
    G4UIcmdWithADoubleAndUnit* fLargestAcceptableStepCMD;
    G4UIcmdWithoutParameter*   fUpdateCMD;

    G4UIcmdWithAnInteger* setCurrentFieldSettingsToModifyCmd;
    G4UIcmdWithAString* setFieldPathCmd;
    G4UIcmdWithAString* setFieldHistNameCmd;
    G4UIcmdWith3VectorAndUnit* setFieldOffsetCmd;
    G4UIcmdWith3VectorAndUnit* setFieldExtentsCmd;
    G4UIcmdWithADouble* setFieldScalingValueCmd;
    G4UIcmdWithAnInteger* setFieldSpaceDimCmd;
    G4UIcmdWithAnInteger* setFieldFieldDimCmd;
    G4UIcmdWithADouble* setFieldAngleXCmd;
    G4UIcmdWithADouble* setFieldAngleYCmd;
    G4UIcmdWithADouble* setFieldAngleZCmd;
    G4UIcmdWithABool* setFieldIsMagneticCmd;
    G4UIcmdWithABool* setFieldSymmetryXCmd;
    G4UIcmdWithABool* setFieldSymmetryYCmd;
    G4UIcmdWithABool* setFieldSymmetryZCmd;
    G4UIcmdWithAString* addNamedFieldsCmd;
    G4UIcmdWithABool* setUseCubicInterpolationCmd;

};

#endif

