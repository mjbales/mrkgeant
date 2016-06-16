#include "MRKRunAction.hh"
#include "MRKRun.hh"

#include "G4Run.hh"
#include "G4UnitsTable.hh"

using namespace std;
MRKRunAction::MRKRunAction(MRKPrimaryGeneratorAction* generator)
{
	outputFileName = "";
	rAMess = new MRKRunActionMessenger(this);
	theGenerator = generator;
	macroFileName = "";
	useFluxTree = false;
	startTime = endTime = 0;
}

MRKRunAction::~MRKRunAction()
{
	G4cout << "Beginning to delete Run Action...";
	delete rAMess;
	G4cout << "COMPLETED!" << G4endl;
}

G4Run* MRKRunAction::GenerateRun()
{

	G4cout << "Generating run..." << G4endl;
	return new MRKRun(theGenerator);

}

void MRKRunAction::setOutputFile(TString outputFileName)
{
	this->outputFileName = outputFileName;
}

void MRKRunAction::BeginOfRunAction(const G4Run* aRun)
{
	MRKRun* theRun = (MRKRun*) aRun;
	theRun->setOutputFile(outputFileName);
	theRun->setMacroFileName(macroFileName);
	theRun->setUseFluxTree(useFluxTree);
	if(theGenerator->getGeneratorMode() == PARTICLE_SOURCE)
	{
		theGenerator->createParticleSourceEventFile();
	}

	G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
	time(&startTime);  //Start Time for Run
}

void MRKRunAction::EndOfRunAction(const G4Run* aRun)
{
	time(&endTime);  //End Time for Run

	double runTime = difftime(endTime, startTime);
	MRKRun* theRun = (MRKRun*) aRun;
	double eventsPerSecond = theRun->GetNumberOfEvent() / runTime;

	G4cout << "############################################################" << G4endl;
	G4cout << " Run Summary - Number of events : " << theRun->GetNumberOfEvent() << G4endl;
	G4cout << "Run Time: " << runTime << G4endl;
	G4cout << "Events per Second: " << eventsPerSecond << G4endl;
	G4cout << "############################################################" << G4endl;
	if(theRun->getFileIsOpen()) theRun->closeAndWrite();
	if(theGenerator->getGeneratorMode() == PARTICLE_SOURCE)
	{
		theGenerator->writeEventsToFile();
	}

}

