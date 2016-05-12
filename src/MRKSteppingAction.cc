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
// $Id: MRKSteppingAction.cc,v 1.9 2006/06/29 17:48:18 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "MRKSteppingAction.hh"
#include "G4SteppingManager.hh"

#include "../include/MRKConstants.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

using namespace CLHEP;

MRKSteppingAction::MRKSteppingAction()
{
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    protonParDef=particleTable->FindParticle("proton");
    electronParDef=particleTable->FindParticle("e-");
    photonParDef=particleTable->FindParticle("gamma");
    emaxTime=ELECTRON_MAX_TIME*second;
    pmaxTime=PROTON_MAX_TIME*second;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MRKSteppingAction::UserSteppingAction(const G4Step* aStep)
{
     G4Track * theTrack = aStep->GetTrack();

    // check if it is alive
    if(theTrack->GetTrackStatus()!=fAlive) { return; }

    if(theTrack->GetDefinition()==electronParDef)
    {
      if(theTrack->GetLocalTime() > emaxTime)
      {
          theTrack->SetTrackStatus(fStopAndKill);
      }
      if(aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "physiCopperCoveringIN")
      {
          theTrack->SetTrackStatus(fStopAndKill);
      }

    }
    else if(theTrack->GetDefinition()==protonParDef)
    {
      if(theTrack->GetLocalTime() > pmaxTime)
      {
          theTrack->SetTrackStatus(fStopAndKill);
      }
      if(aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "physiFakeSBDDetector")
      {
          theTrack->SetTrackStatus(fStopAndKill);
      }
    }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

