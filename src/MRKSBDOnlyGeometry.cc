#include "MRKSBDOnlyGeometry.hh"

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

using namespace CLHEP;

MRKSBDOnlyGeometry::MRKSBDOnlyGeometry(MRKMacroMessenger* inpMacroMessenger) :
	MRKGeometry(inpMacroMessenger)
{
	defineMacroCommands(inpMacroMessenger);

	logicSBDDetectionVolume = nullptr;
	logicSBDDetectorEnvelope = nullptr;

	use1_5mmSBD = true;
	SBDGoldLayerOff = false;
	SBDCanOff = false;

	useSBDDetector = true;

	VisStainlessSteel304 = new G4VisAttributes(G4Colour(.6, .6, .8));
	VisG4_Al = new G4VisAttributes(G4Colour(.7, .7, .7));
	VisG4_Galactic = new G4VisAttributes(G4VisAttributes::Invisible);
	VisG4_POLYCARBONATE = new G4VisAttributes(G4Colour(.9, .9, 1.0));
	VisG4_Si = new G4VisAttributes(G4Colour(1.0, 0, 0));
	VisG4_ALUMINUM_OXIDE = new G4VisAttributes(G4Colour(1, 1, 0.9));
	VisG4_Au = new G4VisAttributes(G4Colour(1, 1, 0));
	VisCartridgeBrass = new G4VisAttributes(G4Colour(.8, .8, 0.0));
	VisMylar = new G4VisAttributes(G4Colour(.7, .7, 1));
	VisEpoxy = new G4VisAttributes(G4Colour(.8, .4, .4));

	SiDeadLayerLength = 22 * nm;
	sbdThicknessModifier=0;

}

MRKSBDOnlyGeometry::~MRKSBDOnlyGeometry()
{

}

void MRKSBDOnlyGeometry::defineMacroCommands(MRKMacroMessenger* inpMacroMessenger)
{
	//Commands will be deleted by inpMacroMessenger
	std::function<void(G4String)> func;

	G4UIdirectory* detDir = new G4UIdirectory("/MRK/det/");
	detDir->SetGuidance("Detector Construction");
	inpMacroMessenger->addCommand(MRKCommandAndFunction(detDir, nullptr));

	G4UIcmdWithABool* use1_5mmSBDCmd = new G4UIcmdWithABool("/MRK/det/use15mmSBD", inpMacroMessenger);
	use1_5mmSBDCmd->SetGuidance("Sets true/false to use 1.5mm SBD or instead the 1.0mm SBD");
	use1_5mmSBDCmd->SetParameterName("use15mmSBD", false);
	use1_5mmSBDCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setUse1_5mmSBD(G4UIcommand::ConvertToBool(a));};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(use1_5mmSBDCmd, func));

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

}

G4VPhysicalVolume* MRKSBDOnlyGeometry::Construct()
{
	G4cout << "Constructing SBD only" << G4endl;
	if(!use1_5mmSBD)
	{
		sbdThicknessModifier = -0.5*mm;
		sbdThicknessModifierVector = G4ThreeVector(0,0,-0.5*sbdThicknessModifier); //Positive
	}
	else
	{
		sbdThicknessModifier = 0*mm;
		sbdThicknessModifierVector = G4ThreeVector(0,0,0); //Positive
	}
	G4cout << "SBD Silicon dead layer thickness: " << SiDeadLayerLength << G4endl;


	materials.createMaterials();

	constructWorld();

	logicSBDDetectorEnvelope = constructSBDDetectorEnvelope();

	logicSBDDetectionVolume = constructRDK2SBDSilicon();

	constructRDK2SBDSiliconHolder();
	if(SiDeadLayerLength > 0.1*nm)
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

	G4cout << "Making SBD Sensitive Detector" << G4endl;

	G4SDManager* SDman = G4SDManager::GetSDMpointer();

	SDman->SetVerboseLevel(1);
	MRKSurfaceBarrierDetector* newSBDDet = new MRKSurfaceBarrierDetector("SBD_det");
	SDman->AddNewDetector(newSBDDet);
	logicSBDDetectionVolume->SetSensitiveDetector(newSBDDet);
	SDman->SetVerboseLevel(0);

	G4cout << "------Returning world from detector construction----" << G4endl;

	return physiWorld;

}


void MRKSBDOnlyGeometry::constructWorld()
{
	G4double WorldLength = 10 * cm;
	G4double WorldWidth = 10 * cm;
	G4double WorldHeight = 10 * cm;
	G4GeometryManager::GetInstance()->SetWorldMaximumExtent(WorldLength);
	G4cout << "Computed tolerance = " << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance() / mm << " mm" << G4endl;

	G4Box* solidWorld = new G4Box("world", WorldLength * .5, WorldWidth * .5, WorldHeight * .5);
	logicWorld = new G4LogicalVolume(solidWorld, materials.getMaterial("G4_Galactic"), "World", 0, 0, 0);

	//  Must place the World Physical volume unrotated at (0,0,0).
	//
	physiWorld = new G4PVPlacement(0,// no rotation
					G4ThreeVector(), // at (0,0,0)
					logicWorld,      // its logical volume
					"World",         // its name
					0,               // its mother  volume
					false,           // no boolean operations
					0);              // copy number
	logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
}

G4LogicalVolume*  MRKSBDOnlyGeometry::constructSBDDetectorEnvelope()
{
	G4ThreeVector position = G4ThreeVector(0,0,-0.556864*cm)+sbdSiliconFrontCenter;
	double thickness=1.6644*cm;

	G4Tubs* solidSBDDetectorEnvelope = new G4Tubs("solidSBDDetectorEnvelope", 0., 1.805 * cm, thickness*0.5, 0., twopi);
	G4LogicalVolume* logicSBDDetectorEnvelope = new G4LogicalVolume(solidSBDDetectorEnvelope, materials.getMaterial("G4_Galactic"), "logicSBDDetectorEnvelope", 0, 0, 0);
	new G4PVPlacement(nullptr, position, logicSBDDetectorEnvelope, "physiSBD1mm5SiliconEnvelope", logicWorld, false, 0);
	logicSBDDetectorEnvelope->SetVisAttributes(G4VisAttributes::Invisible);
	return logicSBDDetectorEnvelope;

}

G4LogicalVolume* MRKSBDOnlyGeometry::constructRDK2SBDSiliconHolder()
{
	G4ThreeVector position = G4ThreeVector(0,0,0.51851*cm);
	G4Tubs* solidSBD1mm5SiliconHolder = new G4Tubs("solidSBD1mm5SiliconHolder", 1.382 * cm, 1.74719 * cm, 0.24384 * cm, 0., twopi);
	G4LogicalVolume* logicSBD1mm5SiliconHolder = new G4LogicalVolume(solidSBD1mm5SiliconHolder, materials.getMaterial("G4_ALUMINUM_OXIDE"), "logicSBD1mm5SiliconHolder", 0, 0, 0);
	new G4PVPlacement(nullptr,  position, logicSBD1mm5SiliconHolder, "physiSBD1mm5SiliconHolder", logicSBDDetectorEnvelope, false, 0);
	G4UserLimits* stepLimit = new G4UserLimits(1 * nm);
	logicSBD1mm5SiliconHolder->SetUserLimits(stepLimit);
	logicSBD1mm5SiliconHolder->SetVisAttributes(VisG4_ALUMINUM_OXIDE);
	return logicSBD1mm5SiliconHolder;
}

G4LogicalVolume* MRKSBDOnlyGeometry::constructRDK2SBDSilicon()
{
	G4ThreeVector position = G4ThreeVector(0,0,0.481864*cm-.5*SiDeadLayerLength)+sbdThicknessModifierVector;
	double thickness=0.1499999999*cm-.5*SiDeadLayerLength+sbdThicknessModifier;

	G4Tubs* solidSBD1mm5Silicon = new G4Tubs("solidSBD1mm5Silicon", 0 * cm, 1.382 * cm, 0.5*thickness, 0., twopi);
	G4LogicalVolume* logicSBD1mm5Silicon = new G4LogicalVolume(solidSBD1mm5Silicon, materials.getMaterial("G4_Si"), "logicSBD1mm5Silicon", 0, 0, 0);
	new G4PVPlacement(nullptr, position, logicSBD1mm5Silicon, "physiSBD1mm5Silicon", logicSBDDetectorEnvelope, false, 0);
	G4UserLimits* stepLimit = new G4UserLimits(1 * nm);
	logicSBD1mm5Silicon->SetUserLimits(stepLimit);
	logicSBD1mm5Silicon->SetVisAttributes(VisG4_Si);
	return logicSBD1mm5Silicon;
}

G4LogicalVolume* MRKSBDOnlyGeometry::constructRDK2SBDDeadLayer()
{
	G4ThreeVector position = G4ThreeVector(0., 0. , 0.556865 * cm); //Get to center of gold layer
	position += G4ThreeVector(0., 0., -0.5*20 * nm); //shift from gold layer position
	position += G4ThreeVector(0., 0., -SiDeadLayerLength * .5); //shift based on deadlayer thickness
	G4Tubs* solidSBDDeadLayer = new G4Tubs("solidSBDDeadLayer", 0 * cm, 1.382 * cm, 0.5 * SiDeadLayerLength, 0., twopi);
	G4LogicalVolume* logicSBDDeadLayer = new G4LogicalVolume(solidSBDDeadLayer, materials.getMaterial("G4_SILICON_DIOXIDE"), "logicSBDDeadLayer", 0, 0, 0);
	new G4PVPlacement(nullptr, position, logicSBDDeadLayer, "physiSBDDeadLayer", logicSBDDetectorEnvelope, false, 0);
	G4UserLimits* stepLimit = new G4UserLimits(.001 * nm);
	logicSBDDeadLayer->SetUserLimits(stepLimit);
	logicSBDDeadLayer->SetVisAttributes(VisG4_Si);
	return logicSBDDeadLayer;
}

G4LogicalVolume* MRKSBDOnlyGeometry::constructRDK2SBDGoldCoating()
{
	G4ThreeVector position = G4ThreeVector(0., 0. , 0.556865 * cm); //Get to center of gold layer
	G4Tubs* solidSBD1mm5GoldCoating = new G4Tubs("solidSBD1mm5GoldCoating", 0 * cm, 1.382 * cm, 0.5*19.999*nm, 0., twopi);
	G4LogicalVolume* logicSBD1mm5GoldCoating = new G4LogicalVolume(solidSBD1mm5GoldCoating, materials.getMaterial("G4_Au"), "logicSBD1mm5GoldCoating", 0, 0, 0);
	new G4PVPlacement(nullptr, position, logicSBD1mm5GoldCoating, "physiSBD1mm5GoldCoating", logicSBDDetectorEnvelope, false, 0);
	G4UserLimits* stepLimit = new G4UserLimits(.001 * nm);
	logicSBD1mm5GoldCoating->SetUserLimits(stepLimit);
	logicSBD1mm5GoldCoating->SetVisAttributes(VisG4_Au);
	return logicSBD1mm5GoldCoating;
}

G4LogicalVolume* MRKSBDOnlyGeometry::constructRDK2SBDAluminumCoating()
{

	G4ThreeVector position = G4ThreeVector(0.,0.,0.406809*cm) + sbdThicknessModifierVector;
	G4Tubs* solidSBD1mm5AluminumCoating = new G4Tubs("solidSBD1mm5AluminumCoating", 0 * cm, 1.382 * cm, 0.0000549999999996942 * cm, 0., twopi);
	G4LogicalVolume* logicSBD1mm5AluminumCoating = new G4LogicalVolume(solidSBD1mm5AluminumCoating, materials.getMaterial("G4_Al"), "logicSBD1mm5AluminumCoating", 0, 0, 0);
	new G4PVPlacement(nullptr, position, logicSBD1mm5AluminumCoating, "physiSBD1mm5AluminumCoating", logicSBDDetectorEnvelope, false, 0);
	logicSBD1mm5AluminumCoating->SetVisAttributes(VisG4_Al);
	return logicSBD1mm5AluminumCoating;
}


G4LogicalVolume* MRKSBDOnlyGeometry::constructRDK2SBDPlasticLining()
{
	G4ThreeVector position = G4ThreeVector(0.* cm, 0 * cm, 0.48997 * cm);
	G4Tubs* solidSBD1mm5PlasticLining = new G4Tubs("solidSBD1mm5PlasticLining", 1.74719 * cm, 1.75039 * cm, 0.27238 * cm, 0., twopi);
	G4LogicalVolume* logicSBD1mm5PlasticLining = new G4LogicalVolume(solidSBD1mm5PlasticLining, materials.getMaterial("G4_POLYCARBONATE"), "logicSBD1mm5PlasticLining", 0, 0, 0);
	new G4PVPlacement(nullptr, position, logicSBD1mm5PlasticLining, "physiSBD1mm5PlasticLining", logicSBDDetectorEnvelope, false, 0);
	logicSBD1mm5PlasticLining->SetVisAttributes(VisG4_POLYCARBONATE);
	return logicSBD1mm5PlasticLining;
}

G4LogicalVolume* MRKSBDOnlyGeometry::constructRDK2SBDBackBrassRings()
{
	G4ThreeVector position = G4ThreeVector(0.* cm, 0 * cm, 0.25943 * cm);
	G4Tubs* solidSBD1mm5BackBrassRings = new G4Tubs("solidSBD1mm5BackBrassRings", 1.511138 * cm, 1.74719 * cm, 0.01524 * cm, 0., twopi);
	G4LogicalVolume* logicSBD1mm5BackBrassRings = new G4LogicalVolume(solidSBD1mm5BackBrassRings, materials.getMaterial("CartridgeBrass"), "logicSBD1mm5BackBrassRings", 0, 0, 0);
	new G4PVPlacement(nullptr, position, logicSBD1mm5BackBrassRings, "physiSBD1mm5BackBrassRings", logicSBDDetectorEnvelope, false, 0);
	logicSBD1mm5BackBrassRings->SetVisAttributes(VisCartridgeBrass);
	return logicSBD1mm5BackBrassRings;
}

G4LogicalVolume* MRKSBDOnlyGeometry::constructRDK2SBDInternalBrassDisc()
{
	G4ThreeVector position = G4ThreeVector(0.* cm, 0 * cm, 0.23089 * cm);
	G4Tubs* solidSBD1mm5InternalBrassDisc = new G4Tubs("solidSBD1mm5InternalBrassDisc", 0.15748 * cm, 1.74719 * cm, 0.0132999999999992 * cm, 0., twopi);
	G4LogicalVolume* logicSBD1mm5InternalBrassDisc = new G4LogicalVolume(solidSBD1mm5InternalBrassDisc, materials.getMaterial("CartridgeBrass"), "logicSBD1mm5InternalBrassDisc", 0, 0, 0);
	new G4PVPlacement(nullptr, position, logicSBD1mm5InternalBrassDisc, "physiSBD1mm5InternalBrassDisc", logicSBDDetectorEnvelope, false, 0);
	logicSBD1mm5InternalBrassDisc->SetVisAttributes(VisCartridgeBrass);
	return logicSBD1mm5InternalBrassDisc;
}

G4LogicalVolume* MRKSBDOnlyGeometry::constructRDK2SBDSpring()
{
	G4ThreeVector position = G4ThreeVector(0. * cm, 0. * cm, -0.088205 * cm);
	G4Tubs* solidSBD1mm5Spring = new G4Tubs("solidSBD1mm5Spring", 0.029 * cm, 0.222 * cm, 0.305795 * cm, 0., twopi);
	G4LogicalVolume* logicSBD1mm5Spring = new G4LogicalVolume(solidSBD1mm5Spring, materials.getMaterial("CartridgeBrass"), "logicSBD1mm5Spring", 0, 0, 0);
	new G4PVPlacement(nullptr, position, logicSBD1mm5Spring, "physiSBD1mm5Spring", logicSBDDetectorEnvelope, false, 0);
	logicSBD1mm5Spring->SetVisAttributes(VisCartridgeBrass);
	return logicSBD1mm5Spring;
}

G4LogicalVolume* MRKSBDOnlyGeometry::constructRDK2SBDHexScrew()
{
	G4ThreeVector position = G4ThreeVector(0. * cm, 0. * cm, -0.6131 * cm);
	G4Tubs* solidSBD1mm5HexScrew = new G4Tubs("solidSBD1mm5HexScrew", 0.20955 * cm, 0.4178 * cm, 0.219099999999999 * cm, 0., twopi);
	G4LogicalVolume* logicSBD1mm5HexScrew = new G4LogicalVolume(solidSBD1mm5HexScrew, materials.getMaterial("CartridgeBrass"), "logicSBD1mm5HexScrew", 0, 0, 0);
	new G4PVPlacement(nullptr, position, logicSBD1mm5HexScrew, "physiSBD1mm5HexScrew", logicSBDDetectorEnvelope, false, 0);
	logicSBD1mm5HexScrew->SetVisAttributes(VisCartridgeBrass);
	return logicSBD1mm5HexScrew;
}

G4LogicalVolume* MRKSBDOnlyGeometry::constructRDK2SBDFrontCase()
{
	G4ThreeVector position = G4ThreeVector(0. * cm, 0. * cm, 0.797275 * cm);
	G4Tubs* solidSBD1mm5FrontCase = new G4Tubs("solidSBD1mm5FrontCase", 1.511138 * cm, 1.805 * cm, 0.0349249999999994 * cm, 0., twopi);
	G4LogicalVolume* logicSBD1mm5FrontCase = new G4LogicalVolume(solidSBD1mm5FrontCase, materials.getMaterial("CartridgeBrass"), "logicSBD1mm5FrontCase", 0, 0, 0);
	new G4PVPlacement(nullptr, position, logicSBD1mm5FrontCase, "physiSBD1mm5FrontCase", logicSBDDetectorEnvelope, false, 0);
	G4UserLimits* stepLimit = new G4UserLimits(1 * nm);
	logicSBD1mm5FrontCase->SetUserLimits(stepLimit);
	logicSBD1mm5FrontCase->SetVisAttributes(VisCartridgeBrass);
	return logicSBD1mm5FrontCase;
}

G4LogicalVolume* MRKSBDOnlyGeometry::constructRDK2SBDMidCase()
{
	G4ThreeVector position = G4ThreeVector(0. * cm, 0. * cm, 0.127675 * cm);
	G4Tubs* solidSBD1mm5MidCase = new G4Tubs("solidSBD1mm5MidCase", 1.75039 * cm, 1.805 * cm, 0.634675 * cm, 0., twopi);
	G4LogicalVolume* logicSBD1mm5MidCase = new G4LogicalVolume(solidSBD1mm5MidCase, materials.getMaterial("CartridgeBrass"), "logicSBD1mm5MidCase", 0, 0, 0);
	new G4PVPlacement(nullptr, position, logicSBD1mm5MidCase, "physiSBD1mm5MidCase", logicSBDDetectorEnvelope, false, 0);
	logicSBD1mm5MidCase->SetVisAttributes(VisCartridgeBrass);
	return logicSBD1mm5MidCase;
}

G4LogicalVolume* MRKSBDOnlyGeometry::constructRDK2SBDBackCase()
{
	G4ThreeVector position = G4ThreeVector(0. * cm, 0. * cm, -0.5324 * cm);
	G4Tubs* solidSBD1mm5BackCase = new G4Tubs("solidSBD1mm5BackCase", 0.4178 * cm, 1.805 * cm, 0.0253999999999994 * cm, 0., twopi);
	G4LogicalVolume* logicSBD1mm5BackCase = new G4LogicalVolume(solidSBD1mm5BackCase, materials.getMaterial("CartridgeBrass"), "logicSBD1mm5BackCase", 0, 0, 0);
	new G4PVPlacement(nullptr, position, logicSBD1mm5BackCase, "physiSBD1mm5BackCase", logicSBDDetectorEnvelope, false, 0);
	logicSBD1mm5BackCase->SetVisAttributes(VisCartridgeBrass);
	return logicSBD1mm5BackCase;
}

void MRKSBDOnlyGeometry::constructSBDEpoxy()
{
	G4double SBDEpoxyLength = .1599999 * cm;
	G4double SBDEpoxyTopCenterZ = 0.556884 * cm + 20 * nm + SBDEpoxyLength * .5;
	G4double SBDEpoxyBottomCenterZ = 0.406664 * cm -20*nm -sbdThicknessModifier- SBDEpoxyLength * .5;
	G4double SBDEpoxyInnerRadius2 = 1.372 * cm;
	G4double SBDEpoxyOuterRadius2 = 1.38 * cm;
	G4double SBDEpoxyInnerRadius1 = 1.3 * cm;
	G4double SBDEpoxyOuterRadius1 = 1.38 * cm;
	G4ThreeVector SBDEpoxyPositionTop = G4ThreeVector(0., 0, SBDEpoxyTopCenterZ );
	G4ThreeVector SBDEpoxyPositionBottom = G4ThreeVector(0., 0, SBDEpoxyBottomCenterZ );
	G4Cons* solidSBDEpoxy = new G4Cons("solidSBDEpoxy", SBDEpoxyInnerRadius1, SBDEpoxyOuterRadius1, SBDEpoxyInnerRadius2, SBDEpoxyOuterRadius2, .5 * SBDEpoxyLength, 0., twopi);

	G4LogicalVolume* logicSBDEpoxy = new G4LogicalVolume(solidSBDEpoxy, materials.getMaterial("EpoxyResin"), "logicSBDEpoxy", 0, 0, 0);
	G4UserLimits* stepLimit = new G4UserLimits(1 * nm);
	logicSBDEpoxy->SetUserLimits(stepLimit);

	G4RotationMatrix* flippedAngle = new G4RotationMatrix();
	flippedAngle->rotateY((180) * deg);
	new G4PVPlacement(flippedAngle, SBDEpoxyPositionTop, logicSBDEpoxy, "physiSBDEpoxyTop", logicSBDDetectorEnvelope, false, 0);
	new G4PVPlacement(nullptr, SBDEpoxyPositionBottom, logicSBDEpoxy, "physiSBDEpoxyBottom", logicSBDDetectorEnvelope, false, 0);

	logicSBDEpoxy->SetVisAttributes(VisEpoxy);
}
