#include "MRKEvents.hh"

//Standard Libraries
#include <iostream>
#include <cmath>
#include <string>
#include <math.h>
#include <sys/stat.h>
#include <vector>

//MJB libraries
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
#include "TH1.h"
#include "TVectorT.h"
#include "TVector3.h"
#include "TMath.h"

#include "MRKConstants.hh"
#include "MRKPhys.hh"
#include "MRKText.hh"

using namespace std;

MRKEvents::MRKEvents(void)
{
	gE = 0;
	eventFile = nullptr;
	eventTree = nullptr;
	numEvents = 0;
	homogeneous = 0;
	ranGen = nullptr;
	fourBody = false;
	normConstant = 0;
	fluxFileLoaded = false;
	fermiOn = true;
	eEMin = 0;
	gEMin = 0;
	gEMax = 0;
	zStart = EVENTGEN_Z_MIN;
	zEnd = EVENTGEN_Z_MAX;
	littleb = 0;
	eE = ePhi = eCTheta = nPhi = nCTheta = gCTheta = gSTheta = eSTheta = nSTheta = cos_en = cos_eg = cos_ng = pP = pCTheta = pSTheta = eP = nE = pCPhi = pSPhi = Prob = pdn = pde = pdg = edg = edn = ndg = gE = gPhi = 0.;
	pKE = mxp0 = eg0 = mzp0 = z0 = ee0 = mxg0 = mze0 = mxe0 = myg0 = ep0 = fraction = x0 = y0 = ranProb = mye0 = pPhi = mzg0 = avg_integrand = myp0 = 0.;
	eventStart = eventEnd = 0;
}

MRKEvents::~MRKEvents(void)
{
	reset();
	if(ranGen != nullptr)
	{
		delete ranGen;
	}
}

void MRKEvents::reset()  //Doesn't delete ranGen but destructor does
{
	if(eventFile != nullptr)
	{
		eventFile->Close();
		delete eventFile;
	}
	eventFile = nullptr;
	eventTree = nullptr;
	if(fluxFileLoaded)
	{
		fluxFileLoaded = false;
		fluxMap.reset();
	}
	numEvents = 0;
}

int MRKEvents::loadEvents(TString fileName, TString treeName)
{
	if(eventFile != nullptr)
	{
		eventFile->Close();
		delete eventFile;
	}

	if(ranGen == nullptr)
	{
		ranGen = new TRandom3();
	}
	eventFileName = fileName;
	eventFile = new TFile(fileName, "READ");

	//Check to see if file was succesfully opened
	if(eventFile->IsZombie())
	{
		reset();
		return -1;
	}

	//Get Tree from file
	eventTree = (TTree*) eventFile->Get(treeName);

	TBranch* n = eventTree->GetBranch("n");

	numEvents = n->GetEntries();
	eventStart = 0;
	eventEnd = eventStart + numEvents - 1;

	//Set our branch addresses for filling
	eventTree->SetBranchAddress("x0", &x0);
	eventTree->SetBranchAddress("y0", &y0);
	eventTree->SetBranchAddress("z0", &z0);

	eventTree->SetBranchAddress("ee0", &ee0);
	eventTree->SetBranchAddress("mxe0", &mxe0);
	eventTree->SetBranchAddress("mye0", &mye0);
	eventTree->SetBranchAddress("mze0", &mze0);

	eventTree->SetBranchAddress("ep0", &ep0);
	eventTree->SetBranchAddress("mxp0", &mxp0);
	eventTree->SetBranchAddress("myp0", &myp0);
	eventTree->SetBranchAddress("mzp0", &mzp0);

	eventTree->SetBranchAddress("eg0", &eg0);

	eventTree->SetBranchAddress("mxg0", &mxg0);
	eventTree->SetBranchAddress("myg0", &myg0);
	eventTree->SetBranchAddress("mzg0", &mzg0);

	//If eg0 is greater than 0 than it must be a four body decay
	if(eventTree->GetBranch("eg0")->GetEntry(0) > 0)
		fourBody = true;
	else
		fourBody = false;
	return 0;

}

void MRKEvents::getPosVel(ParType parType, MRKVector3D& rOut, MRKVector3D& vOut, int eventPos)
{
	eventTree->GetEntry(eventPos);

	getPosVel(parType, rOut, vOut);
}

void MRKEvents::getPosVel(ParType parType, MRKVector3D& rOut, MRKVector3D& vOut)
{
	//Presumes event is currently loaded in the variables.
	if(parType == PROTON)
	{
		p0.setVal(mxp0, myp0, mzp0);
		p0 = convertKEToMomentum(ep0, PMASSE) * p0;
		TVector3 p03(p0.x, p0.y, p0.z);
		TVector3 vOut3 = convertMomentumToVelocity(p03, PMASSE);
		vOut.setVal(vOut3.X(), vOut3.Y(), vOut3.Z());
	}

	else if(parType == ELECTRON)
	{
		p0.setVal(mxe0, mye0, mze0);
		p0 = convertKEToMomentum(ee0, EMASSE) * p0;
		TVector3 p03(p0.x, p0.y, p0.z);
		TVector3 vOut3 = convertMomentumToVelocity(p03, EMASSE);
		vOut.setVal(vOut3.X(), vOut3.Y(), vOut3.Z());

	}

	else if(parType == PHOTON)
	{
		vOut.setVal(mxg0, myg0, mzg0);
		vOut = eg0 * vOut;
	}

	rOut.setVal(x0 * .01, y0 * .01, z0 * .01); //For cm to meter conversion
}

void MRKEvents::getPosDirKE(ParType parType, MRKVector3D& rOut, MRKVector3D& dOut, double& keOut, int eventPos)
{
	eventTree->GetEntry(eventPos);

	getPosDirKE(parType, rOut, dOut, keOut);
}

void MRKEvents::getPosDirKE(ParType parType, MRKVector3D& rOut, MRKVector3D& dOut, double& keOut)
{

	if(parType == PROTON)
	{
		dOut.setVal(mxp0, myp0, mzp0);
		keOut = ep0;
	}

	else if(parType == ELECTRON)
	{
		dOut.setVal(mxe0, mye0, mze0);
		keOut = ee0;

	}

	else if(parType == PHOTON)
	{
		dOut.setVal(mxg0, myg0, mzg0);
		keOut = eg0;
	}

	rOut.setVal(x0 * .01, y0 * .01, z0 * .01); //For cm to meter conversion
}

int MRKEvents::getEventStart()
{
	return eventStart;
}
int MRKEvents::getNumEvents()
{
	return numEvents;
}
int MRKEvents::getEventEnd()
{
	return eventEnd;
}

int MRKEvents::makeDerivedEvents()
{
	Double_t vxp0, vyp0, vzp0;
	Double_t vxe0, vye0, vze0;
	Double_t vxg0, vyg0, vzg0;

	Double_t __attribute__((__unused__)) pxp0;
	Double_t __attribute__((__unused__)) pyp0;
	Double_t __attribute__((__unused__)) pzp0;
	Double_t __attribute__((__unused__)) pxe0;
	Double_t __attribute__((__unused__)) pye0;
	Double_t __attribute__((__unused__)) pze0;
	Double_t __attribute__((__unused__)) pxg0;
	Double_t __attribute__((__unused__)) pyg0;
	Double_t __attribute__((__unused__)) pzg0;

	double momentum, velocity;

	TString derivedFileName = addBeforeExtension(eventFileName, "_Derv"); //Adds Derv suffix before extention (typically .root)

	//Open new file
	TFile derivedFile(derivedFileName, "NEW");
	derivedFile.cd();

	//Confirm file opened
	if(derivedFile.IsZombie())
	{
		return -1;
	}

	//Make new tree
	TTree derivedTree(DEFAULT_EVENTSDERV_TREENAME, "MRK Events Derived");

	//Branches for Protons
	derivedTree.Branch("vxp0", &vxp0, "vxp0/D");
	derivedTree.Branch("vyp0", &vyp0, "vyp0/D");
	derivedTree.Branch("vzp0", &vzp0, "vzp0/D");
	derivedTree.Branch("pxp0", &vxp0, "pxp0/D");
	derivedTree.Branch("pyp0", &vyp0, "pyp0/D");
	derivedTree.Branch("pzp0", &vzp0, "pzp0/D");

	//Branches for Electrons
	derivedTree.Branch("vxe0", &vxe0, "vxe0/D");
	derivedTree.Branch("vye0", &vye0, "vye0/D");
	derivedTree.Branch("vze0", &vze0, "vze0/D");
	derivedTree.Branch("pxe0", &vxe0, "pxe0/D");
	derivedTree.Branch("pye0", &vye0, "pye0/D");
	derivedTree.Branch("pze0", &vze0, "pze0/D");

	//Branches for Photons
	derivedTree.Branch("vxg0", &vxg0, "vxg0/D");
	derivedTree.Branch("vyg0", &vyg0, "vyg0/D");
	derivedTree.Branch("vzg0", &vzg0, "vzg0/D");
	derivedTree.Branch("pxg0", &vxg0, "pxg0/D");
	derivedTree.Branch("pyg0", &vyg0, "pyg0/D");
	derivedTree.Branch("pzg0", &vzg0, "pzg0/D");

	for (int i = 0; i < numEvents; i++)
	{
		eventTree->GetEntry(i);

		//Proton conversions
		momentum = convertKEToMomentum(ep0, PMASSE);
		pxp0 = momentum * mxp0;
		pyp0 = momentum * myp0;
		pzp0 = momentum * mzp0;
		velocity = convertMomentumToVelocity(momentum, PMASSE);
		vxp0 = velocity * mxp0;
		vyp0 = velocity * myp0;
		vzp0 = velocity * mzp0;

		//Electron conversions
		momentum = convertKEToMomentum(ee0, EMASSE);
		pxe0 = momentum * mxe0;
		pye0 = momentum * mye0;
		pze0 = momentum * mze0;
		velocity = convertMomentumToVelocity(momentum, EMASSE);
		vxe0 = velocity * mxe0;
		vye0 = velocity * mye0;
		vze0 = velocity * mze0;

		//Photon conversions
		momentum = convertKEToMomentum(eg0, 0);
		pxg0 = momentum * mxg0;
		pyg0 = momentum * myg0;
		pzg0 = momentum * mzg0;
		velocity = convertMomentumToVelocity(momentum, 0);
		vxg0 = velocity * mxg0;
		vyg0 = velocity * myg0;
		vzg0 = velocity * mzg0;

		derivedTree.Fill();

	}

	derivedTree.Write();

	derivedFile.Close();

	return 0;
}

bool MRKEvents::createRandomParameters(bool useEarlyRejector)
{
	ranProb = ranGen->Rndm();

	//Generate Random parameters
	if(fourBody)
	{
		gE = (gEMax - gEMin) * ranGen->Rndm() + gEMin;  //Photon Energy
		if(useEarlyRejector && ranProb > normConstant / getFitNormConstant(FOURBODY, gE))  //We're going to check ahead and see if this gamma energy will be obviously rejected
		{
			return false;
		}
	}
	else
	{
		gE = 0;
	}

	eE = (ETEMAX - EMASSE - eEMin) * ranGen->Rndm() + EMASSE + eEMin; //Electron Energy
	ePhi = ranGen->Rndm() * TMath::Pi() * 2. - TMath::Pi();  //Electron phi angle

	eCTheta = ranGen->Rndm() * 2. - 1.;  //Electron cos theta angle

	nPhi = ranGen->Rndm() * TMath::Pi() * 2. - TMath::Pi(); //Neutrino phi angle

	nCTheta = ranGen->Rndm() * 2. - 1.;  //Neutrino cos theta angle

	eSTheta = sqrt(1 - eCTheta * eCTheta);  //Electron sin theta
	nSTheta = sqrt(1 - nCTheta * nCTheta);  //Neutrino sin theta

	cos_en = eSTheta * nSTheta * cos(ePhi - nPhi) + eCTheta * nCTheta;

	//Generate parameters that relate to four body events alone
	if(fourBody)
	{
		gPhi = ranGen->Rndm() * TMath::Pi() * 2. - TMath::Pi(); //Photon phi angle
//        thrown++;
		gCTheta = ranGen->Rndm() * 2. - 1.;  //Photon cos theta angle
//        thrown++;
		gSTheta = sqrt(1 - gCTheta * gCTheta); //Photon sin theta angle
		cos_eg = eSTheta * gSTheta * cos(ePhi - gPhi) + eCTheta * gCTheta; //cos opening angle between electron and photon
		cos_ng = nSTheta * gSTheta * cos(nPhi - gPhi) + nCTheta * gCTheta; //cos opening angle between neutrino and photon
	}
	else
	{
		gE = gPhi = gCTheta = gSTheta = cos_eg = cos_ng = 0;
	}

	//Calculate remaining parameters
	eP = sqrt(eE * eE - EMASSE * EMASSE); //Proton energy
	nE = (NMASSE * NMASSE - PMASSE * PMASSE + (EMASSE * EMASSE) - 2. * NMASSE * (eE + gE) + 2. * eE * gE - 2. * eP * gE * cos_eg) / 2. / (NMASSE - (eE + gE) + gE * cos_ng + eP * cos_en); //Neutrino energy

	//If it is not kinematically allowed do not calculate any addition parameters
	//If this occurs this function will typically be called again until the neutrino energy is positive
	if(nE <= 0) return false;

	pKE = NMASSE - PMASSE - eE - gE - nE;  //proton kinetic energy
	pP = sqrt(pKE * pKE + 2. * pKE * PMASSE); //proton momentum
	pCTheta = -(eP * eCTheta + nE * nCTheta + gE * gCTheta) / pP; //proton cos theta
	pSTheta = sqrt(1. - pCTheta * pCTheta); //proton sin theta
	pCPhi = -(eP * eSTheta * cos(ePhi) + nE * nSTheta * cos(nPhi) + gE * gSTheta * cos(gPhi)) / pP / pSTheta; //proton cos phi
	pSPhi = -(eP * eSTheta * sin(ePhi) + nE * nSTheta * sin(nPhi) + gE * gSTheta * sin(gPhi)) / pP / pSTheta; //proton sin phi

	if(fourBody)
	{
		pde = NMASSE * eE - (EMASSE * EMASSE) - nE * (eE - eP * cos_en) - gE * (eE - eP * cos_eg); //proton momentum dot e momentum
		pdn = nE * (NMASSE - (eE - eP * cos_en) - gE * (1. - cos_ng)); //proton momentum dot neutrino momentum
		pdg = gE * (NMASSE - (eE - eP * cos_eg) - nE * (1. - cos_ng)); //proton momentum dot photon momentum
		edn = nE * (eE - eP * cos_en); //electron momentum dot neutrino momentum
		edg = gE * (eE - eP * cos_eg); //electron momentum dot photon momentum
		ndg = nE * gE * (1. - cos_ng); //neutrino momentum dot photon momentum
	}
	//No need for else as these dot products won't be used by JTW formula

	return true;
}

double MRKEvents::getJTWProb()
{
//     return (eE/eP)*FSCONST/(1.-exp(-eE/eP*FSCONST))*eP*eE*pow(ETEMAX-eE,2)*(1.+LITTLEA*eP/eE*cos_en+littleb*EMASSE/eE);
	return (eE / eP) * 2 * TMath::Pi() * FSCONST / (1. - exp(-eE / eP * FSCONST * 2 * TMath::Pi())) * eP * eE * pow(ETEMAX - eE, 2) * (1. + LITTLEA * eP / eE * cos_en + littleb * EMASSE / eE);  //Fermi function corrected 4/25/13
}

double MRKEvents::getGapanovProb()
{
	// (1+lambda^2) term
	Prob = 2. * (1. + (RADDECAY_LAMBDA * RADDECAY_LAMBDA)) * (nE / (edg * edg) * ((EMASSE * EMASSE) * (eE + gE) - edg * gE) + nE * (PMASSE * PMASSE) * eE / (pdg * pdg) - nE / edg / pdg * (eE * pdg - (pKE + PMASSE) * edg + (2. * eE + gE) * pde));

	// (1-lambda^2) term
	Prob = Prob - (1. - (RADDECAY_LAMBDA * RADDECAY_LAMBDA)) * (1. / (edg * edg) * ((EMASSE * EMASSE) * (edn + ndg) - edg * ndg) + (PMASSE * PMASSE) * edn / (pdg * pdg) - 1. / edg / pdg * (edn * pdg - pdn * edg + (2. * edn + ndg) * pde));

	// overall scale
	Prob = -eP * gE * nE / 2. * Prob;

	if(fermiOn)
	{
		Prob *= (eE / eP) * 2 * TMath::Pi() * FSCONST / (1. - exp(-eE / eP * 2 * TMath::Pi() * FSCONST)); //Fermi function added 4/25/13
	}
	return Prob;
}

//Generates a 3 body decay momentums
double MRKEvents::gen3BMom()
{

	//Von Neuman rejection loop
	for (;;)
	{
		//Loop for finding kinematically allowed variables
		for (nE = 0; nE <= 0;)
			createRandomParameters();

		if(homogeneous) break;

		Prob = normConstant * getJTWProb();
		if(Prob >= 1.) return Prob;

		if((Prob) > ranGen->Rndm()) // 1/0.14 suitable for little b < 0.1 (according to Rob)
		break;
	}

	ep0 = pKE;
	ee0 = eE - EMASSE;

	//Electron
	mze0 = eCTheta;
	mye0 = eSTheta * sin(ePhi);
	mxe0 = eSTheta * cos(ePhi);

	//Proton
	mzp0 = pCTheta;
	myp0 = pSTheta * pSPhi;
	mxp0 = pSTheta * pCPhi;

	//No gamma
	eg0 = 0;
	mzg0 = 0;
	myg0 = 0;
	mxg0 = 0;

	return Prob;
}

//Generates a 3 body decay momentums
double MRKEvents::gen4BMom()
{
	//Von Neuman rejection loop
	for (;;)
	{
		//Loop for finding kinematically allowed variables
		for (nE = 0; nE <= 0;)
			createRandomParameters();

		if(homogeneous) break;

		Prob = normConstant * getGapanovProb();

		if(Prob >= 1) return Prob;

		if((Prob) > ranProb) break;

	}

	ep0 = pKE;
	ee0 = eE - EMASSE;
	//Electron
	mze0 = eCTheta;
	mye0 = eSTheta * sin(ePhi);
	mxe0 = eSTheta * cos(ePhi);
	//Proton
	mzp0 = pCTheta;
	myp0 = pSTheta * pSPhi;
	mxp0 = pSTheta * pCPhi;
	//Gamma
	eg0 = gE;
	mzg0 = gCTheta;
	myg0 = gSTheta * sin(gPhi);
	mxg0 = gSTheta * cos(gPhi);

	return Prob;

}

void MRKEvents::genPos()
{
	//Assumes fluxmap is already loaded
	MRKVector2D mapPos;
	double fluxVal;
	if(!fluxFileLoaded)
	{
		cout << "Flux file not loaded.  No new position generated" << endl;
		return;
	}
	for (;;)
	{
		//Determines random position within flux map boundaries
		mapPos.r = fluxMap.getRStart() + fluxMap.getRSpacing() * fluxMap.getRows() * ranGen->Rndm();
		mapPos.z = fluxMap.getZStart() + fluxMap.getZSpacing() * fluxMap.getColumns() * ranGen->Rndm();

		//Determine probability of event at determined position
		fluxVal = 0;
		fluxMap.linearInterp(mapPos, fluxVal);
		//Throw a random number to determine if event occured there
		//repeat process if random is higher than the probability
		if(ranGen->Rndm() <= fluxVal) break;
	}

	//Z position is randomly determined in set range independently
	z0 = ranGen->Rndm() * (zEnd - zStart) + zStart;

	//Once z position is known, the x and y position are expanded by appropriate
	//divergence angle
	x0 = mapPos.r * (1 - (FLUXMAP_POS_Z - z0) * sin(BEAM_DIVERGENCE));
	y0 = mapPos.z * (1 - (FLUXMAP_POS_Z - z0) * sin(BEAM_DIVERGENCE));
}

void MRKEvents::loadFluxFileMap(TString fluxFileName)
{
	if(ranGen == nullptr)
	{
		ranGen = new TRandom3();
	}
	if(fluxMap.loadField(fluxFileName, 1))
	{
		cout << "Error finding flux map: " << fluxFileName << endl;
		return;

	}
	fluxFileLoaded = true;
}

int MRKEvents::makeEventFile(TString fileName, TString fluxFileName, int inpNumEvents, EveType evetype, double inpLittleb, bool inpHomogeneous, double inpGEMin, double inpGEMax, bool inpFermiOn, double inpZStart, double inpZEnd)
{
	if(ranGen == nullptr)
	{
		ranGen = new TRandom3();
	}
	if(evetype == FOURBODY)
		fourBody = true;
	else if(evetype == THREEBODY)
		fourBody = false;
	else
	{
		cout << "Unrecognized evetype. Exiting." << endl;
		return -1;
	}

	fermiOn = inpFermiOn;
	littleb = inpLittleb;
	zStart = inpZStart;
	zEnd = inpZEnd;
	homogeneous = inpHomogeneous;
	gEMin = inpGEMin;
	gEMax = inpGEMax;
	if(normConstant <= 0) setGoodNormConstant(evetype, littleb, gEMin);
	return makeEventFile(fileName, fluxFileName, inpNumEvents);

}

void MRKEvents::loadEventSettingsAndMakeFile(TString eventSettingsFilePath, int eventSet, TString fileName, TString fluxFileName, int inpNumEvents)
{
	loadEventSettings(eventSettingsFilePath, eventSet);
	makeEventFile(fileName, fluxFileName, inpNumEvents);
}

int MRKEvents::makeEventFile(TString fileName, TString fluxFileName, int inpNumEvents)
{
	if(ranGen == nullptr)
	{
		ranGen = new TRandom3();
	}
	numEvents = inpNumEvents;

	if(!fluxFileLoaded)
	{
		loadFluxFileMap(fluxFileName);
	}

	cout << "Making " << numEvents << " events in " << fileName << "." << endl;

	eventFile = new TFile(fileName, "RECREATE");
	eventFileName = fileName;
	int n;
	if(eventFile->IsZombie())
	{
		cout << "Error finding event file: " << eventFileName << endl;
		return -1;

	}
	eventFile->cd();

	eventTree = new TTree(DEFAULT_EVENTS_TREENAME, "Event Tree from MJB Generator");

	eventTree->Branch("n", &n, "n/I");
	eventTree->Branch("x0", &x0, "x0/D");
	eventTree->Branch("y0", &y0, "y0/D");
	eventTree->Branch("z0", &z0, "z0/D");

	eventTree->Branch("ee0", &ee0, "ee0/D");
	eventTree->Branch("ep0", &ep0, "ep0/D");
	eventTree->Branch("eg0", &eg0, "eg0/D");

	eventTree->Branch("mxe0", &mxe0, "mxe0/D");
	eventTree->Branch("mye0", &mye0, "mye0/D");
	eventTree->Branch("mze0", &mze0, "mze0/D");

	eventTree->Branch("mxp0", &mxp0, "mxp0/D");
	eventTree->Branch("myp0", &myp0, "myp0/D");
	eventTree->Branch("mzp0", &mzp0, "mzp0/D");

	eventTree->Branch("mxg0", &mxg0, "mxg0/D");
	eventTree->Branch("myg0", &myg0, "myg0/D");
	eventTree->Branch("mzg0", &mzg0, "mzg0/D");

//    thrown=0;

	for (n = 0; n < numEvents; n++)
	{
		Prob = 0;
		if(fourBody)
		{
			if(gen4BMom() >= 1)
			{
				cout << "Probability > 1 detected" << endl;
				typeAnythingToContinue("Type anything to continue");
				return -1;
			}
		}
		else
		{
			if(gen3BMom() >= 1)
			{
				cout << "Probability >1 detected" << endl;
				typeAnythingToContinue("Type anything to continue");
				return -1;
			}
		}

		if(Prob > 1)
		{
			cout << "Probability greater than 1 detected at event: " << n << endl;
			break;
		}

		genPos();

		eventTree->Fill();

		if((n + 1) % 100000 == 0)
		{
			cout << "Events created: " << n + 1 << endl;    //" Thrown: " << thrown << endl;
		}

	}

	cout << "Writing tree to disk for " << fileName << endl;
	eventTree->Write("", TObject::kOverwrite);

	delete eventFile;
	eventFile = nullptr;
	return 0;
}

int MRKEvents::makeEventFiles(TString firstFileName, TString fluxFileName, int numFiles, int numEventsPer, EveType evetype, double littleb, bool homogeneous, double gEmin, double gEmax, bool inpFermiOn, double inpZStart, double inpZEnd)
{
	fermiOn = inpFermiOn;
	zStart = inpZStart;
	zEnd = inpZEnd;
	if(ranGen == nullptr)
	{
		ranGen = new TRandom3(5211447);
	}
	if(evetype == FOURBODY)
		fourBody = true;
	else if(evetype == THREEBODY)
		fourBody = false;
	else
	{
		cout << "Unrecognized evetype. Exiting." << endl;
		return -1;
	}
	TString currentFileName = firstFileName;
	setGoodNormConstant(evetype, littleb, gEmin);
	for (int i = 0; i < numFiles; i++)
	{
		if(makeEventFile(currentFileName, fluxFileName, numEventsPer, evetype, littleb, homogeneous, gEmin, gEmax)) return -1;
		currentFileName = getNextEventFileName(currentFileName);
	}

	return 0;
}

int MRKEvents::makeDecayWidthCutMultiple(double littlebStart, double littlebIncrement, const int littlebNum)
{
	if(ranGen == nullptr)
	{
		ranGen = new TRandom3();
	}
	vector<Int_t> dwcut;
	dwcut.resize(littlebNum);
	TString lilbFileName = addBeforeExtension(eventFileName, "_lilb2");
	cout << "Making: " << lilbFileName << endl;
	//1 = cut 0 = keep
	TString branchName, leafName;

	TFile lilbFile(lilbFileName, "recreate");

	TTree* lilbTree = new TTree("lilbTree", "lilb tree");

	for (int j = 0; j < littlebNum; j++)
	{
		branchName = "dwcutb" + int2str(j);  //dwcut = decay width cut

		leafName = branchName + "/I";

		lilbTree->Branch(branchName, &dwcut[j], leafName);
	}

	MRKVector3D ePVec, pPVec, nPVec;

	Int_t nentries = 1000000;

	if(normConstant == 0)
	{
		setGoodNormConstant(THREEBODY, littleb, 0);  //We'll see if this is sufficent for little b range
	}

	for (Int_t i = 0; i < nentries; i++)
	{
		eventTree->GetEntry(i);

		//Calculate parameters normally performed by createRandomParameters()
		eE = ee0 + EMASSE;
		eP = convertKEToMomentum(ee0, EMASSE);
		pP = convertKEToMomentum(ep0, PMASSE);
		ePVec.setVal(eP * mxe0, eP * mye0, eP * mze0);
		pPVec.setVal(pP * mxp0, pP * myp0, pP * mzp0);
		nPVec = -1 * (ePVec + pPVec);
		cos_en = ePVec.dotProd(nPVec) / (ePVec.mag() * nPVec.mag());

		for (int j = 0; j < littlebNum; j++)
		{
			littleb = littlebStart + j * littlebIncrement;
			Prob = normConstant * getJTWProb();

			//Ensuring that probability requirements are met
			if(Prob > 1)
			{
				cout << "Error Prob greater than 1 (" << Prob << ") generated at event" << i << ".  Exiting" << endl;
				typeAnythingToContinue("");
				return -1;
			}

			if((Prob) > ranGen->Rndm())
				dwcut[j] = 0;
			else
				dwcut[j] = 1;
		}

		lilbTree->Fill();
	}
	lilbTree->Write();
	lilbFile.Close();
	return 0;
}

double MRKEvents::getFitNormConstant(EveType evetype, double inpGEMin)
{
	double out = 0;
	if(evetype == FOURBODY)
	{
		//out=-3.39e-10+exp(-21.8+inpGEMin*3.28e-3+inpGEMin*inpGEMin*1.08e-5);  //Fit to Fermi function data
		out = -5.51e-14 + 1.11e-12 * inpGEMin + 5.11e-15 * inpGEMin * inpGEMin;
	}
	else
	{
		out = 6e-12; //no little b variation calculated
	}
	return out;
}

void MRKEvents::setGoodNormConstant(EveType evetype, double littleb, double inpGEMin)
{
	if(littleb == 0)
	{
		normConstant = getFitNormConstant(evetype, inpGEMin);
	}
	else
	{
		normConstant = calcNormConstant(evetype, littleb, inpGEMin);
	}
}

double MRKEvents::calcNormConstant(EveType evetype, double littleb, double inpGEMin)
{
	(void) littleb; //Unused for now...
	fourBody = (evetype == FOURBODY);

	gEMin = inpGEMin;

	if(ranGen == nullptr)
	{
		ranGen = new TRandom3();
	}
	double maxProb = 0;
	Prob = 0;

	normConstant = getFitNormConstant(evetype, inpGEMin); //A starting point
	homogeneous = false;

	cout << "Determining Normalization Constant" << endl;
	for (int i = 0; i < 100000; i++)
	{

		if(fourBody)
			gen4BMom();
		else
			gen3BMom();

		//Save maximum probability found so far
		if(Prob > maxProb) maxProb = Prob;

//        if((i+1) % 1000 == 0)
//        {
//            cout << "Percentage Complete: " << (i+1)/1000 <<"%"<< endl;
//        }
	}
	normConstant = (normConstant / maxProb) * .8; //0.8 is a safety factor
	cout << "Calculated normConstant is: " << normConstant << endl;
	return normConstant;
}

void MRKEvents::runEventGen(TString runFileName)
{
	ifstream runFile;

	runFile.open(runFileName);

	TString fluxFileName, firstFileName;
	int numFiles, numEvents;
	EveType evetype;

	//Following defines format of input file
	skipCommentLines(runFile);
	runFile >> firstFileName;
	skipCommentLines(runFile);
	runFile.ignore(1024, '\n');
	skipCommentLines(runFile);
	runFile >> fluxFileName;
	runFile.ignore(1024, '\n');
	skipCommentLines(runFile);
	runFile >> numFiles;
	runFile.ignore(1024, '\n');
	skipCommentLines(runFile);
	runFile >> numEvents;
	runFile.ignore(1024, '\n');
	skipCommentLines(runFile);
	runFile >> fourBody;
	runFile.ignore(1024, '\n');
	skipCommentLines(runFile);
	runFile >> littleb;
	runFile.ignore(1024, '\n');
	skipCommentLines(runFile);
	runFile >> homogeneous;
	runFile.ignore(1024, '\n');
	skipCommentLines(runFile);
	runFile >> gEMin;
	runFile.ignore(1024, '\n');
	skipCommentLines(runFile);
	runFile >> gEMax;
	runFile.close();

	TString eveString;
	TString homoString = "";
	if(fourBody)
	{
		evetype = FOURBODY;
		eveString = "Four Body Neutron Decays in the range of " + d2str(gEMin) + " keV to " + d2str(gEMax) + " keV";
	}
	else
	{
		evetype = THREEBODY;
		eveString = "Three Body Neutron Decays";
	}

	if(homogeneous) homoString = " Homogeneous ";

	cout << "Making " << numFiles << " files each with " << numEvents << " " << homoString << endl;
	cout << eveString << " with little b at " << littleb << "." << endl;
	makeEventFiles(firstFileName, fluxFileName, numFiles, numEvents, evetype, littleb, homogeneous, gEMin, gEMax);
}

void MRKEvents::generateEvent()
{
	if(fourBody)
	{
		if(gen4BMom() >= 1)
		{
			cout << "Probability > 1 detected" << endl;
			typeAnythingToContinue("Type anything to continue");
		}
	}
	else
	{
		if(gen3BMom() >= 1)
		{
			cout << "Probability >1 detected" << endl;
			typeAnythingToContinue("Type anything to continue");
		}
	}

	if(Prob > 1)
	{
		cout << "Probability greater than 1 detected at event: " << endl;
	}

	genPos();
}

void MRKEvents::loadEventSettings(TString eventSettingsFilePath, int eventSet)
{
	if(ranGen != nullptr) delete ranGen;
	if(eventSet < 0)
	{
		cout << "Event Set must be greater than or equal to 0" << endl;
		return;
	}
	TFile ranGenFile(eventSettingsFilePath);
	TVectorD* fourBodyVector = (TVectorD*) ranGenFile.Get("fourBody");
	TVectorD* homogeneousEventsVector = (TVectorD*) ranGenFile.Get("homogeneousEvents");
	TVectorD* littlebVector = (TVectorD*) ranGenFile.Get("littleb");
	TVectorD* normalizationConstantVector = (TVectorD*) ranGenFile.Get("normalizationConstant");
	TVectorD* minimumGammaEnergyVector = (TVectorD*) ranGenFile.Get("minimumGammaEnergy");
	TVectorD* maximumGammaEnergyVector = (TVectorD*) ranGenFile.Get("maximumGammaEnergy");
	TString ranGenName = "ranGen" + int2str(eventSet);
	ranGen = (TRandom3*) ranGenFile.Get(ranGenName);
	ranGenFile.Close();

	fourBody = (*fourBodyVector)[0];
	homogeneous = (*homogeneousEventsVector)[0];
	littleb = (*littlebVector)[0];
	normConstant = (*normalizationConstantVector)[0];
	gEMin = (*minimumGammaEnergyVector)[0];
	gEMax = (*maximumGammaEnergyVector)[0];

	cout << "Is Four Body: " << fourBody << endl;
	cout << "Is Homogenous: " << homogeneous << endl;
	cout << "Little b: " << littleb << endl;
	cout << "Normalization Constant: " << normConstant << endl;
	cout << "Minimum Gamma Energy: " << gEMin << endl;
	cout << "Maximum Gamma Energy: " << gEMax << endl;
	cout << "Random Number Generator Seed: " << ranGen->GetSeed() << endl;

	delete fourBodyVector;
	delete homogeneousEventsVector;
	delete littlebVector;
	delete normalizationConstantVector;
	delete minimumGammaEnergyVector;
	delete maximumGammaEnergyVector;

}

double MRKEvents::calcBR(double inpGEMin, double inpGEMax, double inpEEMin, int numRadEvents, bool inpFermiOn, double neutronLifetime)
{
	double rate = calcFourBodyRate(inpGEMin, inpGEMax, inpEEMin, numRadEvents, inpFermiOn);
	double br = neutronLifetime * rate;
	return br;
}

double MRKEvents::calcFourBodyRate(double inpGEMin, double inpGEMax, double inpEEMin, int numRadEvents, bool inpFermiOn)
{
	gEMin = inpGEMin;
	gEMax = inpGEMax;
	eEMin = inpEEMin;
	fourBody = true;
	fermiOn = inpFermiOn;
	if(ranGen == nullptr)
	{
		ranGen = new TRandom3();
	}

	avg_integrand = 0;
	int thrown = 0;
	double rate;

	for (int i = 0; i < numRadEvents;)
	{
		if(createRandomParameters(false))
		{
			i++;
			Prob = getGapanovProb();
			avg_integrand += Prob;
		}

		thrown++;
	}
	avg_integrand /= (double) numRadEvents;
	double fraction = ((double) numRadEvents) / ((double) thrown);
	rate = FSCONST * (GVCONSTANT * GVCONSTANT) / (pow(TMath::Pi(), 4)) * (ETEMAX - EMASSE) * (gEMax - gEMin) * fraction * avg_integrand;
	rate *= 2. * TMath::Pi() / PLANKCONSTANT;
	return rate;
}

//double cEvents::calcThreeBodyRate(double inpEEMin, int numEvents)
//{
//    fourBody=false;
//    if(ranGen==NULL)
//    {
//        ranGen=new TRandom3();
//    }
//    avg_integrand=0;
//    int thrown=0;
//    double rate;
//
//    for(int i=0;i<numEvents;)
//    {
//        if(createRandomParameters(false))
//        {
//            i++;
//            Prob=getJTWProb();
//            avg_integrand+=Prob;
//        }
//
//        thrown++;
//    }
//    avg_integrand /= (double) numEvents;
//    double fraction=((double) numEvents)/((double) thrown);
//    rate=FSCONST*(GVCONSTANT*GVCONSTANT)/(pow(TMath::Pi(),4))*(ETEMAX-EMASSE)*fraction*avg_integrand;
//	rate*=2.*TMath::Pi()/PLANKCONSTANT;
//    return rate;
//}

//double cEvents::calcLifetime(double inpGEMin,double inpGEMax,double inpEEMin, int numRadEvents,bool inpFermiOn)
//{
//
//    double rate=0
//    rate+=calcThreeBodyRate( inpEEMin,  numEvents);
//    rate+=
//	double tau=1./rate;
//	return tau;
//}

TH1D* calcBRHist(int numPerBin, int numBins, double gEStart, double gEEnd, double inpEEMin, bool inpFermiOn, bool cumulative)
{

	double gEMin, gEMax, increment;

	TH1D* brHist = new TH1D("br_hist", "Branching Ratio Histogram", numBins, gEStart, gEEnd);
	brHist->GetXaxis()->SetTitle("Gamma Energy (keV)");
	brHist->GetYaxis()->SetTitle("Branching Ratio");

	MRKEvents theEvents;

	increment = (gEEnd - gEStart) / ((double) numBins);
	for (int i = 0; i < numBins; i++)
	{

		gEMin = gEStart + i * increment;
		gEMax = gEStart + (i + 1) * increment;
		if(cumulative)
		{
			gEMax = gEEnd;
		}

		cout << "Calculating BR for " << gEMin << " keV to " << gEMax << " keV with electron <" << inpEEMin << " keV" << endl;
		brHist->SetBinContent(i, theEvents.calcBR(gEMin, gEMax, inpEEMin, numPerBin, inpFermiOn, 880.0));
	}

	return brHist;
}
