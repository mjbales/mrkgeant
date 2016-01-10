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
// $Id: MRKDetectorMessenger.hh,v 1.9 2008/09/22 16:41:20 maire Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef MRKDetectorMessenger_h
#define MRKDetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"
#include "MRKDetectorConstruction.hh"
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcmdWithABool;
class G4UIcmdWithADouble;
class G4UIcmdWith3VectorAndUnit;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class MRKDetectorMessenger: public G4UImessenger
{
  public:
    MRKDetectorMessenger(MRKDetectorConstruction*);
   ~MRKDetectorMessenger();

    void SetNewValue(G4UIcommand*, G4String);

  private:
    MRKDetectorConstruction* myDetector;

    G4UIdirectory*             F01detDir;

    G4UIdirectory*             fileDir;

    G4UIdirectory*             detDir;

    G4UIcmdWithADoubleAndUnit* StepMaxCmd;

    G4UIcmdWithAString*  modelCmd;
    G4UIcmdWithABool* useSBDDetectorCmd;
    G4UIcmdWithABool* useBGODetectorsCmd;
    G4UIcmdWithABool* useBAPDDetectorsCmd;
    G4UIcmdWithABool* useBGOPositionalLightOutCmd;
    G4UIcmdWithABool* useBGONormalizedLightOutCmd;
    G4UIcmdWithABool* useBAPDCollectionEfficiencyModelCmd;
    G4UIcmdWithABool* useBGOCalibrationWeakSourceCmd;
    G4UIcmdWithABool* useDoubleWeakSourceCmd;
    G4UIcmdWithABool* useBGOWrappingsCmd;
    G4UIcmdWithABool* useAluminumPlugCmd;
    G4UIcmdWithABool* use1_5mmSBDCmd;

    G4UIcmdWithAnInteger* numBGOsUsedCmd;
    G4UIcmdWith3VectorAndUnit* setWeakSourceOffsetCmd;
    G4UIcmdWith3VectorAndUnit* setSBDDetectorBendOffsetCmd;
    G4UIcmdWithADouble* setAlLengthContractionFactorCmd;
    G4UIcmdWithADouble* setBGOLengthContractionFactorCmd;
    G4UIcmdWithABool* SBDGoldLayerOffCmd;
    G4UIcmdWithADoubleAndUnit* setSiDeadLayerLengthCmd;
    G4UIcmdWith3VectorAndUnit* setGammaDetOffsetCmd;




};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

