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
// $Id: MRKDetectorConstruction.hh,v 1.10 2008/09/22 16:41:20 maire Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef MRKDetectorConstruction_h
#define MRKDetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "MRKGlobalField.hh"
#include "MRKMaterials.hh"
#include "mattphys.hh"

#include "constants.hh"

#include "G4Box.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include <vector>

class G4Tubs;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4VPVParameterisation;
class G4UserLimits;
class MRKDetectorMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

enum ExperimentModel{MODEL_RDK2,MODEL_RDK2BGOSONLY,MODEL_LIFETIMETEST};




class MRKDetectorConstruction : public G4VUserDetectorConstruction
{
  public:

     MRKDetectorConstruction();
    ~MRKDetectorConstruction();

  public:

     G4VPhysicalVolume* Construct();



    const G4VPhysicalVolume* GetWorld()      {return physiWorld;};
    inline ExperimentModel getExperimentModel(){ return experimentModel;}
    inline void setExperimentModel(ExperimentModel inpModel){ G4cout << "Setting experimental model for detector construction." << G4endl;experimentModel=inpModel;}
    void setExperimentModel(string modelName);
    inline void setUseSBDDetector(bool inp){useSBDDetector=inp;}
    inline void setUseBGODetectors(bool inp){useBGODetectors=inp;}
    inline void setUseBAPDDetectors(bool inp){useBAPDDetectors=inp;}
    inline bool getUseSBDDetector(){return useSBDDetector;}
    inline bool getUseBAPDDetectors(){return useBAPDDetectors;}
    inline bool getUseBGODetectors(){return useBGODetectors;}
    inline void setUsePositionalBGOLightOutput(bool inp){usePositionalBGOLightOutput=inp;}
    inline void setUseBAPDCollectionEfficiencyModel(bool inp){useBAPDCollectionEfficiencyModel=inp;}
    inline void setUseBGOCalibrationWeakSource(bool inp){useBGOCalibrationWeakSource=inp;}
    inline bool getUseBGOCalibrationWeakSource(){return useBGOCalibrationWeakSource;}
    inline void setUseBGOWrappings(bool inp){useBGOWrappings=inp;}
    inline void setNumBGOsUsed(int inp){numBGOsUsed=inp;}
    inline int getNumBGOsUsed(){return numBGOsUsed;}
    inline void setUseNormalizedBGOLightOutput(bool inp){useNormalizedBGOLightOutput=inp;}
    inline void setWeakSourceOffset(G4ThreeVector inp){WeakSourceOffset=inp;}

    inline void setUseDoubleWeakSource(bool inp){useDoubleWeakSource=inp;}
    inline void setUseAluminumPlug(bool inp){useAluminumPlug=inp;}
    inline void setSBDDetectorBendOffset(G4ThreeVector inp){sbdDetectorBendOffset=inp;}
    inline void setUse1_5mmSBD(bool inp){use1_5mmSBD=inp;}
    inline void setAlLengthContractionFactor(double inp){AlLengthContractionFactor=inp;}
    inline void setBGOLengthContractionFactor(double inp){BGOLengthContractionFactor=inp;}
    inline void setSBDGoldLayerOff(bool inp){SBDGoldLayerOff=inp;}
    inline void setSiDeadLayerLength(double inp){SiDeadLayerLength=inp;}
    inline void setGammaDetOffset(G4ThreeVector inp){gammaDetOffset=inp;}


  private:

    void defineDistancesAndRotationsRDK2();

    //Volume construction methods
    void constructRDK2World();
    G4VPhysicalVolume* ConstructRDK2();
    G4VPhysicalVolume* ConstructRDK2BGOsOnly();
    G4VPhysicalVolume* ConstructLifetimeTest();


    //Straight Axis Cylinders
    G4LogicalVolume* constructRDK2StraightAndBentBore();
    G4LogicalVolume* constructRDK2HighVoltageTube();
    G4LogicalVolume* constructRDK2FrontNylonSpacer();
    G4LogicalVolume* constructRDK2LargeNylonSpacer();
    G4LogicalVolume* constructRDK2MainGroundCylinder();
    G4LogicalVolume* constructRDK2FrontMirrorGround();
    G4LogicalVolume* constructRDK2OuterMirrorGround();
    G4LogicalVolume* constructRDK2DownstreamNylonSpider();
    G4LogicalVolume* constructRDK2G10APDHolder();
    G4LogicalVolume* constructRDK2ThrowingStar1();
    G4LogicalVolume* constructRDK2AluminumHeatShield();
    G4LogicalVolume* constructRDK2ThrowingStar2();
    G4LogicalVolume* constructRDK2NylonPusherAssembly();
    G4LogicalVolume* constructRDK2UpstreamAluminumTape1();
    G4LogicalVolume* constructRDK2UpstreamAluminumTape2();
    G4LogicalVolume* constructRDK2UpstreamNylonSpider();
    G4LogicalVolume* constructRDK2UpstreamAluminumTape3();
    G4LogicalVolume* constructRDK2BAPDHolder();
    G4LogicalVolume* constructRDK2FrontCopperOnAperture();
    G4LogicalVolume* constructRDK2LeadAperture();
    G4LogicalVolume* constructRDK2InnerCopperOnAperture();
    G4LogicalVolume* constructRDK2ThreadedUpstreamSupportRod1();
    G4LogicalVolume* constructRDK2ThreadedUpstreamSupportRod2();
    G4LogicalVolume* constructRDK2ThreadedUpstreamSupportRod3();
    G4LogicalVolume* constructRDK2HexSupportRod1();
    G4LogicalVolume* constructRDK2HexSupportRod2();
    G4LogicalVolume* constructRDK2HexSupportRod3();
    G4LogicalVolume* constructRDK2ThreadedDownstreamSupportRod1();
    G4LogicalVolume* constructRDK2ThreadedDownstreamSupportRod2();
    G4LogicalVolume* constructRDK2ThreadedDownstreamSupportRod3();
    G4LogicalVolume* constructRDK2LongSteelAdjustmentRod1();
    G4LogicalVolume* constructRDK2LongSteelAdjustmentRod2();
    G4LogicalVolume* constructRDK2LongSteelAdjustmentRod3();
    G4LogicalVolume* constructRDK2ShortSteelAdjustmentRod1();
    G4LogicalVolume* constructRDK2ShortSteelAdjustmentRod2();
    G4LogicalVolume* constructRDK2ShortSteelAdjustmentRod3();
    G4LogicalVolume* constructRDK2MagnetCoil1();
    G4LogicalVolume* constructRDK2MagnetCoil2();
    G4LogicalVolume* constructRDK2MagnetCoil3();
    G4LogicalVolume* constructRDK2MagnetCoil4();
    G4LogicalVolume* constructRDK2MagnetCoil5();
    G4LogicalVolume* constructRDK2MagnetCoil6();
    G4LogicalVolume* constructRDK2MagnetCoil7();
    G4LogicalVolume* constructRDK2AluminumPlug();
    G4LogicalVolume* constructRDK2FarAluminumTube();
    G4LogicalVolume* constructRDK2DSCryostatFlangeA();
    G4LogicalVolume* constructRDK2DSCryostatFlangeB();
    G4LogicalVolume* constructRDK2CoilTray1PartH();
    G4LogicalVolume* constructRDK2CoilTray1PartI();
    G4LogicalVolume* constructRDK2CoilTray1PartK();
    G4LogicalVolume* constructRDK2CoilTray1PartL();
    G4LogicalVolume* constructRDK2CoilTray1PartM();
    G4LogicalVolume* constructRDK2BeamCenter();
    G4LogicalVolume* constructRDK2ReentrantTube();
    G4LogicalVolume* constructRDK2ReentrantTubeWeld();
    G4LogicalVolume* constructRDK2SourceStick();
    G4LogicalVolume* constructRDK2WeakSource();
    G4LogicalVolume* constructRDK2CopperGroundingStrapPart1();
    G4LogicalVolume* constructRDK2CopperGroundingStrapPart2();
    G4LogicalVolume* constructRDK2CopperGroundingStrapPart3();
    G4LogicalVolume* constructRDK2CopperGroundingStrapPart4();
    G4LogicalVolume* constructRDK2CopperGroundingStrapPart5();
    G4LogicalVolume* constructRDK2CopperGroundingStrapPart6();
    G4LogicalVolume* constructRDK2CopperGroundingStrapPart7();
    G4LogicalVolume* constructRDK2CopperGroundingStrapPart8();
    G4LogicalVolume* constructRDK2CopperGroundingStrapPart9();
    G4LogicalVolume* constructRDK2CopperGroundingStrapPart10();
    G4LogicalVolume* constructRDK2CopperGroundingStrapPart11();
    G4LogicalVolume* constructRDK2CopperGroundingStrapPart12();

    //Cylinders on bent axis
    G4LogicalVolume* constructRDK2LargeBentBore();
    G4LogicalVolume* constructRDK2SpacerRing();
    G4LogicalVolume* constructRDK2MagnetCoil9();
    G4LogicalVolume* constructRDK2MagnetCoil10();
    G4LogicalVolume* constructRDK2MagnetCoil11();
    G4LogicalVolume* constructRDK2SBD1mm5Silicon();
    G4LogicalVolume* constructRDK2SBD1mm5SiliconHolder();
    G4LogicalVolume* constructRDK2SBDDeadLayer();
    G4LogicalVolume* constructRDK2SBD1mm5GoldCoating();
    G4LogicalVolume* constructRDK2SBD1mm5AluminumCoating();
    G4LogicalVolume* constructRDK2SBD1mm5PlasticLining();
    G4LogicalVolume* constructRDK2SBD1mm5BackBrassRings();
    G4LogicalVolume* constructRDK2SBD1mm5InternalBrassDisc();
    G4LogicalVolume* constructRDK2SBD1mm5Spring();
    G4LogicalVolume* constructRDK2SBD1mm5HexScrew();
    G4LogicalVolume* constructRDK2SBD1mm5FrontCase();
    G4LogicalVolume* constructRDK2SBD1mm5MidCase();
    G4LogicalVolume* constructRDK2SBD1mm5BackCase();

    G4LogicalVolume* constructRDK2BeOTube();
    G4LogicalVolume* constructRDK2SBD1mm5StainlessSteelTube();
    G4LogicalVolume* constructRDK2CoilTray2PartA();
    G4LogicalVolume* constructRDK2CoilTray2PartB();
    G4LogicalVolume* constructRDK2CoilTray2PartC();
    G4LogicalVolume* constructRDK2CoilTray2PartD();
    G4LogicalVolume* constructRDK2CoilTray2PartE();
    G4LogicalVolume* constructRDK2CoilTray2PartF();
    G4LogicalVolume* constructRDK2OffAxisVacuum();



    //Cylinders on weld axis
    G4LogicalVolume* constructRDK2MagnetCoil8();
    G4LogicalVolume* constructRDK2CoilTray3PartN();
    G4LogicalVolume* constructRDK2CoilTray3PartO();
    G4LogicalVolume* constructRDK2CoilTray3PartP();
    G4LogicalVolume* constructRDK2CoilTray3PartQ();

    //Boxes
    G4LogicalVolume* constructRDK2ActiveAreaBareAPD1();
    G4LogicalVolume* constructRDK2ActiveAreaBareAPD2();
    G4LogicalVolume* constructRDK2ActiveAreaBareAPD3();
    G4LogicalVolume* constructRDK2SiBareAPD1();
    G4LogicalVolume* constructRDK2SiBareAPD2();
    G4LogicalVolume* constructRDK2SiBareAPD3();
    G4LogicalVolume* constructRDK2BAPDBacking1();
    G4LogicalVolume* constructRDK2BAPDBacking2();
    G4LogicalVolume* constructRDK2BAPDBacking3();
    G4LogicalVolume* constructRDK2G10BAPDBackPieces1();
    G4LogicalVolume* constructRDK2G10BAPDBackPieces2();
    G4LogicalVolume* constructRDK2G10BAPDBackPieces3();
    G4LogicalVolume* constructRDK2SiSmallAPD1();
    G4LogicalVolume* constructRDK2SiSmallAPD2();
    G4LogicalVolume* constructRDK2SiSmallAPD3();
    G4LogicalVolume* constructRDK2SiSmallAPD4();
    G4LogicalVolume* constructRDK2SiSmallAPD5();
    G4LogicalVolume* constructRDK2SiSmallAPD6();
    G4LogicalVolume* constructRDK2SiSmallAPD7();
    G4LogicalVolume* constructRDK2SiSmallAPD8();
    G4LogicalVolume* constructRDK2SiSmallAPD9();
    G4LogicalVolume* constructRDK2SiSmallAPD10();
    G4LogicalVolume* constructRDK2SiSmallAPD11();
    G4LogicalVolume* constructRDK2SiSmallAPD12();
    G4LogicalVolume* constructRDK2SmallAPDBacking1();
    G4LogicalVolume* constructRDK2SmallAPDBacking2();
    G4LogicalVolume* constructRDK2SmallAPDBacking3();
    G4LogicalVolume* constructRDK2SmallAPDBacking4();
    G4LogicalVolume* constructRDK2SmallAPDBacking5();
    G4LogicalVolume* constructRDK2SmallAPDBacking6();
    G4LogicalVolume* constructRDK2SmallAPDBacking7();
    G4LogicalVolume* constructRDK2SmallAPDBacking8();
    G4LogicalVolume* constructRDK2SmallAPDBacking9();
    G4LogicalVolume* constructRDK2SmallAPDBacking10();
    G4LogicalVolume* constructRDK2SmallAPDBacking11();
    G4LogicalVolume* constructRDK2SmallAPDBacking12();
    G4LogicalVolume* constructRDK2BGOCrystal1();
    G4LogicalVolume* constructRDK2BGOCrystal2();
    G4LogicalVolume* constructRDK2BGOCrystal3();
    G4LogicalVolume* constructRDK2BGOCrystal4();
    G4LogicalVolume* constructRDK2BGOCrystal5();
    G4LogicalVolume* constructRDK2BGOCrystal6();
    G4LogicalVolume* constructRDK2BGOCrystal7();
    G4LogicalVolume* constructRDK2BGOCrystal8();
    G4LogicalVolume* constructRDK2BGOCrystal9();
    G4LogicalVolume* constructRDK2BGOCrystal10();
    G4LogicalVolume* constructRDK2BGOCrystal11();
    G4LogicalVolume* constructRDK2BGOCrystal12();

    G4VSolid* makeHolesForBGOs(G4VSolid* baseSolid);
    G4VSolid* makeHolesForHexSupportRods(G4VSolid* baseSolid);
    G4VSolid* makeHolesForShortStainlessSupportRods(G4VSolid* baseSolid);
    G4VSolid* makeHolesForLongStainlessSupportRods(G4VSolid* baseSolid);
    G4VSolid* makeHolesForBGOAPDs(G4VSolid* baseSolid);
    G4VSolid* makeHolesForBAPDs(G4VSolid* baseSolid);
    void constructBGOWrappingsImproved();
    void constructSBDEpoxy();


    //Lifetime only
    G4LogicalVolume* constructToyModelElectrode();
    G4LogicalVolume* constructRDK2SBD300mmSquared1mm5Silicon();
    G4LogicalVolume* constructRDK2SBD300mmSquared1mm5SiliconHolder();
    G4LogicalVolume* constructRDK2SBD300mmSquared1mm5GoldCoating();
    G4LogicalVolume* constructRDK2SBD300mmSquared1mm5AluminumCoating();
    G4LogicalVolume* constructRDK2SBD300mmSquared1mm5PlasticLining();
    G4LogicalVolume* constructRDK2SBD300mmSquared1mm5BackBrassRings();
    G4LogicalVolume* constructRDK2SBD300mmSquared1mm5InternalBrassDisc();
    G4LogicalVolume* constructRDK2SBD300mmSquared1mm5Spring();
    G4LogicalVolume* constructRDK2SBD300mmSquared1mm5HexScrew();
    G4LogicalVolume* constructRDK2SBD300mmSquared1mm5FrontCase();
    G4LogicalVolume* constructRDK2SBD300mmSquared1mm5MidCase();
    G4LogicalVolume* constructRDK2SBD300mmSquared1mm5BackCase();
    void constructSBD300mmSquaredEpoxy();
    G4LogicalVolume* constructFakeSBDDetector();


    MRKMaterials* materials;

    G4Box*             solidWorld;    // pointer to the solid envelope
    G4LogicalVolume*   logicWorld;    // pointer to the logical envelope
    G4VPhysicalVolume* physiWorld;    // pointer to the physical envelope
    G4LogicalVolume* logicSBDDetectionVolume;
    G4LogicalVolume* logicBGODetectionVolumes[NUM_BGOS];
    G4LogicalVolume* logicBAPDDetectionVolumes[NUM_BAPDS];



    MRKGlobalField* theGlobalField;

    MRKDetectorMessenger* detectorMessenger;  // pointer to the Messenger

    G4double bendToMagnetZero;
    G4RotationMatrix* rdk2RotationMatrices[3];
    G4RotationMatrix* bapdRotationMatrices[3];
    G4RotationMatrix* bgoRotationMatrices[12];
    G4ThreeVector WeakSourceOffset;
    double BGOLengthContractionFactor;
    double AlLengthContractionFactor;
    double SiDeadLayerLength;
    G4ThreeVector gammaDetOffset;



    ExperimentModel experimentModel;



    bool useSBDDetector,useBGODetectors,useBAPDDetectors;
    bool useBAPDCollectionEfficiencyModel;
    bool useBGOCalibrationWeakSource;
    bool useDoubleWeakSource;
    bool useBGOWrappings;
    bool useAluminumPlug;
    bool use1_5mmSBD; //true =1.5 mm det    false = 1.0 mm det
    bool SBDGoldLayerOff;

    G4ThreeVector sbdDetectorBendOffset;
    G4ThreeVector sbdDetectorOffset;



    bool usePositionalBGOLightOutput;
    bool useNormalizedBGOLightOutput;

    int numBGOsUsed;


    G4UserLimits* BAPDStepLimit;

    G4VisAttributes* VisStainlessSteel304;
    G4VisAttributes* VisG4_Al;
    G4VisAttributes* VisG4_NYLON;
    G4VisAttributes* VisG4_StretchedTeflonTape;
    G4VisAttributes* VisG10;
    G4VisAttributes* VisG4_Cu;
    G4VisAttributes* VisG4_Pb;
    G4VisAttributes* VisG4_Galactic;
    G4VisAttributes* VisG4_Fe;
    G4VisAttributes* VisG4_POLYCARBONATE;
    G4VisAttributes* VisG4_Si;
    G4VisAttributes* VisG4_ALUMINUM_OXIDE;
    G4VisAttributes* VisG4_Au;
    G4VisAttributes* VisCartridgeBrass;
    G4VisAttributes* VisG4_BERYLLIUM_OXIDE;
    G4VisAttributes* VisG4_BGO;
    G4VisAttributes* VisMylar;
    G4VisAttributes* VisEpoxy;
    G4VisAttributes* VisRadioactive;


};
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
