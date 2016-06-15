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
// $Id: exampleN02.cc,v 1.16 2009/10/30 14:59:59 allison Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//Standard Libraries
#include <string>
#include <fstream>

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

//Geant4 Libraries
#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "include/MRKPhysicsList.hh"
#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif
#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif


//MRKGeant Libraries
#include "MRKMacroMessenger.hh"
#include "MRKDetectorConstruction.hh"
#include "MRKPrimaryGeneratorAction.hh"
#include "MRKRunAction.hh"
#include "MRKEventAction.hh"
#include "MRKSteppingAction.hh"
#include "MRKSteppingVerbose.hh"


//Matt Libraries
#include "MRKText.hh"

using namespace std;
using std::string;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{

    G4cout << "Beginning MRKGEANT" << G4endl;
    string macroFileName="";
    if (argc != 1)   // batch mode
    {
        macroFileName = argv[1];

    }
    else
    {
    #ifdef G4UI_USE
    #ifdef G4VIS_USE
      macroFileName = "vis.mac";

    #endif
    #endif

    }
    G4cout << "Using macro file: " << macroFileName << G4endl;


    //Setting up Geant4's random number seeds


    if(argc > 2)
    {
        CLHEP::HepRandom::setTheSeed(str2int(argv[2]));
    }
    G4cout << "The initial Random seed for Geant4(CLHEP) is: " << CLHEP::HepRandom::getTheSeed() << endl;

    //Macro messenger
    MRKMacroMessenger theMacroMessenger;

    // User Verbose output class
    G4VSteppingVerbose* verbosity = new MRKSteppingVerbose;
    G4VSteppingVerbose::SetInstance(verbosity);

    // Run manager
    G4RunManager * runManager = new G4RunManager;


    // User Initialization classes (mandatory)
    G4cout << "Creating detector construction object: " << G4endl;
    MRKDetectorConstruction* detector= new MRKDetectorConstruction(&theMacroMessenger);
    runManager->SetUserInitialization(detector);
    G4cout << "completed!"<<G4endl;


    G4cout << "Creating physics lists object: ";
    runManager->SetUserInitialization(new MRKPhysicsList(detector));
    G4cout << "completed!"<<G4endl;

    // User Action classes

    G4cout << "Creating generator object: ";
    MRKPrimaryGeneratorAction* generator=new MRKPrimaryGeneratorAction(detector);
    runManager->SetUserAction(generator);
    G4cout << "completed!"<<G4endl;

    MRKRunAction* theRunAction =new MRKRunAction(generator);
    runManager->SetUserAction(theRunAction);
    theRunAction->setMacroFileName(macroFileName);

    runManager->SetUserAction(new MRKEventAction);
    runManager->SetUserAction(new MRKSteppingAction);


    #ifdef G4VIS_USE
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();
    #endif

    // Get the pointer to the User Interface manager
    //
    G4UImanager * UImanager = G4UImanager::GetUIpointer();



    if (argc != 1)   // batch mode
    {
        G4String command = "/control/execute ";
        UImanager->ApplyCommand(command+macroFileName);
    }
    else
    {
    #ifdef G4UI_USE
      G4UIExecutive * ui = new G4UIExecutive(argc,argv);
    #ifdef G4VIS_USE
      UImanager->ApplyCommand("/control/execute vis.mac");
    #endif
      ui->SessionStart();
      delete ui;
    #endif

    #ifdef G4VIS_USE
      delete visManager;
    #endif
    }

    // Free the store: user actions, physics_list and detector_description are
    //                 owned and deleted by the run manager, so they should not
    //                 be deleted in the main() program !
    delete runManager;
    delete verbosity;

    return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

