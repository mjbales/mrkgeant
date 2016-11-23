#ifndef RDK2_H_INCLUDED
#define MRKEVENTS_H_INCLUDED

#pragma once

#include "TRandom3.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"

#include "MRKConstants.hh"

#include "MRKVector.hh" //NEED TO REPLACE WITH ROOT 3Vector
#include "MRKVField.hh" //NEED TO REPLACE WITH MRFIELD

///Generates information on daughters (proton, electron, neutrino, and photons) from neutron decay
class MRKEvents
{


public:
	MRKEvents();                                          //Constructor
	MRKEvents(UInt_t inpSeed);                               //Constructor
	~MRKEvents();                                         //Destructor
	void reset();                                       //Resets events to baseline (destructor calls this)
	int loadEvents(TString fileName, TString treeName);    //Designates root file where decay events are located
	int makeDerivedEvents();                            //Creates a secondary file from an already "loaded" event file that contains other simply calculated parameters
	void genPos();           //Generates a vertex in space.  Stores results in variables assigned to tree.
	void getPosVel(ParType parType, MRKVector3D& rOut, MRKVector3D& vOut, int eventPos);  //Returns the position (m) and velocity (m/s) of a chosen particle and event number
	void getPosVel(ParType parType, MRKVector3D& rOut, MRKVector3D& vOut); //Returns the position (m) and velocity (m/s) of a chosen particle presuming event is currently loaded in memory
	void getPosDirKE(ParType parType, MRKVector3D& rOut, MRKVector3D& dOut, double& keOut, int eventPos); //Returns the position (m), direction of momentum, and kinetic energy (keV) of a chosen particle and event number
	void getPosDirKE(ParType parType, MRKVector3D& rOut, MRKVector3D& dOut, double& keOut); //Returns the position (m), direction of momentum, and kinetic energy (keV) of a chosen particle and event number
	int getEventStart();                                //Returns the starting event
	int getNumEvents();                                 //Returns the number of events
	int getEventEnd();                                  //Returns the final event
	inline double getmxe0(){return mxe0;};
	inline double getmye0(){return mye0;};
	inline double getmze0(){return mze0;};
	inline double getee0(){return ee0;};
	inline double getmxp0(){return mxp0;};
	inline double getmyp0(){return myp0;};
	inline double getmzp0(){return mzp0;};
	inline double getep0(){return ep0;};
	inline double getmxg0(){return mxg0;};
	inline double getmyg0(){return myg0;};
	inline double getmzg0(){return mzg0;};
	inline double geteg0(){return eg0;};
	inline double getx0(){return x0;};
	inline double gety0(){return y0;};
	inline double getz0(){return z0;};
	inline double getFourBody(){return fourBody;}
	inline void setFourBody(bool inpFourBody){fourBody=inpFourBody;}
	inline void setHomogeneous(bool inpHomogeneous){homogeneous=inpHomogeneous;}
	inline void setLittleb(double inpLittleb){littleb=inpLittleb;}
	inline void setNormConstant(double inpNormConstant){normConstant=inpNormConstant;}
	inline void setGEMin(double inpGEMin){gEMin=inpGEMin;}
	inline void setGEMax(double inpGEMax){gEMax=inpGEMax;}

	//Event Generation
	void setGoodNormConstant(EveType evetype, double littleb, double gEmin); //Determines/sets normConstant based on a trial run of many decay widths based on the function parameters
	double getFitNormConstant(EveType evetype, double inpGEMin);
	double calcNormConstant(EveType evetype, double littleb, double inpGEMin);

	double calcBR(double inpGEMin, double inpGEMax, double inpEEMin, int numRadEvents, bool inpFermiOn = true, double neutronLifetime = NLIFE);           //Calculates the branching ratio
	double calcFourBodyRate(double inpGEMin, double inpGEMax, double inpEEMin, int numRadEvents, bool inpFermiOn = true);

	void loadFluxFileMap(TString fluxFileName);

	int makeEventFile(TString fileName, TString fluxFileName, int inpNumEvents,                     //Makes an event file given the input requirements
		EveType evetype, double inpLittleb = 0, bool inpHomogeneous = false, double inpGEMin = 0, double inpGEMax = 0, bool inpFermiOn = true, double inpZStart = EVENTGEN_Z_MIN, double inpZEnd = EVENTGEN_Z_MAX);

	int makeEventFiles(TString firstFileName, TString fluxFileName, int numFiles, int numEventsPer,   //Makes multiple event files
		EveType evetype, double littleb, bool inphomogeneous, double gEmin, double gEmax, bool inpFermiOn = true, double inpZStart = EVENTGEN_Z_MIN, double inpZEnd = EVENTGEN_Z_MAX);

	int makeDecayWidthCutMultiple(double littlebStart,    //For homogneous events, make multiple decay width cuts
		double littlebIncrement, const int littlebNum);        //based on a range of little b

	void runEventGen(TString runFileName);               //Takes a input filename (which it looks for in the current directory) which contains settings used to generate event files

	void generateEvent();

	void loadEventSettings(TString eventSettingsFilePath, int eventSet);  //Includes 500 random seeds for each 1 million event file

	void loadEventSettingsAndMakeFile(TString eventSettingsFilePath, int eventSet, TString fileName, TString fluxFileName, int inpNumEvents);  //Includes 500 random seeds for each 1 million event file

	inline double getRandomNumber(){return ranGen->Rndm();}

protected:
	TRandom3* ranGen;                   //Random number generator
	TString eventFileName;               //Event File name to load
	TFile* eventFile;                   //ROOT File holding events
	TTree* eventTree;                   //ROOT Tree holding events
	MRKVector3D p0;
	double x0, y0, z0;                  //Position Variables used to fill tree
	double ep0, mxp0, myp0, mzp0;      //Proton energy and momentum directions used to fill tree
	double ee0, mxe0, mye0, mze0;     //Electron energy and momentum directions used to fill tree
	double eg0, mxg0, myg0, mzg0;     //Photon energy and momentum directions used to fill tree
	double normConstant;                //Constant used to normalize probability to less than, but near, 1
	int numEvents, eventStart, eventEnd;  //Number, start and end event that is stored
	bool homogeneous;                    //Whether the events are homogeneous (whether decay width cut has been applied)
	double littleb;                     //Fierz term in calculation of the cross section for radiative decay
	bool fluxFileLoaded;                //Whether the 2d flux file for vertex generation has been loaded
	bool fourBody;                      //Whether it is a four body event (false - three body)
	double gEMin, gEMax;                  //Gamma Energy Range for generation
	double eEMin;                       //Electron energy minimum
	cSField2D fluxMap;                  //The 2D probability flux map used to create veriticies is stored as a 2D Field
	double zStart, zEnd;               //Creating the beam between these two points

	//Various variables used by event generator.  Stored in object for sharing and speed
	//Naming nomenclature derivives from Rob's Igor Event generator
	// eX = electron parameter, pX = proton parameter, nX = neutrino paramter, gX = photon parameter
	// xE = energy, xKE = kinetic energy (=E-m), xP = momentum, xCTheta = cos(Theta), xSTheta = sin(Theta), xPhi = phi angle
	double eE, ePhi, eCTheta, nPhi, nCTheta, gCTheta, gSTheta, eSTheta, nSTheta, cos_en, cos_eg, cos_ng, pP, pCTheta, pSTheta, eP, nE, pCPhi, pSPhi, Prob, pdn, pde, pdg, edg, edn, ndg, gE, gPhi;
	double pPhi, pKE;
	double avg_integrand, fraction;
	double ranProb;
	bool fermiOn;

//    int thrown;

	//Private Methods
	bool createRandomParameters(bool useEarlyRejector = true);
	double gen3BMom();                    //Generates a 3 body momenta of all particles involved given little b(Fierz term).  Stores results in variables assigned to tree.
	double gen4BMom();         //Generates a 4 body momenta of all particles given a photon energy range. Stores results in variables assigned to tree.

	double getJTWProb();                                          //Calculates the decay width for 3-body decay with arbitrary units from the JTW formula (does not include normConstant)
	double getGapanovProb();                                                    //Calculates the decay width for 4-body decay with arbitrary units from Gapanov's formula (does not include normConstant)
	int makeEventFile(TString fileName, TString fluxFileName, int inpNumEvents);                    //Makes an event file given the input requirements



};

TH1D* calcBRHist(int numPerBin,int numBins,double gEStart,double gEEnd,double inpEEMin,bool inpFermiOn,bool cumulative);

#endif // RDK2_H_INCLUDED
