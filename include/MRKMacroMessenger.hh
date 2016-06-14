#ifndef SRC_MRKMACROMESSENGER_HH_
#define SRC_MRKMACROMESSENGER_HH_

#include <functional>
#include <unordered_map>
#include <string>
#include <utility>
#include <vector>

#include "G4UImessenger.hh"
#include "G4UIcommand.hh"
#include "G4String.hh"
#include "G4UIdirectory.hh"

using MRKCommandAndFunction=std::pair<G4UIcommand*,std::function<void(G4String)>>;

class MRKMacroMessenger: public G4UImessenger
{
public:
	MRKMacroMessenger();
	virtual ~MRKMacroMessenger();

	void addCommand(MRKCommandAndFunction inpCommandAndFunctionPair);
	void SetNewValue(G4UIcommand* command,G4String newValue); //From G4UImessenger
private:
	std::unordered_map<std::string, MRKCommandAndFunction> commandMap;  //Map of command paths to the commands
	std::vector<G4UIdirectory> directories;
};

#endif /* SRC_MRKMACROMESSENGER_HH_ */
