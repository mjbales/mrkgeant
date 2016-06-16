#include "MRKMacroMessenger.hh"

MRKMacroMessenger::MRKMacroMessenger()
{

}

MRKMacroMessenger::~MRKMacroMessenger()
{
	for (auto& x : commandMap)
	{
		delete x.second.first;
	}
}

void MRKMacroMessenger::addCommand(MRKCommandAndFunction inpCommandAndFunctionPair)
{
	commandMap[inpCommandAndFunctionPair.first->GetCommandPath()] = inpCommandAndFunctionPair;
}

void MRKMacroMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if(command->GetMessenger() == nullptr) //Skip directories
		return;
	commandMap[command->GetCommandPath()].second(newValue);
}
