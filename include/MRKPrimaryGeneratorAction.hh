#ifndef MRKPrimaryGeneratorAction_h
#define MRKPrimaryGeneratorAction_h 1

#include <time.h>

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleDefinition.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "Randomize.hh"

#include "TFile.h"
#include "TTree.h"
#include "TString.h"

#include "MRKDetectorConstruction.hh"
#include "MRKEvents.hh"
#include "MRKGeneratorMessenger.hh"
#include "MRKVector.hh"

class G4Event;

enum GeneratorMode
{
	PARTICLE_GUN = 0, RDKEVENTFILE = 1, RDKEVENTGENERATOR = 3, PARTICLE_SOURCE = 4
};

/// Class for the primary event generator.
class MRKPrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction
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

	inline void setFluxMapFileName(TString inpFluxMapFileName){fluxMapFileName=inpFluxMapFileName;}
    inline GeneratorMode getGeneratorMode(){return generatorMode;}
    inline bool getVerbose() { return eventByEventVerbose; }
    inline G4ThreeVector getMomDir() { return momDir;}
    inline double getE0() { return e0;}
    inline int getCurrentEventNumber(){return currentEventNumber;}
    inline G4String getCurrentParticleName(){return particleGun->GetParticleDefinition()->GetParticleName();}


    inline void setCurrentEventNumber(int inp) { currentEventNumber=inp; }
    inline void setGeneratorMode(GeneratorMode inp) { G4cout << "Setting generator mode." << G4endl; generatorMode=inp;}
    inline void setVerbose(bool inpBool){ eventByEventVerbose=inpBool; }

    inline MRKExperimentModel getExperimentModel(){return myDetector->getExperimentModel();}
    inline bool getUseSBDDetector(){return myDetector->getUseSBDDetector();};
    inline bool getUseBAPDDetectors(){return myDetector->getUseBAPDDetectors();};
    inline bool getUseBGODetectors(){return myDetector->getUseBGODetectors();};
    inline int getNumBGOsUsed(){return myDetector->getNumBGOsUsed();}
    inline int getEventStartClock(){return eventStartClock;}
    inline void setEventSetNumber(int inpEventSetNumber){eventSetNumber=inpEventSetNumber;}
    inline void printRandomNumbers(const int numRandomNumbers){for (int i = 0;i< numRandomNumbers;i++) G4cout << theEvents->getRandomNumber() << G4endl;}
    inline void setNumberOfEventsToMake( int inp){numberOfEventsToMake=inp;}
    inline void setBeamOffset(G4ThreeVector inp){beamOffset=inp;}
    inline void setUseManualPos(bool inp){useManualPos=inp;}
    inline void setUseManualDir(bool inp){useManualDir=inp;}
    inline void setUseManualEnergy(bool inp){useManualEnergy=inp;}
    inline void setUseRDKIIQuickStops(bool inp){useRDKIIQuickStops=inp;}

    void writeEventsToFile(){inpFile->cd(); inpTree->Write("", TObject::kOverwrite); inpFile->Close();}

private:
	MRKDetectorConstruction* theDC;
	GeneratorMode generatorMode;
	bool eventByEventVerbose;
	std::string generatorFileName;
	std::string fluxMapFileName;
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
	bool useRDKIIQuickStops; //Sets whether to simulate event based on RDKII parameters

	Double_t vx0,vy0,vz0,x0,y0,z0,e0,mx0,my0,mz0,tofp;

	Double_t mze0;//always check this for speedup

	Double_t Theta,Phi,FloatEnergy;
	Double_t sinTheta,cosTheta,cosPhi,sinPhi;
//		Int_t DecayNum;

//		Int_t hitcodee, hitcodep;
	Int_t dwcutb[41];
//		Int_t dummy;
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

#endif

