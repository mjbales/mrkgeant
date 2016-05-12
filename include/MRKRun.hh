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
// $Id: ExN07Run.hh,v 1.6 2007/05/04 01:49:28 asaim Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//

#ifndef MRKRun_h
#define MRKRun_h 1

#include "globals.hh"
#include "G4Run.hh"

#include "G4THitsMap.hh"
#include "MRKPrimaryGeneratorAction.hh"

#include "TTree.h"
#include "TFile.h"
#include "TMacro.h"
#include "MRKHit.hh"
#include "G4UnitsTable.hh"

#include <string>
#include <time.h>

#include "MRKConstants.hh"
using namespace std;
using std::string;

class G4Event;
class G4HCofThisEvent;

class MRKRun : public G4Run
{
  public:
    MRKRun(MRKPrimaryGeneratorAction* generator);
    virtual ~MRKRun();
    virtual void RecordEvent(const G4Event*);
	void setOutputFile(string outputFileName);
	void closeAndWrite();
	void openOutputFile();
	inline int getBAPDHitCount() const
	{ return BAPDHitCount;}
	inline bool getFileIsOpen() const
    { return fileIsOpen; }
    inline void setMacroFileName(string inp){macroFileName=inp;}
    inline void setUseFluxTree(bool inp){useFluxTree=inp;}

  private:
    bool fileIsOpen;
    G4double GetTotal(const G4THitsMap<G4double> &map) const;
    G4int SBDCollectionID,BGOCollectionID[NUM_BGOS],BAPDCollectionID[NUM_BAPDS];
    Double_t SBDEDepTotal,SBDKEFirst,BGOEDepTotal[NUM_BGOS],BAPDEDepTotal[NUM_BAPDS], SBDTimeFirst,BAPDKEFirst[NUM_BAPDS],BGOKEFirst[NUM_BGOS];
    Double_t SBDPosXFirst,SBDPosYFirst,SBDPosZFirst,SBDMomXFirst,SBDMomYFirst,SBDMomZFirst;
    Double_t MomX,MomY,MomZ,PosX,PosY,PosZ,Time,EDep,KE;
    Int_t Clocks;
    Int_t eventNumber;
    Char_t Particle,HitCode;
	string outputFileName;
	TFile* geantResultsFile;
	TTree* geantResultsTree;
	TTree* geantSBDFluxTree;
	TTree* geantBGOFluxTree[NUM_BGOS];
	TTree* geantBAPDFluxTree[NUM_BAPDS];
	MRKPrimaryGeneratorAction* theGenerator;

	G4THitsMap<G4double>* evtMap;
	MRKHitsCollection* SBDHitsCollection;
	MRKHitsCollection* BGOHitsCollection[NUM_BGOS];
	MRKHitsCollection* BAPDHitsCollection[NUM_BAPDS];
	G4HCofThisEvent* HCE;
	G4String currentParticleName;
	int BAPDHitCount;
	string macroFileName;

	bool useSBDDetector,useBGODetectors,useBAPDDetectors;
	bool useFluxTree;




};

#endif

