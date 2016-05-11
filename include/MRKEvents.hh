//////////////////////////////////////////////////
//MRKEvents - by Matthew Bales
//Event Generator code based on code by Robert Cooper
//
//Creates and stores neutron decay events
//Uses ROOT libraries available from http://root.cern.ch
//
//Units:
//
//Events: Position of verticies: centi-meters
//        Motion of paritcles: direction and kinetic energy in keV
//Runge kutta routine:  Position of verticies: meters
//                      Motion of particles: velocity in meters/second
///////////////////////////////////////////////////


#ifndef CEVENTS_H_INCLUDED
#define MRKEVENTS_H_INCLUDED
#pragma once

//Standard Libraries
#include <iostream>
#include <cmath>
#include <string>
#include <math.h>
#include <sys/stat.h>
#include <vector>

//MJB libraries
#include "cVector.hh" //Math/Physics vectors
#include "constants.hh"
#include "cMRKText.hh"
#include "mattphys.hh"
#include "cVField.hh"

//Root Libraries
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
#include "TH1.h"
#include "TVectorT.h"

using namespace std;
using std::string;


class MRKEvents
{
protected:
    TRandom3* ranGen;                   //Random number generator
	string eventFileName;               //Event File name to load
    TFile* eventFile;                   //ROOT File holding events
    TTree* eventTree;                   //ROOT Tree holding events
    cVector3D p0;
    double x0,y0,z0;                  //Position Variables used to fill tree
    double ep0, mxp0,myp0, mzp0;      //Proton energy and momentum directions used to fill tree
    double ee0, mxe0, mye0, mze0;     //Electron energy and momentum directions used to fill tree
    double eg0, mxg0, myg0, mzg0;     //Photon energy and momentum directions used to fill tree
    double normConstant;                //Constant used to normalize probability to less than, but near, 1
    int numEvents,eventStart,eventEnd;  //Number, start and end event that is stored
    bool homogeneous;                    //Whether the events are homogeneous (whether decay width cut has been applied)
    double littleb;                     //Fierz term in calculation of the cross section for radiative decay
    bool fluxFileLoaded;                //Whether the 2d flux file for vertex generation has been loaded
    bool fourBody;                      //Whether it is a four body event (false - three body)
    double gEMin,gEMax;                  //Gamma Energy Range for generation
    double eEMin;                       //Electron energy minimum
    cSField2D fluxMap;                  //The 2D probability flux map used to create veriticies is stored as a 2D Field

    //Various variables used by event generator.  Stored in object for sharing and speed
    //Naming nomenclature derivives from Rob's Igor Event generator
    // eX = electron parameter, pX = proton parameter, nX = neutrino paramter, gX = photon parameter
    // xE = energy, xKE = kinetic energy (=E-m), xP = momentum, xCTheta = cos(Theta), xSTheta = sin(Theta), xPhi = phi angle
    double eE,ePhi,eCTheta,nPhi,nCTheta,gCTheta,gSTheta,eSTheta,nSTheta,cos_en,cos_eg,cos_ng,pP,pCTheta,pSTheta,eP,nE,pCPhi,pSPhi,Prob,pdn,pde,pdg,edg,edn,ndg,gE,gPhi;
    double pPhi,pKE;
    double avg_integrand, fraction;
    double ranProb;
    bool fermiOn;

//    int thrown;


    //Private Methods
    bool createRandomParameters(bool useEarlyRejector=true);
    double gen3BMom();                    //Generates a 3 body momenta of all particles involved given little b(Fierz term).  Stores results in variables assigned to tree.
	double gen4BMom();         //Generates a 4 body momenta of all particles given a photon energy range. Stores results in variables assigned to tree.


    double getJTWProb();                                          //Calculates the decay width for 3-body decay with arbitrary units from the JTW formula (does not include normConstant)
    double getGapanovProb();                                                    //Calculates the decay width for 4-body decay with arbitrary units from Gapanov's formula (does not include normConstant)
    int makeEventFile(string fileName, string fluxFileName,int inpNumEvents);                    //Makes an event file given the input requirements


public:
	MRKEvents();                                          //Constructor
	MRKEvents(UInt_t inpSeed);                               //Constructor
	~MRKEvents();                                         //Destructor
	void reset();                                       //Resets events to baseline (destructor calls this)
	int loadEvents(string fileName,string treeName);    //Designates root file where decay events are located
    int makeDerivedEvents();                            //Creates a secondary file from an already "loaded" event file that contains other simply calculated parameters
	void genPos();                                      //Generates a vertex in space.  Stores results in variables assigned to tree.
	void getPosVel(ParType parType, cVector3D& rOut,cVector3D& vOut,int eventPos);  //Returns the position (m) and velocity (m/s) of a chosen particle and event number
	void getPosVel(ParType parType, cVector3D& rOut,cVector3D& vOut); //Returns the position (m) and velocity (m/s) of a chosen particle presuming event is currently loaded in memory
    void getPosDirKE(ParType parType, cVector3D& rOut,cVector3D& dOut,double& keOut,int eventPos); //Returns the position (m), direction of momentum, and kinetic energy (keV) of a chosen particle and event number
    void getPosDirKE(ParType parType, cVector3D& rOut,cVector3D& dOut,double& keOut); //Returns the position (m), direction of momentum, and kinetic energy (keV) of a chosen particle and event number
	int getEventStart();                                //Returns the starting event
	int getNumEvents();                                 //Returns the number of events
	int getEventEnd();                                  //Returns the final event
	double getmxe0(){return mxe0;};
    double getmye0(){return mye0;};
    double getmze0(){return mze0;};
    double getee0(){return ee0;};
    double getmxp0(){return mxp0;};
    double getmyp0(){return myp0;};
    double getmzp0(){return mzp0;};
    double getep0(){return ep0;};
    double getmxg0(){return mxg0;};
    double getmyg0(){return myg0;};
    double getmzg0(){return mzg0;};
    double geteg0(){return eg0;};
    double getx0(){return x0;};
    double gety0(){return y0;};
    double getz0(){return z0;};
    double getFourBody(){return fourBody;}
    void setFourBody(bool inpFourBody){fourBody=inpFourBody;}
    void setHomogeneous(bool inpHomogeneous){homogeneous=inpHomogeneous;}
    void setLittleb(double inpLittleb){littleb=inpLittleb;}
    void setNormConstant(double inpNormConstant){normConstant=inpNormConstant;}
    void setGEMin(double inpGEMin){gEMin=inpGEMin;}
    void setGEMax(double inpGEMax){gEMax=inpGEMax;}


    //Event Generation
    void setGoodNormConstant(EveType evetype,double littleb, double gEmin); //Determines/sets normConstant based on a trial run of many decay widths based on the function parameters
    double getFitNormConstant(EveType evetype,double inpGEMin);
    double calcNormConstant(EveType evetype,double littleb, double inpGEMin);


	double calcBR(double inpGEMin,double inpGEMax,double inpEEMin, int numRadEvents,bool inpFermiOn=true, double neutronLifetime=NLIFE);           //Calculates the branching ratio
	double calcFourBodyRate(double inpGEMin,double inpGEMax,double inpEEMin, int numRadEvents,bool inpFermiOn=true);

	void loadFluxFileMap(string fluxFileName);

	int makeEventFile(string fileName, string fluxFileName, int inpNumEvents,                     //Makes an event file given the input requirements
                        EveType evetype, double inpLittleb, bool inpHomogeneous,
                        double inpGEMin, double inpGEMax,bool inpFermiOn=true);



	int makeEventFiles(string firstFileName,  string fluxFileName, int numFiles,int numEventsPer,   //Makes multiple event files
                        EveType evetype, double littleb, bool inphomogeneous,
                        double gEmin,double gEmax,bool inpFermiOn=true);

	int makeDecayWidthCutMultiple(double littlebStart,    //For homogneous events, make multiple decay width cuts
                                double littlebIncrement,const int littlebNum);        //based on a range of little b

	void runEventGen(string runFileName);               //Takes a input filename (which it looks for in the current directory) which contains settings used to generate event files

	void generateEvent();

	void loadEventSettings(string eventSettingsFilePath, int eventSet);  //Includes 500 random seeds for each 1 million event file

	void loadEventSettingsAndMakeFile(string eventSettingsFilePath, int eventSet,string fileName, string fluxFileName,int inpNumEvents);  //Includes 500 random seeds for each 1 million event file

	inline double getRandomNumber(){return ranGen->Rndm();}


};

TH1D* calcBRHist(int numPerBin,int numBins,double gEStart,double gEEnd,double inpEEMin,bool inpFermiOn);

#endif // CEVENTS_H_INCLUDED
