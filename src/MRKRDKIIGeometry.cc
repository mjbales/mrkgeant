#include "MRKRDKIIGeometry.hh"

#include <iostream>
#include <functional>

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4SDManager.hh"
#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4RotationMatrix.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4VSDFilter.hh"
#include "G4SDParticleFilter.hh"
#include "G4SDParticleWithEnergyFilter.hh"
#include "G4PSFlatSurfaceCurrent.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4Colour.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

#include "MRKSurfaceBarrierDetector.hh"
#include "MRKBGODetector.hh"
#include "MRKBAPDDetector.hh"
#include "MRKConstants.hh"
#include "MRKText.hh"

using namespace std;

MRKRDKIIGeometry::MRKRDKIIGeometry(MRKMacroMessenger* inpMacroMessenger) :
	MRKGeometry(inpMacroMessenger)
{
	defineMacroCommands(inpMacroMessenger);
	bendToMagnetZero = 42.4 * CLHEP::cm;

	logicSBDDetectionVolume = nullptr;
	numBGOsUsed = 12;

	useBAPDCollectionEfficiencyModel = true;
	useBGOCalibrationWeakSource = false;
	useDoubleWeakSource = false;
	useNormalizedBGOLightOutput = true;
	usePositionalBGOLightOutput = true;
	useBGOWrappings = true;
	useAluminumPlug = true;
	use1_5mmSBD = true;
	SBDGoldLayerOff = false;
	SBDCanOff = false;

	useSBDDetector = true;
	useBGODetectors = true;
	useBAPDDetectors = true;
	BAPDStepLimit = nullptr;

	VisStainlessSteel304 = new G4VisAttributes(G4Colour(.6, .6, .8));
	VisG4_Al = new G4VisAttributes(G4Colour(.7, .7, .7));
	VisG4_NYLON = new G4VisAttributes(G4Colour(.95, 0.95, 0.95));
	VisG4_StretchedTeflonTape = new G4VisAttributes(G4Colour(.85, 0.95, 0.85));
	VisG10 = new G4VisAttributes(G4Colour(0.1, 0.5, 0.5));
	VisG4_Cu = new G4VisAttributes(G4Colour(.9, 0.7, 0));
	VisG4_Pb = new G4VisAttributes(G4Colour(0.1, 0.1, 0.1));
	VisG4_Galactic = new G4VisAttributes(G4VisAttributes::Invisible);
	VisG4_Fe = new G4VisAttributes(G4Colour(.2, .1, .1));
	VisG4_POLYCARBONATE = new G4VisAttributes(G4Colour(.9, .9, 1.0));
	VisG4_Si = new G4VisAttributes(G4Colour(1.0, 0, 0));
	VisG4_ALUMINUM_OXIDE = new G4VisAttributes(G4Colour(1, 1, 0.9));
	VisG4_Au = new G4VisAttributes(G4Colour(1, 1, 0));
	VisCartridgeBrass = new G4VisAttributes(G4Colour(.8, .8, 0.0));
	VisG4_BERYLLIUM_OXIDE = new G4VisAttributes(G4Colour(.4, .5, .4));
	VisG4_BGO = new G4VisAttributes(G4Colour(0, 1.0, 0.0));
	VisMylar = new G4VisAttributes(G4Colour(.7, .7, 1));
	VisEpoxy = new G4VisAttributes(G4Colour(.8, .4, .4));
	VisRadioactive = new G4VisAttributes(G4Colour(0, 1, 0));

	WeakSourceOffset = G4ThreeVector(0, 0, 0);
	BGOLengthContractionFactor = .999;
	AlLengthContractionFactor = .9963;
	SiDeadLayerLength = 22 * CLHEP::nm;
	gammaDetOffset = G4ThreeVector(0, 0, 0);
	sbdDetectorBendOffset = G4ThreeVector(0, 0, -1.9 * CLHEP::mm); //Due to thermal contraction of SBD away from bend

}

MRKRDKIIGeometry::~MRKRDKIIGeometry()
{

}

void MRKRDKIIGeometry::defineMacroCommands(MRKMacroMessenger* inpMacroMessenger)
{
	//Commands will be deleted by inpMacroMessenger
	std::function<void(G4String)> func;

	G4UIdirectory* detDir = new G4UIdirectory("/MRK/det/");
	detDir->SetGuidance("Detector Construction");
	inpMacroMessenger->addCommand(MRKCommandAndFunction(detDir, nullptr));

	G4UIcmdWithABool* useSBDDetectorCmd = new G4UIcmdWithABool("/MRK/det/useSBDDetector", inpMacroMessenger);
	useSBDDetectorCmd->SetGuidance("Sets true/false to use SBD detectors in RDK2 geometries.");
	useSBDDetectorCmd->SetParameterName("useSBDDetector", false);
	useSBDDetectorCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setUseSBDDetector(G4UIcommand::ConvertToBool(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(useSBDDetectorCmd, func));

	G4UIcmdWithABool* useBGODetectorsCmd = new G4UIcmdWithABool("/MRK/det/useBGODetectors", inpMacroMessenger);
	useBGODetectorsCmd->SetGuidance("Sets true/false to use BGO detectors in RDK2 geometries.");
	useBGODetectorsCmd->SetParameterName("useBGODetectors", false);
	useBGODetectorsCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setUseBGODetectors(G4UIcommand::ConvertToBool(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(useBGODetectorsCmd, func));

	G4UIcmdWithABool* useBAPDDetectorsCmd = new G4UIcmdWithABool("/MRK/det/useBAPDDetectors", inpMacroMessenger);
	useBAPDDetectorsCmd->SetGuidance("Sets true/false to use BAPD detectors in RDK2 geometries.");
	useBAPDDetectorsCmd->SetParameterName("useBAPDDetectors", false);
	useBAPDDetectorsCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setUseBAPDDetectors(G4UIcommand::ConvertToBool(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(useBAPDDetectorsCmd, func));

	G4UIcmdWithABool* useBGOPositionalLightOutCmd = new G4UIcmdWithABool("/MRK/det/useBGOPositionalLightOut", inpMacroMessenger);
	useBGOPositionalLightOutCmd->SetGuidance("Sets true/false to use BGO Positional Light output");
	useBGOPositionalLightOutCmd->SetParameterName("usePositionalBGOLightOutput", false);
	useBGOPositionalLightOutCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setUsePositionalBGOLightOutput(G4UIcommand::ConvertToBool(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(useBGOPositionalLightOutCmd, func));

	G4UIcmdWithABool* useBGONormalizedLightOutCmd = new G4UIcmdWithABool("/MRK/det/useBGONormalizedLightOutCmd", inpMacroMessenger);
	useBGONormalizedLightOutCmd->SetGuidance("Sets true/false to set the use of BGO Positional Light Output's integral to be normalized across the bar");
	useBGONormalizedLightOutCmd->SetParameterName("useNormalizedBGOLightOutput", false);
	useBGONormalizedLightOutCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setUseNormalizedBGOLightOutput(G4UIcommand::ConvertToBool(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(useBGONormalizedLightOutCmd, func));

	G4UIcmdWithABool* useBAPDCollectionEfficiencyModelCmd = new G4UIcmdWithABool("/MRK/det/useBAPDCollectionEfficiencyModel", inpMacroMessenger);
	useBAPDCollectionEfficiencyModelCmd->SetGuidance("Sets true/false to use BAPD Collection Efficiency Model");
	useBAPDCollectionEfficiencyModelCmd->SetParameterName("useBAPDCollectionEfficiencyModel", false);
	useBAPDCollectionEfficiencyModelCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setUseBAPDCollectionEfficiencyModel(G4UIcommand::ConvertToBool(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(useBAPDCollectionEfficiencyModelCmd, func));

	G4UIcmdWithABool* useBGOCalibrationWeakSourceCmd = new G4UIcmdWithABool("/MRK/det/useBGOCalibrationWeakSource", inpMacroMessenger);
	useBGOCalibrationWeakSourceCmd->SetGuidance("Sets true/false to use the BGO Calibration setup with the weak source");
	useBGOCalibrationWeakSourceCmd->SetParameterName("useBGOCalibrationWeakSource", false);
	useBGOCalibrationWeakSourceCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setUseBGOCalibrationWeakSource(G4UIcommand::ConvertToBool(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(useBGOCalibrationWeakSourceCmd, func));

	G4UIcmdWithABool* useDoubleWeakSourceCmd = new G4UIcmdWithABool("/MRK/det/useDoubleWeakSource", inpMacroMessenger);
	useDoubleWeakSourceCmd->SetGuidance("Sets true/false to use two calibration weak sources (two plastic cylinders) in model");
	useDoubleWeakSourceCmd->SetParameterName("useDoubleWeakSource", false);
	useDoubleWeakSourceCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setUseDoubleWeakSource(G4UIcommand::ConvertToBool(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(useDoubleWeakSourceCmd, func));

	G4UIcmdWithABool* useBGOWrappingsCmd = new G4UIcmdWithABool("/MRK/det/useBGOWrappings", inpMacroMessenger);
	useBGOWrappingsCmd->SetGuidance("Sets true/false to use Aluminized Mylar and other wrappings around BGOs");
	useBGOWrappingsCmd->SetParameterName("useBGOWrapping", false);
	useBGOWrappingsCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setUseBGOWrappings(G4UIcommand::ConvertToBool(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(useBGOWrappingsCmd, func));

	G4UIcmdWithABool* useAluminumPlugCmd = new G4UIcmdWithABool("/MRK/det/useAluminumPlug", inpMacroMessenger);
	useAluminumPlugCmd->SetGuidance("Sets true/false to use Aluminized Plug to simulate matter that may reflect electrons");
	useAluminumPlugCmd->SetParameterName("useAluminumPlug", false);
	useAluminumPlugCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setUseAluminumPlug(G4UIcommand::ConvertToBool(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(useAluminumPlugCmd, func));

	G4UIcmdWithABool* use1_5mmSBDCmd = new G4UIcmdWithABool("/MRK/det/use15mmSBD", inpMacroMessenger);
	use1_5mmSBDCmd->SetGuidance("Sets true/false to use 1.5mm SBD or instead the 1.0mm SBD");
	use1_5mmSBDCmd->SetParameterName("use15mmSBD", false);
	use1_5mmSBDCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setUse1_5mmSBD(G4UIcommand::ConvertToBool(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(use1_5mmSBDCmd, func));

	G4UIcmdWithADouble* setAlLengthContractionFactorCmd = new G4UIcmdWithADouble("/MRK/det/setAlLengthContractionFactor", inpMacroMessenger);
	setAlLengthContractionFactorCmd->SetGuidance("Sets length thermal contraction factor for Al");
	setAlLengthContractionFactorCmd->SetParameterName("AlLengthContractionFactor", false);
	setAlLengthContractionFactorCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setUseAluminumPlug(G4UIcommand::ConvertToDouble(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(setAlLengthContractionFactorCmd, func));

	G4UIcmdWithADouble* setBGOLengthContractionFactorCmd = new G4UIcmdWithADouble("/MRK/det/setBGOLengthContractionFactor", inpMacroMessenger);
	setBGOLengthContractionFactorCmd->SetGuidance("Sets length thermal contraction factor for BGO");
	setBGOLengthContractionFactorCmd->SetParameterName("BGOLengthContractionFactor", false);
	setBGOLengthContractionFactorCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setBGOLengthContractionFactor(G4UIcommand::ConvertToDouble(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(setBGOLengthContractionFactorCmd, func));

	G4UIcmdWithAnInteger* numBGOsUsedCmd = new G4UIcmdWithAnInteger("/MRK/det/numBGOsUsed", inpMacroMessenger);
	numBGOsUsedCmd->SetGuidance("Sets number of BGOs to create in detector construction");
	numBGOsUsedCmd->SetParameterName("numBGOsUsed", false);
	numBGOsUsedCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setNumBGOsUsed(G4UIcommand::ConvertToInt(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(numBGOsUsedCmd, func));

	G4UIcmdWith3VectorAndUnit* setWeakSourceOffsetCmd = new G4UIcmdWith3VectorAndUnit("/MRK/det/setWeakSourceOffset", inpMacroMessenger);
	setWeakSourceOffsetCmd->SetGuidance("Sets offset for weak source (already centered on BGO)");
	setWeakSourceOffsetCmd->SetParameterName("weakSourceOffsetX", "weakSourceOffsetY", "weakSourceOffsetZ", false);
	setWeakSourceOffsetCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setWeakSourceOffset(G4UIcommand::ConvertToDimensioned3Vector(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(setWeakSourceOffsetCmd, func));

	G4UIcmdWith3VectorAndUnit* setSBDDetectorBendOffsetCmd = new G4UIcmdWith3VectorAndUnit("/MRK/det/setSBDDetectorBendOffset", inpMacroMessenger);
	setSBDDetectorBendOffsetCmd->SetGuidance("Sets offset for SBD Detector (along bend axis)");
	setSBDDetectorBendOffsetCmd->SetParameterName("SBDDetectorBendOffsetX", "SBDDetectorBendOffsetY", "SBDDetectorBendOffsetZ", false);
	setSBDDetectorBendOffsetCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setSBDDetectorBendOffset(G4UIcommand::ConvertToDimensioned3Vector(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(setSBDDetectorBendOffsetCmd, func));

	G4UIcmdWithABool* SBDGoldLayerOffCmd = new G4UIcmdWithABool("/MRK/det/SBDGoldLayerOff", inpMacroMessenger);
	SBDGoldLayerOffCmd->SetGuidance("Sets true/false to turn the gold layer of the SBD off");
	SBDGoldLayerOffCmd->SetParameterName("SBDGoldLayerOff", false);
	SBDGoldLayerOffCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setSBDGoldLayerOff(G4UIcommand::ConvertToBool(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(SBDGoldLayerOffCmd, func));

	G4UIcmdWithABool* SBDCanOffCmd = new G4UIcmdWithABool("/MRK/det/SBDCanOff", inpMacroMessenger);
	SBDCanOffCmd->SetGuidance("Sets true/false to turn the can and other SBD components off");
	SBDCanOffCmd->SetParameterName("SBDCanOff", false);
	SBDCanOffCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setSBDCanOff(G4UIcommand::ConvertToBool(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(SBDCanOffCmd, func));

	G4UIcmdWithADoubleAndUnit* setSiDeadLayerLengthCmd = new G4UIcmdWithADoubleAndUnit("/MRK/det/setSiDeadLayerLengthCmd", inpMacroMessenger);
	setSiDeadLayerLengthCmd->SetGuidance("Sets length Si dead layer on SBD");
	setSiDeadLayerLengthCmd->SetParameterName("SiDeadLayerLength", false);
	setSiDeadLayerLengthCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setSiDeadLayerLength(G4UIcommand::ConvertToDimensionedDouble(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(setSiDeadLayerLengthCmd, func));

	G4UIcmdWith3VectorAndUnit* setGammaDetOffsetCmd = new G4UIcmdWith3VectorAndUnit("/MRK/det/setGammaDetOffset", inpMacroMessenger);
	setGammaDetOffsetCmd->SetGuidance("Sets offset for gamma detectors and electrostatic mirror (need to set field manually)");
	setGammaDetOffsetCmd->SetParameterName("gammaDetOffsetX", "gammaDetOffsetY", "gammaDetOffsetZ", false);
	setGammaDetOffsetCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setGammaDetOffset(G4UIcommand::ConvertToDimensioned3Vector(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(setGammaDetOffsetCmd, func));
}

G4VPhysicalVolume* MRKRDKIIGeometry::Construct()
{
	G4cout << "Constructing RDK2 Experiment" << G4endl;

	defineDistancesAndRotationsRDK2();
	numBGOsUsed = 12;

	G4SDManager* SDman = G4SDManager::GetSDMpointer();

	SDman->SetVerboseLevel(1);

	materials.createMaterials();

	constructRDK2World();

	//Straight Axis Cylinders
	/*
	constructRDK2StraightAndBentBore();
	constructRDK2HighVoltageTube();
	constructRDK2FrontNylonSpacer();
	constructRDK2LargeNylonSpacer();
	constructRDK2MainGroundCylinder();
	constructRDK2FrontMirrorGround();
	constructRDK2OuterMirrorGround();
	constructRDK2DownstreamNylonSpider();
	constructRDK2G10APDHolder();
	constructRDK2ThrowingStar1();
	constructRDK2AluminumHeatShield();
	constructRDK2ThrowingStar2();
	constructRDK2NylonPusherAssembly();
	constructRDK2UpstreamAluminumTape1();
	constructRDK2UpstreamAluminumTape2();
	constructRDK2UpstreamNylonSpider();
	constructRDK2UpstreamAluminumTape3();
	constructRDK2BAPDHolder();
	constructRDK2FrontCopperOnAperture();
	constructRDK2LeadAperture();
	constructRDK2InnerCopperOnAperture();
	constructRDK2ThreadedUpstreamSupportRod1();
	constructRDK2ThreadedUpstreamSupportRod2();
	constructRDK2ThreadedUpstreamSupportRod3();
	constructRDK2HexSupportRod1();
	constructRDK2HexSupportRod2();
	constructRDK2HexSupportRod3();
	constructRDK2ThreadedDownstreamSupportRod1();
	constructRDK2ThreadedDownstreamSupportRod2();
	constructRDK2ThreadedDownstreamSupportRod3();
	constructRDK2LongSteelAdjustmentRod1();
	constructRDK2LongSteelAdjustmentRod2();
	constructRDK2LongSteelAdjustmentRod3();
	constructRDK2ShortSteelAdjustmentRod1();
	constructRDK2ShortSteelAdjustmentRod2();
	constructRDK2ShortSteelAdjustmentRod3();
	constructRDK2MagnetCoil1();
	constructRDK2MagnetCoil2();
	constructRDK2MagnetCoil3();
	constructRDK2MagnetCoil4();
	constructRDK2MagnetCoil5();
	constructRDK2MagnetCoil6();
	constructRDK2MagnetCoil7();
	constructRDK2DSCryostatFlangeA();
	constructRDK2DSCryostatFlangeB();
	constructRDK2CoilTray1PartH();
	constructRDK2CoilTray1PartI();
	constructRDK2CoilTray1PartK();
	constructRDK2CoilTray1PartL();
	constructRDK2CoilTray1PartM();
	constructRDK2CopperGroundingStrapPart1();
	constructRDK2CopperGroundingStrapPart2();
	constructRDK2CopperGroundingStrapPart3();
	constructRDK2CopperGroundingStrapPart4();
	constructRDK2CopperGroundingStrapPart5();
	constructRDK2CopperGroundingStrapPart6();
	constructRDK2CopperGroundingStrapPart7();
	constructRDK2CopperGroundingStrapPart8();
	constructRDK2CopperGroundingStrapPart9();
	constructRDK2CopperGroundingStrapPart10();
	constructRDK2CopperGroundingStrapPart11();
	constructRDK2CopperGroundingStrapPart12();

	//Bend Axis Cylinders
	constructRDK2LargeBentBore();
	constructRDK2SpacerRing();
	constructRDK2MagnetCoil9();
	constructRDK2MagnetCoil10();
	constructRDK2MagnetCoil11();
	*/
	logicSBDDetectionVolume = constructRDK2SBDSilicon();

	constructRDK2SBDSiliconHolder();
	if(SiDeadLayerLength > 0)
	{
		constructRDK2SBDDeadLayer();
	}
	if(!SBDGoldLayerOff)
	{
		constructRDK2SBDGoldCoating();
	}
	if(!SBDCanOff)
	{
		constructRDK2SBDAluminumCoating();
		constructRDK2SBDPlasticLining();
		constructRDK2SBDBackBrassRings();
		constructRDK2SBDInternalBrassDisc();
		constructRDK2SBDSpring();
		constructRDK2SBDHexScrew();
		constructRDK2SBDFrontCase();
		constructRDK2SBDMidCase();
		constructRDK2SBDBackCase();
		constructSBDEpoxy();
	}
	/*
	constructRDK2BeOTube();
	constructRDK2SBD1mm5StainlessSteelTube();
	constructRDK2CoilTray2PartA();
	constructRDK2CoilTray2PartB();
	constructRDK2CoilTray2PartC();
	constructRDK2CoilTray2PartD();
	constructRDK2CoilTray2PartE();
	constructRDK2CoilTray2PartF();
	//Weld Axis cylinders
	constructRDK2MagnetCoil8();
	constructRDK2CoilTray3PartN();
	constructRDK2CoilTray3PartO();
	constructRDK2CoilTray3PartP();
	constructRDK2CoilTray3PartQ();
	*/
	//Boxes
	logicBAPDDetectionVolumes[0] = constructRDK2ActiveAreaBareAPD1();
	logicBAPDDetectionVolumes[1] = constructRDK2ActiveAreaBareAPD2();
	logicBAPDDetectionVolumes[2] = constructRDK2ActiveAreaBareAPD3();
	/*
	constructRDK2SiBareAPD1();
	constructRDK2SiBareAPD2();
	constructRDK2SiBareAPD3();
	constructRDK2BAPDBacking1();
	constructRDK2BAPDBacking2();
	constructRDK2BAPDBacking3();
	constructRDK2G10BAPDBackPieces1();
	constructRDK2G10BAPDBackPieces2();
	constructRDK2G10BAPDBackPieces3();
	constructRDK2SiSmallAPD1();
	constructRDK2SiSmallAPD2();
	constructRDK2SiSmallAPD3();
	constructRDK2SiSmallAPD4();
	constructRDK2SiSmallAPD5();
	constructRDK2SiSmallAPD6();
	constructRDK2SiSmallAPD7();
	constructRDK2SiSmallAPD8();
	constructRDK2SiSmallAPD9();
	constructRDK2SiSmallAPD10();
	constructRDK2SiSmallAPD11();
	constructRDK2SiSmallAPD12();
	constructRDK2SmallAPDBacking1();
	constructRDK2SmallAPDBacking2();
	constructRDK2SmallAPDBacking3();
	constructRDK2SmallAPDBacking4();
	constructRDK2SmallAPDBacking5();
	constructRDK2SmallAPDBacking6();
	constructRDK2SmallAPDBacking7();
	constructRDK2SmallAPDBacking8();
	constructRDK2SmallAPDBacking9();
	constructRDK2SmallAPDBacking10();
	constructRDK2SmallAPDBacking11();
	constructRDK2SmallAPDBacking12();

	*/
	logicBGODetectionVolumes[0] = constructRDK2BGOCrystal1();
	logicBGODetectionVolumes[1] = constructRDK2BGOCrystal2();
	logicBGODetectionVolumes[2] = constructRDK2BGOCrystal3();
	logicBGODetectionVolumes[3] = constructRDK2BGOCrystal4();
	logicBGODetectionVolumes[4] = constructRDK2BGOCrystal5();
	logicBGODetectionVolumes[5] = constructRDK2BGOCrystal6();
	logicBGODetectionVolumes[6] = constructRDK2BGOCrystal7();
	logicBGODetectionVolumes[7] = constructRDK2BGOCrystal8();
	logicBGODetectionVolumes[8] = constructRDK2BGOCrystal9();
	logicBGODetectionVolumes[9] = constructRDK2BGOCrystal10();
	logicBGODetectionVolumes[10] = constructRDK2BGOCrystal11();
	logicBGODetectionVolumes[11] = constructRDK2BGOCrystal12();
/*
	if(useBGOWrappings)
	{
		constructBGOWrappingsImproved();
	}
*/
	//Setup senstive detectors
	if(useSBDDetector)
	{
		G4cout << "Making SBD Sensitive Detector" << G4endl;
		G4cout << "SBD Silicon dead layer thickness: " << SiDeadLayerLength << G4endl;
		MRKSurfaceBarrierDetector* newSBDDet = new MRKSurfaceBarrierDetector("SBD_det");
		SDman->AddNewDetector(newSBDDet);
		logicSBDDetectionVolume->SetSensitiveDetector(newSBDDet);
	}

	if(useBGODetectors)
	{
		G4cout << "Making BGO Sensitive Detectors" << G4endl;
		string BGODetName;
		MRKBGODetector* BGODets[NUM_BGOS];
		for(int i=0;i<NUM_BGOS;i++)
		{
			BGODetName="BGO_"+int2str(i+1)+"_det";

			BGODets[i] = new MRKBGODetector(BGODetName.data(),i,gammaDetOffset,usePositionalBGOLightOutput,useNormalizedBGOLightOutput);

			SDman->AddNewDetector(BGODets[i]);
			logicBGODetectionVolumes[i]->SetSensitiveDetector(BGODets[i]);
		}
	}

	if(useBAPDDetectors)
	{
		G4cout << "Making BAPD Sensitive Detectors" << G4endl;
		TH1D* sio2Hist=getSiO2Hist();
		MRKBAPDDetector* BAPDDets[NUM_BAPDS];
		string BAPDDetName;
		for(int i=0;i<NUM_BAPDS;i++)
		{
			BAPDDetName="BAPD_"+int2str(i+1)+"_det";
			BAPDDets[i] = new MRKBAPDDetector(BAPDDetName.data(),sio2Hist,i,gammaDetOffset,useBAPDCollectionEfficiencyModel);
			SDman->AddNewDetector(BAPDDets[i]);
			logicBAPDDetectionVolumes[i]->SetSensitiveDetector(BAPDDets[i]);
		}

	}

	if(useBGOCalibrationWeakSource)
	{
		constructRDK2ReentrantTube();
		constructRDK2ReentrantTubeWeld();
		constructRDK2SourceStick();
		constructRDK2WeakSource();
	}
	else
	{
		constructRDK2BeamCenter();
		constructRDK2OffAxisVacuum();
		if(useAluminumPlug)
		{
			constructRDK2AluminumPlug();
		}
		constructRDK2FarAluminumTube();

	}

	SDman->SetVerboseLevel(0);

	G4cout << "------Returning world from detector construction----" << G4endl;

	//Output file to GDML
//    #include "G4GDMLParser.hh"
//    G4cout <<"#############################################################"<< G4endl << "Outputting GDML to: /home/mjbales/school/rdk/mrkgeantrun/test.gdml" << G4endl;
//    G4GDMLParser parser;
//    parser.Write("/home/mjbales/school/rdk/mrkgeantrun/test.gdml",physiWorld,false);

	return physiWorld;

}

void MRKRDKIIGeometry::defineDistancesAndRotationsRDK2()
{
	for (int i = 0; i < 3; i++)
	{
		rdk2RotationMatrices[i] = new G4RotationMatrix();
	}
	rdk2RotationMatrices[1]->rotateY(-BEND_ANGLE * deg);
	rdk2RotationMatrices[2]->rotateY(-WELD_ANGLE * deg);

	sbdDetectorOffset = sbdDetectorBendOffset.rotateY(BEND_ANGLE * deg);
	G4cout << "SBD offset: " << sbdDetectorOffset << endl;

	for (int i = 0; i < 3; i++)
	{
		G4double BAPDangle = (-i * 120. - 30.);
		if(BAPDangle > 360.) BAPDangle -= 360.;
		bapdRotationMatrices[i] = new G4RotationMatrix();
		bapdRotationMatrices[i]->rotateZ(-BAPDangle * deg);
	}

	for (int i = 0; i < 12; i++)
	{
		G4double BGOangle = (-i * 30. - 15. + 90.);
		if(BGOangle > 360.) BGOangle -= 360.;
		bgoRotationMatrices[i] = new G4RotationMatrix();
		bgoRotationMatrices[i]->rotateZ(-BGOangle * deg);
	}

}

void MRKRDKIIGeometry::constructRDK2World()
{
	G4double WorldLength = 50 * CLHEP::cm;
	G4double WorldWidth = 50 * CLHEP::cm;
	G4double WorldHeight = 150 * CLHEP::cm;
	G4GeometryManager::GetInstance()->SetWorldMaximumExtent(WorldLength);
	G4cout << "Computed tolerance = " << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance() / CLHEP::mm << " CLHEP::mm" << G4endl;

	G4Box* solidWorld = new G4Box("world", WorldLength * .5, WorldWidth * .5, WorldHeight * .5);
	logicWorld = new G4LogicalVolume(solidWorld, materials.getMaterial("G4_Galactic"), "World", 0, 0, 0);

	//  Must place the World Physical volume unrotated at (0,0,0).
	//
	physiWorld = new G4PVPlacement(0, // no rotation
	G4ThreeVector(), // at (0,0,0)
	logicWorld,      // its logical volume
	"World",         // its name
	0,               // its mother  volume
	false,           // no boolean operations
	0);              // copy number
	logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2StraightAndBentBore()
{
	G4double boreLengthedToMakeWeld = 6.11 * CLHEP::cm * sin(WELD_ANGLE * CLHEP::pi / 180.);
	G4Tubs* solidStraightBore = new G4Tubs("solidStraightBore", 5.847 * CLHEP::cm, 6.111 * CLHEP::cm, 22.64 * CLHEP::cm + .5 * boreLengthedToMakeWeld, 0., CLHEP::twopi);

	G4double BoreSubtractionBoxLength = 4 * 6.11 * CLHEP::cm;  //Make it extra big
	G4double BoreSubtractionBoxCenterZ = -22.64 * CLHEP::cm - .5 * boreLengthedToMakeWeld - .5 * BoreSubtractionBoxLength + 5.847 * CLHEP::cm * sin(WELD_ANGLE * CLHEP::pi / 180.);
	G4Box* solidBoreSubtractionBox = new G4Box("solidStraightBoreSubtractionBox", .5 * BoreSubtractionBoxLength, .5 * BoreSubtractionBoxLength, .5 * BoreSubtractionBoxLength);

	G4RotationMatrix* SubBoxRotation = new G4RotationMatrix();
	SubBoxRotation->rotateY(-WELD_ANGLE * deg);

	G4VSolid* solidStraightBoreWEnd = new G4SubtractionSolid("BoreWEnd", solidStraightBore, solidBoreSubtractionBox, SubBoxRotation, G4ThreeVector(0, 0, BoreSubtractionBoxCenterZ));

	G4Tubs* solidBentBore = new G4Tubs("solidBentBore", 5.847 * CLHEP::cm, 6.111 * CLHEP::cm, 13.2461 * CLHEP::cm + .5 * boreLengthedToMakeWeld, 0., CLHEP::twopi);
	BoreSubtractionBoxCenterZ = -13.2461 * CLHEP::cm - .5 * boreLengthedToMakeWeld - .5 * BoreSubtractionBoxLength + 5.847 * CLHEP::cm * sin(WELD_ANGLE * CLHEP::pi / 180.);

	G4VSolid* solidBentBoreWEnd = new G4SubtractionSolid("BoreWEnd", solidBentBore, solidBoreSubtractionBox, SubBoxRotation, G4ThreeVector(0, 0, BoreSubtractionBoxCenterZ));

	G4RotationMatrix* BoreRotationMatrix = new G4RotationMatrix();
	BoreRotationMatrix->rotateZ(180. * deg);
	BoreRotationMatrix->rotateY((180. + BEND_ANGLE) * deg);
	G4ThreeVector angledBendCenter(-2.18623709199112 * CLHEP::cm + boreLengthedToMakeWeld * sin(WELD_ANGLE * CLHEP::pi / 180.), 0 * CLHEP::cm, -13.0644377065223 * CLHEP::cm - 22.64 * CLHEP::cm + boreLengthedToMakeWeld * cos(WELD_ANGLE * CLHEP::pi / 180.));

	G4VSolid* solidCombinedAngledBores = new G4UnionSolid("CombinedAngledBores", solidStraightBoreWEnd, solidBentBoreWEnd, BoreRotationMatrix, angledBendCenter);

	G4LogicalVolume* logicCombinedAngledBores = new G4LogicalVolume(solidCombinedAngledBores, materials.getMaterial("StainlessSteel304"), "logicCombinedAngledBores", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 22.64 * CLHEP::cm - .5 * boreLengthedToMakeWeld), logicCombinedAngledBores, "physiCombinedAngledBores", logicWorld, false, 0);
	logicCombinedAngledBores->SetVisAttributes(VisStainlessSteel304);
	return logicCombinedAngledBores;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2HighVoltageTube()
{
	G4Tubs* solidHighVoltageTube = new G4Tubs("solidHighVoltageTube", 2.38 * CLHEP::cm, 2.54 * CLHEP::cm, 1 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicHighVoltageTube = new G4LogicalVolume(solidHighVoltageTube, materials.getMaterial("G4_Al"), "logicHighVoltageTube", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 33.72 * CLHEP::cm), logicHighVoltageTube, "physiHighVoltageTube", logicWorld, false, 0);
	logicHighVoltageTube->SetVisAttributes(VisG4_Al);
	return logicHighVoltageTube;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2FrontNylonSpacer()
{
	G4Tubs* solidFrontNylonSpacer = new G4Tubs("solidFrontNylonSpacer", 2.38 * CLHEP::cm, 2.54 * CLHEP::cm, 0.16 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicFrontNylonSpacer = new G4LogicalVolume(solidFrontNylonSpacer, materials.getMaterial("G4_NYLON-6/6"), "logicFrontNylonSpacer", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 32.56 * CLHEP::cm), logicFrontNylonSpacer, "physiFrontNylonSpacer", logicWorld, false, 0);
	logicFrontNylonSpacer->SetVisAttributes(VisG4_NYLON);
	return logicFrontNylonSpacer;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2LargeNylonSpacer()
{
	G4Tubs* solidLargeNylonSpacer = new G4Tubs("solidLargeNylonSpacer", 2.54 * CLHEP::cm, 2.86 * CLHEP::cm, 3.255 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicLargeNylonSpacer = new G4LogicalVolume(solidLargeNylonSpacer, materials.getMaterial("G4_NYLON-6/6"), "logicLargeNylonSpacer", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 35.655 * CLHEP::cm), logicLargeNylonSpacer, "physiLargeNylonSpacer", logicWorld, false, 0);
	logicLargeNylonSpacer->SetVisAttributes(VisG4_NYLON);
	return logicLargeNylonSpacer;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2MainGroundCylinder()
{
	G4Tubs* solidMainGroundCylinder = new G4Tubs("solidMainGroundCylinder", 2.86 * CLHEP::cm, 3.18 * CLHEP::cm, 6.35 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicMainGroundCylinder = new G4LogicalVolume(solidMainGroundCylinder, materials.getMaterial("G4_Al"), "logicMainGroundCylinder", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 38.75 * CLHEP::cm), logicMainGroundCylinder, "physiMainGroundCylinder", logicWorld, false, 0);
	logicMainGroundCylinder->SetVisAttributes(VisG4_Al);
	return logicMainGroundCylinder;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2FrontMirrorGround()
{
	G4Tubs* solidFrontMirrorGround = new G4Tubs("solidFrontMirrorGround", 2.54 * CLHEP::cm, 3.18 * CLHEP::cm, 0.08 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicFrontMirrorGround = new G4LogicalVolume(solidFrontMirrorGround, materials.getMaterial("G4_Al"), "logicFrontMirrorGround", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 32.32 * CLHEP::cm), logicFrontMirrorGround, "physiFrontMirrorGround", logicWorld, false, 0);
	logicFrontMirrorGround->SetVisAttributes(VisG4_Al);
	return logicFrontMirrorGround;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2OuterMirrorGround()
{
	G4Tubs* solidOuterMirrorGround = new G4Tubs("solidOuterMirrorGround", 3.18 * CLHEP::cm, 5.24 * CLHEP::cm, 0.32 * CLHEP::cm, 0., CLHEP::twopi);

	G4VSolid* solidOuterMirrorGroundWShortHoles = makeHolesForShortStainlessSupportRods(solidOuterMirrorGround);
	G4VSolid* solidOuterMirrorGroundWAllStainlessHoles = makeHolesForLongStainlessSupportRods(solidOuterMirrorGroundWShortHoles);
	G4VSolid* solidOuterMirrorGroundWAllHoles = makeHolesForHexSupportRods(solidOuterMirrorGroundWAllStainlessHoles);

	G4LogicalVolume* logicOuterMirrorGround = new G4LogicalVolume(solidOuterMirrorGroundWAllHoles, materials.getMaterial("G4_Al"), "logicOuterMirrorGround", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 35.89 * CLHEP::cm), logicOuterMirrorGround, "physiOuterMirrorGround", logicWorld, false, 0);
	logicOuterMirrorGround->SetVisAttributes(VisG4_Al);
	return logicOuterMirrorGround;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2DownstreamNylonSpider()
{
	G4Tubs* solidDownstreamNylonSpider = new G4Tubs("solidDownstreamNylonSpider", 3.81 * CLHEP::cm, 5.4 * CLHEP::cm, 0.95 * CLHEP::cm, 0., CLHEP::twopi);

	G4VSolid* solidDownstreamNylonSpiderWShortHoles = makeHolesForShortStainlessSupportRods(solidDownstreamNylonSpider);
	G4VSolid* solidDownstreamNylonSpiderWAllStainlessHoles = makeHolesForLongStainlessSupportRods(solidDownstreamNylonSpiderWShortHoles);
	G4VSolid* solidDownstreamNylonSpiderWAllHoles = makeHolesForHexSupportRods(solidDownstreamNylonSpiderWAllStainlessHoles);

	G4LogicalVolume* logicDownstreamNylonSpider = new G4LogicalVolume(solidDownstreamNylonSpiderWAllHoles, materials.getMaterial("G4_NYLON-6/6"), "logicDownstreamNylonSpider", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 34.62 * CLHEP::cm), logicDownstreamNylonSpider, "physiDownstreamNylonSpider", logicWorld, false, 0);
	logicDownstreamNylonSpider->SetVisAttributes(VisG4_NYLON);
	return logicDownstreamNylonSpider;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2G10APDHolder()
{
	G4Tubs* solidG10APDHolder = new G4Tubs("solidG10APDHolder", 3.49 * CLHEP::cm, 5.4 * CLHEP::cm, 0.635 * CLHEP::cm, 0., CLHEP::twopi);

	G4VSolid* solidG10APDHolderWHexRodHoles = makeHolesForHexSupportRods(solidG10APDHolder);
	G4VSolid* solidG10APDHolderWAllRodHoles = makeHolesForLongStainlessSupportRods(solidG10APDHolderWHexRodHoles);
	G4VSolid* solidG10APDHolderWAllHoles = makeHolesForBGOAPDs(solidG10APDHolderWAllRodHoles);

	G4LogicalVolume* logicG10APDHolder = new G4LogicalVolume(solidG10APDHolderWAllHoles, materials.getMaterial("G10"), "logicG10APDHolder", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 33.035 * CLHEP::cm), logicG10APDHolder, "physiG10APDHolder", logicWorld, false, 0);
	logicG10APDHolder->SetVisAttributes(VisG10);
	return logicG10APDHolder;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2ThrowingStar1()
{
	G4Tubs* solidThrowingStar1 = new G4Tubs("solidThrowingStar1", 3.492 * CLHEP::cm * AlLengthContractionFactor, 4.953 * CLHEP::cm * AlLengthContractionFactor, 0.3175 * CLHEP::cm * AlLengthContractionFactor, 0., CLHEP::twopi);

	G4VSolid* solidThrowingStar1WHexRodHoles = makeHolesForHexSupportRods(solidThrowingStar1);
	G4VSolid* solidThrowingStar1WAllRodHoles = makeHolesForLongStainlessSupportRods(solidThrowingStar1WHexRodHoles);
	G4VSolid* solidThrowingStar1WAllHoles = makeHolesForBGOs(solidThrowingStar1WAllRodHoles);

	G4LogicalVolume* logicThrowingStar1 = new G4LogicalVolume(solidThrowingStar1WAllHoles, materials.getMaterial("G4_Al"), "logicThrowingStar1", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 32.0825 * CLHEP::cm), logicThrowingStar1, "physiThrowingStar1", logicWorld, false, 0);
	logicThrowingStar1->SetVisAttributes(VisG4_Al);
	return logicThrowingStar1;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2AluminumHeatShield()
{
	G4Tubs* solidAluminumHeatShield = new G4Tubs("solidAluminumHeatShield", 4.953 * CLHEP::cm, 5.12 * CLHEP::cm, 10.1575 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicAluminumHeatShield = new G4LogicalVolume(solidAluminumHeatShield, materials.getMaterial("G4_Al"), "logicAluminumHeatShield", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 22.2425 * CLHEP::cm), logicAluminumHeatShield, "physiAluminumHeatShield", logicWorld, false, 0);
	logicAluminumHeatShield->SetVisAttributes(VisG4_Al);
	return logicAluminumHeatShield;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2ThrowingStar2()
{
	G4Tubs* solidThrowingStar2 = new G4Tubs("solidThrowingStar2", 3.492 * CLHEP::cm * AlLengthContractionFactor, 4.953 * CLHEP::cm * AlLengthContractionFactor, 0.317500000000001 * CLHEP::cm * AlLengthContractionFactor, 0., CLHEP::twopi);
	G4VSolid* solidThrowingStar2WHexRodHoles = makeHolesForHexSupportRods(solidThrowingStar2);
	G4VSolid* solidThrowingStar2WAllRodHoles = makeHolesForLongStainlessSupportRods(solidThrowingStar2WHexRodHoles);
	G4VSolid* solidThrowingStar2WAllHoles = makeHolesForBGOs(solidThrowingStar2WAllRodHoles);

	G4LogicalVolume* logicThrowingStar2 = new G4LogicalVolume(solidThrowingStar2WAllHoles, materials.getMaterial("G4_Al"), "logicThrowingStar2", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 12.4025 * CLHEP::cm), logicThrowingStar2, "physiThrowingStar2", logicWorld, false, 0);
	logicThrowingStar2->SetVisAttributes(VisG4_Al);
	return logicThrowingStar2;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2NylonPusherAssembly()
{
	G4Tubs* solidNylonPusherAssembly = new G4Tubs("solidNylonPusherAssembly", 3.492 * CLHEP::cm, 5.4 * CLHEP::cm, 1.35 * CLHEP::cm, 0., CLHEP::twopi);

	G4VSolid* solidNylonPusherAssemblyWHexRodHoles = makeHolesForHexSupportRods(solidNylonPusherAssembly);
	G4VSolid* solidNylonPusherAssemblyWAllHoles = makeHolesForLongStainlessSupportRods(solidNylonPusherAssemblyWHexRodHoles);

	G4LogicalVolume* logicNylonPusherAssembly = new G4LogicalVolume(solidNylonPusherAssemblyWAllHoles, materials.getMaterial("G4_NYLON-6/6"), "logicNylonPusherAssembly", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 10.73 * CLHEP::cm), logicNylonPusherAssembly, "physiNylonPusherAssembly", logicWorld, false, 0);
	logicNylonPusherAssembly->SetVisAttributes(VisG4_NYLON);
	return logicNylonPusherAssembly;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2UpstreamAluminumTape1()
{
	G4Tubs* solidUpstreamAluminumTape1 = new G4Tubs("solidUpstreamAluminumTape1", 3.461 * CLHEP::cm, 3.492 * CLHEP::cm, 1.35 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicUpstreamAluminumTape1 = new G4LogicalVolume(solidUpstreamAluminumTape1, materials.getMaterial("G4_Al"), "logicUpstreamAluminumTape1", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 10.73 * CLHEP::cm), logicUpstreamAluminumTape1, "physiUpstreamAluminumTape1", logicWorld, false, 0);
	logicUpstreamAluminumTape1->SetVisAttributes(VisG4_Al);
	return logicUpstreamAluminumTape1;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2UpstreamAluminumTape2()
{
	G4Tubs* solidUpstreamAluminumTape2 = new G4Tubs("solidUpstreamAluminumTape2", 3.492 * CLHEP::cm, 3.81 * CLHEP::cm, 0.0154999999999994 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicUpstreamAluminumTape2 = new G4LogicalVolume(solidUpstreamAluminumTape2, materials.getMaterial("G4_Al"), "logicUpstreamAluminumTape2", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 9.3645 * CLHEP::cm), logicUpstreamAluminumTape2, "physiUpstreamAluminumTape2", logicWorld, false, 0);
	logicUpstreamAluminumTape2->SetVisAttributes(VisG4_Al);
	return logicUpstreamAluminumTape2;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2UpstreamNylonSpider()
{
	G4Tubs* solidUpstreamNylonSpider = new G4Tubs("solidUpstreamNylonSpider", 3.81 * CLHEP::cm, 5.4 * CLHEP::cm, 0.952499999999997 * CLHEP::cm, 0., CLHEP::twopi);

	G4VSolid* solidUpstreamNylonSpiderWHexRodHoles = makeHolesForHexSupportRods(solidUpstreamNylonSpider);
	G4VSolid* solidUpstreamNylonSpiderWAllHoles = makeHolesForLongStainlessSupportRods(solidUpstreamNylonSpiderWHexRodHoles);

	G4LogicalVolume* logicUpstreamNylonSpider = new G4LogicalVolume(solidUpstreamNylonSpiderWAllHoles, materials.getMaterial("G4_NYLON-6/6"), "logicUpstreamNylonSpider", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 8.4275 * CLHEP::cm), logicUpstreamNylonSpider, "physiUpstreamNylonSpider", logicWorld, false, 0);
	logicUpstreamNylonSpider->SetVisAttributes(VisG4_NYLON);
	return logicUpstreamNylonSpider;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2UpstreamAluminumTape3()
{
	G4Tubs* solidUpstreamAluminumTape3 = new G4Tubs("solidUpstreamAluminumTape3", 3.779 * CLHEP::cm, 3.81 * CLHEP::cm, 0.736999999999998 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicUpstreamAluminumTape3 = new G4LogicalVolume(solidUpstreamAluminumTape3, materials.getMaterial("G4_Al"), "logicUpstreamAluminumTape3", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 8.612 * CLHEP::cm), logicUpstreamAluminumTape3, "physiUpstreamAluminumTape3", logicWorld, false, 0);
	logicUpstreamAluminumTape3->SetVisAttributes(VisG4_Al);
	return logicUpstreamAluminumTape3;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2BAPDHolder()
{
	G4Tubs* solidBAPDHolder = new G4Tubs("solidBAPDHolder", 1.75 * CLHEP::cm, 5.5 * CLHEP::cm, 0.15875 * CLHEP::cm, 0., CLHEP::twopi);

	G4VSolid* solidBAPDHolderWBAPDHoles = makeHolesForBAPDs(solidBAPDHolder);
	G4VSolid* solidBAPDHolderWAllHoles = makeHolesForHexSupportRods(solidBAPDHolderWBAPDHoles);

	G4LogicalVolume* logicBAPDHolder = new G4LogicalVolume(solidBAPDHolderWAllHoles, materials.getMaterial("G4_Al"), "logicBAPDHolder", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 3.94125 * CLHEP::cm), logicBAPDHolder, "physiBAPDHolder", logicWorld, false, 0);
	logicBAPDHolder->SetVisAttributes(VisG4_Al);
	return logicBAPDHolder;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2FrontCopperOnAperture()
{
	G4Tubs* solidFrontCopperOnAperture = new G4Tubs("solidFrontCopperOnAperture", 2.375 * CLHEP::cm, 5.725 * CLHEP::cm, 0.0899999999999999 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicFrontCopperOnAperture = new G4LogicalVolume(solidFrontCopperOnAperture, materials.getMaterial("G4_Cu"), "logicFrontCopperOnAperture", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 2.63 * CLHEP::cm), logicFrontCopperOnAperture, "physiFrontCopperOnAperture", logicWorld, false, 0);
	logicFrontCopperOnAperture->SetVisAttributes(VisG4_Cu);
	return logicFrontCopperOnAperture;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2LeadAperture()
{
	G4Tubs* solidLeadAperture = new G4Tubs("solidLeadAperture", 2.54 * CLHEP::cm, 5.725 * CLHEP::cm, 1.27 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicLeadAperture = new G4LogicalVolume(solidLeadAperture, materials.getMaterial("G4_Pb"), "logicLeadAperture", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 1.27 * CLHEP::cm), logicLeadAperture, "physiLeadAperture", logicWorld, false, 0);
	logicLeadAperture->SetVisAttributes(VisG4_Pb);
	return logicLeadAperture;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2InnerCopperOnAperture()
{
	G4Tubs* solidInnerCopperOnAperture = new G4Tubs("solidInnerCopperOnAperture", 2.375 * CLHEP::cm, 2.54 * CLHEP::cm, 1.27 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicInnerCopperOnAperture = new G4LogicalVolume(solidInnerCopperOnAperture, materials.getMaterial("G4_Cu"), "logicInnerCopperOnAperture", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 1.27 * CLHEP::cm), logicInnerCopperOnAperture, "physiInnerCopperOnAperture", logicWorld, false, 0);
	logicInnerCopperOnAperture->SetVisAttributes(VisG4_Cu);
	return logicInnerCopperOnAperture;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2ThreadedUpstreamSupportRod1()
{
	G4Tubs* solidThreadedUpstreamSupportRod1 = new G4Tubs("solidThreadedUpstreamSupportRod1", 0 * CLHEP::cm, 0.238 * CLHEP::cm, 4.95 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicThreadedUpstreamSupportRod1 = new G4LogicalVolume(solidThreadedUpstreamSupportRod1, materials.getMaterial("G4_Al"), "logicThreadedUpstreamSupportRod1", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(3.97098628397279 * CLHEP::cm, 2.29265 * CLHEP::cm, 7.77 * CLHEP::cm), logicThreadedUpstreamSupportRod1, "physiThreadedUpstreamSupportRod1", logicWorld, false, 0);
	logicThreadedUpstreamSupportRod1->SetVisAttributes(VisG4_Al);
	return logicThreadedUpstreamSupportRod1;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2ThreadedUpstreamSupportRod2()
{
	G4Tubs* solidThreadedUpstreamSupportRod2 = new G4Tubs("solidThreadedUpstreamSupportRod2", 0 * CLHEP::cm, 0.238 * CLHEP::cm, 4.95 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicThreadedUpstreamSupportRod2 = new G4LogicalVolume(solidThreadedUpstreamSupportRod2, materials.getMaterial("G4_Al"), "logicThreadedUpstreamSupportRod2", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, -4.5853 * CLHEP::cm, 7.77 * CLHEP::cm), logicThreadedUpstreamSupportRod2, "physiThreadedUpstreamSupportRod2", logicWorld, false, 0);
	logicThreadedUpstreamSupportRod2->SetVisAttributes(VisG4_Al);
	return logicThreadedUpstreamSupportRod2;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2ThreadedUpstreamSupportRod3()
{
	G4Tubs* solidThreadedUpstreamSupportRod3 = new G4Tubs("solidThreadedUpstreamSupportRod3", 0 * CLHEP::cm, 0.238 * CLHEP::cm, 4.95 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicThreadedUpstreamSupportRod3 = new G4LogicalVolume(solidThreadedUpstreamSupportRod3, materials.getMaterial("G4_Al"), "logicThreadedUpstreamSupportRod3", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(-3.97098628397279 * CLHEP::cm, 2.29265 * CLHEP::cm, 7.77 * CLHEP::cm), logicThreadedUpstreamSupportRod3, "physiThreadedUpstreamSupportRod3", logicWorld, false, 0);
	logicThreadedUpstreamSupportRod3->SetVisAttributes(VisG4_Al);
	return logicThreadedUpstreamSupportRod3;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2HexSupportRod1()
{
	G4Tubs* solidHexSupportRod1 = new G4Tubs("solidHexSupportRod1", 0 * CLHEP::cm, 0.3334 * CLHEP::cm, 9.5225 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicHexSupportRod1 = new G4LogicalVolume(solidHexSupportRod1, materials.getMaterial("G4_Al"), "logicHexSupportRod1", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(3.97098628397279 * CLHEP::cm, 2.29265 * CLHEP::cm, 22.2425 * CLHEP::cm), logicHexSupportRod1, "physiHexSupportRod1", logicWorld, false, 0);
	logicHexSupportRod1->SetVisAttributes(VisG4_Al);
	return logicHexSupportRod1;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2HexSupportRod2()
{
	G4Tubs* solidHexSupportRod2 = new G4Tubs("solidHexSupportRod2", 0 * CLHEP::cm, 0.3334 * CLHEP::cm, 9.5225 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicHexSupportRod2 = new G4LogicalVolume(solidHexSupportRod2, materials.getMaterial("G4_Al"), "logicHexSupportRod2", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, -4.5853 * CLHEP::cm, 22.2425 * CLHEP::cm), logicHexSupportRod2, "physiHexSupportRod2", logicWorld, false, 0);
	logicHexSupportRod2->SetVisAttributes(VisG4_Al);
	return logicHexSupportRod2;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2HexSupportRod3()
{
	G4Tubs* solidHexSupportRod3 = new G4Tubs("solidHexSupportRod3", 0 * CLHEP::cm, 0.3334 * CLHEP::cm, 9.5225 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicHexSupportRod3 = new G4LogicalVolume(solidHexSupportRod3, materials.getMaterial("G4_Al"), "logicHexSupportRod3", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(-3.97098628397279 * CLHEP::cm, 2.29265 * CLHEP::cm, 22.2425 * CLHEP::cm), logicHexSupportRod3, "physiHexSupportRod3", logicWorld, false, 0);
	logicHexSupportRod3->SetVisAttributes(VisG4_Al);
	return logicHexSupportRod3;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2ThreadedDownstreamSupportRod1()
{
	G4Tubs* solidThreadedDownstreamSupportRod1 = new G4Tubs("solidThreadedDownstreamSupportRod1", 0 * CLHEP::cm, 0.238 * CLHEP::cm, 1.9025 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicThreadedDownstreamSupportRod1 = new G4LogicalVolume(solidThreadedDownstreamSupportRod1, materials.getMaterial("G4_Al"), "logicThreadedDownstreamSupportRod1", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(3.97098628397279 * CLHEP::cm, 2.29265 * CLHEP::cm, 33.6675 * CLHEP::cm), logicThreadedDownstreamSupportRod1, "physiThreadedDownstreamSupportRod1", logicWorld, false, 0);
	logicThreadedDownstreamSupportRod1->SetVisAttributes(VisG4_Al);
	return logicThreadedDownstreamSupportRod1;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2ThreadedDownstreamSupportRod2()
{
	G4Tubs* solidThreadedDownstreamSupportRod2 = new G4Tubs("solidThreadedDownstreamSupportRod2", 0 * CLHEP::cm, 0.238 * CLHEP::cm, 1.9025 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicThreadedDownstreamSupportRod2 = new G4LogicalVolume(solidThreadedDownstreamSupportRod2, materials.getMaterial("G4_Al"), "logicThreadedDownstreamSupportRod2", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, -4.5853 * CLHEP::cm, 33.6675 * CLHEP::cm), logicThreadedDownstreamSupportRod2, "physiThreadedDownstreamSupportRod2", logicWorld, false, 0);
	logicThreadedDownstreamSupportRod2->SetVisAttributes(VisG4_Al);
	return logicThreadedDownstreamSupportRod2;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2ThreadedDownstreamSupportRod3()
{
	G4Tubs* solidThreadedDownstreamSupportRod3 = new G4Tubs("solidThreadedDownstreamSupportRod3", 0 * CLHEP::cm, 0.238 * CLHEP::cm, 1.9025 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicThreadedDownstreamSupportRod3 = new G4LogicalVolume(solidThreadedDownstreamSupportRod3, materials.getMaterial("G4_Al"), "logicThreadedDownstreamSupportRod3", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(-3.97098628397279 * CLHEP::cm, 2.29265 * CLHEP::cm, 33.6675 * CLHEP::cm), logicThreadedDownstreamSupportRod3, "physiThreadedDownstreamSupportRod3", logicWorld, false, 0);
	logicThreadedDownstreamSupportRod3->SetVisAttributes(VisG4_Al);
	return logicThreadedDownstreamSupportRod3;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2LongSteelAdjustmentRod1()
{
	G4Tubs* solidLongSteelAdjustmentRod1 = new G4Tubs("solidLongSteelAdjustmentRod1", 0 * CLHEP::cm, 0.238 * CLHEP::cm, 18.13625 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicLongSteelAdjustmentRod1 = new G4LogicalVolume(solidLongSteelAdjustmentRod1, materials.getMaterial("StainlessSteel304"), "logicLongSteelAdjustmentRod1", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 4.5853 * CLHEP::cm, 26.56375 * CLHEP::cm), logicLongSteelAdjustmentRod1, "physiLongSteelAdjustmentRod1", logicWorld, false, 0);
	logicLongSteelAdjustmentRod1->SetVisAttributes(VisStainlessSteel304);
	return logicLongSteelAdjustmentRod1;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2LongSteelAdjustmentRod2()
{
	G4Tubs* solidLongSteelAdjustmentRod2 = new G4Tubs("solidLongSteelAdjustmentRod2", 0 * CLHEP::cm, 0.238 * CLHEP::cm, 18.13625 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicLongSteelAdjustmentRod2 = new G4LogicalVolume(solidLongSteelAdjustmentRod2, materials.getMaterial("StainlessSteel304"), "logicLongSteelAdjustmentRod2", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(3.97098628397279 * CLHEP::cm, -2.29265 * CLHEP::cm, 26.56375 * CLHEP::cm), logicLongSteelAdjustmentRod2, "physiLongSteelAdjustmentRod2", logicWorld, false, 0);
	logicLongSteelAdjustmentRod2->SetVisAttributes(VisStainlessSteel304);
	return logicLongSteelAdjustmentRod2;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2LongSteelAdjustmentRod3()
{
	G4Tubs* solidLongSteelAdjustmentRod3 = new G4Tubs("solidLongSteelAdjustmentRod3", 0 * CLHEP::cm, 0.238 * CLHEP::cm, 18.13625 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicLongSteelAdjustmentRod3 = new G4LogicalVolume(solidLongSteelAdjustmentRod3, materials.getMaterial("StainlessSteel304"), "logicLongSteelAdjustmentRod3", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(-3.97098628397279 * CLHEP::cm, -2.29265 * CLHEP::cm, 26.56375 * CLHEP::cm), logicLongSteelAdjustmentRod3, "physiLongSteelAdjustmentRod3", logicWorld, false, 0);
	logicLongSteelAdjustmentRod3->SetVisAttributes(VisStainlessSteel304);
	return logicLongSteelAdjustmentRod3;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2ShortSteelAdjustmentRod1()
{
	G4Tubs* solidShortSteelAdjustmentRod1 = new G4Tubs("solidShortSteelAdjustmentRod1", 0 * CLHEP::cm, 0.238 * CLHEP::cm, 5.04 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicShortSteelAdjustmentRod1 = new G4LogicalVolume(solidShortSteelAdjustmentRod1, materials.getMaterial("StainlessSteel304"), "logicShortSteelAdjustmentRod1", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(4.5853 * CLHEP::cm, 0 * CLHEP::cm, 39.66 * CLHEP::cm), logicShortSteelAdjustmentRod1, "physiShortSteelAdjustmentRod1", logicWorld, false, 0);
	logicShortSteelAdjustmentRod1->SetVisAttributes(VisStainlessSteel304);
	return logicShortSteelAdjustmentRod1;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2ShortSteelAdjustmentRod2()
{
	G4Tubs* solidShortSteelAdjustmentRod2 = new G4Tubs("solidShortSteelAdjustmentRod2", 0 * CLHEP::cm, 0.238 * CLHEP::cm, 5.04 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicShortSteelAdjustmentRod2 = new G4LogicalVolume(solidShortSteelAdjustmentRod2, materials.getMaterial("StainlessSteel304"), "logicShortSteelAdjustmentRod2", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(-2.29265 * CLHEP::cm, -3.97098628397279 * CLHEP::cm, 39.66 * CLHEP::cm), logicShortSteelAdjustmentRod2, "physiShortSteelAdjustmentRod2", logicWorld, false, 0);
	logicShortSteelAdjustmentRod2->SetVisAttributes(VisStainlessSteel304);
	return logicShortSteelAdjustmentRod2;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2ShortSteelAdjustmentRod3()
{
	G4Tubs* solidShortSteelAdjustmentRod3 = new G4Tubs("solidShortSteelAdjustmentRod3", 0 * CLHEP::cm, 0.238 * CLHEP::cm, 5.04 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicShortSteelAdjustmentRod3 = new G4LogicalVolume(solidShortSteelAdjustmentRod3, materials.getMaterial("StainlessSteel304"), "logicShortSteelAdjustmentRod3", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(-2.29265 * CLHEP::cm, 3.97098628397279 * CLHEP::cm, 39.66 * CLHEP::cm), logicShortSteelAdjustmentRod3, "physiShortSteelAdjustmentRod3", logicWorld, false, 0);
	logicShortSteelAdjustmentRod3->SetVisAttributes(VisStainlessSteel304);
	return logicShortSteelAdjustmentRod3;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2MagnetCoil1()
{
	G4Tubs* solidMagnetCoil1 = new G4Tubs("solidMagnetCoil1", 7 * CLHEP::cm, 8.28 * CLHEP::cm, 3 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicMagnetCoil1 = new G4LogicalVolume(solidMagnetCoil1, materials.getMaterial("G4_Cu"), "logicMagnetCoil1", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 39.4 * CLHEP::cm), logicMagnetCoil1, "physiMagnetCoil1", logicWorld, false, 0);
	logicMagnetCoil1->SetVisAttributes(VisG4_Cu);
	return logicMagnetCoil1;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2MagnetCoil2()
{
	G4Tubs* solidMagnetCoil2 = new G4Tubs("solidMagnetCoil2", 8.32 * CLHEP::cm, 10.285 * CLHEP::cm, 3 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicMagnetCoil2 = new G4LogicalVolume(solidMagnetCoil2, materials.getMaterial("G4_Cu"), "logicMagnetCoil2", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 39.4 * CLHEP::cm), logicMagnetCoil2, "physiMagnetCoil2", logicWorld, false, 0);
	logicMagnetCoil2->SetVisAttributes(VisG4_Cu);
	return logicMagnetCoil2;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2MagnetCoil3()
{
	G4Tubs* solidMagnetCoil3 = new G4Tubs("solidMagnetCoil3", 7 * CLHEP::cm, 7.885 * CLHEP::cm, 15 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicMagnetCoil3 = new G4LogicalVolume(solidMagnetCoil3, materials.getMaterial("G4_Cu"), "logicMagnetCoil3", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 20.9 * CLHEP::cm), logicMagnetCoil3, "physiMagnetCoil3", logicWorld, false, 0);
	logicMagnetCoil3->SetVisAttributes(VisG4_Cu);
	return logicMagnetCoil3;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2MagnetCoil4()
{
	G4Tubs* solidMagnetCoil4 = new G4Tubs("solidMagnetCoil4", 7.91 * CLHEP::cm, 8.96 * CLHEP::cm, 15 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicMagnetCoil4 = new G4LogicalVolume(solidMagnetCoil4, materials.getMaterial("G4_Cu"), "logicMagnetCoil4", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 20.9 * CLHEP::cm), logicMagnetCoil4, "physiMagnetCoil4", logicWorld, false, 0);
	logicMagnetCoil4->SetVisAttributes(VisG4_Cu);
	return logicMagnetCoil4;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2MagnetCoil5()
{
	G4Tubs* solidMagnetCoil5 = new G4Tubs("solidMagnetCoil5", 7 * CLHEP::cm, 7.57 * CLHEP::cm, 0.75 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicMagnetCoil5 = new G4LogicalVolume(solidMagnetCoil5, materials.getMaterial("G4_Cu"), "logicMagnetCoil5", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 4.65 * CLHEP::cm), logicMagnetCoil5, "physiMagnetCoil5", logicWorld, false, 0);
	logicMagnetCoil5->SetVisAttributes(VisG4_Cu);
	return logicMagnetCoil5;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2MagnetCoil6()
{
	G4Tubs* solidMagnetCoil6 = new G4Tubs("solidMagnetCoil6", 7.605 * CLHEP::cm, 9.277 * CLHEP::cm, 0.75 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicMagnetCoil6 = new G4LogicalVolume(solidMagnetCoil6, materials.getMaterial("G4_Cu"), "logicMagnetCoil6", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 4.65 * CLHEP::cm), logicMagnetCoil6, "physiMagnetCoil6", logicWorld, false, 0);
	logicMagnetCoil6->SetVisAttributes(VisG4_Cu);
	return logicMagnetCoil6;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2MagnetCoil7()
{
	G4Tubs* solidMagnetCoil7 = new G4Tubs("solidMagnetCoil7", 9.317 * CLHEP::cm, 10.46 * CLHEP::cm, 0.75 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicMagnetCoil7 = new G4LogicalVolume(solidMagnetCoil7, materials.getMaterial("G4_Cu"), "logicMagnetCoil7", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 4.65 * CLHEP::cm), logicMagnetCoil7, "physiMagnetCoil7", logicWorld, false, 0);
	logicMagnetCoil7->SetVisAttributes(VisG4_Cu);
	return logicMagnetCoil7;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2AluminumPlug()
{
	G4Tubs* solidAluminumPlug = new G4Tubs("solidAluminumPlug", 0 * CLHEP::cm, 2.0637 * CLHEP::cm, 2.5 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicAluminumPlug = new G4LogicalVolume(solidAluminumPlug, materials.getMaterial("G4_Al"), "logicAluminumPlug", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 64.5 * CLHEP::cm), logicAluminumPlug, "physiAluminumPlug", logicWorld, false, 0);
	logicAluminumPlug->SetVisAttributes(VisG4_Al);
	return logicAluminumPlug;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2FarAluminumTube()
{
	G4Tubs* solidFarAluminumTube = new G4Tubs("solidFarAluminumTube", 2.0637 * CLHEP::cm, 2.2225 * CLHEP::cm, 13.97 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicFarAluminumTube = new G4LogicalVolume(solidFarAluminumTube, materials.getMaterial("G4_Al"), "logicFarAluminumTube", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 60.02 * CLHEP::cm), logicFarAluminumTube, "physiFarAluminumTube", logicWorld, false, 0);
	logicFarAluminumTube->SetVisAttributes(VisG4_Al);
	return logicFarAluminumTube;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2DSCryostatFlangeA()
{
	G4Tubs* solidDSCryostatFlangeA = new G4Tubs("solidDSCryostatFlangeA", 7.70255 * CLHEP::cm, 13.49375 * CLHEP::cm, 0.47371 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicDSCryostatFlangeA = new G4LogicalVolume(solidDSCryostatFlangeA, materials.getMaterial("StainlessSteel304"), "logicDSCryostatFlangeA", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 45.75407 * CLHEP::cm), logicDSCryostatFlangeA, "physiDSCryostatFlangeA", logicWorld, false, 0);
	logicDSCryostatFlangeA->SetVisAttributes(VisStainlessSteel304);
	return logicDSCryostatFlangeA;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2DSCryostatFlangeB()
{
	G4Tubs* solidDSCryostatFlangeB = new G4Tubs("solidDSCryostatFlangeB", 0, 18.7452 * CLHEP::cm, 0.635 * CLHEP::cm, 0., CLHEP::twopi);
	G4Tubs* solidDSCrysostateFlangeBSubtractionSolid = new G4Tubs("solidDSCrysostateFlangeBSubtractionSolid", 0, 6.11124 * CLHEP::cm, 0.7 * CLHEP::cm, 0., CLHEP::twopi);
	G4VSolid* solidDSCryostatFlangeBWHole = new G4SubtractionSolid("solidDSCryostatFlangeBWHole", solidDSCryostatFlangeB, solidDSCrysostateFlangeBSubtractionSolid, 0, G4ThreeVector(5.715 * CLHEP::cm, 0, 0 * CLHEP::cm));

	G4LogicalVolume* logicDSCryostatFlangeB = new G4LogicalVolume(solidDSCryostatFlangeBWHole, materials.getMaterial("StainlessSteel304"), "logicDSCryostatFlangeB", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(-5.715 * CLHEP::cm, 0 * CLHEP::cm, 44.64536 * CLHEP::cm), logicDSCryostatFlangeB, "physiDSCryostatFlangeB", logicWorld, false, 0);
	logicDSCryostatFlangeB->SetVisAttributes(VisStainlessSteel304);
	return logicDSCryostatFlangeB;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CoilTray1PartH()
{
	G4Tubs* solidCoilTray1PartH = new G4Tubs("solidCoilTray1PartH", 6.35 * CLHEP::cm, 6.975475 * CLHEP::cm, 20.32 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCoilTray1PartH = new G4LogicalVolume(solidCoilTray1PartH, materials.getMaterial("G4_Al"), "logicCoilTray1PartH", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 23.40334 * CLHEP::cm), logicCoilTray1PartH, "physiCoilTray1PartH", logicWorld, false, 0);
	logicCoilTray1PartH->SetVisAttributes(VisG4_Al);
	return logicCoilTray1PartH;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CoilTray1PartI()
{
	G4Tubs* solidCoilTray1PartI = new G4Tubs("solidCoilTray1PartI", 6.975475 * CLHEP::cm, 12.7 * CLHEP::cm, 0.66167 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCoilTray1PartI = new G4LogicalVolume(solidCoilTray1PartI, materials.getMaterial("G4_Al"), "logicCoilTray1PartI", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 43.06167 * CLHEP::cm), logicCoilTray1PartI, "physiCoilTray1PartI", logicWorld, false, 0);
	logicCoilTray1PartI->SetVisAttributes(VisG4_Al);
	return logicCoilTray1PartI;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CoilTray1PartK()
{
	G4Tubs* solidCoilTray1PartK = new G4Tubs("solidCoilTray1PartK", 6.975475 * CLHEP::cm, 10.4775 * CLHEP::cm, 0.23495 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCoilTray1PartK = new G4LogicalVolume(solidCoilTray1PartK, materials.getMaterial("G4_Al"), "logicCoilTray1PartK", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 36.13763 * CLHEP::cm), logicCoilTray1PartK, "physiCoilTray1PartK", logicWorld, false, 0);
	logicCoilTray1PartK->SetVisAttributes(VisG4_Al);
	return logicCoilTray1PartK;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CoilTray1PartL()
{
	G4Tubs* solidCoilTray1PartL = new G4Tubs("solidCoilTray1PartL", 6.975475 * CLHEP::cm, 10.4775 * CLHEP::cm, 0.233584999999998 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCoilTray1PartL = new G4LogicalVolume(solidCoilTray1PartL, materials.getMaterial("G4_Al"), "logicCoilTray1PartL", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 5.633595 * CLHEP::cm), logicCoilTray1PartL, "physiCoilTray1PartL", logicWorld, false, 0);
	logicCoilTray1PartL->SetVisAttributes(VisG4_Al);
	return logicCoilTray1PartL;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CoilTray1PartM()
{
	G4Tubs* solidCoilTray1PartM = new G4Tubs("solidCoilTray1PartM", 6.975475 * CLHEP::cm, 12.7 * CLHEP::cm, 0.3937 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCoilTray1PartM = new G4LogicalVolume(solidCoilTray1PartM, materials.getMaterial("G4_Al"), "logicCoilTray1PartM", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 3.47704 * CLHEP::cm), logicCoilTray1PartM, "physiCoilTray1PartM", logicWorld, false, 0);
	logicCoilTray1PartM->SetVisAttributes(VisG4_Al);
	return logicCoilTray1PartM;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2BeamCenter()
{
	G4Tubs* solidBeamCenter = new G4Tubs("solidBeamCenter", 0 * CLHEP::cm, 1.7 * CLHEP::cm, 31 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicBeamCenter = new G4LogicalVolume(solidBeamCenter, materials.getMaterial("G4_Galactic"), "logicBeamCenter", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 31 * CLHEP::cm), logicBeamCenter, "physiBeamCenter", logicWorld, false, 0);
	logicBeamCenter->SetVisAttributes(VisG4_Galactic);
	return logicBeamCenter;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2ReentrantTube()
{
	G4Tubs* solidReentrantTube = new G4Tubs("solidReentrantTube", 1.2319 * CLHEP::cm, 1.27 * CLHEP::cm, 14.14 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicReentrantTube = new G4LogicalVolume(solidReentrantTube, materials.getMaterial("StainlessSteel304"), "logicReentrantTube", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 18.25 * CLHEP::cm) + WeakSourceOffset, logicReentrantTube, "physiReentrantTube", logicWorld, false, 0);
	logicReentrantTube->SetVisAttributes(VisStainlessSteel304);
	return logicReentrantTube;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2ReentrantTubeWeld()
{
	G4Tubs* solidReentrantTubeWeld = new G4Tubs("solidReentrantTubeWeld", 1.27 * CLHEP::cm, 1.524 * CLHEP::cm, 0.635 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicReentrantTubeWeld = new G4LogicalVolume(solidReentrantTubeWeld, materials.getMaterial("StainlessSteel304"), "logicReentrantTubeWeld", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 15.765 * CLHEP::cm) + WeakSourceOffset, logicReentrantTubeWeld, "physiReentrantTubeWeld", logicWorld, false, 0);
	logicReentrantTubeWeld->SetVisAttributes(VisStainlessSteel304);
	return logicReentrantTubeWeld;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SourceStick()
{
	G4Tubs* solidSourceStick = new G4Tubs("solidSourceStick", 0.582 * CLHEP::cm, 0.635 * CLHEP::cm, 12.4425 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicSourceStick = new G4LogicalVolume(solidSourceStick, materials.getMaterial("G4_Fe"), "logicSourceStick", 0, 0, 0);
	new G4PVPlacement(0, G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 41.9575 * CLHEP::cm) + WeakSourceOffset, logicSourceStick, "physiSourceStick", logicWorld, false, 0);
	logicSourceStick->SetVisAttributes(VisG4_Fe);
	return logicSourceStick;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2WeakSource()
{
	G4double weakSourceLength = 7.49 * CLHEP::cm;
	if(useDoubleWeakSource)
	{
		weakSourceLength *= 2.;
	}
	G4ThreeVector weakSourceCenter = G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 29.515 * CLHEP::cm - weakSourceLength * .5) + WeakSourceOffset;
	G4Tubs* solidWeakSource = new G4Tubs("solidWeakSource", 0 * CLHEP::cm, 0.635 * CLHEP::cm, weakSourceLength * .5, 0., CLHEP::twopi);
	G4LogicalVolume* logicWeakSource = new G4LogicalVolume(solidWeakSource, materials.getMaterial("G4_POLYCARBONATE"), "logicWeakSource", 0, 0, 0);
	new G4PVPlacement(0, weakSourceCenter, logicWeakSource, "physiWeakSource", logicWorld, false, 0);
	logicWeakSource->SetVisAttributes(VisG4_POLYCARBONATE);

//	G4Tubs* solidBaSalts = new G4Tubs("solidBaSalts",0*CLHEP::cm,.5*4.75*CLHEP::mm,1.*CLHEP::mm,0., twopi);
//	G4LogicalVolume* logicBaSalts = new G4LogicalVolume(solidBaSalts,materials->GetMaterial("G4_Ba"),"logicBaSalts",0,0,0);
//	new G4PVPlacement(0, G4ThreeVector(0*CLHEP::cm,0*CLHEP::cm,-3.745*CLHEP::cm+4.75*CLHEP::mm-1.0*CLHEP::mm)+WeakSourceOffset,logicBaSalts,"physiBaSalts",logicWeakSource,false,0);
//	logicBaSalts->SetVisAttributes(VisRadioactive);

	return logicWeakSource;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CopperGroundingStrapPart1()
{
	G4Tubs* solidCopperGroundingStrapPart1 = new G4Tubs("solidCopperGroundingStrapPart1", 5.12 * CLHEP::cm, 5.1762 * CLHEP::cm, 0.1455 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCopperGroundingStrapPart1 = new G4LogicalVolume(solidCopperGroundingStrapPart1, materials.getMaterial("G4_Cu"), "logicCopperGroundingStrapPart1", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 31.6925 * CLHEP::cm), logicCopperGroundingStrapPart1, "physiCopperGroundingStrapPart1", logicWorld, false, 0);
	logicCopperGroundingStrapPart1->SetVisAttributes(VisG4_Cu);
	return logicCopperGroundingStrapPart1;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CopperGroundingStrapPart2()
{
	G4Tubs* solidCopperGroundingStrapPart2 = new G4Tubs("solidCopperGroundingStrapPart2", 5.12 * CLHEP::cm, 5.1762 * CLHEP::cm, 0.1455 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCopperGroundingStrapPart2 = new G4LogicalVolume(solidCopperGroundingStrapPart2, materials.getMaterial("G4_Cu"), "logicCopperGroundingStrapPart2", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 28.9918 * CLHEP::cm), logicCopperGroundingStrapPart2, "physiCopperGroundingStrapPart2", logicWorld, false, 0);
	logicCopperGroundingStrapPart2->SetVisAttributes(VisG4_Cu);
	return logicCopperGroundingStrapPart2;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CopperGroundingStrapPart3()
{
	G4Tubs* solidCopperGroundingStrapPart3 = new G4Tubs("solidCopperGroundingStrapPart3", 5.12 * CLHEP::cm, 5.1762 * CLHEP::cm, 0.1455 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCopperGroundingStrapPart3 = new G4LogicalVolume(solidCopperGroundingStrapPart3, materials.getMaterial("G4_Cu"), "logicCopperGroundingStrapPart3", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 26.2911 * CLHEP::cm), logicCopperGroundingStrapPart3, "physiCopperGroundingStrapPart3", logicWorld, false, 0);
	logicCopperGroundingStrapPart3->SetVisAttributes(VisG4_Cu);
	return logicCopperGroundingStrapPart3;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CopperGroundingStrapPart4()
{
	G4Tubs* solidCopperGroundingStrapPart4 = new G4Tubs("solidCopperGroundingStrapPart4", 5.12 * CLHEP::cm, 5.1762 * CLHEP::cm, 0.1455 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCopperGroundingStrapPart4 = new G4LogicalVolume(solidCopperGroundingStrapPart4, materials.getMaterial("G4_Cu"), "logicCopperGroundingStrapPart4", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 23.5904 * CLHEP::cm), logicCopperGroundingStrapPart4, "physiCopperGroundingStrapPart4", logicWorld, false, 0);
	logicCopperGroundingStrapPart4->SetVisAttributes(VisG4_Cu);
	return logicCopperGroundingStrapPart4;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CopperGroundingStrapPart5()
{
	G4Tubs* solidCopperGroundingStrapPart5 = new G4Tubs("solidCopperGroundingStrapPart5", 5.12 * CLHEP::cm, 5.1762 * CLHEP::cm, 0.1455 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCopperGroundingStrapPart5 = new G4LogicalVolume(solidCopperGroundingStrapPart5, materials.getMaterial("G4_Cu"), "logicCopperGroundingStrapPart5", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 20.8897 * CLHEP::cm), logicCopperGroundingStrapPart5, "physiCopperGroundingStrapPart5", logicWorld, false, 0);
	logicCopperGroundingStrapPart5->SetVisAttributes(VisG4_Cu);
	return logicCopperGroundingStrapPart5;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CopperGroundingStrapPart6()
{
	G4Tubs* solidCopperGroundingStrapPart6 = new G4Tubs("solidCopperGroundingStrapPart6", 5.12 * CLHEP::cm, 5.1762 * CLHEP::cm, 0.1455 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCopperGroundingStrapPart6 = new G4LogicalVolume(solidCopperGroundingStrapPart6, materials.getMaterial("G4_Cu"), "logicCopperGroundingStrapPart6", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 18.189 * CLHEP::cm), logicCopperGroundingStrapPart6, "physiCopperGroundingStrapPart6", logicWorld, false, 0);
	logicCopperGroundingStrapPart6->SetVisAttributes(VisG4_Cu);
	return logicCopperGroundingStrapPart6;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CopperGroundingStrapPart7()
{
	G4Tubs* solidCopperGroundingStrapPart7 = new G4Tubs("solidCopperGroundingStrapPart7", 5.12 * CLHEP::cm, 5.1762 * CLHEP::cm, 0.1455 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCopperGroundingStrapPart7 = new G4LogicalVolume(solidCopperGroundingStrapPart7, materials.getMaterial("G4_Cu"), "logicCopperGroundingStrapPart7", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 15.4883 * CLHEP::cm), logicCopperGroundingStrapPart7, "physiCopperGroundingStrapPart7", logicWorld, false, 0);
	logicCopperGroundingStrapPart7->SetVisAttributes(VisG4_Cu);
	return logicCopperGroundingStrapPart7;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CopperGroundingStrapPart8()
{
	G4Tubs* solidCopperGroundingStrapPart8 = new G4Tubs("solidCopperGroundingStrapPart8", 5.12 * CLHEP::cm, 5.1762 * CLHEP::cm, 0.1455 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCopperGroundingStrapPart8 = new G4LogicalVolume(solidCopperGroundingStrapPart8, materials.getMaterial("G4_Cu"), "logicCopperGroundingStrapPart8", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 12.7876 * CLHEP::cm), logicCopperGroundingStrapPart8, "physiCopperGroundingStrapPart8", logicWorld, false, 0);
	logicCopperGroundingStrapPart8->SetVisAttributes(VisG4_Cu);
	return logicCopperGroundingStrapPart8;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CopperGroundingStrapPart9()
{
	G4Tubs* solidCopperGroundingStrapPart9 = new G4Tubs("solidCopperGroundingStrapPart9", 5.4 * CLHEP::cm, 5.4562 * CLHEP::cm, 0.1455 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCopperGroundingStrapPart9 = new G4LogicalVolume(solidCopperGroundingStrapPart9, materials.getMaterial("G4_Cu"), "logicCopperGroundingStrapPart9", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 34.3932 * CLHEP::cm), logicCopperGroundingStrapPart9, "physiCopperGroundingStrapPart9", logicWorld, false, 0);
	logicCopperGroundingStrapPart9->SetVisAttributes(VisG4_Cu);
	return logicCopperGroundingStrapPart9;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CopperGroundingStrapPart10()
{
	G4Tubs* solidCopperGroundingStrapPart10 = new G4Tubs("solidCopperGroundingStrapPart10", 3.5 * CLHEP::cm, 3.5562 * CLHEP::cm, 0.1455 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCopperGroundingStrapPart10 = new G4LogicalVolume(solidCopperGroundingStrapPart10, materials.getMaterial("G4_Cu"), "logicCopperGroundingStrapPart10", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 37.0939 * CLHEP::cm), logicCopperGroundingStrapPart10, "physiCopperGroundingStrapPart10", logicWorld, false, 0);
	logicCopperGroundingStrapPart10->SetVisAttributes(VisG4_Cu);
	return logicCopperGroundingStrapPart10;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CopperGroundingStrapPart11()
{
	G4Tubs* solidCopperGroundingStrapPart11 = new G4Tubs("solidCopperGroundingStrapPart11", 3.5 * CLHEP::cm, 3.5562 * CLHEP::cm, 0.1455 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCopperGroundingStrapPart11 = new G4LogicalVolume(solidCopperGroundingStrapPart11, materials.getMaterial("G4_Cu"), "logicCopperGroundingStrapPart11", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 39.7946 * CLHEP::cm), logicCopperGroundingStrapPart11, "physiCopperGroundingStrapPart11", logicWorld, false, 0);
	logicCopperGroundingStrapPart11->SetVisAttributes(VisG4_Cu);
	return logicCopperGroundingStrapPart11;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CopperGroundingStrapPart12()
{
	G4Tubs* solidCopperGroundingStrapPart12 = new G4Tubs("solidCopperGroundingStrapPart12", 3.5 * CLHEP::cm, 3.5562 * CLHEP::cm, 0.1455 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCopperGroundingStrapPart12 = new G4LogicalVolume(solidCopperGroundingStrapPart12, materials.getMaterial("G4_Cu"), "logicCopperGroundingStrapPart12", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 0 * CLHEP::cm, 42.4953 * CLHEP::cm), logicCopperGroundingStrapPart12, "physiCopperGroundingStrapPart12", logicWorld, false, 0);
	logicCopperGroundingStrapPart12->SetVisAttributes(VisG4_Cu);
	return logicCopperGroundingStrapPart12;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2LargeBentBore()
{
	G4Tubs* solidLargeBentBore = new G4Tubs("solidLargeBentBore", 7.03834 * CLHEP::cm, 7.3025 * CLHEP::cm, 5.08 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicLargeBentBore = new G4LogicalVolume(solidLargeBentBore, materials.getMaterial("StainlessSteel304"), "logicLargeBentBore", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], G4ThreeVector(-5.07676532771093 * CLHEP::cm, 0 * CLHEP::cm, -30.3375533319243 * CLHEP::cm), logicLargeBentBore, "physiLargeBentBore", logicWorld, false, 0);
	logicLargeBentBore->SetVisAttributes(VisStainlessSteel304);
	return logicLargeBentBore;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SpacerRing()
{
	G4Tubs* solidSpacerRing = new G4Tubs("solidSpacerRing", 6.115 * CLHEP::cm, 7.038339 * CLHEP::cm, 0.4064 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicSpacerRing = new G4LogicalVolume(solidSpacerRing, materials.getMaterial("StainlessSteel304"), "logicSpacerRing", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], G4ThreeVector(-4.30539883696046 * CLHEP::cm, 0 * CLHEP::cm, -25.7280489445799 * CLHEP::cm), logicSpacerRing, "physiSpacerRing", logicWorld, false, 0);
	logicSpacerRing->SetVisAttributes(VisStainlessSteel304);
	return logicSpacerRing;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2MagnetCoil9()
{
	G4Tubs* solidMagnetCoil9 = new G4Tubs("solidMagnetCoil9", 7 * CLHEP::cm, 10.756 * CLHEP::cm, 0.75 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicMagnetCoil9 = new G4LogicalVolume(solidMagnetCoil9, materials.getMaterial("G4_Cu"), "logicMagnetCoil9", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], G4ThreeVector(-0.767471367252151 * CLHEP::cm, 0 * CLHEP::cm, -4.58622804714813 * CLHEP::cm), logicMagnetCoil9, "physiMagnetCoil9", logicWorld, false, 0);
	logicMagnetCoil9->SetVisAttributes(VisG4_Cu);
	return logicMagnetCoil9;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2MagnetCoil10()
{
	G4Tubs* solidMagnetCoil10 = new G4Tubs("solidMagnetCoil10", 7 * CLHEP::cm, 8.58 * CLHEP::cm, 8.75 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicMagnetCoil10 = new G4LogicalVolume(solidMagnetCoil10, materials.getMaterial("G4_Cu"), "logicMagnetCoil10", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], G4ThreeVector(-2.41794742585893 * CLHEP::cm, 0 * CLHEP::cm, -14.4490840625204 * CLHEP::cm), logicMagnetCoil10, "physiMagnetCoil10", logicWorld, false, 0);
	logicMagnetCoil10->SetVisAttributes(VisG4_Cu);
	return logicMagnetCoil10;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2MagnetCoil11()
{
	G4Tubs* solidMagnetCoil11 = new G4Tubs("solidMagnetCoil11", 8 * CLHEP::cm, 10.52 * CLHEP::cm, 3.375 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicMagnetCoil11 = new G4LogicalVolume(solidMagnetCoil11, materials.getMaterial("G4_Cu"), "logicMagnetCoil11", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], G4ThreeVector(-4.50167344984998 * CLHEP::cm, 0 * CLHEP::cm, -26.900939781928 * CLHEP::cm), logicMagnetCoil11, "physiMagnetCoil11", logicWorld, false, 0);
	logicMagnetCoil11->SetVisAttributes(VisG4_Cu);
	return logicMagnetCoil11;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SBDSiliconHolder()
{
	G4ThreeVector position = G4ThreeVector(-4.59465367880202 * CLHEP::cm, 0 * CLHEP::cm, -27.4565677206964 * CLHEP::cm) + sbdDetectorOffset;
	G4Tubs* solidSBD1mm5SiliconHolder = new G4Tubs("solidSBD1mm5SiliconHolder", 1.382 * CLHEP::cm, 1.74719 * CLHEP::cm, 0.24384 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicSBD1mm5SiliconHolder = new G4LogicalVolume(solidSBD1mm5SiliconHolder, materials.getMaterial("G4_ALUMINUM_OXIDE"), "logicSBD1mm5SiliconHolder", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], gammaDetOffset + position, logicSBD1mm5SiliconHolder, "physiSBD1mm5SiliconHolder", logicWorld, false, 0);
	G4UserLimits* stepLimit = new G4UserLimits(1 * CLHEP::nm);
	logicSBD1mm5SiliconHolder->SetUserLimits(stepLimit);
	logicSBD1mm5SiliconHolder->SetVisAttributes(VisG4_ALUMINUM_OXIDE);
	return logicSBD1mm5SiliconHolder;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SBDSilicon()
{
	double shrink = SiDeadLayerLength;  //Amount to shrink length SBD Silicon
	if(!use1_5mmSBD)
	{
		shrink += 0.5 * CLHEP::mm;
	}

	G4ThreeVector position = G4ThreeVector(-4.60070201336639 * CLHEP::cm, 0 * CLHEP::cm, -27.4927111428503 * CLHEP::cm) + sbdDetectorOffset + G4ThreeVector(-SIN_BEND * shrink * .5, 0, -COS_BEND * shrink * .5);
	G4Tubs* solidSBD1mm5Silicon = new G4Tubs("solidSBD1mm5Silicon", 0 * CLHEP::cm, 1.382 * CLHEP::cm, 0.0749999999999993 * CLHEP::cm - 0.5 * shrink, 0., CLHEP::twopi);
	G4LogicalVolume* logicSBD1mm5Silicon = new G4LogicalVolume(solidSBD1mm5Silicon, materials.getMaterial("G4_Si"), "logicSBD1mm5Silicon", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], position, logicSBD1mm5Silicon, "physiSBD1mm5Silicon", logicWorld, false, 0);
	G4UserLimits* stepLimit = new G4UserLimits(1 * CLHEP::nm);
	logicSBD1mm5Silicon->SetUserLimits(stepLimit);
	logicSBD1mm5Silicon->SetVisAttributes(VisG4_Si);
	return logicSBD1mm5Silicon;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SBDDeadLayer()
{
	G4ThreeVector position = G4ThreeVector(-4.5883232779 * CLHEP::cm, 0 * CLHEP::cm, -27.4187387364 * CLHEP::cm) + sbdDetectorOffset; //Get to center of gold layer
	position += G4ThreeVector(-0.1650476059 * 10 * CLHEP::nm, 0, -0.9862856015 * 10 * CLHEP::nm); //shift from gold layer position
	position += G4ThreeVector(-0.1650476059 * SiDeadLayerLength * .5, 0, -0.9862856015 * SiDeadLayerLength * .5); //shift based on deadlayer thickness
	G4Tubs* solidSBDDeadLayer = new G4Tubs("solidSBDDeadLayer", 0 * CLHEP::cm, 1.382 * CLHEP::cm, 0.5 * SiDeadLayerLength, 0., CLHEP::twopi);
	G4LogicalVolume* logicSBDDeadLayer = new G4LogicalVolume(solidSBDDeadLayer, materials.getMaterial("G4_SILICON_DIOXIDE"), "logicSBDDeadLayer", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], position, logicSBDDeadLayer, "physiSBDDeadLayer", logicWorld, false, 0);
	G4UserLimits* stepLimit = new G4UserLimits(.001 * CLHEP::nm);
	logicSBDDeadLayer->SetUserLimits(stepLimit);
	logicSBDDeadLayer->SetVisAttributes(VisG4_Si);
	return logicSBDDeadLayer;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SBDGoldCoating()
{
	G4ThreeVector position = G4ThreeVector(-4.5883232779 * CLHEP::cm, 0 * CLHEP::cm, -27.4187387364 * CLHEP::cm) + sbdDetectorOffset;
	G4Tubs* solidSBD1mm5GoldCoating = new G4Tubs("solidSBD1mm5GoldCoating", 0 * CLHEP::cm, 1.382 * CLHEP::cm, 0.000001 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicSBD1mm5GoldCoating = new G4LogicalVolume(solidSBD1mm5GoldCoating, materials.getMaterial("G4_Au"), "logicSBD1mm5GoldCoating", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], position, logicSBD1mm5GoldCoating, "physiSBD1mm5GoldCoating", logicWorld, false, 0);
	G4UserLimits* stepLimit = new G4UserLimits(.001 * CLHEP::nm);
	logicSBD1mm5GoldCoating->SetUserLimits(stepLimit);
	logicSBD1mm5GoldCoating->SetVisAttributes(VisG4_Au);
	return logicSBD1mm5GoldCoating;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SBDAluminumCoating()
{
	double shrink = 0; //Amount to shrink length SBD Silicon
	if(!use1_5mmSBD)
	{
		shrink = 0.5 * CLHEP::mm;
	}
	G4ThreeVector position = G4ThreeVector(-4.61308966142426 * CLHEP::cm, 0 * CLHEP::cm, -27.5667368086737 * CLHEP::cm) + sbdDetectorOffset + G4ThreeVector(0.1650476059 * shrink * .5, 0, 0.9862856015 * shrink * .5);
	G4Tubs* solidSBD1mm5AluminumCoating = new G4Tubs("solidSBD1mm5AluminumCoating", 0 * CLHEP::cm, 1.382 * CLHEP::cm, 0.0000549999999996942 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicSBD1mm5AluminumCoating = new G4LogicalVolume(solidSBD1mm5AluminumCoating, materials.getMaterial("G4_Al"), "logicSBD1mm5AluminumCoating", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], position, logicSBD1mm5AluminumCoating, "physiSBD1mm5AluminumCoating", logicWorld, false, 0);
	logicSBD1mm5AluminumCoating->SetVisAttributes(VisG4_Al);
	return logicSBD1mm5AluminumCoating;
}
//
//G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SBD1mmSilicon()
//{
//    G4ThreeVector position=G4ThreeVector(-4.60070201336639*CLHEP::cm,0*CLHEP::cm,-27.4927111428503*CLHEP::cm)+sbdDetectorOffset;
//	G4Tubs* solidSBD1mm5Silicon =  new G4Tubs("solidSBD1mm5Silicon",0*CLHEP::cm,1.382*CLHEP::cm,0.0749999999999993*CLHEP::cm,0., twopi);
//	G4LogicalVolume* logicSBD1mm5Silicon = new G4LogicalVolume(solidSBD1mm5Silicon,materials->GetMaterial("G4_Si"),"logicSBD1mm5Silicon",0,0,0);
//	new G4PVPlacement(rdk2RotationMatrices[1], position,logicSBD1mm5Silicon,"physiSBD1mm5Silicon",logicWorld,false,0);
//	G4UserLimits* stepLimit=new G4UserLimits(1*CLHEP::nm);
//    logicSBD1mm5Silicon->SetUserLimits(stepLimit);
//	logicSBD1mm5Silicon->SetVisAttributes(VisG4_Si);
//	return logicSBD1mm5Silicon;
//}
//
//G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SBD1mmGoldCoating()
//{
//    G4ThreeVector position=G4ThreeVector(-4.58832179245078*CLHEP::cm,0*CLHEP::cm,-27.418729859879*CLHEP::cm)+sbdDetectorOffset;
//	G4Tubs* solidSBD1mm5GoldCoating =  new G4Tubs("solidSBD1mm5GoldCoating",0*CLHEP::cm,1.382*CLHEP::cm,0.000001*CLHEP::cm,0., twopi);
//	G4LogicalVolume* logicSBD1mm5GoldCoating = new G4LogicalVolume(solidSBD1mm5GoldCoating,materials->GetMaterial("G4_Au"),"logicSBD1mm5GoldCoating",0,0,0);
//	new G4PVPlacement(rdk2RotationMatrices[1], position,logicSBD1mm5GoldCoating,"physiSBD1mm5GoldCoating",logicWorld,false,0);
//	G4UserLimits* stepLimit=new G4UserLimits(1*CLHEP::nm);
//    logicSBD1mm5GoldCoating->SetUserLimits(stepLimit);
//	logicSBD1mm5GoldCoating->SetVisAttributes(VisG4_Au);
//	return logicSBD1mm5GoldCoating;
//}
//
//G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SBD1mmAluminumCoating()
//{
//    G4ThreeVector position=G4ThreeVector(-4.61308966142426*CLHEP::cm,0*CLHEP::cm,-27.5667368086737*CLHEP::cm)+sbdDetectorOffset;
//	G4Tubs* solidSBD1mm5AluminumCoating =  new G4Tubs("solidSBD1mm5AluminumCoating",0*CLHEP::cm,1.382*CLHEP::cm,0.0000549999999996942*CLHEP::cm,0., twopi);
//	G4LogicalVolume* logicSBD1mm5AluminumCoating = new G4LogicalVolume(solidSBD1mm5AluminumCoating,materials->GetMaterial("G4_Al"),"logicSBD1mm5AluminumCoating",0,0,0);
//	new G4PVPlacement(rdk2RotationMatrices[1], position,logicSBD1mm5AluminumCoating,"physiSBD1mm5AluminumCoating",logicWorld,false,0);
//	logicSBD1mm5AluminumCoating->SetVisAttributes(VisG4_Al);
//	return logicSBD1mm5AluminumCoating;
//}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SBDPlasticLining()
{
	G4ThreeVector position = G4ThreeVector(-4.59936413747328 * CLHEP::cm, 0 * CLHEP::cm, -27.4847163117643 * CLHEP::cm) + sbdDetectorOffset;
	G4Tubs* solidSBD1mm5PlasticLining = new G4Tubs("solidSBD1mm5PlasticLining", 1.74719 * CLHEP::cm, 1.75039 * CLHEP::cm, 0.27238 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicSBD1mm5PlasticLining = new G4LogicalVolume(solidSBD1mm5PlasticLining, materials.getMaterial("G4_POLYCARBONATE"), "logicSBD1mm5PlasticLining", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], position, logicSBD1mm5PlasticLining, "physiSBD1mm5PlasticLining", logicWorld, false, 0);
	logicSBD1mm5PlasticLining->SetVisAttributes(VisG4_POLYCARBONATE);
	return logicSBD1mm5PlasticLining;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SBDBackBrassRings()
{
	G4ThreeVector position = G4ThreeVector(-4.6374142125284 * CLHEP::cm, 0 * CLHEP::cm, -27.7120945943427 * CLHEP::cm) + sbdDetectorOffset;
	G4Tubs* solidSBD1mm5BackBrassRings = new G4Tubs("solidSBD1mm5BackBrassRings", 1.511138 * CLHEP::cm, 1.74719 * CLHEP::cm, 0.01524 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicSBD1mm5BackBrassRings = new G4LogicalVolume(solidSBD1mm5BackBrassRings, materials.getMaterial("CartridgeBrass"), "logicSBD1mm5BackBrassRings", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], position, logicSBD1mm5BackBrassRings, "physiSBD1mm5BackBrassRings", logicWorld, false, 0);
	logicSBD1mm5BackBrassRings->SetVisAttributes(VisCartridgeBrass);
	return logicSBD1mm5BackBrassRings;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SBDInternalBrassDisc()
{
	G4ThreeVector position = G4ThreeVector(-4.64212467119967 * CLHEP::cm, 0 * CLHEP::cm, -27.7402431854105 * CLHEP::cm) + sbdDetectorOffset;
	G4Tubs* solidSBD1mm5InternalBrassDisc = new G4Tubs("solidSBD1mm5InternalBrassDisc", 0.15748 * CLHEP::cm, 1.74719 * CLHEP::cm, 0.0132999999999992 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicSBD1mm5InternalBrassDisc = new G4LogicalVolume(solidSBD1mm5InternalBrassDisc, materials.getMaterial("CartridgeBrass"), "logicSBD1mm5InternalBrassDisc", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], position, logicSBD1mm5InternalBrassDisc, "physiSBD1mm5InternalBrassDisc", logicWorld, false, 0);
	logicSBD1mm5InternalBrassDisc->SetVisAttributes(VisCartridgeBrass);
	return logicSBD1mm5InternalBrassDisc;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SBDSpring()
{
	G4ThreeVector position = G4ThreeVector(-4.69479053699178 * CLHEP::cm, 0 * CLHEP::cm, -28.0549619894331 * CLHEP::cm) + sbdDetectorOffset;
	G4Tubs* solidSBD1mm5Spring = new G4Tubs("solidSBD1mm5Spring", 0.029 * CLHEP::cm, 0.222 * CLHEP::cm, 0.305795 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicSBD1mm5Spring = new G4LogicalVolume(solidSBD1mm5Spring, materials.getMaterial("CartridgeBrass"), "logicSBD1mm5Spring", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], position, logicSBD1mm5Spring, "physiSBD1mm5Spring", logicWorld, false, 0);
	logicSBD1mm5Spring->SetVisAttributes(VisCartridgeBrass);
	return logicSBD1mm5Spring;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SBDHexScrew()
{
	G4ThreeVector position = G4ThreeVector(-4.78142320007002 * CLHEP::cm, 0 * CLHEP::cm, -28.5726583702519 * CLHEP::cm) + sbdDetectorOffset;
	G4Tubs* solidSBD1mm5HexScrew = new G4Tubs("solidSBD1mm5HexScrew", 0.20955 * CLHEP::cm, 0.4178 * CLHEP::cm, 0.219099999999999 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicSBD1mm5HexScrew = new G4LogicalVolume(solidSBD1mm5HexScrew, materials.getMaterial("CartridgeBrass"), "logicSBD1mm5HexScrew", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], position, logicSBD1mm5HexScrew, "physiSBD1mm5HexScrew", logicWorld, false, 0);
	logicSBD1mm5HexScrew->SetVisAttributes(VisCartridgeBrass);
	return logicSBD1mm5HexScrew;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SBDFrontCase()
{
	G4ThreeVector position = G4ThreeVector(-4.54864418295427 * CLHEP::cm, 0 * CLHEP::cm, -27.1816258149839 * CLHEP::cm) + sbdDetectorOffset;
	G4Tubs* solidSBD1mm5FrontCase = new G4Tubs("solidSBD1mm5FrontCase", 1.511138 * CLHEP::cm, 1.805 * CLHEP::cm, 0.0349249999999994 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicSBD1mm5FrontCase = new G4LogicalVolume(solidSBD1mm5FrontCase, materials.getMaterial("CartridgeBrass"), "logicSBD1mm5FrontCase", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], position, logicSBD1mm5FrontCase, "physiSBD1mm5FrontCase", logicWorld, false, 0);
	G4UserLimits* stepLimit = new G4UserLimits(1 * CLHEP::nm);
	logicSBD1mm5FrontCase->SetUserLimits(stepLimit);
	logicSBD1mm5FrontCase->SetVisAttributes(VisCartridgeBrass);
	return logicSBD1mm5FrontCase;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SBDMidCase()
{
	G4ThreeVector position = G4ThreeVector(-4.65916005983858 * CLHEP::cm, 0 * CLHEP::cm, -27.8420426537732 * CLHEP::cm) + sbdDetectorOffset;
	G4Tubs* solidSBD1mm5MidCase = new G4Tubs("solidSBD1mm5MidCase", 1.75039 * CLHEP::cm, 1.805 * CLHEP::cm, 0.634675 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicSBD1mm5MidCase = new G4LogicalVolume(solidSBD1mm5MidCase, materials.getMaterial("CartridgeBrass"), "logicSBD1mm5MidCase", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], position, logicSBD1mm5MidCase, "physiSBD1mm5MidCase", logicWorld, false, 0);
	logicSBD1mm5MidCase->SetVisAttributes(VisCartridgeBrass);
	return logicSBD1mm5MidCase;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SBDBackCase()
{
	G4ThreeVector position = G4ThreeVector(-4.76810385827706 * CLHEP::cm, 0 * CLHEP::cm, -28.4930651222079 * CLHEP::cm) + sbdDetectorOffset;
	G4Tubs* solidSBD1mm5BackCase = new G4Tubs("solidSBD1mm5BackCase", 0.4178 * CLHEP::cm, 1.805 * CLHEP::cm, 0.0253999999999994 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicSBD1mm5BackCase = new G4LogicalVolume(solidSBD1mm5BackCase, materials.getMaterial("CartridgeBrass"), "logicSBD1mm5BackCase", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], position, logicSBD1mm5BackCase, "physiSBD1mm5BackCase", logicWorld, false, 0);
	logicSBD1mm5BackCase->SetVisAttributes(VisCartridgeBrass);
	return logicSBD1mm5BackCase;
}

void MRKRDKIIGeometry::constructSBDEpoxy()
{
	G4double SBDEpoxyLength = .16 * CLHEP::cm;
	G4double SBDEpoxyTopCenterZ = -27.799998 * CLHEP::cm + SBDEpoxyLength * .5;
	G4double SBDEpoxyBottomCenterZ = -27.9502 * CLHEP::cm - SBDEpoxyLength * .5;
	G4double SBDEpoxyInnerRadius2 = 1.372 * CLHEP::cm;
	G4double SBDEpoxyOuterRadius2 = 1.38 * CLHEP::cm;
	G4double SBDEpoxyInnerRadius1 = 1.3 * CLHEP::cm;
	G4double SBDEpoxyOuterRadius1 = 1.38 * CLHEP::cm;
	G4ThreeVector SBDEpoxyPositionTop = G4ThreeVector(SBDEpoxyTopCenterZ * sin(BEND_ANGLE * CLHEP::pi / 180.), 0, SBDEpoxyTopCenterZ * cos(BEND_ANGLE * CLHEP::pi / 180.));
	G4ThreeVector SBDEpoxyPositionBottom = G4ThreeVector(SBDEpoxyBottomCenterZ * sin(BEND_ANGLE * CLHEP::pi / 180.), 0, SBDEpoxyBottomCenterZ * cos(BEND_ANGLE * CLHEP::pi / 180.));
	G4Cons* solidSBDEpoxy = new G4Cons("solidSBDEpoxy", SBDEpoxyInnerRadius1, SBDEpoxyOuterRadius1, SBDEpoxyInnerRadius2, SBDEpoxyOuterRadius2, .5 * SBDEpoxyLength, 0., CLHEP::twopi);

	G4LogicalVolume* logicSBDEpoxy = new G4LogicalVolume(solidSBDEpoxy, materials.getMaterial("EpoxyResin"), "logicSBDEpoxy", 0, 0, 0);
	G4UserLimits* stepLimit = new G4UserLimits(1 * CLHEP::nm);
	logicSBDEpoxy->SetUserLimits(stepLimit);

	G4RotationMatrix* flippedBend = new G4RotationMatrix();
	flippedBend->rotateY((-BEND_ANGLE + 180) * deg);
	new G4PVPlacement(rdk2RotationMatrices[1], SBDEpoxyPositionTop + sbdDetectorOffset, logicSBDEpoxy, "physiSBDEpoxyTop", logicWorld, false, 0);
	new G4PVPlacement(flippedBend, SBDEpoxyPositionBottom + sbdDetectorOffset, logicSBDEpoxy, "physiSBDEpoxyBottom", logicWorld, false, 0);

	logicSBDEpoxy->SetVisAttributes(VisEpoxy);
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2BeOTube()
{
	G4ThreeVector position = G4ThreeVector(-6.00399155419902 * CLHEP::cm, 0 * CLHEP::cm, -35.8784387739396 * CLHEP::cm) + sbdDetectorOffset;
	G4Tubs* solidBeOTube = new G4Tubs("solidBeOTube", 2.88925 * CLHEP::cm, 3.048 * CLHEP::cm, 8.852668 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicBeOTube = new G4LogicalVolume(solidBeOTube, materials.getMaterial("G4_BERYLLIUM_OXIDE"), "logicBeOTube", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], position, logicBeOTube, "physiBeOTube", logicWorld, false, 0);
	logicBeOTube->SetVisAttributes(VisG4_BERYLLIUM_OXIDE);
	return logicBeOTube;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SBD1mm5StainlessSteelTube()
{
	G4ThreeVector position = G4ThreeVector(-6.12695202056522 * CLHEP::cm, 0 * CLHEP::cm, -36.6132215470848 * CLHEP::cm) + sbdDetectorOffset;
	G4Tubs* solidSBD1mm5StainlessSteelTube = new G4Tubs("solidSBD1mm5StainlessSteelTube", 1.2319 * CLHEP::cm, 1.27 * CLHEP::cm, 8.107668 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicSBD1mm5StainlessSteelTube = new G4LogicalVolume(solidSBD1mm5StainlessSteelTube, materials.getMaterial("StainlessSteel304"), "logicSBD1mm5StainlessSteelTube", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], position, logicSBD1mm5StainlessSteelTube, "physiSBD1mm5StainlessSteelTube", logicWorld, false, 0);
	logicSBD1mm5StainlessSteelTube->SetVisAttributes(VisStainlessSteel304);
	return logicSBD1mm5StainlessSteelTube;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CoilTray2PartA()
{
	G4Tubs* solidCoilTray2PartA = new G4Tubs("solidCoilTray2PartA", 6.35 * CLHEP::cm, 6.975475 * CLHEP::cm, 10.6299 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCoilTray2PartA = new G4LogicalVolume(solidCoilTray2PartA, materials.getMaterial("G4_Al"), "logicCoilTray2PartA", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], G4ThreeVector(-2.26333743059288 * CLHEP::cm, 0 * CLHEP::cm, -13.5251711624244 * CLHEP::cm), logicCoilTray2PartA, "physiCoilTray2PartA", logicWorld, false, 0);
	logicCoilTray2PartA->SetVisAttributes(VisG4_Al);
	return logicCoilTray2PartA;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CoilTray2PartB()
{
	G4Tubs* solidCoilTray2PartB = new G4Tubs("solidCoilTray2PartB", 7.4676 * CLHEP::cm, 7.9756 * CLHEP::cm, 4.04368 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCoilTray2PartB = new G4LogicalVolume(solidCoilTray2PartB, materials.getMaterial("G4_Al"), "logicCoilTray2PartB", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], G4ThreeVector(-4.60804002992296 * CLHEP::cm, 0 * CLHEP::cm, -27.5365614006947 * CLHEP::cm), logicCoilTray2PartB, "physiCoilTray2PartB", logicWorld, false, 0);
	logicCoilTray2PartB->SetVisAttributes(VisG4_Al);
	return logicCoilTray2PartB;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CoilTray2PartC()
{
	G4Tubs* solidCoilTray2PartC = new G4Tubs("solidCoilTray2PartC", 6.975475 * CLHEP::cm, 12.7 * CLHEP::cm, 0.39243 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCoilTray2PartC = new G4LogicalVolume(solidCoilTray2PartC, materials.getMaterial("G4_Al"), "logicCoilTray2PartC", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], G4ThreeVector(-0.573667517022367 * CLHEP::cm, 0 * CLHEP::cm, -3.42810190525506 * CLHEP::cm), logicCoilTray2PartC, "physiCoilTray2PartC", logicWorld, false, 0);
	logicCoilTray2PartC->SetVisAttributes(VisG4_Al);
	return logicCoilTray2PartC;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CoilTray2PartD()
{
	G4Tubs* solidCoilTray2PartD = new G4Tubs("solidCoilTray2PartD", 6.975475 * CLHEP::cm, 10.795 * CLHEP::cm, 0.23563 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCoilTray2PartD = new G4LogicalVolume(solidCoilTray2PartD, materials.getMaterial("G4_Al"), "logicCoilTray2PartD", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], G4ThreeVector(-0.930312286622471 * CLHEP::cm, 0 * CLHEP::cm, -5.55932701019281 * CLHEP::cm), logicCoilTray2PartD, "physiCoilTray2PartD", logicWorld, false, 0);
	logicCoilTray2PartD->SetVisAttributes(VisG4_Al);
	return logicCoilTray2PartD;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CoilTray2PartE()
{
	G4Tubs* solidCoilTray2PartE = new G4Tubs("solidCoilTray2PartE", 6.975475 * CLHEP::cm, 10.795 * CLHEP::cm, 0.23662 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCoilTray2PartE = new G4LogicalVolume(solidCoilTray2PartE, materials.getMaterial("G4_Al"), "logicCoilTray2PartE", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], G4ThreeVector(-3.90116754163861 * CLHEP::cm, 0 * CLHEP::cm, -23.312457975007 * CLHEP::cm), logicCoilTray2PartE, "physiCoilTray2PartE", logicWorld, false, 0);
	logicCoilTray2PartE->SetVisAttributes(VisG4_Al);
	return logicCoilTray2PartE;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CoilTray2PartF()
{
	G4Tubs* solidCoilTray2PartF = new G4Tubs("solidCoilTray2PartF", 7.9756 * CLHEP::cm, 11.1125 * CLHEP::cm, 0.65659 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCoilTray2PartF = new G4LogicalVolume(solidCoilTray2PartF, materials.getMaterial("G4_Al"), "logicCoilTray2PartF", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], G4ThreeVector(-5.16749034617648 * CLHEP::cm, 0 * CLHEP::cm, -30.8797046642333 * CLHEP::cm), logicCoilTray2PartF, "physiCoilTray2PartF", logicWorld, false, 0);
	logicCoilTray2PartF->SetVisAttributes(VisG4_Al);
	return logicCoilTray2PartF;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2OffAxisVacuum()
{
	G4Tubs* solidOffAxisVacuum = new G4Tubs("solidOffAxisVacuum", 0 * CLHEP::cm, 1.7 * CLHEP::cm, 11.25 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicOffAxisVacuum = new G4LogicalVolume(solidOffAxisVacuum, materials.getMaterial("G4_Galactic"), "logicOffAxisVacuum", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[1], G4ThreeVector(-2.35192838351466 * CLHEP::cm, 0 * CLHEP::cm, -14.0545698219056 * CLHEP::cm), logicOffAxisVacuum, "physiOffAxisVacuum", logicWorld, false, 0);
	logicOffAxisVacuum->SetVisAttributes(VisG4_Galactic);
	return logicOffAxisVacuum;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2MagnetCoil8()
{
	G4Tubs* solidMagnetCoil8 = new G4Tubs("solidMagnetCoil8", 7 * CLHEP::cm, 10.15 * CLHEP::cm, 1.75 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicMagnetCoil8 = new G4LogicalVolume(solidMagnetCoil8, materials.getMaterial("G4_Cu"), "logicMagnetCoil8", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[2], G4ThreeVector(-0.253127637634431 * CLHEP::cm, 0 * CLHEP::cm, 0.0210332847080999 * CLHEP::cm), logicMagnetCoil8, "physiMagnetCoil8", logicWorld, false, 0);
	logicMagnetCoil8->SetVisAttributes(VisG4_Cu);
	return logicMagnetCoil8;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CoilTray3PartN()
{
	G4Tubs* solidCoilTray3PartN = new G4Tubs("solidCoilTray3PartN", 6.58 * CLHEP::cm, 6.999 * CLHEP::cm, 2.4638 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCoilTray3PartN = new G4LogicalVolume(solidCoilTray3PartN, materials.getMaterial("G4_Al"), "logicCoilTray3PartN", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[2], G4ThreeVector(-0.253127637634431 * CLHEP::cm, 0 * CLHEP::cm, 0.0210332847080999 * CLHEP::cm), logicCoilTray3PartN, "physiCoilTray3PartN", logicWorld, false, 0);
	logicCoilTray3PartN->SetVisAttributes(VisG4_Al);
	return logicCoilTray3PartN;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CoilTray3PartO()
{
	G4Tubs* solidCoilTray3PartO = new G4Tubs("solidCoilTray3PartO", 10.95 * CLHEP::cm, 12.83 * CLHEP::cm, 1.9812 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCoilTray3PartO = new G4LogicalVolume(solidCoilTray3PartO, materials.getMaterial("G4_Al"), "logicCoilTray3PartO", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[2], G4ThreeVector(-0.253127637634431 * CLHEP::cm, 0 * CLHEP::cm, 0.0210332847080999 * CLHEP::cm), logicCoilTray3PartO, "physiCoilTray3PartO", logicWorld, false, 0);
	logicCoilTray3PartO->SetVisAttributes(VisG4_Al);
	return logicCoilTray3PartO;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CoilTray3PartP()
{
	G4Tubs* solidCoilTray3PartP = new G4Tubs("solidCoilTray3PartP", 7 * CLHEP::cm, 9 * CLHEP::cm, 0.25 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCoilTray3PartP = new G4LogicalVolume(solidCoilTray3PartP, materials.getMaterial("G4_Al"), "logicCoilTray3PartP", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[2], G4ThreeVector(-0.41874405265884 * CLHEP::cm, 0 * CLHEP::cm, -1.97209772028742 * CLHEP::cm), logicCoilTray3PartP, "physiCoilTray3PartP", logicWorld, false, 0);
	logicCoilTray3PartP->SetVisAttributes(VisG4_Al);
	return logicCoilTray3PartP;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2CoilTray3PartQ()
{
	G4Tubs* solidCoilTray3PartQ = new G4Tubs("solidCoilTray3PartQ", 7 * CLHEP::cm, 9 * CLHEP::cm, 0.25 * CLHEP::cm, 0., CLHEP::twopi);
	G4LogicalVolume* logicCoilTray3PartQ = new G4LogicalVolume(solidCoilTray3PartQ, materials.getMaterial("G4_Al"), "logicCoilTray3PartQ", 0, 0, 0);
	new G4PVPlacement(rdk2RotationMatrices[2], G4ThreeVector(-0.0875112226100227 * CLHEP::cm, 0 * CLHEP::cm, 2.01416428970362 * CLHEP::cm), logicCoilTray3PartQ, "physiCoilTray3PartQ", logicWorld, false, 0);
	logicCoilTray3PartQ->SetVisAttributes(VisG4_Al);
	return logicCoilTray3PartQ;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2ActiveAreaBareAPD1()
{
	G4Box* solidActiveAreaBareAPD1 = new G4Box("solidActiveAreaBareAPD1", 1.4 * CLHEP::cm, 1.4 * CLHEP::cm, 0.003 * CLHEP::cm);
	G4LogicalVolume* logicActiveAreaBareAPD1 = new G4LogicalVolume(solidActiveAreaBareAPD1, materials.getMaterial("G4_Si"), "logicActiveAreaBareAPD1", 0, 0, 0);
	new G4PVPlacement(bapdRotationMatrices[0], gammaDetOffset + G4ThreeVector(3.00699608651225 * CLHEP::cm * AlLengthContractionFactor, -1.73609 * CLHEP::cm * AlLengthContractionFactor, 3.897 * CLHEP::cm), logicActiveAreaBareAPD1, "physiActiveAreaBareAPD1", logicWorld, false, 0);
	logicActiveAreaBareAPD1->SetVisAttributes(VisG4_Si);
	return logicActiveAreaBareAPD1;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2ActiveAreaBareAPD2()
{
	G4Box* solidActiveAreaBareAPD2 = new G4Box("solidActiveAreaBareAPD2", 1.4 * CLHEP::cm, 1.4 * CLHEP::cm, 0.003 * CLHEP::cm);
	G4LogicalVolume* logicActiveAreaBareAPD2 = new G4LogicalVolume(solidActiveAreaBareAPD2, materials.getMaterial("G4_Si"), "logicActiveAreaBareAPD2", 0, 0, 0);
	new G4PVPlacement(bapdRotationMatrices[1], gammaDetOffset + G4ThreeVector(-3.00699608651225 * CLHEP::cm * AlLengthContractionFactor, -1.73609 * CLHEP::cm * AlLengthContractionFactor, 3.897 * CLHEP::cm), logicActiveAreaBareAPD2, "physiActiveAreaBareAPD2", logicWorld, false, 0);
	logicActiveAreaBareAPD2->SetVisAttributes(VisG4_Si);
	return logicActiveAreaBareAPD2;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2ActiveAreaBareAPD3()
{
	G4Box* solidActiveAreaBareAPD3 = new G4Box("solidActiveAreaBareAPD3", 1.4 * CLHEP::cm, 1.4 * CLHEP::cm, 0.003 * CLHEP::cm);
	G4LogicalVolume* logicActiveAreaBareAPD3 = new G4LogicalVolume(solidActiveAreaBareAPD3, materials.getMaterial("G4_Si"), "logicActiveAreaBareAPD3", 0, 0, 0);
	new G4PVPlacement(bapdRotationMatrices[2], gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 3.47218 * CLHEP::cm * AlLengthContractionFactor, 3.897 * CLHEP::cm), logicActiveAreaBareAPD3, "physiActiveAreaBareAPD3", logicWorld, false, 0);
	logicActiveAreaBareAPD3->SetVisAttributes(VisG4_Si);
	return logicActiveAreaBareAPD3;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SiBareAPD1()
{
	G4Box* solidSiBareAPD1 = new G4Box("solidSiBareAPD1", 1.5164 * CLHEP::cm, 1.5164 * CLHEP::cm, 0.003 * CLHEP::cm);

	G4Box* solidActiveAreaSub1 = new G4Box("solidActiveAreaSub1", 1.4 * CLHEP::cm, 1.4 * CLHEP::cm, 0.004 * CLHEP::cm);
	G4VSolid* solidSiBareAPDInactive1 = new G4SubtractionSolid("solidSiBareAPDInactive1", solidSiBareAPD1, solidActiveAreaSub1, 0, G4ThreeVector(0, 0, 0));

	G4LogicalVolume* logicSiBareAPD1 = new G4LogicalVolume(solidSiBareAPDInactive1, materials.getMaterial("G4_Si"), "logicSiBareAPD1", 0, 0, 0);
	new G4PVPlacement(bapdRotationMatrices[0], gammaDetOffset + G4ThreeVector(3.00699608651225 * CLHEP::cm * AlLengthContractionFactor, -1.73609 * CLHEP::cm * AlLengthContractionFactor, 3.897 * CLHEP::cm), logicSiBareAPD1, "physiSiBareAPD1", logicWorld, false, 0);
	logicSiBareAPD1->SetVisAttributes(VisG4_Si);
	return logicSiBareAPD1;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SiBareAPD2()
{
	G4Box* solidSiBareAPD2 = new G4Box("solidSiBareAPD2", 1.5164 * CLHEP::cm, 1.5164 * CLHEP::cm, 0.003 * CLHEP::cm);

	G4Box* solidActiveAreaSub2 = new G4Box("solidActiveAreaSub2", 1.4 * CLHEP::cm, 1.4 * CLHEP::cm, 0.004 * CLHEP::cm);
	G4VSolid* solidSiBareAPDInactive2 = new G4SubtractionSolid("solidSiBareAPDInactive2", solidSiBareAPD2, solidActiveAreaSub2, 0, G4ThreeVector(0, 0, 0));

	G4LogicalVolume* logicSiBareAPD2 = new G4LogicalVolume(solidSiBareAPDInactive2, materials.getMaterial("G4_Si"), "logicSiBareAPD2", 0, 0, 0);
	new G4PVPlacement(bapdRotationMatrices[1], gammaDetOffset + G4ThreeVector(-3.00699608651225 * CLHEP::cm * AlLengthContractionFactor, -1.73609 * CLHEP::cm * AlLengthContractionFactor, 3.897 * CLHEP::cm), logicSiBareAPD2, "physiSiBareAPD2", logicWorld, false, 0);
	logicSiBareAPD2->SetVisAttributes(VisG4_Si);
	return logicSiBareAPD2;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SiBareAPD3()
{
	G4Box* solidSiBareAPD3 = new G4Box("solidSiBareAPD3", 1.5164 * CLHEP::cm, 1.5164 * CLHEP::cm, 0.003 * CLHEP::cm);

	G4Box* solidActiveAreaSub3 = new G4Box("solidActiveAreaSub3", 1.4 * CLHEP::cm, 1.4 * CLHEP::cm, 0.004 * CLHEP::cm);
	G4VSolid* solidSiBareAPDInactive3 = new G4SubtractionSolid("solidSiBareAPDInactive3", solidSiBareAPD3, solidActiveAreaSub3, 0, G4ThreeVector(0, 0, 0));

	G4LogicalVolume* logicSiBareAPD3 = new G4LogicalVolume(solidSiBareAPDInactive3, materials.getMaterial("G4_Si"), "logicSiBareAPD3", 0, 0, 0);
	new G4PVPlacement(bapdRotationMatrices[2], gammaDetOffset + G4ThreeVector(0 * CLHEP::cm, 3.47218 * CLHEP::cm * AlLengthContractionFactor, 3.897 * CLHEP::cm), logicSiBareAPD3, "physiSiBareAPD3", logicWorld, false, 0);
	logicSiBareAPD3->SetVisAttributes(VisG4_Si);
	return logicSiBareAPD3;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2BAPDBacking1()
{
	G4Box* solidBAPDBacking1 = new G4Box("solidBAPDBacking1", 1.7018 * CLHEP::cm, 1.7018 * CLHEP::cm, 0.04699 * CLHEP::cm);
	G4LogicalVolume* logicBAPDBacking1 = new G4LogicalVolume(solidBAPDBacking1, materials.getMaterial("G4_ALUMINUM_OXIDE"), "logicBAPDBacking1", 0, 0, 0);
	new G4PVPlacement(bapdRotationMatrices[0], gammaDetOffset + G4ThreeVector(3.00699608651225 * CLHEP::cm * AlLengthContractionFactor, -1.73609 * CLHEP::cm * AlLengthContractionFactor, 3.847 * CLHEP::cm), logicBAPDBacking1, "physiBAPDBacking1", logicWorld, false, 0);
	logicBAPDBacking1->SetVisAttributes(VisG4_ALUMINUM_OXIDE);
	return logicBAPDBacking1;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2BAPDBacking2()
{
	G4Box* solidBAPDBacking2 = new G4Box("solidBAPDBacking2", 1.7018 * CLHEP::cm, 1.7018 * CLHEP::cm, 0.04699);
	G4LogicalVolume* logicBAPDBacking2 = new G4LogicalVolume(solidBAPDBacking2, materials.getMaterial("G4_ALUMINUM_OXIDE"), "logicBAPDBacking2", 0, 0, 0);
	new G4PVPlacement(bapdRotationMatrices[1], gammaDetOffset + G4ThreeVector(-3.00699608651225 * CLHEP::cm * AlLengthContractionFactor, -1.73609 * CLHEP::cm * AlLengthContractionFactor, 3.847 * CLHEP::cm), logicBAPDBacking2, "physiBAPDBacking2", logicWorld, false, 0);
	logicBAPDBacking2->SetVisAttributes(VisG4_ALUMINUM_OXIDE);
	return logicBAPDBacking2;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2BAPDBacking3()
{
	G4Box* solidBAPDBacking3 = new G4Box("solidBAPDBacking3", 1.7018 * CLHEP::cm, 1.7018 * CLHEP::cm, 0.04699 * CLHEP::cm);
	G4LogicalVolume* logicBAPDBacking3 = new G4LogicalVolume(solidBAPDBacking3, materials.getMaterial("G4_ALUMINUM_OXIDE"), "logicBAPDBacking3", 0, 0, 0);
	new G4PVPlacement(bapdRotationMatrices[2], gammaDetOffset + G4ThreeVector(0 * CLHEP::cm * AlLengthContractionFactor, 3.47218 * CLHEP::cm * AlLengthContractionFactor, 3.847 * CLHEP::cm), logicBAPDBacking3, "physiBAPDBacking3", logicWorld, false, 0);
	logicBAPDBacking3->SetVisAttributes(VisG4_ALUMINUM_OXIDE);
	return logicBAPDBacking3;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2G10BAPDBackPieces1()
{
	G4Box* solidG10BAPDBackPieces1 = new G4Box("solidG10BAPDBackPieces1", 1.6891 * CLHEP::cm, 2.5146 * CLHEP::cm, 0.317499999999999 * CLHEP::cm);

	G4Tubs* solidG10BAPDBackPieces1Sub = new G4Tubs("solidG10BAPDBackPieces1Sub", 5.5 * CLHEP::cm, 6.5 * CLHEP::cm, 0.4 * CLHEP::cm, 0., CLHEP::twopi);
	G4VSolid* solidG10BAPDBackPieces1WShavedEdges = new G4SubtractionSolid("solidG10BAPDBackPieces1WShavedEdges", solidG10BAPDBackPieces1, solidG10BAPDBackPieces1Sub, 0, G4ThreeVector(-3.47218 * CLHEP::cm, 0, 0));

	G4LogicalVolume* logicG10BAPDBackPieces1 = new G4LogicalVolume(solidG10BAPDBackPieces1WShavedEdges, materials.getMaterial("G10"), "logicG10BAPDBackPieces1", 0, 0, 0);
	new G4PVPlacement(bapdRotationMatrices[0], gammaDetOffset + G4ThreeVector(3.00699608651225 * CLHEP::cm * AlLengthContractionFactor, -1.73609 * CLHEP::cm * AlLengthContractionFactor, 3.4465 * CLHEP::cm), logicG10BAPDBackPieces1, "physiG10BAPDBackPieces1", logicWorld, false, 0);
	logicG10BAPDBackPieces1->SetVisAttributes(VisG10);
	return logicG10BAPDBackPieces1;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2G10BAPDBackPieces2()
{
	G4Box* solidG10BAPDBackPieces2 = new G4Box("solidG10BAPDBackPieces2", 1.6891 * CLHEP::cm, 2.5146 * CLHEP::cm, 0.317499999999999 * CLHEP::cm);

	G4Tubs* solidG10BAPDBackPieces2Sub = new G4Tubs("solidG10BAPDBackPieces2Sub", 5.5 * CLHEP::cm, 6.5 * CLHEP::cm, 0.4 * CLHEP::cm, 0., CLHEP::twopi);
	G4VSolid* solidG10BAPDBackPieces2WShavedEdges = new G4SubtractionSolid("solidG10BAPDBackPieces2WShavedEdges", solidG10BAPDBackPieces2, solidG10BAPDBackPieces2Sub, 0, G4ThreeVector(-3.47218 * CLHEP::cm, 0, 0));

	G4LogicalVolume* logicG10BAPDBackPieces2 = new G4LogicalVolume(solidG10BAPDBackPieces2WShavedEdges, materials.getMaterial("G10"), "logicG10BAPDBackPieces2", 0, 0, 0);
	new G4PVPlacement(bapdRotationMatrices[1], gammaDetOffset + G4ThreeVector(-3.00699608651225 * CLHEP::cm * AlLengthContractionFactor, -1.73609 * CLHEP::cm * AlLengthContractionFactor, 3.4465 * CLHEP::cm), logicG10BAPDBackPieces2, "physiG10BAPDBackPieces2", logicWorld, false, 0);
	logicG10BAPDBackPieces2->SetVisAttributes(VisG10);
	return logicG10BAPDBackPieces2;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2G10BAPDBackPieces3()
{
	G4Box* solidG10BAPDBackPieces3 = new G4Box("solidG10BAPDBackPieces3", 1.6891 * CLHEP::cm, 2.5146 * CLHEP::cm, 0.317499999999999 * CLHEP::cm);

	G4Tubs* solidG10BAPDBackPieces3Sub = new G4Tubs("solidG10BAPDBackPieces3Sub", 5.5 * CLHEP::cm, 6.5 * CLHEP::cm, 0.4 * CLHEP::cm, 0., CLHEP::twopi);
	G4VSolid* solidG10BAPDBackPieces3WShavedEdges = new G4SubtractionSolid("solidG10BAPDBackPieces3WShavedEdges", solidG10BAPDBackPieces3, solidG10BAPDBackPieces3Sub, 0, G4ThreeVector(-3.47218 * CLHEP::cm, 0, 0));

	G4LogicalVolume* logicG10BAPDBackPieces3 = new G4LogicalVolume(solidG10BAPDBackPieces3WShavedEdges, materials.getMaterial("G10"), "logicG10BAPDBackPieces3", 0, 0, 0);
	new G4PVPlacement(bapdRotationMatrices[2], gammaDetOffset + G4ThreeVector(0 * CLHEP::cm * AlLengthContractionFactor, 3.47218 * CLHEP::cm * AlLengthContractionFactor, 3.4465 * CLHEP::cm), logicG10BAPDBackPieces3, "physiG10BAPDBackPieces3", logicWorld, false, 0);
	logicG10BAPDBackPieces3->SetVisAttributes(VisG10);
	return logicG10BAPDBackPieces3;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SiSmallAPD1()
{
	G4Box* solidSiSmallAPD1 = new G4Box("solidSiSmallAPD1", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.015000000000000 * CLHEP::cm);
	G4LogicalVolume* logicSiSmallAPD1 = new G4LogicalVolume(solidSiSmallAPD1, materials.getMaterial("G4_Si"), "logicSiSmallAPD1", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[0], gammaDetOffset + G4ThreeVector(1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, 4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, 32.415 * CLHEP::cm), logicSiSmallAPD1, "physiSiSmallAPD1", logicWorld, false, 0);
	logicSiSmallAPD1->SetVisAttributes(VisG4_Si);
	return logicSiSmallAPD1;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SiSmallAPD2()
{
	G4Box* solidSiSmallAPD2 = new G4Box("solidSiSmallAPD2", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.015000000000000 * CLHEP::cm);
	G4LogicalVolume* logicSiSmallAPD2 = new G4LogicalVolume(solidSiSmallAPD2, materials.getMaterial("G4_Si"), "logicSiSmallAPD2", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[1], gammaDetOffset + G4ThreeVector(2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, 2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, 32.415 * CLHEP::cm), logicSiSmallAPD2, "physiSiSmallAPD2", logicWorld, false, 0);
	logicSiSmallAPD2->SetVisAttributes(VisG4_Si);
	return logicSiSmallAPD2;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SiSmallAPD3()
{
	G4Box* solidSiSmallAPD3 = new G4Box("solidSiSmallAPD3", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.015000000000000 * CLHEP::cm);
	G4LogicalVolume* logicSiSmallAPD3 = new G4LogicalVolume(solidSiSmallAPD3, materials.getMaterial("G4_Si"), "logicSiSmallAPD3", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[2], gammaDetOffset + G4ThreeVector(4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, 1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, 32.415 * CLHEP::cm), logicSiSmallAPD3, "physiSiSmallAPD3", logicWorld, false, 0);
	logicSiSmallAPD3->SetVisAttributes(VisG4_Si);
	return logicSiSmallAPD3;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SiSmallAPD4()
{
	G4Box* solidSiSmallAPD4 = new G4Box("solidSiSmallAPD4", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.015 * CLHEP::cm);
	G4LogicalVolume* logicSiSmallAPD4 = new G4LogicalVolume(solidSiSmallAPD4, materials.getMaterial("G4_Si"), "logicSiSmallAPD4", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[3], gammaDetOffset + G4ThreeVector(4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, -1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, 32.415 * CLHEP::cm), logicSiSmallAPD4, "physiSiSmallAPD4", logicWorld, false, 0);
	logicSiSmallAPD4->SetVisAttributes(VisG4_Si);
	return logicSiSmallAPD4;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SiSmallAPD5()
{
	G4Box* solidSiSmallAPD5 = new G4Box("solidSiSmallAPD5", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.015 * CLHEP::cm);
	G4LogicalVolume* logicSiSmallAPD5 = new G4LogicalVolume(solidSiSmallAPD5, materials.getMaterial("G4_Si"), "logicSiSmallAPD5", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[4], gammaDetOffset + G4ThreeVector(2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, -2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, 32.415 * CLHEP::cm), logicSiSmallAPD5, "physiSiSmallAPD5", logicWorld, false, 0);
	logicSiSmallAPD5->SetVisAttributes(VisG4_Si);
	return logicSiSmallAPD5;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SiSmallAPD6()
{
	G4Box* solidSiSmallAPD6 = new G4Box("solidSiSmallAPD6", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.015 * CLHEP::cm);
	G4LogicalVolume* logicSiSmallAPD6 = new G4LogicalVolume(solidSiSmallAPD6, materials.getMaterial("G4_Si"), "logicSiSmallAPD6", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[5], gammaDetOffset + G4ThreeVector(1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, -4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, 32.415 * CLHEP::cm), logicSiSmallAPD6, "physiSiSmallAPD6", logicWorld, false, 0);
	logicSiSmallAPD6->SetVisAttributes(VisG4_Si);
	return logicSiSmallAPD6;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SiSmallAPD7()
{
	G4Box* solidSiSmallAPD7 = new G4Box("solidSiSmallAPD7", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.015 * CLHEP::cm);
	G4LogicalVolume* logicSiSmallAPD7 = new G4LogicalVolume(solidSiSmallAPD7, materials.getMaterial("G4_Si"), "logicSiSmallAPD7", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[6], gammaDetOffset + G4ThreeVector(-1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, -4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, 32.415 * CLHEP::cm), logicSiSmallAPD7, "physiSiSmallAPD7", logicWorld, false, 0);
	logicSiSmallAPD7->SetVisAttributes(VisG4_Si);
	return logicSiSmallAPD7;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SiSmallAPD8()
{
	G4Box* solidSiSmallAPD8 = new G4Box("solidSiSmallAPD8", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.015 * CLHEP::cm);
	G4LogicalVolume* logicSiSmallAPD8 = new G4LogicalVolume(solidSiSmallAPD8, materials.getMaterial("G4_Si"), "logicSiSmallAPD8", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[7], gammaDetOffset + G4ThreeVector(-2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, -2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, 32.415 * CLHEP::cm), logicSiSmallAPD8, "physiSiSmallAPD8", logicWorld, false, 0);
	logicSiSmallAPD8->SetVisAttributes(VisG4_Si);
	return logicSiSmallAPD8;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SiSmallAPD9()
{
	G4Box* solidSiSmallAPD9 = new G4Box("solidSiSmallAPD9", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.015 * CLHEP::cm);
	G4LogicalVolume* logicSiSmallAPD9 = new G4LogicalVolume(solidSiSmallAPD9, materials.getMaterial("G4_Si"), "logicSiSmallAPD9", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[8], gammaDetOffset + G4ThreeVector(-4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, -1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, 32.415 * CLHEP::cm), logicSiSmallAPD9, "physiSiSmallAPD9", logicWorld, false, 0);
	logicSiSmallAPD9->SetVisAttributes(VisG4_Si);
	return logicSiSmallAPD9;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SiSmallAPD10()
{
	G4Box* solidSiSmallAPD10 = new G4Box("solidSiSmallAPD10", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.015 * CLHEP::cm);
	G4LogicalVolume* logicSiSmallAPD10 = new G4LogicalVolume(solidSiSmallAPD10, materials.getMaterial("G4_Si"), "logicSiSmallAPD10", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[9], gammaDetOffset + G4ThreeVector(-4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, 1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, 32.415 * CLHEP::cm), logicSiSmallAPD10, "physiSiSmallAPD10", logicWorld, false, 0);
	logicSiSmallAPD10->SetVisAttributes(VisG4_Si);
	return logicSiSmallAPD10;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SiSmallAPD11()
{
	G4Box* solidSiSmallAPD11 = new G4Box("solidSiSmallAPD11", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.015 * CLHEP::cm);
	G4LogicalVolume* logicSiSmallAPD11 = new G4LogicalVolume(solidSiSmallAPD11, materials.getMaterial("G4_Si"), "logicSiSmallAPD11", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[10], gammaDetOffset + G4ThreeVector(-2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, 2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, 32.415 * CLHEP::cm), logicSiSmallAPD11, "physiSiSmallAPD11", logicWorld, false, 0);
	logicSiSmallAPD11->SetVisAttributes(VisG4_Si);
	return logicSiSmallAPD11;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SiSmallAPD12()
{
	G4Box* solidSiSmallAPD12 = new G4Box("solidSiSmallAPD12", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.015 * CLHEP::cm);
	G4LogicalVolume* logicSiSmallAPD12 = new G4LogicalVolume(solidSiSmallAPD12, materials.getMaterial("G4_Si"), "logicSiSmallAPD12", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[11], gammaDetOffset + G4ThreeVector(-1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, 4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, 32.415 * CLHEP::cm), logicSiSmallAPD12, "physiSiSmallAPD12", logicWorld, false, 0);
	logicSiSmallAPD12->SetVisAttributes(VisG4_Si);
	return logicSiSmallAPD12;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SmallAPDBacking1()
{
	G4Box* solidSmallAPDBacking1 = new G4Box("solidSmallAPDBacking1", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.05 * CLHEP::cm);
	G4LogicalVolume* logicSmallAPDBacking1 = new G4LogicalVolume(solidSmallAPDBacking1, materials.getMaterial("G4_ALUMINUM_OXIDE"), "logicSmallAPDBacking1", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[0], gammaDetOffset + G4ThreeVector(1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, 4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, 32.48 * CLHEP::cm), logicSmallAPDBacking1, "physiSmallAPDBacking1", logicWorld, false, 0);
	logicSmallAPDBacking1->SetVisAttributes(VisG4_ALUMINUM_OXIDE);
	return logicSmallAPDBacking1;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SmallAPDBacking2()
{
	G4Box* solidSmallAPDBacking2 = new G4Box("solidSmallAPDBacking2", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.05 * CLHEP::cm);
	G4LogicalVolume* logicSmallAPDBacking2 = new G4LogicalVolume(solidSmallAPDBacking2, materials.getMaterial("G4_ALUMINUM_OXIDE"), "logicSmallAPDBacking2", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[1], gammaDetOffset + G4ThreeVector(2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, 2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, 32.48 * CLHEP::cm), logicSmallAPDBacking2, "physiSmallAPDBacking2", logicWorld, false, 0);
	logicSmallAPDBacking2->SetVisAttributes(VisG4_ALUMINUM_OXIDE);
	return logicSmallAPDBacking2;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SmallAPDBacking3()
{
	G4Box* solidSmallAPDBacking3 = new G4Box("solidSmallAPDBacking3", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.05 * CLHEP::cm);
	G4LogicalVolume* logicSmallAPDBacking3 = new G4LogicalVolume(solidSmallAPDBacking3, materials.getMaterial("G4_ALUMINUM_OXIDE"), "logicSmallAPDBacking3", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[2], gammaDetOffset + G4ThreeVector(4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, 1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, 32.48 * CLHEP::cm), logicSmallAPDBacking3, "physiSmallAPDBacking3", logicWorld, false, 0);
	logicSmallAPDBacking3->SetVisAttributes(VisG4_ALUMINUM_OXIDE);
	return logicSmallAPDBacking3;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SmallAPDBacking4()
{
	G4Box* solidSmallAPDBacking4 = new G4Box("solidSmallAPDBacking4", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.05 * CLHEP::cm);
	G4LogicalVolume* logicSmallAPDBacking4 = new G4LogicalVolume(solidSmallAPDBacking4, materials.getMaterial("G4_ALUMINUM_OXIDE"), "logicSmallAPDBacking4", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[3], gammaDetOffset + G4ThreeVector(4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, -1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, 32.48 * CLHEP::cm), logicSmallAPDBacking4, "physiSmallAPDBacking4", logicWorld, false, 0);
	logicSmallAPDBacking4->SetVisAttributes(VisG4_ALUMINUM_OXIDE);
	return logicSmallAPDBacking4;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SmallAPDBacking5()
{
	G4Box* solidSmallAPDBacking5 = new G4Box("solidSmallAPDBacking5", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.05 * CLHEP::cm);
	G4LogicalVolume* logicSmallAPDBacking5 = new G4LogicalVolume(solidSmallAPDBacking5, materials.getMaterial("G4_ALUMINUM_OXIDE"), "logicSmallAPDBacking5", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[4], gammaDetOffset + G4ThreeVector(2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, -2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, 32.48 * CLHEP::cm), logicSmallAPDBacking5, "physiSmallAPDBacking5", logicWorld, false, 0);
	logicSmallAPDBacking5->SetVisAttributes(VisG4_ALUMINUM_OXIDE);
	return logicSmallAPDBacking5;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SmallAPDBacking6()
{
	G4Box* solidSmallAPDBacking6 = new G4Box("solidSmallAPDBacking6", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.05 * CLHEP::cm);
	G4LogicalVolume* logicSmallAPDBacking6 = new G4LogicalVolume(solidSmallAPDBacking6, materials.getMaterial("G4_ALUMINUM_OXIDE"), "logicSmallAPDBacking6", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[5], gammaDetOffset + G4ThreeVector(1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, -4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, 32.48 * CLHEP::cm), logicSmallAPDBacking6, "physiSmallAPDBacking6", logicWorld, false, 0);
	logicSmallAPDBacking6->SetVisAttributes(VisG4_ALUMINUM_OXIDE);
	return logicSmallAPDBacking6;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SmallAPDBacking7()
{
	G4Box* solidSmallAPDBacking7 = new G4Box("solidSmallAPDBacking7", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.05 * CLHEP::cm);
	G4LogicalVolume* logicSmallAPDBacking7 = new G4LogicalVolume(solidSmallAPDBacking7, materials.getMaterial("G4_ALUMINUM_OXIDE"), "logicSmallAPDBacking7", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[6], gammaDetOffset + G4ThreeVector(-1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, -4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, 32.48 * CLHEP::cm), logicSmallAPDBacking7, "physiSmallAPDBacking7", logicWorld, false, 0);
	logicSmallAPDBacking7->SetVisAttributes(VisG4_ALUMINUM_OXIDE);
	return logicSmallAPDBacking7;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SmallAPDBacking8()
{
	G4Box* solidSmallAPDBacking8 = new G4Box("solidSmallAPDBacking8", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.05 * CLHEP::cm);
	G4LogicalVolume* logicSmallAPDBacking8 = new G4LogicalVolume(solidSmallAPDBacking8, materials.getMaterial("G4_ALUMINUM_OXIDE"), "logicSmallAPDBacking8", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[7], gammaDetOffset + G4ThreeVector(-2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, -2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, 32.48 * CLHEP::cm), logicSmallAPDBacking8, "physiSmallAPDBacking8", logicWorld, false, 0);
	logicSmallAPDBacking8->SetVisAttributes(VisG4_ALUMINUM_OXIDE);
	return logicSmallAPDBacking8;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SmallAPDBacking9()
{
	G4Box* solidSmallAPDBacking9 = new G4Box("solidSmallAPDBacking9", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.05 * CLHEP::cm);
	G4LogicalVolume* logicSmallAPDBacking9 = new G4LogicalVolume(solidSmallAPDBacking9, materials.getMaterial("G4_ALUMINUM_OXIDE"), "logicSmallAPDBacking9", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[8], gammaDetOffset + G4ThreeVector(-4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, -1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, 32.48 * CLHEP::cm), logicSmallAPDBacking9, "physiSmallAPDBacking9", logicWorld, false, 0);
	logicSmallAPDBacking9->SetVisAttributes(VisG4_ALUMINUM_OXIDE);
	return logicSmallAPDBacking9;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SmallAPDBacking10()
{
	G4Box* solidSmallAPDBacking10 = new G4Box("solidSmallAPDBacking10", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.05 * CLHEP::cm);
	G4LogicalVolume* logicSmallAPDBacking10 = new G4LogicalVolume(solidSmallAPDBacking10, materials.getMaterial("G4_ALUMINUM_OXIDE"), "logicSmallAPDBacking10", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[9], gammaDetOffset + G4ThreeVector(-4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, 1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, 32.48 * CLHEP::cm), logicSmallAPDBacking10, "physiSmallAPDBacking10", logicWorld, false, 0);
	logicSmallAPDBacking10->SetVisAttributes(VisG4_ALUMINUM_OXIDE);
	return logicSmallAPDBacking10;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SmallAPDBacking11()
{
	G4Box* solidSmallAPDBacking11 = new G4Box("solidSmallAPDBacking11", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.05 * CLHEP::cm);
	G4LogicalVolume* logicSmallAPDBacking11 = new G4LogicalVolume(solidSmallAPDBacking11, materials.getMaterial("G4_ALUMINUM_OXIDE"), "logicSmallAPDBacking11", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[10], gammaDetOffset + G4ThreeVector(-2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, 2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, 32.48 * CLHEP::cm), logicSmallAPDBacking11, "physiSmallAPDBacking11", logicWorld, false, 0);
	logicSmallAPDBacking11->SetVisAttributes(VisG4_ALUMINUM_OXIDE);
	return logicSmallAPDBacking11;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2SmallAPDBacking12()
{
	G4Box* solidSmallAPDBacking12 = new G4Box("solidSmallAPDBacking12", 0.84 * CLHEP::cm, 0.775 * CLHEP::cm, 0.05 * CLHEP::cm);
	G4LogicalVolume* logicSmallAPDBacking12 = new G4LogicalVolume(solidSmallAPDBacking12, materials.getMaterial("G4_ALUMINUM_OXIDE"), "logicSmallAPDBacking12", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[11], gammaDetOffset + G4ThreeVector(-1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, 4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, 32.48 * CLHEP::cm), logicSmallAPDBacking12, "physiSmallAPDBacking12", logicWorld, false, 0);
	logicSmallAPDBacking12->SetVisAttributes(VisG4_ALUMINUM_OXIDE);
	return logicSmallAPDBacking12;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2BGOCrystal1()
{
	G4Box* solidBGOCrystal1 = new G4Box("solidBGOCrystal1", 0.6 * CLHEP::cm * BGOLengthContractionFactor, 0.6 * CLHEP::cm * BGOLengthContractionFactor, 10 * CLHEP::cm * BGOLengthContractionFactor);
	G4LogicalVolume* logicBGOCrystal1 = new G4LogicalVolume(solidBGOCrystal1, materials.getMaterial("G4_BGO"), "logicBGOCrystal1", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[0], gammaDetOffset + G4ThreeVector(1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, 4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, 22.4 * CLHEP::cm), logicBGOCrystal1, "physiBGOCrystal1", logicWorld, false, 0);
	logicBGOCrystal1->SetVisAttributes(VisG4_BGO);
	return logicBGOCrystal1;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2BGOCrystal2()
{
	G4Box* solidBGOCrystal2 = new G4Box("solidBGOCrystal2", 0.6 * CLHEP::cm * BGOLengthContractionFactor, 0.6 * CLHEP::cm * BGOLengthContractionFactor, 10 * CLHEP::cm * BGOLengthContractionFactor);
	G4LogicalVolume* logicBGOCrystal2 = new G4LogicalVolume(solidBGOCrystal2, materials.getMaterial("G4_BGO"), "logicBGOCrystal2", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[1], gammaDetOffset + G4ThreeVector(2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, 2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, 22.4 * CLHEP::cm), logicBGOCrystal2, "physiBGOCrystal2", logicWorld, false, 0);
	logicBGOCrystal2->SetVisAttributes(VisG4_BGO);
	return logicBGOCrystal2;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2BGOCrystal3()
{
	G4Box* solidBGOCrystal3 = new G4Box("solidBGOCrystal3", 0.6 * CLHEP::cm * BGOLengthContractionFactor, 0.6 * CLHEP::cm * BGOLengthContractionFactor, 10 * CLHEP::cm * BGOLengthContractionFactor);
	G4LogicalVolume* logicBGOCrystal3 = new G4LogicalVolume(solidBGOCrystal3, materials.getMaterial("G4_BGO"), "logicBGOCrystal3", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[2], gammaDetOffset + G4ThreeVector(4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, 1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, 22.4 * CLHEP::cm), logicBGOCrystal3, "physiBGOCrystal3", logicWorld, false, 0);
	logicBGOCrystal3->SetVisAttributes(VisG4_BGO);
	return logicBGOCrystal3;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2BGOCrystal4()
{
	G4Box* solidBGOCrystal4 = new G4Box("solidBGOCrystal4", 0.6 * CLHEP::cm * BGOLengthContractionFactor, 0.6 * CLHEP::cm * BGOLengthContractionFactor, 10 * CLHEP::cm * BGOLengthContractionFactor);
	G4LogicalVolume* logicBGOCrystal4 = new G4LogicalVolume(solidBGOCrystal4, materials.getMaterial("G4_BGO"), "logicBGOCrystal4", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[3], gammaDetOffset + G4ThreeVector(4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, -1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, 22.4 * CLHEP::cm), logicBGOCrystal4, "physiBGOCrystal4", logicWorld, false, 0);
	logicBGOCrystal4->SetVisAttributes(VisG4_BGO);
	return logicBGOCrystal4;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2BGOCrystal5()
{
	G4Box* solidBGOCrystal5 = new G4Box("solidBGOCrystal5", 0.6 * CLHEP::cm * BGOLengthContractionFactor, 0.6 * CLHEP::cm * BGOLengthContractionFactor, 10 * CLHEP::cm * BGOLengthContractionFactor);
	G4LogicalVolume* logicBGOCrystal5 = new G4LogicalVolume(solidBGOCrystal5, materials.getMaterial("G4_BGO"), "logicBGOCrystal5", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[4], gammaDetOffset + G4ThreeVector(2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, -2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, 22.4 * CLHEP::cm), logicBGOCrystal5, "physiBGOCrystal5", logicWorld, false, 0);
	logicBGOCrystal5->SetVisAttributes(VisG4_BGO);
	return logicBGOCrystal5;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2BGOCrystal6()
{
	G4Box* solidBGOCrystal6 = new G4Box("solidBGOCrystal6", 0.6 * CLHEP::cm * BGOLengthContractionFactor, 0.6 * CLHEP::cm * BGOLengthContractionFactor, 10 * CLHEP::cm * BGOLengthContractionFactor);
	G4LogicalVolume* logicBGOCrystal6 = new G4LogicalVolume(solidBGOCrystal6, materials.getMaterial("G4_BGO"), "logicBGOCrystal6", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[5], gammaDetOffset + G4ThreeVector(1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, -4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, 22.4 * CLHEP::cm), logicBGOCrystal6, "physiBGOCrystal6", logicWorld, false, 0);
	logicBGOCrystal6->SetVisAttributes(VisG4_BGO);
	return logicBGOCrystal6;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2BGOCrystal7()
{
	G4Box* solidBGOCrystal7 = new G4Box("solidBGOCrystal7", 0.6 * CLHEP::cm * BGOLengthContractionFactor, 0.6 * CLHEP::cm * BGOLengthContractionFactor, 10 * CLHEP::cm * BGOLengthContractionFactor);
	G4LogicalVolume* logicBGOCrystal7 = new G4LogicalVolume(solidBGOCrystal7, materials.getMaterial("G4_BGO"), "logicBGOCrystal7", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[6], gammaDetOffset + G4ThreeVector(-1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, -4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, 22.4 * CLHEP::cm), logicBGOCrystal7, "physiBGOCrystal7", logicWorld, false, 0);
	logicBGOCrystal7->SetVisAttributes(VisG4_BGO);
	return logicBGOCrystal7;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2BGOCrystal8()
{
	G4Box* solidBGOCrystal8 = new G4Box("solidBGOCrystal8", 0.6 * CLHEP::cm * BGOLengthContractionFactor, 0.6 * CLHEP::cm * BGOLengthContractionFactor, 10 * CLHEP::cm * BGOLengthContractionFactor);
	G4LogicalVolume* logicBGOCrystal8 = new G4LogicalVolume(solidBGOCrystal8, materials.getMaterial("G4_BGO"), "logicBGOCrystal8", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[7], gammaDetOffset + G4ThreeVector(-2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, -2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, 22.4 * CLHEP::cm), logicBGOCrystal8, "physiBGOCrystal8", logicWorld, false, 0);
	logicBGOCrystal8->SetVisAttributes(VisG4_BGO);
	return logicBGOCrystal8;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2BGOCrystal9()
{
	G4Box* solidBGOCrystal9 = new G4Box("solidBGOCrystal9", 0.6 * CLHEP::cm * BGOLengthContractionFactor, 0.6 * CLHEP::cm * BGOLengthContractionFactor, 10 * CLHEP::cm * BGOLengthContractionFactor);
	G4LogicalVolume* logicBGOCrystal9 = new G4LogicalVolume(solidBGOCrystal9, materials.getMaterial("G4_BGO"), "logicBGOCrystal9", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[8], gammaDetOffset + G4ThreeVector(-4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, -1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, 22.4 * CLHEP::cm), logicBGOCrystal9, "physiBGOCrystal9", logicWorld, false, 0);
	logicBGOCrystal9->SetVisAttributes(VisG4_BGO);
	return logicBGOCrystal9;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2BGOCrystal10()
{
	G4Box* solidBGOCrystal10 = new G4Box("solidBGOCrystal10", 0.6 * CLHEP::cm * BGOLengthContractionFactor, 0.6 * CLHEP::cm * BGOLengthContractionFactor, 10 * CLHEP::cm * BGOLengthContractionFactor);
	G4LogicalVolume* logicBGOCrystal10 = new G4LogicalVolume(solidBGOCrystal10, materials.getMaterial("G4_BGO"), "logicBGOCrystal10", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[9], gammaDetOffset + G4ThreeVector(-4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, 1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, 22.4 * CLHEP::cm), logicBGOCrystal10, "physiBGOCrystal10", logicWorld, false, 0);
	logicBGOCrystal10->SetVisAttributes(VisG4_BGO);
	return logicBGOCrystal10;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2BGOCrystal11()
{
	G4Box* solidBGOCrystal11 = new G4Box("solidBGOCrystal11", 0.6 * CLHEP::cm * BGOLengthContractionFactor, 0.6 * CLHEP::cm * BGOLengthContractionFactor, 10 * CLHEP::cm * BGOLengthContractionFactor);
	G4LogicalVolume* logicBGOCrystal11 = new G4LogicalVolume(solidBGOCrystal11, materials.getMaterial("G4_BGO"), "logicBGOCrystal11", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[10], gammaDetOffset + G4ThreeVector(-2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, 2.98505127677901 * CLHEP::cm * AlLengthContractionFactor, 22.4 * CLHEP::cm), logicBGOCrystal11, "physiBGOCrystal11", logicWorld, false, 0);
	logicBGOCrystal11->SetVisAttributes(VisG4_BGO);
	return logicBGOCrystal11;
}

G4LogicalVolume* MRKRDKIIGeometry::constructRDK2BGOCrystal12()
{
	G4Box* solidBGOCrystal12 = new G4Box("solidBGOCrystal12", 0.6 * CLHEP::cm * BGOLengthContractionFactor, 0.6 * CLHEP::cm * BGOLengthContractionFactor, 10 * CLHEP::cm * BGOLengthContractionFactor);
	G4LogicalVolume* logicBGOCrystal12 = new G4LogicalVolume(solidBGOCrystal12, materials.getMaterial("G4_BGO"), "logicBGOCrystal12", 0, 0, 0);
	new G4PVPlacement(bgoRotationMatrices[11], gammaDetOffset + G4ThreeVector(-1.09260459890029 * CLHEP::cm * AlLengthContractionFactor, 4.0776558756793 * CLHEP::cm * AlLengthContractionFactor, 22.4 * CLHEP::cm), logicBGOCrystal12, "physiBGOCrystal12", logicWorld, false, 0);
	logicBGOCrystal12->SetVisAttributes(VisG4_BGO);
	return logicBGOCrystal12;
}

G4VSolid* MRKRDKIIGeometry::makeHolesForBGOs(G4VSolid* baseSolid)
{
	G4Box* solidBGOForSub = new G4Box("solidBGOSubtractionSolid", 0.635 * CLHEP::cm, 0.635 * CLHEP::cm, 10 * CLHEP::cm);
	string baseName = baseSolid->GetName();
	string newName;
	G4VSolid* finalSolid = baseSolid;
	for (int i = 0; i < 12; i++)
	{
		G4double BGORadialOffsetToCenter = (3.604 + 0.635) * CLHEP::cm * AlLengthContractionFactor;
		G4double BGOangle = (-i * 30. - 15. + 90.);
		if(BGOangle > 360.) BGOangle -= 360.;

		G4double BGOx = cos(BGOangle * CLHEP::pi / 180.) * BGORadialOffsetToCenter;
		G4double BGOy = sin(BGOangle * CLHEP::pi / 180.) * BGORadialOffsetToCenter;
		G4ThreeVector BGOLocation = G4ThreeVector(BGOx, BGOy, 0);
		newName = baseName + "_BGO" + int2str(i + 1) + "Holes";
		finalSolid = new G4SubtractionSolid(newName.data(), finalSolid, solidBGOForSub, bgoRotationMatrices[i], BGOLocation);
	}

	return finalSolid;

}

G4VSolid* MRKRDKIIGeometry::makeHolesForHexSupportRods(G4VSolid* baseSolid)
{
	G4Tubs* solidHexRodForSub = new G4Tubs("solidHexRodSubtractionSolid", 0., 1.01 * 0.334 * CLHEP::cm, 10. * CLHEP::cm, 0., CLHEP::twopi);
	string baseName = baseSolid->GetName();
	string newName;
	G4VSolid* finalSolid = baseSolid;
	for (int i = 0; i < 3; i++)
	{

		G4double HexRodx = 4.5853 * sin((i * 120. + 60.) * CLHEP::pi / 180.) * CLHEP::cm;
		G4double HexRody = 4.5853 * cos((i * 120. + 60.) * CLHEP::pi / 180.) * CLHEP::cm;
		G4ThreeVector HexRodLocation = G4ThreeVector(HexRodx, HexRody, 0);
		newName = baseName + "_HexRod" + int2str(i + 1) + "Holes";
		finalSolid = new G4SubtractionSolid(newName.data(), finalSolid, solidHexRodForSub, 0, HexRodLocation);
	}

	return finalSolid;

}

G4VSolid* MRKRDKIIGeometry::makeHolesForShortStainlessSupportRods(G4VSolid* baseSolid)
{
	G4Tubs* solidShortStainlessForSub = new G4Tubs("solidShortStainlessSubtractionSolid", 0., 1.01 * 0.334 * CLHEP::cm, 10. * CLHEP::cm, 0., CLHEP::twopi);
	string baseName = baseSolid->GetName();
	string newName;
	G4VSolid* finalSolid = baseSolid;
	for (int i = 0; i < 3; i++)
	{

		G4double ShortStainlessx = 4.5853 * sin((i * 120. + 90.) * CLHEP::pi / 180.) * CLHEP::cm;
		G4double ShortStainlessy = 4.5853 * cos((i * 120. + 90.) * CLHEP::pi / 180.) * CLHEP::cm;
		G4ThreeVector ShortStainlessLocation = G4ThreeVector(ShortStainlessx, ShortStainlessy, 0);
		newName = baseName + "_ShortStainless" + int2str(i + 1) + "Holes";
		finalSolid = new G4SubtractionSolid(newName.data(), finalSolid, solidShortStainlessForSub, 0, ShortStainlessLocation);
	}

	return finalSolid;

}

G4VSolid* MRKRDKIIGeometry::makeHolesForLongStainlessSupportRods(G4VSolid* baseSolid)
{
	G4Tubs* solidLongStainlessForSub = new G4Tubs("solidLongStainlessSubtractionSolid", 0., 1.01 * 0.334 * CLHEP::cm, 10. * CLHEP::cm, 0., CLHEP::twopi);
	string baseName = baseSolid->GetName();
	string newName;
	G4VSolid* finalSolid = baseSolid;
	for (int i = 0; i < 3; i++)
	{

		G4double LongStainlessx = 4.5853 * sin((i * 120.) * CLHEP::pi / 180.) * CLHEP::cm;
		G4double LongStainlessy = 4.5853 * cos((i * 120.) * CLHEP::pi / 180.) * CLHEP::cm;
		G4ThreeVector LongStainlessLocation = G4ThreeVector(LongStainlessx, LongStainlessy, 0);
		newName = baseName + "_LongStainless" + int2str(i + 1) + "Holes";
		finalSolid = new G4SubtractionSolid(newName.data(), finalSolid, solidLongStainlessForSub, 0, LongStainlessLocation);
	}

	return finalSolid;

}

G4VSolid* MRKRDKIIGeometry::makeHolesForBGOAPDs(G4VSolid* baseSolid)
{
	G4Box* solidBGOAPDForSub = new G4Box("solidBGOAPDAPDSubtractionSolid", 0.84001 * CLHEP::cm, 0.84001 * CLHEP::cm, 1.3 * CLHEP::cm);
	string baseName = baseSolid->GetName();
	string newName;
	G4VSolid* finalSolid = baseSolid;
	for (int i = 0; i < 12; i++)
	{
		G4double BGOAPDRadialOffsetToCenter = (3.604 + 0.635) * CLHEP::cm * AlLengthContractionFactor;
		G4double BGOAPDangle = (-i * 30. - 15. + 90.);
		if(BGOAPDangle > 360.) BGOAPDangle -= 360.;

		G4double BGOAPDx = cos(BGOAPDangle * CLHEP::pi / 180.) * BGOAPDRadialOffsetToCenter;
		G4double BGOAPDy = sin(BGOAPDangle * CLHEP::pi / 180.) * BGOAPDRadialOffsetToCenter;
		G4ThreeVector BGOAPDLocation = G4ThreeVector(BGOAPDx, BGOAPDy, 0);
		newName = baseName + "_BGOAPD" + int2str(i + 1) + "Holes";
		finalSolid = new G4SubtractionSolid(newName.data(), finalSolid, solidBGOAPDForSub, bgoRotationMatrices[i], BGOAPDLocation);
	}

	return finalSolid;

}

G4VSolid* MRKRDKIIGeometry::makeHolesForBAPDs(G4VSolid* baseSolid)
{
	G4Box* solidBAPDForSub = new G4Box("solidBAPDAPDSubtractionSolid", 1.701801 * CLHEP::cm, 1.701801 * CLHEP::cm, 1.3 * CLHEP::cm);
	string baseName = baseSolid->GetName();
	string newName;
	G4VSolid* finalSolid = baseSolid;
	for (int i = 0; i < 3; i++)
	{
		G4double BAPDRadialOffsetToCenter = (3.47218) * CLHEP::cm * AlLengthContractionFactor;
		G4double BAPDangle = (-i * 120. - 30.);
		if(BAPDangle > 360.) BAPDangle -= 360.;

		G4double BAPDx = cos(BAPDangle * CLHEP::pi / 180.) * BAPDRadialOffsetToCenter;
		G4double BAPDy = sin(BAPDangle * CLHEP::pi / 180.) * BAPDRadialOffsetToCenter;
		G4ThreeVector BAPDLocation = G4ThreeVector(BAPDx, BAPDy, 0);
		newName = baseName + "_BAPD" + int2str(i + 1) + "Holes";
		finalSolid = new G4SubtractionSolid(newName.data(), finalSolid, solidBAPDForSub, bapdRotationMatrices[i], BAPDLocation);
	}

	return finalSolid;

}

//Construction of wrappings around BGO bars using two boxes and subtraction instead of unions
void MRKRDKIIGeometry::constructBGOWrappingsImproved()
{

	G4double BGOWidth = 1.2 * CLHEP::cm;
	G4double BGOLength = 20 * CLHEP::cm;
	G4double BGOCenterAlongZ = 22.4 * CLHEP::cm;
	G4double BGORadialOffsetToCenter = 4.2215 * CLHEP::cm * AlLengthContractionFactor;

	//Mylar
	G4double BGOMylarLayerThickness = 12.4 * CLHEP::um;
	G4double BGOMylarLayerWidth = BGOWidth + 2 * BGOMylarLayerThickness;

	G4Box* solidMylarBox = new G4Box("solidMylarBox", .5 * BGOMylarLayerWidth, .5 * BGOMylarLayerWidth, .5 * BGOLength);
	G4Box* solidMylarSubtractionBox = new G4Box("solidMylarSubtractionBox", .5 * BGOWidth + 1 * CLHEP::nm, .5 * BGOWidth + 0.1 * CLHEP::nm, .5 * BGOLength + 1. * CLHEP::mm); //Make sure there will end up being a small airgap before the BGO AND make sure subtraction is good by having the box be longer than necessary
	G4VSolid* solidBGOMylarLayer = new G4SubtractionSolid("solidBGOMylarLayer", solidMylarBox, solidMylarSubtractionBox, 0, G4ThreeVector(0, 0, 0));

	G4LogicalVolume* logicBGOMylarLayer = new G4LogicalVolume(solidBGOMylarLayer, materials.getMaterial("G4_MYLAR"), "logicBGOMylarLayer", 0, 0, 0);
	logicBGOMylarLayer->SetVisAttributes(VisMylar);

	//Aluminum
	G4double BGOAlLayerThickness = 0.065 * CLHEP::um;
	G4double BGOAlLayerWidth = BGOWidth + 2 * BGOAlLayerThickness + 2 * BGOMylarLayerThickness;

	G4Box* solidAlBox = new G4Box("solidAlBox", .5 * BGOAlLayerWidth, .5 * BGOAlLayerWidth, .5 * BGOLength);
	G4Box* solidAlSubtractionBox = new G4Box("solidAlSubtractionBox", .5 * BGOMylarLayerWidth + 1 * CLHEP::nm, .5 * BGOMylarLayerWidth + 0.1 * CLHEP::nm, .5 * BGOLength + 1. * CLHEP::mm); //Make sure there will end up being a small airgap before the BGO AND make sure subtraction is good by having the box be longer than necessary
	G4VSolid* solidBGOAlLayer = new G4SubtractionSolid("solidBGOAlLayer", solidAlBox, solidAlSubtractionBox, 0, G4ThreeVector(0, 0, 0));

	G4LogicalVolume* logicBGOAlLayer = new G4LogicalVolume(solidBGOAlLayer, materials.getMaterial("G4_Al"), "logicBGOAlLayer", 0, 0, 0);
	logicBGOAlLayer->SetVisAttributes(VisG4_Al);

	//Teflon - around ends of BGO

	G4double BGOTeflonLayerThickness = 0.01 * CLHEP::cm;

	//LayerA
	G4double BGOTeflonLayerAWidth = BGOWidth + 2 * BGOTeflonLayerThickness + 2 * BGOMylarLayerThickness + 2 * BGOAlLayerThickness;
	G4double BGOTeflonLayerALength = 1.2 * CLHEP::cm;
	G4double BGOTeflonLayerACenterZ = 31.7 * CLHEP::cm;

	G4Box* solidTeflonLayerABox = new G4Box("solidTeflonBox", .5 * BGOTeflonLayerAWidth, .5 * BGOTeflonLayerAWidth, .5 * BGOTeflonLayerALength);
	G4Box* solidTeflonLayerASubtractionBox = new G4Box("solidTeflonLayerASubtractionBox", .5 * BGOAlLayerWidth + 1 * CLHEP::nm, .5 * BGOAlLayerWidth + 1 * CLHEP::nm, .5 * BGOTeflonLayerALength + 1. * CLHEP::mm); //Make sure there will end up being a small airgap before the BGO AND make sure subtraction is good by having the box be longer than necessary
	G4VSolid* solidBGOTeflonLayerA = new G4SubtractionSolid("solidBGOTeflonLayerA", solidTeflonLayerABox, solidTeflonLayerASubtractionBox, 0, G4ThreeVector(0, 0, 0));

	G4LogicalVolume* logicBGOTeflonLayerA = new G4LogicalVolume(solidBGOTeflonLayerA, materials.getMaterial("StretchedTeflonTape"), "logicBGOTeflonLayerA", 0, 0, 0);
	logicBGOTeflonLayerA->SetVisAttributes(VisG4_StretchedTeflonTape);

	//LayerC
	G4double BGOTeflonLayerCWidth = BGOWidth + 2 * BGOTeflonLayerThickness + 2 * BGOMylarLayerThickness + 2 * BGOAlLayerThickness;
	G4double BGOTeflonLayerCLength = 0.35 * CLHEP::cm;
	G4double BGOTeflonLayerCCenterZ = 12.75 * CLHEP::cm;

	G4Box* solidTeflonLayerCBox = new G4Box("solidTeflonBox", .5 * BGOTeflonLayerCWidth, .5 * BGOTeflonLayerCWidth, .5 * BGOTeflonLayerCLength);
	G4Box* solidTeflonLayerCSubtractionBox = new G4Box("solidTeflonLayerCSubtractionBox", .5 * BGOAlLayerWidth + 1 * CLHEP::nm, .5 * BGOAlLayerWidth + 1 * CLHEP::nm, .5 * BGOTeflonLayerCLength + 1. * CLHEP::mm); //Make sure there will end up being a small airgap before the BGO AND make sure subtraction is good by having the box be longer than necessary
	G4VSolid* solidBGOTeflonLayerC = new G4SubtractionSolid("solidBGOTeflonLayerC", solidTeflonLayerCBox, solidTeflonLayerCSubtractionBox, 0, G4ThreeVector(0, 0, 0));

	G4LogicalVolume* logicBGOTeflonLayerC = new G4LogicalVolume(solidBGOTeflonLayerC, materials.getMaterial("StretchedTeflonTape"), "logicBGOTeflonLayerC", 0, 0, 0);
	logicBGOTeflonLayerC->SetVisAttributes(VisG4_StretchedTeflonTape);

	//LayerB
	G4double BGOTeflonLayerBWidth = BGOWidth + 4 * BGOTeflonLayerThickness + 2 * BGOMylarLayerThickness + 2 * BGOAlLayerThickness;
	G4double BGOTeflonLayerBLength = 1.3 * CLHEP::cm;
	G4double BGOTeflonLayerBCenterZ = 31.6 * CLHEP::cm;

	G4Box* solidTeflonLayerBBox = new G4Box("solidTeflonBox", .5 * BGOTeflonLayerBWidth, .5 * BGOTeflonLayerBWidth, .5 * BGOTeflonLayerBLength);
	G4Box* solidTeflonLayerBSubtractionBox = new G4Box("solidTeflonLayerBSubtractionBox", .5 * BGOTeflonLayerAWidth + 1 * CLHEP::nm, .5 * BGOTeflonLayerAWidth + 1 * CLHEP::nm, .5 * BGOTeflonLayerBLength + 1. * CLHEP::mm); //Make sure there will end up being a small airgap before the BGO AND make sure subtraction is good by having the box be longer than necessary
	G4VSolid* solidBGOTeflonLayerB = new G4SubtractionSolid("solidBGOTeflonLayerB", solidTeflonLayerBBox, solidTeflonLayerBSubtractionBox, 0, G4ThreeVector(0, 0, 0));

	G4LogicalVolume* logicBGOTeflonLayerB = new G4LogicalVolume(solidBGOTeflonLayerB, materials.getMaterial("StretchedTeflonTape"), "logicBGOTeflonLayerB", 0, 0, 0);
	logicBGOTeflonLayerB->SetVisAttributes(VisG4_StretchedTeflonTape);

	//LayerD
	G4double BGOTeflonLayerDWidth = BGOWidth + 4 * BGOTeflonLayerThickness + 2 * BGOMylarLayerThickness + 2 * BGOAlLayerThickness;
	G4double BGOTeflonLayerDLength = 1 * CLHEP::cm;
	G4double BGOTeflonLayerDCenterZ = 12.9 * CLHEP::cm;

	G4Box* solidTeflonLayerDBox = new G4Box("solidTeflonBox", .5 * BGOTeflonLayerDWidth, .5 * BGOTeflonLayerDWidth, .5 * BGOTeflonLayerDLength);
	G4Box* solidTeflonLayerDSubtractionBox = new G4Box("solidTeflonLayerDSubtractionBox", .5 * BGOTeflonLayerCWidth + 1 * CLHEP::nm, .5 * BGOTeflonLayerCWidth + 1 * CLHEP::nm, .5 * BGOTeflonLayerDLength + 1. * CLHEP::mm); //Make sure there will end up being a small airgap before the BGO AND make sure subtraction is good by having the box be longer than necessary
	G4VSolid* solidBGOTeflonLayerD = new G4SubtractionSolid("solidBGOTeflonLayerD", solidTeflonLayerDBox, solidTeflonLayerDSubtractionBox, 0, G4ThreeVector(0, 0, 0));

	G4LogicalVolume* logicBGOTeflonLayerD = new G4LogicalVolume(solidBGOTeflonLayerD, materials.getMaterial("StretchedTeflonTape"), "logicBGOTeflonLayerD", 0, 0, 0);
	logicBGOTeflonLayerD->SetVisAttributes(VisG4_StretchedTeflonTape);

	//LayerE
	G4double BGOTeflonLayerEWidth = BGOWidth + 6 * BGOTeflonLayerThickness + 2 * BGOMylarLayerThickness + 2 * BGOAlLayerThickness;
	G4double BGOTeflonLayerELength = 1.2 * CLHEP::cm;
	G4double BGOTeflonLayerECenterZ = 13.1 * CLHEP::cm;

	G4Box* solidTeflonLayerEBox = new G4Box("solidTeflonBox", .5 * BGOTeflonLayerEWidth, .5 * BGOTeflonLayerEWidth, .5 * BGOTeflonLayerELength);
	G4Box* solidTeflonLayerESubtractionBox = new G4Box("solidTeflonLayerESubtractionBox", .5 * BGOTeflonLayerDWidth + 1 * CLHEP::nm, .5 * BGOTeflonLayerDWidth + 1 * CLHEP::nm, .5 * BGOTeflonLayerELength + 1. * CLHEP::mm); //Make sure there will end up being a small airgap before the BGO AND make sure subtraction is good by having the box be longer than necessary
	G4VSolid* solidBGOTeflonLayerE = new G4SubtractionSolid("solidBGOTeflonLayerE", solidTeflonLayerEBox, solidTeflonLayerESubtractionBox, 0, G4ThreeVector(0, 0, 0));

	G4LogicalVolume* logicBGOTeflonLayerE = new G4LogicalVolume(solidBGOTeflonLayerE, materials.getMaterial("StretchedTeflonTape"), "logicBGOTeflonLayerE", 0, 0, 0);
	logicBGOTeflonLayerE->SetVisAttributes(VisG4_StretchedTeflonTape);

	string physiName;

	for (int i = 0; i < 12; i++)
	{
		G4double BGOangle = (-i * 30. - 15. + 90.);
		if(BGOangle > 360.) BGOangle -= 360.;

		G4double BGOx = cos(BGOangle * CLHEP::pi / 180.) * BGORadialOffsetToCenter;
		G4double BGOy = sin(BGOangle * CLHEP::pi / 180.) * BGORadialOffsetToCenter;

		G4ThreeVector BGOLocationCenter = G4ThreeVector(BGOx, BGOy, BGOCenterAlongZ);

		G4ThreeVector BGOLocationTeflonLayerA = G4ThreeVector(BGOx, BGOy, BGOTeflonLayerACenterZ);
		G4ThreeVector BGOLocationTeflonLayerB = G4ThreeVector(BGOx, BGOy, BGOTeflonLayerBCenterZ);
		G4ThreeVector BGOLocationTeflonLayerC = G4ThreeVector(BGOx, BGOy, BGOTeflonLayerCCenterZ);
		G4ThreeVector BGOLocationTeflonLayerD = G4ThreeVector(BGOx, BGOy, BGOTeflonLayerDCenterZ);
		G4ThreeVector BGOLocationTeflonLayerE = G4ThreeVector(BGOx, BGOy, BGOTeflonLayerECenterZ);

		physiName = "physBGOAlWrap_" + int2str(i + 1);
		new G4PVPlacement(bgoRotationMatrices[i], gammaDetOffset + BGOLocationCenter, logicBGOAlLayer, physiName.data(), logicWorld, false, i);

		physiName = "physBGOMylarWrap_" + int2str(i + 1);
		new G4PVPlacement(bgoRotationMatrices[i], gammaDetOffset + BGOLocationCenter, logicBGOMylarLayer, physiName.data(), logicWorld, false, i);

		physiName = "physBGOTeflonLayerA_" + int2str(i + 1);
		new G4PVPlacement(bgoRotationMatrices[i], gammaDetOffset + BGOLocationTeflonLayerA, logicBGOTeflonLayerA, physiName.data(), logicWorld, false, i);
		physiName = "physBGOTeflonLayerB_" + int2str(i + 1);
		new G4PVPlacement(bgoRotationMatrices[i], gammaDetOffset + BGOLocationTeflonLayerB, logicBGOTeflonLayerB, physiName.data(), logicWorld, false, i);
		physiName = "physBGOTeflonLayerC_" + int2str(i + 1);
		new G4PVPlacement(bgoRotationMatrices[i], gammaDetOffset + BGOLocationTeflonLayerC, logicBGOTeflonLayerC, physiName.data(), logicWorld, false, i);
		physiName = "physBGOTeflonLayerD_" + int2str(i + 1);
		new G4PVPlacement(bgoRotationMatrices[i], gammaDetOffset + BGOLocationTeflonLayerD, logicBGOTeflonLayerD, physiName.data(), logicWorld, false, i);
		physiName = "physBGOTeflonLayerE_" + int2str(i + 1);
		new G4PVPlacement(bgoRotationMatrices[i], gammaDetOffset + BGOLocationTeflonLayerE, logicBGOTeflonLayerE, physiName.data(), logicWorld, false, i);

	}

	G4double AlBeamWrapLength = 18.73 * CLHEP::cm;
	G4double AlBeamWrapCenterZ = 22.35 * CLHEP::cm;
	G4double AlBeamWrapInnerRadius = 3.492 * CLHEP::cm;
	G4double AlBeamWrapOuterRadius = AlBeamWrapInnerRadius + 0.049 * CLHEP::um;

	G4double MylarBeamWrapLength = 18.73 * CLHEP::cm;
	G4double MylarBeamWrapCenterZ = 22.35 * CLHEP::cm;
	G4double MylarBeamWrapInnerRadius = AlBeamWrapOuterRadius;
	G4double MylarBeamWrapOuterRadius = MylarBeamWrapInnerRadius + 12.8 * CLHEP::um;

	G4ThreeVector AlBeamWrapPosition, MylarBeamWrapPosition;
	AlBeamWrapPosition = G4ThreeVector(0, 0, AlBeamWrapCenterZ);
	MylarBeamWrapPosition = G4ThreeVector(0, 0, MylarBeamWrapCenterZ);
	G4Tubs* solidAlBeamWrap = new G4Tubs("solidAlBeamWrap", AlBeamWrapInnerRadius, AlBeamWrapOuterRadius, 0.5 * AlBeamWrapLength, 0., CLHEP::twopi);
	G4LogicalVolume* logicAlBeamWrap = new G4LogicalVolume(solidAlBeamWrap, materials.getMaterial("G4_Al"), "logicAlBeamWrap", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + AlBeamWrapPosition, logicAlBeamWrap, "physiAlBeamWrap", logicWorld, false, 0);

	G4Tubs* solidMylarBeamWrap = new G4Tubs("solidMylarBeamWrap", MylarBeamWrapInnerRadius, MylarBeamWrapOuterRadius, 0.5 * MylarBeamWrapLength, 0., CLHEP::twopi);
	G4LogicalVolume* logicMylarBeamWrap = new G4LogicalVolume(solidMylarBeamWrap, materials.getMaterial("G4_MYLAR"), "logicMylarBeamWrap", 0, 0, 0);
	new G4PVPlacement(0, gammaDetOffset + MylarBeamWrapPosition, logicMylarBeamWrap, "physiMylarBeamWrap", logicWorld, false, 0);

	logicAlBeamWrap->SetVisAttributes(G4VisAttributes::Invisible);
	logicMylarBeamWrap->SetVisAttributes(G4VisAttributes::Invisible);

}
