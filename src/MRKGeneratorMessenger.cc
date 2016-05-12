#include "GeneratorMessenger.hh"

#include "MRKPrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "globals.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GeneratorMessenger::GeneratorMessenger(MRKPrimaryGeneratorAction* myGen)
:myGenerator(myGen)
{

    fileDir = new G4UIdirectory("/MRK/file/");
    fileDir->SetGuidance("file control");

    genInpCmd = new G4UIcmdWithAString("/MRK/file/inpfile",this);
    genInpCmd->SetGuidance("Choose generator input file.");
    genInpCmd->SetParameterName("filename",false);
    genInpCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    fluxMapCmd = new G4UIcmdWithAString("/MRK/file/fluxmap",this);
    fluxMapCmd->SetGuidance("Choose fluxmap input file");
    fluxMapCmd->SetParameterName("filename",false);
    fluxMapCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    eventByEventVerbose= new G4UIcmdWithABool("/MRK/eventByEventVerbose",this);
    eventByEventVerbose->SetGuidance("Sets whether to display event by even text.");
    eventByEventVerbose->SetParameterName("bool",false);
    eventByEventVerbose->AvailableForStates(G4State_PreInit,G4State_Idle);

    genModeCmd=new G4UIcmdWithAnInteger("/MRK/file/genMode",this);
    genModeCmd->SetGuidance("Choose generator mode: 0=PARTICLE_GUN,1=RDKEVENTFILE,3=EVENTGENERATOR,4=PARTICLE_SOURCE");
    genModeCmd->SetParameterName("mode",false);
    genModeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    eventSetNumberCmd=new G4UIcmdWithAnInteger("/MRK/file/eventSetNumber",this);
    eventSetNumberCmd->SetGuidance("The event set that should be run for the event generator");
    eventSetNumberCmd->SetParameterName("set",false);
    eventSetNumberCmd->AvailableForStates(G4State_PreInit,G4State_Idle);


    eventStartCmd=new G4UIcmdWithAnInteger("/MRK/file/eventstart",this);
    eventStartCmd->SetGuidance("Choose the starting event");
    eventStartCmd->SetParameterName("mode",false);
    eventStartCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    printRandomNumbersCmd=new G4UIcmdWithAnInteger("/MRK/printRandomNumbers",this);
    printRandomNumbersCmd->SetGuidance("Prints a number of random numbers for testing.  Note: EventSettings File must be loaded first.");
    printRandomNumbersCmd->SetParameterName("mode",false);
    printRandomNumbersCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    setNumberOfEventsToMakeCmd=new G4UIcmdWithAnInteger("/MRK/file/setNumberOfEventsToMake",this);
    setNumberOfEventsToMakeCmd->SetGuidance("Sets the number of events to make for event file generation");
    setNumberOfEventsToMakeCmd->SetParameterName("mode",false);
    setNumberOfEventsToMakeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    setBeamOffsetCmd=new G4UIcmdWith3VectorAndUnit("/MRK/setBeamOffset",this);
    setBeamOffsetCmd->SetGuidance("Offsets the location of events generated");
    setBeamOffsetCmd->SetParameterName("beamOffsetX","beamOffsetY","beamOffsetZ",false);
    setBeamOffsetCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    useManualDirCmd= new G4UIcmdWithABool("/MRK/useManualDir",this);
    useManualDirCmd->SetGuidance("Sets whether to ignore event settings on direction");
    useManualDirCmd->SetParameterName("bool",false);
    useManualDirCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    useManualPosCmd= new G4UIcmdWithABool("/MRK/useManualPos",this);
    useManualPosCmd->SetGuidance("Sets whether to ignore event settings on position");
    useManualPosCmd->SetParameterName("bool",false);
    useManualPosCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    useManualEnergyCmd= new G4UIcmdWithABool("/MRK/useManualEnergy",this);
    useManualEnergyCmd->SetGuidance("Sets whether to ignore event settings on energy");
    useManualEnergyCmd->SetParameterName("bool",false);
    useManualEnergyCmd->AvailableForStates(G4State_PreInit,G4State_Idle);



}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GeneratorMessenger::~GeneratorMessenger()
{
    delete genInpCmd;
    delete fluxMapCmd;
    delete eventByEventVerbose;
    delete fileDir;
    delete genModeCmd;
    delete eventSetNumberCmd;
    delete printRandomNumbersCmd;
    delete setNumberOfEventsToMakeCmd;
    delete setBeamOffsetCmd;
    delete eventStartCmd;
    delete useManualDirCmd;
    delete useManualEnergyCmd;
    delete useManualPosCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GeneratorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
    if( command == genInpCmd )
        { myGenerator->loadInputFile(newValue);}
    else if( command == fluxMapCmd )
        { myGenerator->setFluxMapFileName(newValue);}
    else if( command == eventByEventVerbose )
        { myGenerator->setVerbose(eventByEventVerbose->GetNewBoolValue(newValue));}
    else if( command == genModeCmd)
        { myGenerator->setGeneratorMode((GeneratorMode)genModeCmd->GetNewIntValue(newValue));}
    else if( command == eventStartCmd)
        { myGenerator->setCurrentEventNumber((int) eventStartCmd->GetNewIntValue(newValue));}
    else if( command == printRandomNumbersCmd)
        { myGenerator->printRandomNumbers((int) printRandomNumbersCmd->GetNewIntValue(newValue));}
    else if( command == setNumberOfEventsToMakeCmd)
        { myGenerator->setNumberOfEventsToMake((int) setNumberOfEventsToMakeCmd->GetNewIntValue(newValue));}
    else if( command == eventSetNumberCmd)
        { myGenerator->setEventSetNumber((int) eventSetNumberCmd->GetNewIntValue(newValue));}
    else if( command == setBeamOffsetCmd)
        { myGenerator->setBeamOffset( setBeamOffsetCmd->GetNew3VectorValue(newValue));}
    else if( command == useManualDirCmd )
        { myGenerator->setUseManualDir(useManualDirCmd->GetNewBoolValue(newValue));}
    else if( command == useManualPosCmd )
        { myGenerator->setUseManualPos(useManualPosCmd->GetNewBoolValue(newValue));}
    else if( command == useManualEnergyCmd )
        { myGenerator->setUseManualEnergy(useManualEnergyCmd->GetNewBoolValue(newValue));}


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
