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

#include "MRKBAPDDetector.hh"
#include "MRKHit.hh"
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
#include "mattphys.hh"


//Detector numbers 0-11 BGO, 12-14 BAPDS
MRKBAPDDetector::MRKBAPDDetector(G4String name,TH1D* inpSio2Hist, int inpDetNum, G4ThreeVector inpGammaDetOffset, bool inpUseCollectionEfficiencyModel):G4VSensitiveDetector(name)
{

    HCID=-1;
    G4String HCname=name+"_collection";
    collectionName.insert(HCname);
    sio2Hist=inpSio2Hist;
    DetNum=inpDetNum;
    useCollectionEfficiencyModel=inpUseCollectionEfficiencyModel;
	bapdFrontFacePosition=3.9*cm + inpGammaDetOffset.z();
}

MRKBAPDDetector::~MRKBAPDDetector(){;}

void MRKBAPDDetector::Initialize(G4HCofThisEvent* HCE)
{
    (void) HCE;

    mrkCollection = new MRKHitsCollection(SensitiveDetectorName,collectionName[0]);

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    protonParDef=particleTable->FindParticle("proton");
    electronParDef=particleTable->FindParticle("e-");
    photonParDef=particleTable->FindParticle("gamma");
}

G4bool MRKBAPDDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
    bool returnBool=false;
    bool sio2Kill=false;
    MRKHit* newHit;
    double depth;
    G4ParticleDefinition* theParDef;
    Char_t theParType;
    double transCoeff;


    //If the prestep is on a geometry boundary and it's here then it's just entering the volume
    G4StepPoint* preStep = aStep->GetPreStepPoint();
    if (preStep->GetStepStatus() == fGeomBoundary)
    {
//        G4cout << "Flux tree hit added!" << G4endl;
        returnBool=true;
        theParDef=aStep->GetTrack()->GetDefinition();

        newHit = new MRKHit();
        newHit->SetMom(preStep->GetMomentum());
        newHit->SetPos(preStep->GetPosition());
        newHit->SetTime(preStep->GetGlobalTime());
        newHit->SetKE(preStep->GetKineticEnergy());

        if(theParDef==electronParDef)
        {
//            G4cout << "Electron hit BAPD" << G4endl;

            theParType=MRKPARTYPE_ELECTRON;
            newHit->SetHitCode(MRKHITCODE_DETIN);
        }
        else if(theParDef==photonParDef)
        {
            theParType=MRKPARTYPE_PHOTON;

//            G4cout << "Photon hit BAPD" << G4endl;

            if(sio2Hist!=NULL)
            {

                gKE=preStep->GetKineticEnergy()/keV;

//                G4cout << "Applying SiO2 layer Gamma Energy: "<< gKE<<" keV" << G4endl;

                if(gKE < 25 && gKE > 0.1)
                {
                    gMomentum=preStep->GetMomentum();

                    thicknessMultiplier=gMomentum.getR()/abs(gMomentum.z()); //Due to angle of incidence
                    //Data sheet is based on 100 nm.  To achieve 35 nm, I use 35./100.
                    transCoeff=pow(sio2Hist->Interpolate(gKE),(thicknessMultiplier*35./100.));

                    if(G4UniformRand()> transCoeff)
                    {

                        aStep->GetTrack()->SetTrackStatus(fKillTrackAndSecondaries);

                        sio2Kill=true;
                        returnBool=false;
//                        G4cout << "Killed by SiO2" << G4endl;
                       newHit->SetHitCode(MRKHITCODE_SIO2REJECT);

                    }
                    else
                    {
                        newHit->SetHitCode(MRKHITCODE_DETIN);
                    }

                }

            }

        }
        else if(theParDef==protonParDef)
        {
//            G4cout << "Proton hit BAPD" << G4endl;
            theParType=MRKPARTYPE_PROTON;
            newHit->SetHitCode(MRKHITCODE_DETIN);
        }
        else
        {
            theParType=MRKPARTYPE_NOTYPE;
            newHit->SetHitCode(MRKHITCODE_DETIN);
        }

        newHit->SetParType(theParType);


        mrkCollection->insert(newHit);
    }

    //If the poststep is on a geometry boundary and it's here then it's just exiting the volume
    G4StepPoint* postStep = aStep->GetPostStepPoint();

    if (postStep->GetStepStatus() == fGeomBoundary && !sio2Kill)
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
    if (eDep > 0. && !sio2Kill)
    {
        eDep=preStep->GetKineticEnergy();  //The particle is killed in this model, so let's get the remaining energy
//        G4cout << "EDep hit added!" << G4endl;
        newHit = new MRKHit();
        eDep*=preStep->GetWeight();
        newHit->SetHitCode(MRKHITCODE_DETEDEP);

        if(useCollectionEfficiencyModel)
        {

            depth=(bapdFrontFacePosition-postStep->GetPosition().z())/um;
           // G4cout << "E0 " << eDep/eV << "    Depth " << depth << G4endl;
            eDep= BAPDCollectionEfficiencyModel(eDep/eV,depth)*eV;
          //  typeAnythingToContinue("");

        }



        newHit->SetEDep(eDep);
        mrkCollection->insert(newHit);
        aStep->GetTrack()->SetTrackStatus(fKillTrackAndSecondaries);  //Kill photon as we are not going to track secondary electrons
        returnBool = true;
    }

    return returnBool;
}


void MRKBAPDDetector::EndOfEvent(G4HCofThisEvent* HCE)
{
    if(HCID<0)
    {
        HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    }

    HCE->AddHitsCollection( HCID, mrkCollection );
}

void MRKBAPDDetector::clear()
{
}

void MRKBAPDDetector::DrawAll()
{
}

void MRKBAPDDetector::PrintAll()
{
}


//Assumes eV and um
double MRKBAPDDetector::BAPDCollectionEfficiencyModel(double inpEnergy, double depth)
{

    double collectionEfficiency=0.;

    if(depth>60.001 || depth < 0)
    {
        G4cout << "Depth: " << depth << G4endl;
        typeAnythingToContinue("CE Error depth not in range.");
    }

    if(depth > 37) //Partial collection region at the back of the BAPD
    {
        collectionEfficiency=pow(1.-((depth-37.)/23.),2);
    }
    else if(depth>3.5355) //Full collection efficiency region
    {
            collectionEfficiency=1.;
            if(DetNum==2)
            {
                //Apply g factor as in paper
                if(inpEnergy<2500)
                    collectionEfficiency*=1.12;
                else if(inpEnergy < 5900)
                    collectionEfficiency*=-.000035294118*inpEnergy+1.208235; //Slope from 2.5 kev to 5.9 keV from 1.12 to 1.

            }

    }
    else
    {
        if(DetNum==0)
        {
            if(depth>.6894)
                collectionEfficiency=0.0495*depth+0.8259;
            else
                collectionEfficiency=1.2162*depth+.0215;
        }
        else if(DetNum==1)
        {

            if(depth>.7248)
                collectionEfficiency=0.0758*depth+0.73306;
            else if(depth>.1591)
                collectionEfficiency=1.2162*depth-0.0935;
            else
                collectionEfficiency=.5657*depth+0.01;
        }
        else if(DetNum==2)
        {
            if(depth>0.4066)
                collectionEfficiency=0.0339*depth+0.8792;
            else
                collectionEfficiency=1.2626*depth+0.37947;
        }
    }



    double w0Constant=0.12;
    double w1Constant=7.6;

    double CE= collectionEfficiency*inpEnergy;

    double width= (CE/2.355)*sqrt(w0Constant*w0Constant+(w1Constant*w1Constant)/CE);


    double outputEnergy=G4RandGauss::shoot(CE,width);

    if(outputEnergy<0)
        outputEnergy=0.;

//    G4cout << "   C " << collectionEfficiency << "    Width " << width << "   EF " << outputEnergy<<G4endl;

   // G4cout << inpEnergy << " " << depth << " " << outputEnergy << " " << G4endl;

    return outputEnergy;
}
