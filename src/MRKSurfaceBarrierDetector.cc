#include "MRKSurfaceBarrierDetector.hh"

#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"
#include "G4StepStatus.hh"
#include "G4Track.hh"
#include "G4VSolid.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VPVParameterisation.hh"
#include "G4UnitsTable.hh"
#include "G4GeometryTolerance.hh"
#include "G4SDManager.hh"

#include "MRKPhys.hh"
#include "MRKHit.hh"


MRKSurfaceBarrierDetector::MRKSurfaceBarrierDetector(G4String name):G4VSensitiveDetector(name)
{
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	protonParDef=particleTable->FindParticle("proton");
	electronParDef=particleTable->FindParticle("e-");
	photonParDef=particleTable->FindParticle("gamma");

	mrkCollection=nullptr;
    HCID=-1;
    G4String HCname=name+"_collection";
    collectionName.insert(HCname);
}

MRKSurfaceBarrierDetector::~MRKSurfaceBarrierDetector(){;}

void MRKSurfaceBarrierDetector::Initialize(G4HCofThisEvent* HCE)
{
    (void) HCE;

    mrkCollection = new MRKHitsCollection(SensitiveDetectorName,collectionName[0]);


}

G4bool MRKSurfaceBarrierDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
    bool returnBool=false;
    MRKHit* newHit;

    //If the prestep is on a geometry boundary and it's here then it's just entering the volume
    G4StepPoint* preStep = aStep->GetPreStepPoint();
    if (preStep->GetStepStatus() == fGeomBoundary)
    {
       // G4cout << "Flux tree hit added!" << G4endl;
        returnBool=true;
        G4ParticleDefinition* theParDef=aStep->GetTrack()->GetDefinition();
        Char_t theParType;
        if(theParDef==electronParDef)
            theParType=MRKPARTYPE_ELECTRON;
        else if(theParDef==photonParDef)
            theParType=MRKPARTYPE_PHOTON;
        else if(theParDef==protonParDef)
            theParType=MRKPARTYPE_PROTON;
        else
            theParType=MRKPARTYPE_NOTYPE;

        newHit = new MRKHit();
        newHit->SetMom(preStep->GetMomentum());
        newHit->SetPos(preStep->GetPosition());
        newHit->SetTime(preStep->GetGlobalTime());
        newHit->SetKE(preStep->GetKineticEnergy());
        newHit->SetHitCode(MRKHITCODE_DETIN);
        newHit->SetParType(theParType);

        mrkCollection->insert(newHit);
    }

    //If the poststep is on a geometry boundary and it's here then it's just exiting the volume
    G4StepPoint* postStep = aStep->GetPostStepPoint();

    if (postStep->GetStepStatus() == fGeomBoundary)
    {
        newHit = new MRKHit();
        newHit->SetMom(postStep->GetMomentum());
        newHit->SetPos(postStep->GetPosition());
        newHit->SetTime(postStep->GetGlobalTime());
        newHit->SetKE(postStep->GetKineticEnergy());
        newHit->SetHitCode(MRKHITCODE_DETOUT);

        G4ParticleDefinition* theParDef=aStep->GetTrack()->GetDefinition();
        Char_t theParType;
        if(theParDef==electronParDef)
            theParType=MRKPARTYPE_ELECTRON;
        else if(theParDef==photonParDef)
            theParType=MRKPARTYPE_PHOTON;
        else if(theParDef==protonParDef)
            theParType=MRKPARTYPE_PROTON;
        else
            theParType=MRKPARTYPE_NOTYPE;
        newHit->SetParType(theParType);

        mrkCollection->insert(newHit);

        returnBool = true;
    }

    G4double eDep =aStep->GetTotalEnergyDeposit();
    if (eDep > 0.)
    {
     //   G4cout << "EDep hit added!" << G4endl;
        newHit = new MRKHit();
        eDep*=preStep->GetWeight();
        newHit->SetHitCode(MRKHITCODE_DETEDEP);
        newHit->SetEDep(eDep);
        mrkCollection->insert(newHit);
        returnBool = true;
    }

    return returnBool;
}


void MRKSurfaceBarrierDetector::EndOfEvent(G4HCofThisEvent* HCE)
{
    if(HCID<0)
    { HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); }
    HCE->AddHitsCollection( HCID, mrkCollection );
}

void MRKSurfaceBarrierDetector::clear()
{
}

void MRKSurfaceBarrierDetector::DrawAll()
{
}

void MRKSurfaceBarrierDetector::PrintAll()
{
}
