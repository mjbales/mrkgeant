#include "MRKPrimaryGeneratorAction.hh"
#include "MRKDetectorConstruction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4UnitsTable.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"

#include "MRKConstants.hh"
#include "MRKPhys.hh"
#include "MRKText.hh"

using namespace std;
MRKPrimaryGeneratorAction::MRKPrimaryGeneratorAction(MRKDetectorConstruction* myDC)
:myDetector(myDC)
{
	vx0=vy0=vz0=x0=y0=z0=e0=mx0=my0=mz0=tofp=mze0=0.;

	Theta=Phi=FloatEnergy=sinTheta=cosTheta=cosPhi=sinPhi=0.;

	eventSetNumber=0;
	eventStartClock=0;

    theDC=myDC;
    sourceToDetectorDistance=5.5*CLHEP::cm;
    generatorMode=PARTICLE_GUN;
    G4int n_particle = 1;
    particleGun = new G4ParticleGun(n_particle);
    particleSource = new G4GeneralParticleSource();

    fileLoaded=false;
    eventByEventVerbose=false;

    useManualDir=false;
    useManualEnergy=false;
    useManualPos=false;
    useRDKIIQuickStops=false;


    inpFile=nullptr;
    inpTree=nullptr;
    theEvents=nullptr;


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
    particleGun->SetParticleEnergy(100.*CLHEP::keV);



    genMess=new MRKGeneratorMessenger(this);
    generatorFileName="";
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
        inpFile=nullptr;
    }

    if(theEvents!=nullptr)
    {
        delete theEvents;
        theEvents=nullptr;
    }

    G4cout << "COMPLETED!" << G4endl;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void MRKPrimaryGeneratorAction::loadInputFile(TString inputFileName)
{

    fileLoaded=false;
    currentEventNumber=0;
    generatorFileName=inputFileName;
    if(inpFile!=nullptr)
    {
        delete inpFile;
        inpFile=nullptr;

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
    if(inpFile != nullptr)
    {
        delete inpFile;
        inpFile=nullptr;
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
    if(inpFile != nullptr)
    {
        delete inpFile;
        inpFile=nullptr;
        fileLoaded=false;
    }
    inpFile = new TFile(inputFileName,"READ");
    if(inpFile->IsZombie()){
        G4cout << "Error in input file:"<< inputFileName << G4endl;
        delete inpFile;
        inpFile=nullptr;
        return;
    }
    TString currentParticleName=particleGun->GetParticleDefinition()->GetParticleName();

    G4cout << "Loading " << currentParticleName << " data from event file:  " << inputFileName << G4endl;
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

    if(theEvents != nullptr)
    {
        delete theEvents;
        theEvents=nullptr;
    }
    theEvents=new MRKEvents(); //initial seed does not matter, it will be changed soon from the file.
//    theEvents->loadFluxFileMap(fluxMapFileName);
    string currentParticleName=particleGun->GetParticleDefinition()->GetParticleName();

    G4cout << "Generating events from event settings file:  " << inputFileName << G4endl;
    TString eventFileName=filePathFromFullPath(inputFileName)+"Events"+addBeforeExtension(fileNameFromFullPath(inputFileName),"_Set"+int2str(eventSetNumber))(13,999);
    theEvents->loadEventSettingsAndMakeFile(inputFileName,eventSetNumber,eventFileName,fluxMapFileName,numberOfEventsToMake);
    delete theEvents;
    theEvents=nullptr;
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
                if(useRDKIIQuickStops)
                {
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
                }


                pos.set(x0*CLHEP::m,y0*CLHEP::m,z0*CLHEP::m);

                dirP.setVal(mx0,my0,mz0);
            }
            momDir.set(dirP.x,dirP.y,dirP.z);
            if(!useManualDir)
                particleGun->SetParticleMomentumDirection(momDir);
            if(!useManualEnergy)
                particleGun->SetParticleEnergy(e0*CLHEP::keV);
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
        x0=anEvent->GetPrimaryVertex()->GetX0()/CLHEP::m;
        y0=anEvent->GetPrimaryVertex()->GetY0()/CLHEP::m;
        z0=anEvent->GetPrimaryVertex()->GetZ0()/CLHEP::m;
        e0=anEvent->GetPrimaryVertex()->GetPrimary()->GetKineticEnergy()/CLHEP::keV;
        mx0=anEvent->GetPrimaryVertex()->GetPrimary()->GetPx();
        my0=anEvent->GetPrimaryVertex()->GetPrimary()->GetPy();
        mz0=anEvent->GetPrimaryVertex()->GetPrimary()->GetPz();
        inpTree->Fill();
    }


}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
