#include "MRKDetectorConstruction.hh"

#include "MRKRDKIIGeometry.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"

using namespace CLHEP;
MRKDetectorConstruction::MRKDetectorConstruction(MRKMacroMessenger* inpMacroMessenger)
{
	theMacroMessenger = inpMacroMessenger;
	defineMacroCommands(inpMacroMessenger);

	physiWorld = nullptr;
	theGeometry = nullptr;

	// ensure the global field is initialized
	theGlobalField = MRKGlobalField::getObject();

	experimentModel = MODEL_RDK2;
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

	G4UIcmdWithAString* modelCmd = new G4UIcmdWithAString("/MRK/det/setmodel",inpMacroMessenger);
	modelCmd->SetGuidance("Sets model used in detector construction");
	modelCmd->SetParameterName("filename",false);
	modelCmd->AvailableForStates(G4State_PreInit);
	func = [=](G4String a)
	{	this->setExperimentModel(a);};
	inpMacroMessenger->addCommand(MRKCommandAndFunction(modelCmd, func));

}

void MRKDetectorConstruction::setExperimentModel(G4String inpModelString)
{
	if(inpModelString == "RDK2")
		experimentModel = MODEL_RDK2;
	else
		G4cout << inpModelString << " not a recognized model" << inpModelString;
	}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* MRKDetectorConstruction::Construct()
{
	switch (experimentModel)
	{
		case MODEL_RDK2:
			theGeometry = new MRKRDKIIGeometry(theMacroMessenger);
			break;
		default:
			break;
	}
	physiWorld = theGeometry->Construct();

	theGlobalField->constructFields(theGeometry->getWorldLogicalVolume());

	return physiWorld;
}

