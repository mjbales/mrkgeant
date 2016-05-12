#include "mattphys.hh"
#include <iostream>

//Cuts
TCut CUT_E ="r.hitcodee == 1";
TCut CUT_P ="r.hitcodep == 1";
TCut CUT_EP =CUT_E && CUT_P;
TCut CUT_EP_DW = CUT_EP && "rd.dwcutb10==0";
TCut CUT_BARE ="r.hitcodeg >= 15 && r.hitcodeg <= 17";
TCut CUT_BGO ="r.hitcodeg >= 18 && r.hitcodeg <= 25";
TCut CUT_EPG = CUT_BGO && CUT_EP;
TCut CUT_EPB = CUT_BARE && CUT_EP;
TCut CUT_TOF = "r.tofp > 1.4e-6 && r.tofp < 25e-6";
TCut CUT_EKE = "rd.eeBS > 25";
TCut CUT_EP_ALL = CUT_EP && CUT_TOF && CUT_EKE;
TCut CUT_EPG_ALL = CUT_EPG && CUT_TOF && CUT_EKE;
TCut CUT_EPB_ALL = CUT_EPB && CUT_TOF && CUT_EKE;


cVector3D convertMomentumToVelocity(cVector3D inpP, double massEnergy){
     return inpP*(CSPEED/ sqrt( inpP.x*inpP.x+inpP.y*inpP.y +inpP.z*inpP.z + pow(massEnergy,2)));
}

double convertMomentumToVelocity(double inpP, double massEnergy){
     return CSPEED*inpP/ sqrt( inpP*inpP+ pow(massEnergy,2));
}

double convertKEToMomentum(double inpKE, double massEnergy){
     return sqrt(pow(inpKE+massEnergy,2)-pow(massEnergy,2));
}

double convertMomentumToKE(double inpMom, double massEnergy){
    return sqrt(inpMom*inpMom+massEnergy*massEnergy)-massEnergy;
}

double convertMomentumToKE(cVector3D inpMom, double massEnergy){
    return sqrt(inpMom.x*inpMom.x+inpMom.y*inpMom.y+inpMom.z*inpMom.z+massEnergy*massEnergy)-massEnergy;

}

cVector3D convertVelocityToMomentum(cVector3D vIn,double massEnergy)
{
    return vIn.scale(relGamma(vIn)*massEnergy*INVCSPEED); //Gamma*mass*velocity
}

double convertVelocityToMomentum(double vIn,double massEnergy)
{
    return relGamma(vIn)*massEnergy*INVCSPEED*vIn;
}

double convertVelocityToKE(cVector3D vIn, double massEnergy){

    return convertMomentumToKE(convertVelocityToMomentum(vIn,massEnergy),massEnergy);
}

double convertVelocityToKE(double vx,double vy,double vz, double massEnergy)
{
    cVector3D vIn(vx,vy,vz);
    return convertVelocityToKE(vIn,massEnergy);
}

double relGamma(double vMagIn)
{
    return 1./sqrt(1.-vMagIn*vMagIn*INVCSQUARED);
}

double relGamma(cVector3D vIn)
{
    return 1./sqrt(1.-(vIn.x*vIn.x+vIn.y*vIn.y+vIn.z*vIn.z)*INVCSQUARED);
}



//////////////////////////////////////////////////////////////////////////////////////
//Created from directions from Brian Fisher
double totalBSProb(double inpAngleDegrees)
{
	double A0 = 0.159118;
	double A1 = 0.0108824;
	double A2 = 0.0468;

	return A0+A1*exp(A2*inpAngleDegrees);
}
//////////////////////////////////////////////////////////////////////////////////////
double BSSpectrumPeak(double inpAngleDegrees)
{
	double c0 = 0.37;
	double c1 = 0.70;
	double c2 = 0.09;

	return c0+c1*(1.-1./(1.+exp(c2*(inpAngleDegrees-63.))));
}
//////////////////////////////////////////////////////////////////////////////////////
double probOfBSEnergy(double X,double inpAngleDegrees)
{
	double P = BSSpectrumPeak(inpAngleDegrees);
	return exp(-(pow(X-P,2))/(.7*(.1+X)*(1.2-X)));
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// Returns energy detectable by detector after backscatter
// No minimum energy cut off accounted for
// entry angle from norm in radians
double electronBS(double inpEE,double entryAngle)
{
    double eeKept;    //Energy backscattered
    double eeDetect;//Energy detectable
    double bsProb;  //Probability of backscatter
    double X;       //Fraction of backscattering energy


    bsProb=totalBSProb(entryAngle*180./PI);

    if(gRandom->Rndm() < bsProb)  //Did it backscatter
    {
        for(;;) //Ok it did backcatter so we'll try to find the energy
        {
            X=gRandom->Rndm();  //Let's guess this fraction of the energy is left
            if(gRandom->Rndm() < probOfBSEnergy(X,entryAngle*180./PI) )  //Check versus Brian's model
            {
                eeKept=inpEE*X;     //Yup it still has this fraction
                break;              //So let's get out of here
            }
        }
    }
    else
        eeKept=0.;                    //Not backscattered, so absorbed, so electron doesn't keep it's energy

    eeDetect=inpEE-eeKept;          //The detectable energy is the amount the electron looses


    eeDetect=gRandom->Gaus(eeDetect,SBD_ENERGY_STD);  //For energy resolution of SBD detector

    return eeDetect;
}


double linearInterp(double inpPos, vector<double>* inpVector, double inpStart, double inpSpacing)
{
    double floor;
    double mod = modf((inpPos-inpStart)/inpSpacing,&floor);
	unsigned int i=int(floor);
	if(i > inpVector->size() - 1){
		return 0;
	}
    return inpVector->at(i)*(1-mod)+inpVector->at(i+1)*mod;
}

double calcEntryAngle(double vxf,double vyf, double vzf)
{
    double vxtf,vztf;
    double vrtf;
    vztf= vzf*COS_BEND+vxf*SIN_BEND;  				//Replaced coordinate transformation for speed
    vxtf = -vzf*SIN_BEND+vxf*COS_BEND;

    vrtf=sqrt(vyf*vyf+vxtf*vxtf);

    return atan(vrtf/-vztf);

}

//Blurs histogram.
//Assumes poisson statistics if bool set to true;
/*TH1D* createBlurredHistogram(TH1D* inpHist,double fwhm, double inpWidthPos,bool poissonStatistics)
{
    double binSize=inpHist->GetBinWidth(1); //I assume constant bin size btw
    double numBins=inpHist->GetNbinsX();
    double topOfHighestBin = inpHist->GetBinLowEdge(numBins)+binSize;
    double bottomOfLowestBin=inpHist->GetBinLowEdge(1);
    double std=fwhm/(2*sqrt(2*log(2.)));

    string newName = inpHist->GetName();
    newName+="_blurred";
    string newTitle=inpHist->GetTitle();
    newTitle+=" - blurred";
    TH1D* blurredHist=new TH1D(newName.data(),newTitle.data(),numBins,bottomOfLowestBin,topOfHighestBin);

    double A,dataCenter,blurCenter,amountToAdd,prevBlurredContent;
    double erfLow,erfHigh;


    //Should change this to using theHist->Eval()
    for(int i=0;i<numBins;i++)
    {
        dataCenter=inpHist->GetBinCenter(i+1);
        A=inpHist->GetBinContent(i+1);
        if(A>0)
        {
            for(int j=0;j<numBins;j++)
            {
                blurCenter=blurredHist->GetBinCenter(j+1);
                if(poissonStatistics)
                    amountToAdd=TMath::Poisson(j*.52,dataCenter*.52)*A; //currently customized to a problem in code
                else
                {
                    erfLow=TMath::Erf((blurCenter - .5*binSize-dataCenter)/(sqrt(2.)*std));
                    erfHigh=TMath::Erf((blurCenter + .5*binSize-dataCenter)/(sqrt(2.)*std));
                    amountToAdd=.5*(erfHigh-erfLow)*A;
                }
                prevBlurredContent = blurredHist->GetBinContent(j+1);


                blurredHist->SetBinContent(j+1,amountToAdd+prevBlurredContent);
            }
        }

    }

    return blurredHist;

}
*/

int roundDouble2Int(double inp)
{
    return (int)ceil(inp-.5);
}


bool transmissionThroughSiO2Layer(double gKE,double gMomZ, vector<double>* sio2Vector)
{
    double thicknessMultiplier=gKE/abs(gMomZ); //Due to angle of incidence
    double transCoeff;
    if(gKE>=30)
        transCoeff=1;
    else
    {
        double dataStart=ABSORB_DATA_START;
        double dataSpacing=ABSORB_DATA_SPACING;
        double base=linearInterp(gKE, sio2Vector,dataStart,dataSpacing);
        transCoeff=pow(base,thicknessMultiplier);
    }
    if(G4UniformRand() < (1-transCoeff)) //If absorbed in SiO2
    {
        return false;
    }
    return true;
}

double gaussProb(double x1,double x2, double mean, double sigma)
{
    double t1=(x1-mean)/sigma;
    double t2=(x2-mean)/sigma;

    return .5*(TMath::Erf(t2/SQRT2)-TMath::Erf(t1/SQRT2));
}

TH1D* getSiO2Hist()
{

    string fileName="SiO2_Absorbtion.dat";

	if(!FileExists(fileName))
	{
	    cout << fileName << " not found!" << endl;
	   // typeAnythingToContinue("");
        return NULL;
	}
    //SIO2 Absorbtion
    string tempString;
    double temp,transCoeff;

    double bottomOfHist=ABSORB_DATA_START-.5*ABSORB_DATA_SPACING;
    double topOfHist=bottomOfHist+ABSORB_DATA_NUM*ABSORB_DATA_SPACING;

    TH1D* sio2Hist = new TH1D("SiO2Hist","SiO2 Tranmission Hist for 100 nm",ABSORB_DATA_NUM,bottomOfHist,topOfHist);


	ifstream sio2File;



    sio2File.open(fileName.data());

    //Skipping past first two lines of dat file
    getNonCommentLine(sio2File,tempString);
    getNonCommentLine(sio2File,tempString);


    for(int i=0;i<ABSORB_DATA_NUM;i++)
    {
        sio2File >> temp >> transCoeff;
        sio2Hist->SetBinContent(i+1,transCoeff);

    }
    sio2File.close();

    return sio2Hist;
}
