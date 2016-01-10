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
// $Id: PhysicsList.cc,v 1.12 2009/09/15 12:51:49 maire Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PhysicsList.hh"
#include "PhysicsListMessenger.hh"

#include "PhysListEmStandard.hh"
#include "PhysListEmStandardSS.hh"
#include "PhysListEmStandardSSM.hh"
#include "PhysListEmStandardGS.hh"
#include "PhysListEmStandardWVI.hh"

#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"

#include "G4EmStandardPhysics_option4.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4UserSpecialCuts.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"



#include "MRKDetectorConstruction.hh"

#include "G4LossTableManager.hh"
#include "G4UnitsTable.hh"
#include "StepMax.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::PhysicsList(MRKDetectorConstruction* p)
: G4VModularPhysicsList()
{
  G4LossTableManager::Instance();
  pDet = p;

  currentDefaultCut   = 1*nm;
  cutForGamma         = currentDefaultCut;
  cutForElectron      = currentDefaultCut;
  cutForPositron      = currentDefaultCut;
  cutForProton        = currentDefaultCut;

  pMessenger = new PhysicsListMessenger(this);

  SetVerboseLevel(1);

  // EM physics
  emName = G4String("emlivermore");
  emPhysicsList = new G4EmLivermorePhysics();

  // Decay physics and all particles
  decPhysicsList = new G4DecayPhysics();

  raddecayList = new G4RadioactiveDecayPhysics();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::~PhysicsList()
{
  delete pMessenger;
  delete emPhysicsList;
  delete decPhysicsList;
  delete raddecayList;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4EmProcessOptions.hh"
#include "G4Decay.hh"
#include "G4ProcessManager.hh"

void PhysicsList::ConstructProcess()
{
  // Transportation
  //
  AddTransportation();

  // Electromagnetic physics list
  //
  emPhysicsList->ConstructProcess();

  // Em options
  //
//  G4EmProcessOptions emOptions;
//
//  emOptions.SetBuildCSDARange(true);
//  emOptions.SetDEDXBinningForCSDARange(8*20);
//  emOptions.SetFluo(true); // To activate deexcitation processes and fluorescence
//  emOptions.SetAuger(true); // To activate Auger effect if deexcitation is activated
//  emOptions.SetPIXE(true); // To activate Particle Induced X-Ray Emission (PIXE)

 // decay physics list
  //
  decPhysicsList->ConstructProcess();

  //Radioactive Decay
  raddecayList->ConstructProcess();

  // step limitation (as a full process)
  //
  AddStepMax();


  //Add electron max flight time
 // G4ProcessManager* pmanager = G4Electron::ElectronDefinition()->GetProcessManager();
  //pmanager->AddProcess(new G4UserSpecialCuts(),-1,-1,1);
}

void PhysicsList::ConstructParticle()
{
    decPhysicsList->ConstructParticle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::AddPhysicsList(const G4String& name)
{
 // if (verboseLevel>1) {
    G4cout << "PhysicsList::AddPhysicsList: <" << name << ">" << G4endl;
 // }

  if (name == emName) return;


  G4double lowLimit = 250*eV;
  G4double highLimit = 1*MeV;

  if (name == "local") {

    emName = name;
    delete emPhysicsList;
    emPhysicsList = new PhysListEmStandard(name);
    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit, highLimit);

  } else if (name == "emstandard_opt0") {

    emName = name;
    delete emPhysicsList;
    emPhysicsList = new G4EmStandardPhysics();
    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit, highLimit);

  } else if (name == "emstandard_opt1") {

    emName = name;
    delete emPhysicsList;
    emPhysicsList = new G4EmStandardPhysics_option1();
    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit, highLimit);

  } else if (name == "emstandard_opt2") {

    emName = name;
    delete emPhysicsList;
    emPhysicsList = new G4EmStandardPhysics_option2();
    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit, highLimit);


  } else if (name == "emstandard_opt3") {

    emName = name;
    delete emPhysicsList;
    emPhysicsList = new G4EmStandardPhysics_option3();
    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit, highLimit);

    }
    else if (name == "emstandard_opt4") {

        emName = name;
        delete emPhysicsList;
        emPhysicsList = new G4EmStandardPhysics_option4();
        G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit, highLimit);
    }
    else if (name == "standardSS") {

    emName = name;
    delete emPhysicsList;
    emPhysicsList = new PhysListEmStandardSS(name);
    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit, highLimit);

    } else if (name == "standardSSM") {

    emName = name;
    delete emPhysicsList;
    emPhysicsList = new PhysListEmStandardSSM(name);
    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit, highLimit);

  } else if (name == "standardWVI") {

    emName = name;
    delete emPhysicsList;
    emPhysicsList = new PhysListEmStandardWVI(name);
    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit, highLimit);

  } else if (name == "standardGS") {

    emName = name;
    delete emPhysicsList;
    emPhysicsList = new PhysListEmStandardGS(name);
    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit, highLimit);

  } else if (name == "emlivermore") {
    emName = name;
    delete emPhysicsList;
    emPhysicsList = new G4EmLivermorePhysics();
    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit, highLimit);

  } else if (name == "empenelope") {
    emName = name;
    delete emPhysicsList;
    emPhysicsList = new G4EmPenelopePhysics();
    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit, highLimit);



  } else {

    G4cout << "PhysicsList::AddPhysicsList: <" << name << ">"
           << " is not defined"
           << G4endl;
  }


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4Decay.hh"

void PhysicsList::AddDecay()
{
  // Decay Process
  //
  G4Decay* fDecayProcess = new G4Decay();

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();

    if (fDecayProcess->IsApplicable(*particle)) {

      pmanager ->AddProcess(fDecayProcess);

      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(fDecayProcess, idxPostStep);
      pmanager ->SetProcessOrdering(fDecayProcess, idxAtRest);

    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



void PhysicsList::AddStepMax()
{
  // Step limitation seen as a process
  stepMaxProcess = new StepMax();

  theParticleIterator->reset();
  while ((*theParticleIterator)()){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();

    if (stepMaxProcess->IsApplicable(*particle) && pmanager)
      {
	pmanager ->AddDiscreteProcess(stepMaxProcess);
      }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"

void PhysicsList::SetCuts()
{
  // set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma
  G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(250*eV, 1*GeV);
  SetCutValue(cutForGamma, "gamma");
  SetCutValue(cutForElectron, "e-");
  SetCutValue(cutForPositron, "e+");
  SetCutValue(cutForProton,"proton");

  if (verboseLevel >0){
    G4cout << "PhysicsList::SetCuts:";
  }

  if (verboseLevel>0) DumpCutValuesTable();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::SetCutForGamma(G4double cut)
{
  cutForGamma = cut;
  SetParticleCuts(cutForGamma, G4Gamma::Gamma());
}

void PhysicsList::SetCutForProton(G4double cut)
{
  cutForProton = cut;
  SetParticleCuts(cutForProton, G4Proton::Proton());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::SetCutForElectron(G4double cut)
{
  cutForElectron = cut;
  SetParticleCuts(cutForElectron, G4Electron::Electron());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::SetCutForPositron(G4double cut)
{
  cutForPositron = cut;
  SetParticleCuts(cutForPositron, G4Positron::Positron());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


