//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: MRKRunAction.cc,v 1.9 2006/06/29 17:48:16 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "MRKRunAction.hh"
#include "MRKRun.hh"

#include "G4Run.hh"
#include "G4UnitsTable.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MRKRunAction::MRKRunAction(MRKPrimaryGeneratorAction* generator)
{
    outputFileName="";
    rAMess=new MRKRunActionMessenger(this);
    theGenerator=generator;
    macroFileName="";
    useFluxTree=false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

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

void MRKRunAction::setOutputFile(string outputFileName)
{
    this->outputFileName=outputFileName;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

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
  time (&startTime);  //Start Time for Run
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MRKRunAction::EndOfRunAction(const G4Run* aRun)
{
    time(&endTime);  //End Time for Run

    double runTime = difftime (endTime,startTime);
	MRKRun* theRun = (MRKRun*) aRun;
	double eventsPerSecond = theRun->GetNumberOfEvent()/runTime;

  G4cout
    << "############################################################" << G4endl;
  G4cout
    << " Run Summary - Number of events : " << theRun->GetNumberOfEvent()
    << G4endl;
    G4cout << "Run Time: " << runTime << G4endl;
    G4cout << "Events per Second: " << eventsPerSecond << G4endl;
  G4cout
    << "############################################################" << G4endl;
    if(theRun->getFileIsOpen())
        theRun->closeAndWrite();
    if(theGenerator->getGeneratorMode() == PARTICLE_SOURCE)
    {
        theGenerator->writeEventsToFile();
    }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



