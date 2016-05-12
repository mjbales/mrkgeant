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
// $Id: MRKPrimaryGeneratorAction.hh,v 1.7 2006/06/29 17:47:43 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef MRKPrimaryGeneratorAction_h
#define MRKPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleDefinition.hh"
#include "G4ThreeVector.hh"

#include "globals.hh"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"

#include "MRKDetectorConstruction.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "Randomize.hh"
#include <time.h>

#include "MRKEvents.hh"
#include "MRKGeneratorMessenger.hh"
#include "MRKVector.hh"
using namespace std;


class G4Event;

enum GeneratorMode{PARTICLE_GUN=0,RDKEVENTFILE=1,RDKEVENTGENERATOR=3,PARTICLE_SOURCE=4};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class MRKPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    MRKPrimaryGeneratorAction(MRKDetectorConstruction*);
   ~MRKPrimaryGeneratorAction();

  public:
    void GeneratePrimaries(G4Event*);
    void loadInputFile(TString inputFileName);
    void createParticleSourceEventFile();
    void loadRDKEventFile(TString inputFileName);
    void makeAndLoadRDKEventFile(TString inputFileName);  //also loads flux map
    void setFluxMapFileName(TString inpFluxMapFileName){fluxMapFileName=inpFluxMapFileName;}

    inline GeneratorMode getGeneratorMode(){return generatorMode;}



    inline bool getVerbose() { return eventByEventVerbose; }
    inline G4ThreeVector getMomDir() { return momDir;}
    inline double getE0() { return e0;}
    inline int getCurrentEventNumber(){return currentEventNumber;}
    inline G4String getCurrentParticleName(){return particleGun->GetParticleDefinition()->GetParticleName();}


    inline void setCurrentEventNumber(int inp) { currentEventNumber=inp; }
    inline void setGeneratorMode(GeneratorMode inp) { G4cout << "Setting generator mode." << G4endl; generatorMode=inp;}
    inline void setVerbose(bool inpBool){ eventByEventVerbose=inpBool; }

    inline ExperimentModel getExperimentModel(){return myDetector->getExperimentModel();}
    inline bool getUseSBDDetector(){return myDetector->getUseSBDDetector();};
    inline bool getUseBAPDDetectors(){return myDetector->getUseBAPDDetectors();};
    inline bool getUseBGODetectors(){return myDetector->getUseBGODetectors();};
    inline int getNumBGOsUsed(){return myDetector->getNumBGOsUsed();}
    inline double getEventStartClock(){return eventStartClock;}
    inline void setEventSetNumber(int inpEventSetNumber){eventSetNumber=inpEventSetNumber;}
    inline void printRandomNumbers(const int numRandomNumbers){for (int i = 0;i< numRandomNumbers;i++) G4cout << theEvents->getRandomNumber() << G4endl;}
    inline void setNumberOfEventsToMake( int inp){numberOfEventsToMake=inp;}
    inline void setBeamOffset(G4ThreeVector inp){beamOffset=inp;}
    inline void setUseManualPos(bool inp){useManualPos=inp;}
    inline void setUseManualDir(bool inp){useManualDir=inp;}
    inline void setUseManualEnergy(bool inp){useManualEnergy=inp;}

    void writeEventsToFile(){inpFile->cd(); inpTree->Write("", TObject::kOverwrite); inpFile->Close();}









  private:
        MRKDetectorConstruction* theDC;
        GeneratorMode generatorMode;
        bool eventByEventVerbose;
        string inputFileName;
        string fluxMapFileName;
		bool fileLoaded;



		G4double sourceToDetectorDistance;

		MRKVector3D r,dirE,dirP,dirG;
		G4ThreeVector pos,momDir;
		int currentEventNumber;

		TFile* inpFile;
		TTree* inpTree;

		bool useManualPos;
		bool useManualDir;
		bool useManualEnergy;

		Double_t vx0,vy0,vz0,x0,y0,z0,e0,mx0,my0,mz0,tofp;

		Double_t mze0; //always check this for speedup

		Double_t Theta,Phi,FloatEnergy;
		Double_t sinTheta,cosTheta,cosPhi,sinPhi;
		Int_t DecayNum;

		Int_t hitcodee, hitcodep;
		Int_t dwcutb[41];
		Int_t dummy;
        G4ParticleTable* particleTable;
		G4ParticleDefinition* electronParDef;
		G4ParticleDefinition* protonParDef;
		G4ParticleDefinition* gammaParDef;
		G4ParticleDefinition* geantinoParDef;
        G4ParticleGun* particleGun;
        G4GeneralParticleSource* particleSource;
        MRKDetectorConstruction* myDetector;
        MRKGeneratorMessenger* genMess;
        MRKEvents* theEvents;

        int eventStartClock;
        int eventSetNumber;
        int numberOfEventsToMake;

        G4ThreeVector beamOffset;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


