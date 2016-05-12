//////////////////////////////////////////////////
//mattphys - by Matthew Bales
//misc math and physics functions that are useful in multiple applicaitons
//
///////////////////////////////////////////////////
#ifndef MATTPHYS_H_INCLUDED
#define MATTPHYS_H_INCLUDED

//Standard Libraries
#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

//Matt Libraries
#include "constants.hh"
#include "cVector.hh"

//ROOT Libraries
#include "TRandom3.h"
#include "TMath.h"
#include "TCut.h"
#include "TH1.h"
#include "cMRKText.hh"

#include "Randomize.hh"

using namespace std;
using std::string;

cVector3D convertMomentumToVelocity(cVector3D pIn, double massEnergy);  //Converts momentum in keV to m/s
double convertMomentumToVelocity(double inpP,double massEnergy);        //Converts momentum in keV to velocity in m/s
double convertKEToMomentum(double inpKE, double massEnergy);            //Converts kinetic energy in keV to momentum in keV/c
double convertMomentumToKE(double inpMom, double massEnergy);           //Converts momentum in keV to kinetic energy in keV
double convertMomentumToKE(cVector3D inpMom, double massEnergy);        //Converts momentum in keV to kinetic energy in keV
double convertVelocityToKE(cVector3D vIn, double massEnergy);           //Converts velocity in m/s to kinetic energy in keV
double convertVelocityToKE(double vx,double vy,double vz, double massEnergy);   //Converts veloccity in m/s to kinetic energy in keV
cVector3D convertVelocityToMomentum(cVector3D vIn,double massEnergy);   //Converts velocity in m/s to momentum in keV
double convertVelocityToMomentum(double vIn,double massEnergy);         //Converts velocity in m/s to momentum in keV
inline double SQ(double inp){return inp*inp;}                           //outputs the inp squared

double relGamma(double vMagIn);     //calculates special relativity's gamma constant assuming v is in m/s
double relGamma(cVector3D vIn);     //calculates special relativity's gamma constant assuming v is in m/s

double calcEntryAngle(double vxf,double vyf, double vzf);   //Calculates the entry angle into the SBD detector for RDK2 given the three inputted velocities in normal coordinate

//Backscattering
//Brian's Model
double totalBSProb(double inpAngleDegrees);                     //Total probability of backscattering given an inputted angle
double BSSpectrumPeak(double inpAngleDegrees);                  //Determines what the relative size of the backscatter's spectrum peak is given an angle
double probOfBSEnergy(double X,double inpAngleDegrees);         //Determines where on the backscattered peak the energy lies
double electronBS(double inpEE,double entryAngle);              //Outputs energy detectable by detector after backscatter

double linearInterp(double inpPos, vector<double>* inpVector, double inpStart, double inpSpacing);      //Linearly interpolates a STL vector (returns 0 if outside limits)
//TH1F* createBlurredHistogram(TH1F* inpHist,double fwhm, double inpWidthPos, bool poissonStatistics);    //Blurs a ROOT histogram using a constant width gaussian (NOTE poissonStatistics flag should be left as FALSE till this is update)
int roundDouble2Int(double inp);    //Simple conversion from a double to and int

bool transmissionThroughSiO2Layer(double gKE,double gMomZ, TRandom* ranGen, vector<double>* sio2Vector);  //Returns true if gamma gets transmitted by Silicon Dioxide layer.

double gaussProb(double x1,double x2, double mean, double sigma); //Calculates probability/integral of gaussian accross a range

TH1D* getSiO2Hist(); //Gets silicon dioxide layer from file

//Cuts for RDK
extern TCut CUT_E;      //Include only events where electron hits the SBD (NOTE: this includes only MRK simulation hits. You must seperatly exclude based on eeBS energy to account for backscatter)
extern TCut CUT_P;      //Include only events where proton hits the SBD(NOTE: This includes only MRK simulation hits. You must seperatly exclude based on Time of Flight(ToF)
extern TCut CUT_EP;     //Include only events where electron and proton hits
extern TCut CUT_EP_DW;  //Include only events where ep hits plus default decay width cut where b=0
extern TCut CUT_BARE;   //Include only events where a photon hits the bare APD.
extern TCut CUT_BGO;    //Include only events where a photon hits a BGO crystal
extern TCut CUT_EPG;    //Include only events where the electron and proton hit the SBD and a photon hits the BGO
extern TCut CUT_EPB;    //Include only events where the electron and proton hit the SBD and a photon hits the bare APD
extern TCut CUT_TOF;    //Include only events where the proton has a time of flight between the experimental limits
extern TCut CUT_EKE;    //Include only events where the electron energy after backscatter meets the required minimum detection energy
extern TCut CUT_EP_ALL; //Include only events where EP, TOF, and EKE are met
extern TCut CUT_EPG_ALL;//Include only events where EPG, TOF, and EKE are met
extern TCut CUT_EPB_ALL;//Include only events where EPB, TOF, and EKE are met


#endif // MATTPHYS_H_INCLUDED
