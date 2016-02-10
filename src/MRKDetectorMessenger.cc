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
// $Id: MRKDetectorMessenger.cc,v 1.12 2008/09/22 16:41:20 maire Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "MRKDetectorMessenger.hh"

#include "MRKDetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "globals.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MRKDetectorMessenger::MRKDetectorMessenger(MRKDetectorConstruction* myDet)
:myDetector(myDet)
{

  detDir = new G4UIdirectory("/MRK/det/");
  detDir->SetGuidance("Detector Construction");

  fileDir = new G4UIdirectory("/MRK/file/");
  fileDir->SetGuidance("file control");

  modelCmd = new G4UIcmdWithAString("/MRK/det/setmodel",this);
  modelCmd->SetGuidance("Sets model used in detector construction");
  modelCmd->SetParameterName("filename",false);
  modelCmd->AvailableForStates(G4State_PreInit);

  useSBDDetectorCmd = new G4UIcmdWithABool("/MRK/det/useSBDDetector",this);
  useSBDDetectorCmd->SetGuidance("Sets true/false to use SBD detectors in RDK2 geometries.");
  useSBDDetectorCmd->SetParameterName("useSBDDetector",false);
  useSBDDetectorCmd->AvailableForStates(G4State_PreInit);

  useBGODetectorsCmd = new G4UIcmdWithABool("/MRK/det/useBGODetectors",this);
  useBGODetectorsCmd->SetGuidance("Sets true/false to use BGO detectors in RDK2 geometries.");
  useBGODetectorsCmd->SetParameterName("useBGODetectors",false);
  useBGODetectorsCmd->AvailableForStates(G4State_PreInit);

  useBAPDDetectorsCmd = new G4UIcmdWithABool("/MRK/det/useBAPDDetectors",this);
  useBAPDDetectorsCmd->SetGuidance("Sets true/false to use BAPD detectors in RDK2 geometries.");
  useBAPDDetectorsCmd->SetParameterName("useBAPDDetectors",false);
  useBAPDDetectorsCmd->AvailableForStates(G4State_PreInit);


  useBGOPositionalLightOutCmd = new G4UIcmdWithABool("/MRK/det/useBGOPositionalLightOut",this);
  useBGOPositionalLightOutCmd->SetGuidance("Sets true/false to use BGO Positional Light output");
  useBGOPositionalLightOutCmd->SetParameterName("usePositionalBGOLightOutput",false);
  useBGOPositionalLightOutCmd->AvailableForStates(G4State_PreInit);

  useBGONormalizedLightOutCmd = new G4UIcmdWithABool("/MRK/det/useBGONormalizedLightOutCmd",this);
  useBGONormalizedLightOutCmd->SetGuidance("Sets true/false to set the use of BGO Positional Light Output's integral to be normalized across the bar");
  useBGONormalizedLightOutCmd->SetParameterName("useNormalizedBGOLightOutput",false);
  useBGONormalizedLightOutCmd->AvailableForStates(G4State_PreInit);


  useBAPDCollectionEfficiencyModelCmd = new G4UIcmdWithABool("/MRK/det/useBAPDCollectionEfficiencyModel",this);
  useBAPDCollectionEfficiencyModelCmd->SetGuidance("Sets true/false to use BAPD Collection Efficiency Model");
  useBAPDCollectionEfficiencyModelCmd->SetParameterName("useBAPDCollectionEfficiencyModel",false);
  useBAPDCollectionEfficiencyModelCmd->AvailableForStates(G4State_PreInit);

  useBGOCalibrationWeakSourceCmd = new G4UIcmdWithABool("/MRK/det/useBGOCalibrationWeakSource",this);
  useBGOCalibrationWeakSourceCmd->SetGuidance("Sets true/false to use the BGO Calibration setup with the weak source");
  useBGOCalibrationWeakSourceCmd->SetParameterName("useBGOCalibrationWeakSource",false);
  useBGOCalibrationWeakSourceCmd->AvailableForStates(G4State_PreInit);

  useDoubleWeakSourceCmd = new G4UIcmdWithABool("/MRK/det/useDoubleWeakSource",this);
  useDoubleWeakSourceCmd->SetGuidance("Sets true/false to use two calibration weak sources (two plastic cylinders) in model");
  useDoubleWeakSourceCmd->SetParameterName("useDoubleWeakSource",false);
  useDoubleWeakSourceCmd->AvailableForStates(G4State_PreInit);


  useBGOWrappingsCmd = new G4UIcmdWithABool("/MRK/det/useBGOWrappings",this);
  useBGOWrappingsCmd->SetGuidance("Sets true/false to use Aluminized Mylar and other wrappings around BGOs");
  useBGOWrappingsCmd->SetParameterName("useBGOWrapping",false);
  useBGOWrappingsCmd->AvailableForStates(G4State_PreInit);

  useAluminumPlugCmd = new G4UIcmdWithABool("/MRK/det/useAluminumPlug",this);
  useAluminumPlugCmd->SetGuidance("Sets true/false to use Aluminized Plug to simulate matter that may reflect electrons");
  useAluminumPlugCmd->SetParameterName("useAluminumPlug",false);
  useAluminumPlugCmd->AvailableForStates(G4State_PreInit);

  use1_5mmSBDCmd = new G4UIcmdWithABool("/MRK/det/use15mmSBD",this);
  use1_5mmSBDCmd->SetGuidance("Sets true/false to use 1.5mm SBD or instead the 1.0mm SBD");
  use1_5mmSBDCmd->SetParameterName("use15mmSBD",false);
  use1_5mmSBDCmd->AvailableForStates(G4State_PreInit);

  setAlLengthContractionFactorCmd = new G4UIcmdWithADouble("/MRK/det/setAlLengthContractionFactor",this);
  setAlLengthContractionFactorCmd->SetGuidance("Sets length thermal contraction factor for Al");
  setAlLengthContractionFactorCmd->SetParameterName("AlLengthContractionFactor",false);
  setAlLengthContractionFactorCmd->AvailableForStates(G4State_PreInit);

  setBGOLengthContractionFactorCmd = new G4UIcmdWithADouble("/MRK/det/setBGOLengthContractionFactor",this);
  setBGOLengthContractionFactorCmd->SetGuidance("Sets length thermal contraction factor for BGO");
  setBGOLengthContractionFactorCmd->SetParameterName("BGOLengthContractionFactor",false);
  setBGOLengthContractionFactorCmd->AvailableForStates(G4State_PreInit);

  numBGOsUsedCmd = new G4UIcmdWithAnInteger("/MRK/det/numBGOsUsed",this);
  numBGOsUsedCmd->SetGuidance("Sets number of BGOs to create in detector construction");
  numBGOsUsedCmd->SetParameterName("numBGOsUsed",false);
  numBGOsUsedCmd->AvailableForStates(G4State_PreInit);

  setWeakSourceOffsetCmd = new G4UIcmdWith3VectorAndUnit("/MRK/det/setWeakSourceOffset",this);
  setWeakSourceOffsetCmd->SetGuidance("Sets offset for weak source (already centered on BGO)");
  setWeakSourceOffsetCmd->SetParameterName("weakSourceOffsetX","weakSourceOffsetY","weakSourceOffsetZ",false);
  setWeakSourceOffsetCmd->AvailableForStates(G4State_PreInit);

  setSBDDetectorBendOffsetCmd = new G4UIcmdWith3VectorAndUnit("/MRK/det/setSBDDetectorBendOffset",this);
  setSBDDetectorBendOffsetCmd->SetGuidance("Sets offset for SBD Detector (along bend axis)");
  setSBDDetectorBendOffsetCmd->SetParameterName("SBDDetectorBendOffsetX","SBDDetectorBendOffsetY","SBDDetectorBendOffsetZ",false);
  setSBDDetectorBendOffsetCmd->AvailableForStates(G4State_PreInit);

  SBDGoldLayerOffCmd = new G4UIcmdWithABool("/MRK/det/SBDGoldLayerOff",this);
  SBDGoldLayerOffCmd->SetGuidance("Sets true/false to turn the gold layer of the SBD off");
  SBDGoldLayerOffCmd->SetParameterName("SBDGoldLayerOff",false);
  SBDGoldLayerOffCmd->AvailableForStates(G4State_PreInit);

  SBDCanOffCmd = new G4UIcmdWithABool("/MRK/det/SBDCanOff",this);
  SBDCanOffCmd->SetGuidance("Sets true/false to turn the can and other SBD components off");
  SBDCanOffCmd->SetParameterName("SBDCanOff",false);
  SBDCanOffCmd->AvailableForStates(G4State_PreInit);

  setSiDeadLayerLengthCmd= new G4UIcmdWithADoubleAndUnit("/MRK/det/setSiDeadLayerLengthCmd",this);
  setSiDeadLayerLengthCmd->SetGuidance("Sets length Si dead layer on SBD");
  setSiDeadLayerLengthCmd->SetParameterName("SiDeadLayerLength",false);
  setSiDeadLayerLengthCmd->AvailableForStates(G4State_PreInit);

  setGammaDetOffsetCmd = new G4UIcmdWith3VectorAndUnit("/MRK/det/setGammaDetOffset",this);
  setGammaDetOffsetCmd->SetGuidance("Sets offset for gamma detectors and electrostatic mirror (need to set field manually)");
  setGammaDetOffsetCmd->SetParameterName("gammaDetOffsetX","gammaDetOffsetY","gammaDetOffsetZ",false);
  setGammaDetOffsetCmd->AvailableForStates(G4State_PreInit);




}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MRKDetectorMessenger::~MRKDetectorMessenger()
{
    delete detDir;
    delete fileDir;
    delete modelCmd;
    delete useBAPDDetectorsCmd;
    delete useBGODetectorsCmd;
    delete useSBDDetectorCmd;
    delete useDoubleWeakSourceCmd;
    delete useBGONormalizedLightOutCmd;

    delete useBGOPositionalLightOutCmd;

    delete useBAPDCollectionEfficiencyModelCmd;
    delete useBGOCalibrationWeakSourceCmd;
    delete useBGOWrappingsCmd;

    delete numBGOsUsedCmd;
    delete setWeakSourceOffsetCmd;
    delete setGammaDetOffsetCmd;



    delete useAluminumPlugCmd;
    delete setSBDDetectorBendOffsetCmd;
    delete use1_5mmSBDCmd;
    delete setAlLengthContractionFactorCmd;
    delete setBGOLengthContractionFactorCmd;
    delete SBDGoldLayerOffCmd;
    delete SBDCanOffCmd;

    delete setSiDeadLayerLengthCmd;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MRKDetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{


    if( command ==  modelCmd)
        { myDetector->setExperimentModel(newValue);}

    else if( command ==  useSBDDetectorCmd)
        { myDetector->setUseSBDDetector(useSBDDetectorCmd->GetNewBoolValue(newValue));}

    else if( command ==  useBGODetectorsCmd)
        myDetector->setUseBGODetectors(useBGODetectorsCmd->GetNewBoolValue(newValue));

    else if( command ==  useBAPDDetectorsCmd)
        myDetector->setUseBAPDDetectors(useBAPDDetectorsCmd->GetNewBoolValue(newValue));



    else if( command ==  useBGOPositionalLightOutCmd)
        myDetector->setUsePositionalBGOLightOutput(useBGOPositionalLightOutCmd->GetNewBoolValue(newValue));

    else if( command ==  useBGONormalizedLightOutCmd)
        myDetector->setUseNormalizedBGOLightOutput(useBGONormalizedLightOutCmd->GetNewBoolValue(newValue));

    else if( command ==  useBAPDCollectionEfficiencyModelCmd)
        myDetector->setUseBAPDCollectionEfficiencyModel(useBAPDCollectionEfficiencyModelCmd->GetNewBoolValue(newValue));

    else if( command ==  useBGOCalibrationWeakSourceCmd)
        myDetector->setUseBGOCalibrationWeakSource(useBGOCalibrationWeakSourceCmd->GetNewBoolValue(newValue));
    else if( command ==  useDoubleWeakSourceCmd)
        myDetector->setUseDoubleWeakSource(useDoubleWeakSourceCmd->GetNewBoolValue(newValue));

    else if( command == useBGOWrappingsCmd)
        myDetector->setUseBGOWrappings(useBGOWrappingsCmd->GetNewBoolValue(newValue));

    else if( command == useAluminumPlugCmd)
        myDetector->setUseAluminumPlug(useAluminumPlugCmd->GetNewBoolValue(newValue));
    else if( command == use1_5mmSBDCmd)
        myDetector->setUse1_5mmSBD(use1_5mmSBDCmd->GetNewBoolValue(newValue));

    else if( command ==  numBGOsUsedCmd)
        myDetector->setNumBGOsUsed(numBGOsUsedCmd->GetNewIntValue(newValue));
    else if( command == setWeakSourceOffsetCmd)
        myDetector->setWeakSourceOffset(setWeakSourceOffsetCmd->GetNew3VectorValue(newValue));
    else if( command == setGammaDetOffsetCmd)
        myDetector->setGammaDetOffset(setGammaDetOffsetCmd->GetNew3VectorValue(newValue));
    else if( command == setSBDDetectorBendOffsetCmd)
        myDetector->setSBDDetectorBendOffset(setSBDDetectorBendOffsetCmd->GetNew3VectorValue(newValue));
    else if( command ==  setAlLengthContractionFactorCmd)
        myDetector->setAlLengthContractionFactor(setAlLengthContractionFactorCmd->GetNewDoubleValue(newValue));
    else if( command ==  setBGOLengthContractionFactorCmd)
        myDetector->setBGOLengthContractionFactor(setBGOLengthContractionFactorCmd->GetNewDoubleValue(newValue));
    else if( command == SBDGoldLayerOffCmd)
        myDetector->setSBDGoldLayerOff(SBDGoldLayerOffCmd->GetNewBoolValue(newValue));
    else if( command == setSiDeadLayerLengthCmd)
        myDetector->setSiDeadLayerLength(setSiDeadLayerLengthCmd->GetNewDoubleValue(newValue));
    else if( command == SBDGoldLayerOffCmd)
        myDetector->setSBDGoldLayerOff(SBDGoldLayerOffCmd->GetNewBoolValue(newValue));



}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
