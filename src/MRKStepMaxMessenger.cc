#include "G4UIcmdWithADoubleAndUnit.hh"

#include "../include/MRKStepMax.hh"
#include "../include/MRKStepMaxMessenger.hh"

MRKStepMaxMessenger::MRKStepMaxMessenger(MRKStepMax* stepM) :
	stepMax(stepM)
{
	StepMaxCmd = new G4UIcmdWithADoubleAndUnit("/MRK/stepMax", this);
	StepMaxCmd->SetGuidance("Set max allowed step length");
	StepMaxCmd->SetParameterName("mxStep", false);
	StepMaxCmd->SetRange("mxStep>0.");
	StepMaxCmd->SetUnitCategory("Length");
	StepMaxCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

MRKStepMaxMessenger::~MRKStepMaxMessenger()
{
	delete StepMaxCmd;
}

void MRKStepMaxMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if(command == StepMaxCmd)
	{
		stepMax->SetMaxStep(StepMaxCmd->GetNewDoubleValue(newValue));
	}
}

