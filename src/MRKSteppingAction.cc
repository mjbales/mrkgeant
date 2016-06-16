#include "MRKSteppingAction.hh"
#include "G4SteppingManager.hh"

#include "MRKConstants.hh"

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

