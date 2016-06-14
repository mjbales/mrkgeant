#ifndef SRC_MRKRDKIIGEOMETRY_HH_
#define SRC_MRKRDKIIGEOMETRY_HH_

#include <vector>

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4UserLimits.hh"

#include "MRKMaterials.hh"
#include "MRKGeometry.hh"
#include "MRKMacroMessenger.hh"


class MRKRDKIIGeometry : public MRKGeometry
{
public:
	MRKRDKIIGeometry(MRKMacroMessenger* inpMacroMessenger);
	~MRKRDKIIGeometry();

	G4VPhysicalVolume* Construct();

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
	inline void setSBDCanOff(bool inp){SBDCanOff=inp;}
private:
	void defineMacroCommands(MRKMacroMessenger* inpMacroMessenger);
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

	G4Box*             solidWorld;    // pointer to the solid envelope
	G4LogicalVolume*   logicWorld;    // pointer to the logical envelope
	G4VPhysicalVolume* physiWorld;    // pointer to the physical envelope
	G4LogicalVolume* logicSBDDetectionVolume;
	G4LogicalVolume* logicBGODetectionVolumes[NUM_BGOS];
	G4LogicalVolume* logicBAPDDetectionVolumes[NUM_BAPDS];

	G4double bendToMagnetZero;
	G4RotationMatrix* rdk2RotationMatrices[3];
	G4RotationMatrix* bapdRotationMatrices[3];
	G4RotationMatrix* bgoRotationMatrices[12];
	G4ThreeVector WeakSourceOffset;
	double BGOLengthContractionFactor;
	double AlLengthContractionFactor;
	double SiDeadLayerLength;
	G4ThreeVector gammaDetOffset;

	bool useSBDDetector,useBGODetectors,useBAPDDetectors;
	bool useBAPDCollectionEfficiencyModel;
	bool useBGOCalibrationWeakSource;
	bool useDoubleWeakSource;
	bool useBGOWrappings;
	bool useAluminumPlug;
	bool use1_5mmSBD; //true =1.5 mm det    false = 1.0 mm det
	bool SBDGoldLayerOff;
	bool SBDCanOff;

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

#endif /* SRC_MRKRDKIIGEOMETRY_HH_ */
