#include "MRKDetectorConstruction.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"

#include "MRKRDKIIGeometry.hh"
#include "MRKSBDOnlyGeometry.hh"

using namespace CLHEP;
MRKDetectorConstruction::MRKDetectorConstruction(MRKMacroMessenger* inpMacroMessenger)
{
	theMacroMessenger = inpMacroMessenger;
	defineMacroCommands(inpMacroMessenger);

	physiWorld = nullptr;
	theGeometry = new MRKRDKIIGeometry(theMacroMessenger);

	// ensure the global field is initialized
	theGlobalField = MRKGlobalField::getObject();

	experimentModel = MRKExperimentModel::MODEL_RDK2;
}

MRKDetectorConstruction::~MRKDetectorConstruction()
{

	G4cout << "Beginning to delete Detector Construction Object...";
	delete theGeometry;
	G4cout << "COMPLETED!" << G4endl;

}

void MRKDetectorConstruction::defineMacroCommands(MRKMacroMessenger* inpMacroMessenger)
{
	//Commands will be deleted by inpMacroMessenger
	std::function<void(G4String)> func;

	G4UIdirectory* detDir = new G4UIdirectory("/MRK/det/");
	detDir->SetGuidance("Detector Construction");
	inpMacroMessenger->addCommand(MRKCommandAndFunction(detDir, nullptr));

	G4UIcmdWithAString* modelCmd = new G4UIcmdWithAString("/MRK/det/setmodel", inpMacroMessenger);
	modelCmd->SetGuidance("Sets model used in detector construction");
	modelCmd->SetParameterName("filename", false);
	modelCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setExperimentModel(a);};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(modelCmd, func));

}

void MRKDetectorConstruction::setExperimentModel(G4String inpModelString)
{
	delete theGeometry;
	theGeometry = nullptr;
	if(inpModelString == "RDK2")
	{
		experimentModel = MRKExperimentModel::MODEL_RDK2;
		theGeometry = new MRKRDKIIGeometry(theMacroMessenger);
	}
	else if(inpModelString == "SBDONLY" || inpModelString == "SBDOnly")
	{
		G4cout << "Set to SBD Only Model" << G4endl;
		experimentModel = MRKExperimentModel::MODEL_SBDONLY;
		theGeometry = new MRKSBDOnlyGeometry(theMacroMessenger);
	}
	else
	{
		G4cout << inpModelString << " not a recognized model" << inpModelString << G4endl;
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* MRKDetectorConstruction::Construct()
{
	physiWorld = theGeometry->Construct();

	theGlobalField->constructFields(theGeometry->getWorldLogicalVolume());

	return physiWorld;
}

