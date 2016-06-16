#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"

#include "MRKPhysicsList.hh"
#include "MRKPhysicsListMessenger.hh"

MRKPhysicsListMessenger::MRKPhysicsListMessenger(MRKPhysicsList* pPhys) :
	pPhysicsList(pPhys)
{
	physDir = new G4UIdirectory("/MRK/phys/");
	physDir->SetGuidance("physics list commands");

	gammaCutCmd = new G4UIcmdWithADoubleAndUnit("/MRK/phys/setGCut", this);
	gammaCutCmd->SetGuidance("Set gamma cut.");
	gammaCutCmd->SetParameterName("Gcut", false);
	gammaCutCmd->SetUnitCategory("Length");
	gammaCutCmd->SetRange("Gcut>0.0");
	gammaCutCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	electCutCmd = new G4UIcmdWithADoubleAndUnit("/MRK/phys/setECut", this);
	electCutCmd->SetGuidance("Set electron cut.");
	electCutCmd->SetParameterName("Ecut", false);
	electCutCmd->SetUnitCategory("Length");
	electCutCmd->SetRange("Ecut>0.0");
	electCutCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	protoCutCmd = new G4UIcmdWithADoubleAndUnit("/MRK/phys/setPCut", this);
	protoCutCmd->SetGuidance("Set positron cut.");
	protoCutCmd->SetParameterName("Pcut", false);
	protoCutCmd->SetUnitCategory("Length");
	protoCutCmd->SetRange("Pcut>0.0");
	protoCutCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	allCutCmd = new G4UIcmdWithADoubleAndUnit("/MRK/phys/setCuts", this);
	allCutCmd->SetGuidance("Set cut for all.");
	allCutCmd->SetParameterName("cut", false);
	allCutCmd->SetUnitCategory("Length");
	allCutCmd->SetRange("cut>0.0");
	allCutCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	rCmd = new G4UIcmdWithADoubleAndUnit("/MRK/phys/getRange", this);
	rCmd->SetGuidance("get the electron cut for the current material.");
	rCmd->SetParameterName("energy", false);
	rCmd->SetRange("energy>0.");
	rCmd->SetUnitCategory("Energy");
	rCmd->AvailableForStates(G4State_Idle);

	pListCmd = new G4UIcmdWithAString("/MRK/phys/addPhysics", this);
	pListCmd->SetGuidance("Add modula physics list.");
	pListCmd->SetParameterName("PList", false);
	pListCmd->AvailableForStates(G4State_PreInit);
}

MRKPhysicsListMessenger::~MRKPhysicsListMessenger()
{
	delete gammaCutCmd;
	delete electCutCmd;
	delete protoCutCmd;
	delete allCutCmd;
	delete rCmd;
	delete pListCmd;
	delete physDir;
}

void MRKPhysicsListMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if(command == gammaCutCmd)
	{
		pPhysicsList->SetCutForGamma(gammaCutCmd->GetNewDoubleValue(newValue));
	}

	if(command == electCutCmd)
	{
		pPhysicsList->SetCutForElectron(electCutCmd->GetNewDoubleValue(newValue));
	}

	if(command == protoCutCmd)
	{
		pPhysicsList->SetCutForPositron(protoCutCmd->GetNewDoubleValue(newValue));
	}

	if(command == allCutCmd)
	{
		G4double cut = allCutCmd->GetNewDoubleValue(newValue);
		pPhysicsList->SetCutForGamma(cut);
		pPhysicsList->SetCutForElectron(cut);
		pPhysicsList->SetCutForPositron(cut);
		pPhysicsList->SetCutForProton(cut);
	}

	if(command == pListCmd)
	{
		pPhysicsList->AddPhysicsList(newValue);
	}
}

