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
// $Id: MRKPrimaryGeneratorAction.cc,v 1.7 2006/06/29 17:48:13 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "MRKPrimaryGeneratorAction.hh"
#include "MRKDetectorConstruction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4UnitsTable.hh"
#include "globals.hh"

#include "../include/MRKConstants.hh"
#include "../include/MRKPhys.hh"
#include "../include/MRKText.hh"



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//Currently electrons only!
MRKPrimaryGeneratorAction::MRKPrimaryGeneratorAction(MRKDetectorConstruction* myDC)
:myDetector(myDC)
{
    theDC=myDC;
    sourceToDetectorDistance=5.5*cm;
    generatorMode=PARTICLE_GUN;
    G4int n_particle = 1;
    particleGun = new G4ParticleGun(n_particle);
    particleSource = new G4GeneralParticleSource();

    fileLoaded=false;
    eventByEventVerbose=false;

    useManualDir=false;
    useManualEnergy=false;
    useManualPos=false;


    inpFile=NULL;
    inpTree=NULL;
    theEvents=NULL;


    currentEventNumber=0;
    numberOfEventsToMake=1000000;

    particleTable = G4ParticleTable::GetParticleTable();
    protonParDef=particleTable->FindParticle("proton");
    electronParDef=particleTable->FindParticle("e-");
    gammaParDef=particleTable->FindParticle("gamma");
    geantinoParDef=particleTable->FindParticle("geantino");

    // default particle
    particleGun->SetParticleDefinition(electronParDef);
    particleGun->SetParticlePosition(G4ThreeVector(0.,0.,0.));
    particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.5,.5));
    particleGun->SetParticleEnergy(100.*keV);



    genMess=new MRKGeneratorMessenger(this);
    inputFileName="";
    beamOffset=G4ThreeVector(0,0,0);

}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MRKPrimaryGeneratorAction::~MRKPrimaryGeneratorAction()
{
    G4cout << "Beginning to delete Primary Generator Action...";
    delete particleGun;
    delete genMess;
    if(inpFile!=0)
    {
        inpFile->Close();
        delete inpFile;
        inpFile=NULL;
    }

    if(theEvents!=NULL)
    {
        delete theEvents;
        theEvents=NULL;
    }

    G4cout << "COMPLETED!" << G4endl;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void MRKPrimaryGeneratorAction::loadInputFile(TString inputFileName)
{

    fileLoaded=false;
    currentEventNumber=0;
    this->inputFileName=inputFileName;
    if(inpFile!=NULL)
    {
        delete inpFile;
        inpFile=NULL;

    }

    switch(generatorMode)
    {
        case RDKEVENTGENERATOR:
            makeAndLoadRDKEventFile(inputFileName);
            break;
        case RDKEVENTFILE:
            loadRDKEventFile(inputFileName);
            break;


        default:
            G4cout << "No input file loading is necessary for this generator mode. " << G4endl;
    }


}

void MRKPrimaryGeneratorAction::createParticleSourceEventFile()
{
    if(inpFile != NULL)
    {
        delete inpFile;
        inpFile=NULL;
        fileLoaded=false;
    }
    inpFile = new TFile("Events_GPS.root","RECREATE");
    inpFile->cd();
    inpTree=new TTree(DEFAULT_EVENTS_TREENAME,"Event Tree from GPS");


    inpTree->Branch("x0",&x0,"x0/D");
    inpTree->Branch("y0",&y0,"y0/D");
    inpTree->Branch("z0",&z0,"z0/D");

    inpTree->Branch("e0",&e0,"e0/D");

    inpTree->Branch("mx0",&mx0,"mx0/D");
    inpTree->Branch("my0",&my0,"my0/D");
    inpTree->Branch("mz0",&mz0,"mz0/D");
    fileLoaded=true;
    gROOT->cd();



}

void MRKPrimaryGeneratorAction::loadRDKEventFile(TString inputFileName)
{
    if(inpFile != NULL)
    {
        delete inpFile;
        inpFile=NULL;
        fileLoaded=false;
    }
    inpFile = new TFile(inputFileName,"READ");
    if(inpFile->IsZombie()){
        cout << "Error in input file:"<< inputFileName << endl;
        delete inpFile;
        inpFile=NULL;
        return;
    }
    TString currentParticleName=particleGun->GetParticleDefinition()->GetParticleName();

    G4cout << "Loading " << currentParticleName << " data from event file:  " << inputFileName << endl;
    inpTree=(TTree*) inpFile->Get(DEFAULT_EVENTS_TREENAME);
    inpTree->SetBranchAddress("x0",&x0);
    inpTree->SetBranchAddress("y0",&y0);
    inpTree->SetBranchAddress("z0",&z0);
    if(currentParticleName=="e-")
    {

        inpTree->SetBranchAddress("mxe0",&mx0);
        inpTree->SetBranchAddress("mye0",&my0);
        inpTree->SetBranchAddress("mze0",&mz0);
        inpTree->SetBranchAddress("ee0",&e0);

    }
    if(currentParticleName=="proton")
    {

        inpTree->SetBranchAddress("mxp0",&mx0);
        inpTree->SetBranchAddress("myp0",&my0);
        inpTree->SetBranchAddress("mzp0",&mz0);
        inpTree->SetBranchAddress("ep0",&e0);
        inpTree->SetBranchAddress("mze0",&mze0);

    }
    if(currentParticleName=="gamma")
    {

        inpTree->SetBranchAddress("mxg0",&mx0);
        inpTree->SetBranchAddress("myg0",&my0);
        inpTree->SetBranchAddress("mzg0",&mz0);
        inpTree->SetBranchAddress("eg0",&e0);
        inpTree->SetBranchAddress("mze0",&mze0);

    }

    fileLoaded=true;
    gROOT->cd();
}

void MRKPrimaryGeneratorAction::makeAndLoadRDKEventFile(TString inputFileName)
{

    if(theEvents != NULL)
    {
        delete theEvents;
        theEvents=NULL;
    }
    theEvents=new MRKEvents(); //initial seed does not matter, it will be changed soon from the file.
//    theEvents->loadFluxFileMap(fluxMapFileName);
    string currentParticleName=particleGun->GetParticleDefinition()->GetParticleName();

    G4cout << "Generating events from event settings file:  " << inputFileName << G4endl;
    TString eventFileName=filePathFromFullPath(inputFileName)+"Events"+addBeforeExtension(fileNameFromFullPath(inputFileName),"_Set"+int2str(eventSetNumber))(13,999);
    theEvents->loadEventSettingsAndMakeFile(inputFileName,eventSetNumber,eventFileName,fluxMapFileName,numberOfEventsToMake);
    delete theEvents;
    theEvents=NULL;
    loadRDKEventFile(eventFileName);

}


void MRKPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{

    switch(generatorMode)
    {
        case PARTICLE_GUN:
            break;
        case RDKEVENTFILE:
        case RDKEVENTGENERATOR:
            if(fileLoaded)
            {
                inpTree->GetEntry(currentEventNumber);

                //For a speed up, do not run events that are far outside detectable range 140516
                if(beamOffset.mag() < 0.001 && (z0 > 0.33 || z0 < -.12))
                {
                    e0=0;
                }

                //Kill wrong way electrons for speedup
                if(particleGun->GetParticleDefinition()->GetParticleName() == "e-")
                {
                    if(mz0 >0.45)
                    {
                        e0=0;
                    }
                }
                else
                {
                    if(mze0 >0.45)
                    {
                        e0=0;
                    }
                }


                pos.set(x0*m,y0*m,z0*m);

                dirP.setVal(mx0,my0,mz0);
            }
            momDir.set(dirP.x,dirP.y,dirP.z);
            if(!useManualDir)
                particleGun->SetParticleMomentumDirection(momDir);
            if(!useManualEnergy)
                particleGun->SetParticleEnergy(e0*keV);
            if(!useManualPos)
                particleGun->SetParticlePosition(pos+beamOffset);
            break;
//         case RDKEVENTGENERATOR:
//            if(fileLoaded)
//            {
//                theEvents->generateEvent();
//                string currentParticle=particleGun->GetParticleDefinition()->GetParticleName();
//                if(currentParticle == "e-")
//                {
//                    mx0=theEvents->getmxe0();
//                    my0=theEvents->getmye0();
//                    mz0=theEvents->getmze0();
//                    e0=theEvents->getee0();
//                }
//                else if(currentParticle == "proton")
//                {
//                    mx0=theEvents->getmxp0();
//                    my0=theEvents->getmyp0();
//                    mz0=theEvents->getmzp0();
//                    e0=theEvents->getep0();
//                }
//                else if(currentParticle == "gamma")
//                {
//                    mx0=theEvents->getmxg0();
//                    my0=theEvents->getmyg0();
//                    mz0=theEvents->getmzg0();
//                    e0=theEvents->geteg0();
//                }
//                x0=theEvents->getx0();
//                y0=theEvents->gety0();
//                z0=theEvents->getz0();
//
//                pos.set(x0*cm,y0*cm,z0*cm);
//            }
//
//            momDir.set(mx0,my0,mz0);
//            particleGun->SetParticleMomentumDirection(momDir);
//            particleGun->SetParticleEnergy(e0*keV);
//            particleGun->SetParticlePosition(pos);
//
//            break;

        default:
            break;

    }


    if(eventByEventVerbose)
    {
        G4cout << G4endl<<"RDK Event Number: "<< currentEventNumber << " Gun set to: " << getCurrentParticleName() <<"  "<< G4BestUnit(particleGun->GetParticleEnergy(),"Energy") << "  " << G4BestUnit(particleGun->GetParticleMomentum(),"Energy")<<  " @ " << G4BestUnit(particleGun->GetParticlePosition(),"Length") << G4endl;
        eventStartClock= clock();
    }

    if(generatorMode != PARTICLE_SOURCE)
    {
        particleGun->GeneratePrimaryVertex(anEvent);
    }
    else
    {
        particleSource->GeneratePrimaryVertex(anEvent);
        x0=anEvent->GetPrimaryVertex()->GetX0()/m;
        y0=anEvent->GetPrimaryVertex()->GetY0()/m;
        z0=anEvent->GetPrimaryVertex()->GetZ0()/m;
        e0=anEvent->GetPrimaryVertex()->GetPrimary()->GetKineticEnergy()/keV;
        mx0=anEvent->GetPrimaryVertex()->GetPrimary()->GetPx();
        my0=anEvent->GetPrimaryVertex()->GetPrimary()->GetPy();
        mz0=anEvent->GetPrimaryVertex()->GetPrimary()->GetPz();
        inpTree->Fill();
    }


}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
