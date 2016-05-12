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

#include "MRKBGODetector.hh"
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

#include "../include/MRKPhys.hh"

using namespace CLHEP;

MRKBGODetector::MRKBGODetector(G4String name,int inpDetNum, G4ThreeVector inpGammaDetOffset,bool inpusePositionalLightOutput,bool inpuseNormalizedlightOutput):G4VSensitiveDetector(name)
{
    HCID=-1;
    G4String HCname=name+"_collection";
    collectionName.insert(HCname);
    usePositionalLightOutput=inpusePositionalLightOutput;
    useNormalizedLightOutput=inpuseNormalizedlightOutput;
    DetNum=inpDetNum;

	bgoCenterZPosition=224*mm+inpGammaDetOffset.z();

    //Constants for BGO light response along length--I shouldn't have these stored each time they are created...

    double kk0Dummy[12]={0.9994634,0.9996557,0.999513,0.9963012,0.9978678,0.9997649,0.998882,0.9991077,0.9932819,0.9982696,0.9984449,0.9987223};
    double kk1Dummy[12]={3.986864e-05,0.0001689317,5.064656e-05,7.003159e-05,-4.994234e-05,-0.0002110803,-3.110464e-05,-0.000195551,-0.0002297314,-3.916408e-05,2.321166e-05,-1.501302e-05};
    double kk2Dummy[12]={2.370452e-06,3.815403e-07,2.070204e-06,1.066584e-05,6.392543e-06,1.425570e-06,4.000117e-06,3.397849e-06,2.053844e-05,5.875048e-06,5.764383e-06,5.524947e-06};
    double kk3Dummy[12]={1.751450e-08,-1.849636e-08,-1.176445e-09,2.041891e-08,6.756355e-08,1.650474e-09,3.531353e-08,5.803961e-08,3.642482e-08,6.059123e-08,5.107648e-08,9.335619e-08};
    double kk4Dummy[12]={-5.952597e-12,9.001259e-10,1.595719e-10,-6.976799e-10,5.757778e-11,7.425284e-10,1.138028e-10,-2.687829e-10,-4.223121e-09,-6.766136e-10,-1.236748e-09,-1.018134e-09};
    double kk5Dummy[12]={-2.620916e-12,2.281255e-12,-1.274924e-12,-4.08466e-12,-8.450335e-12,3.617150e-12,-4.012398e-12,-5.354198e-12,-7.169351e-13,-8.380383e-12,-3.582307e-12,-1.27932e-11};
    double kk6Dummy[12]={6.048114e-14,-3.857624e-14,1.131760e-14,7.401177e-14,3.295636e-14,-1.26734e-14,2.109971e-14,3.401312e-14,3.269036e-13,6.981891e-14,1.798018e-13,1.564707e-13};
    double norm_funcDummy[12]={1.015887,1.013419,1.011222,1.028474,1.025036,1.017557,1.017506,1.009917,1.023982,1.014295,1.018611,1.019129};





    //Ugly as hell...forgive me...I'm sooo lazy
    for(int i=0;i<12;i++)
    {
        kk0[i]=kk0Dummy[i];
        kk1[i]=kk1Dummy[i];
        kk2[i]=kk2Dummy[i];
        kk3[i]=kk3Dummy[i];
        kk4[i]=kk4Dummy[i];
        kk5[i]=kk5Dummy[i];
        kk6[i]=kk6Dummy[i];
        norm_func[i]=norm_funcDummy[i];
    }



}

MRKBGODetector::~MRKBGODetector(){;}

void MRKBGODetector::Initialize(G4HCofThisEvent* HCE)
{
    (void) HCE;

    mrkCollection = new MRKHitsCollection(SensitiveDetectorName,collectionName[0]);

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    protonParDef=particleTable->FindParticle("proton");
    electronParDef=particleTable->FindParticle("e-");
    photonParDef=particleTable->FindParticle("gamma");
}

G4bool MRKBGODetector::ProcessHits(G4Step* aStep, G4TouchableHistory*)
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
        {
            theParType=MRKPARTYPE_PHOTON;
        }
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
    if (eDep > 0. )
    {
     //   G4cout << "EDep hit added!" << G4endl;
        newHit = new MRKHit();
        eDep*=preStep->GetWeight();
        newHit->SetHitCode(MRKHITCODE_DETEDEP);

        if(usePositionalLightOutput)
            eDep *= getBGOPosGain(postStep->GetPosition().z());

        newHit->SetEDep(eDep);
        mrkCollection->insert(newHit);
        returnBool = true;
    }

    return returnBool;
}


void MRKBGODetector::EndOfEvent(G4HCofThisEvent* HCE)
{
    if(HCID<0)
    { HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); }
    HCE->AddHitsCollection( HCID, mrkCollection );
}

void MRKBGODetector::clear()
{
}

void MRKBGODetector::DrawAll()
{
}

void MRKBGODetector::PrintAll()
{
}

double MRKBGODetector::getBGOPosGain(G4double z)
{

   double zTrans=(bgoCenterZPosition-z)/mm; //Adjust to center of BGO
   //double zTrans=-z/mm; //Temporary for BGO centered case
   // if(zTrans>100.001 || zTrans < -100.001)
    //{
    //    G4cout << "Error in BGO gain. Position is " << z/mm << " mm" << G4endl;
    //    return 0;
    //}
    double gain =(kk0[DetNum] + kk1[DetNum]*zTrans + kk2[DetNum]*pow(zTrans,2) + kk3[DetNum]*pow(zTrans,3) + kk4[DetNum]*pow(zTrans,4) + kk5[DetNum]*pow(zTrans,5) + kk6[DetNum]*pow(zTrans,6));
    if(useNormalizedLightOutput)
        gain /= norm_func[DetNum];
    //G4cout << "BGO Gain is: " << gain << G4endl;
    return gain;
}
