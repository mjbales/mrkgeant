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

#include "MRKPhysicsList.hh"
#include "MRKPhysicsListMessenger.hh"
#include "MRKStepMax.hh"

using namespace CLHEP;

MRKPhysicsList::MRKPhysicsList(MRKDetectorConstruction* p) :
	G4VModularPhysicsList()
{
	G4LossTableManager::Instance();
	pDet = p;

	currentDefaultCut = 1 * nm;
	cutForGamma = currentDefaultCut;
	cutForElectron = currentDefaultCut;
	cutForPositron = currentDefaultCut;
	cutForProton = currentDefaultCut;

	pMessenger = new MRKPhysicsListMessenger(this);

	SetVerboseLevel(1);

	// EM physics
	emName = G4String("emlivermore");
	emPhysicsList = new G4EmLivermorePhysics();

	// G4EmParameters* param = G4EmParameters::Instance();
	// param->SetMinEnergy(100*eV);

	// Decay physics and all particles
	decPhysicsList = new G4DecayPhysics();

	raddecayList = new G4RadioactiveDecayPhysics();

	stepMaxProcess = nullptr;

}

MRKPhysicsList::~MRKPhysicsList()
{
	delete pMessenger;
	delete emPhysicsList;
	delete decPhysicsList;
	delete raddecayList;
}

#include "G4EmProcessOptions.hh"
#include "G4Decay.hh"
#include "G4ProcessManager.hh"

void MRKPhysicsList::ConstructProcess()
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

	// G4EmParameters* param = G4EmParameters::Instance();
	// param->SetMinEnergy(100*eV);

	//Add electron max flight time
	// G4ProcessManager* pmanager = G4Electron::ElectronDefinition()->GetProcessManager();
	//pmanager->AddProcess(new G4UserSpecialCuts(),-1,-1,1);
}

void MRKPhysicsList::ConstructParticle()
{
	decPhysicsList->ConstructParticle();
}

void MRKPhysicsList::AddPhysicsList(const G4String& name)
{

	// if (verboseLevel>1) {
	G4cout << "PhysicsList::AddPhysicsList: <" << name << ">" << G4endl;
	// }

	if (name == emName) return;

	G4double lowLimit = 250*eV;
	G4double highLimit = 1*MeV;

	if (name == "emstandard_opt0")
	{

		emName = name;
		delete emPhysicsList;
		emPhysicsList = new G4EmStandardPhysics();
		G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit, highLimit);

	}
	else if (name == "emstandard_opt1")
	{

		emName = name;
		delete emPhysicsList;
		emPhysicsList = new G4EmStandardPhysics_option1();
		G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit, highLimit);

	}
	else if (name == "emstandard_opt2")
	{

		emName = name;
		delete emPhysicsList;
		emPhysicsList = new G4EmStandardPhysics_option2();
		G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit, highLimit);

	}
	else if (name == "emstandard_opt3")
	{

		emName = name;
		delete emPhysicsList;
		emPhysicsList = new G4EmStandardPhysics_option3();
		G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit, highLimit);

	}
	else if (name == "emstandard_opt4")
	{

		emName = name;
		delete emPhysicsList;
		emPhysicsList = new G4EmStandardPhysics_option4();
		G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit, highLimit);
	}

	else if (name == "emlivermore")
	{
		emName = name;
		delete emPhysicsList;
		emPhysicsList = new G4EmLivermorePhysics();
		G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit, highLimit);

	}
	else if (name == "empenelope")
	{
		emName = name;
		delete emPhysicsList;
		emPhysicsList = new G4EmPenelopePhysics();
		G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit, highLimit);

	}
	else
	{

		G4cout << "PhysicsList::AddPhysicsList: <" << name << ">"
		<< " is not defined"
		<< G4endl;
	}

}

#include "G4Decay.hh"

void MRKPhysicsList::AddDecay()
{
	// Decay Process
	//
	G4Decay* fDecayProcess = new G4Decay();

	theParticleIterator->reset();
	while ((*theParticleIterator)())
	{
		G4ParticleDefinition* particle = theParticleIterator->value();
		G4ProcessManager* pmanager = particle->GetProcessManager();

		if(fDecayProcess->IsApplicable(*particle))
		{

			pmanager->AddProcess(fDecayProcess);

			// set ordering for PostStepDoIt and AtRestDoIt
			pmanager->SetProcessOrdering(fDecayProcess, idxPostStep);
			pmanager->SetProcessOrdering(fDecayProcess, idxAtRest);

		}
	}
}

void MRKPhysicsList::AddStepMax()
{
	// Step limitation seen as a process
	stepMaxProcess = new MRKStepMax();

	theParticleIterator->reset();
	while ((*theParticleIterator)())
	{
		G4ParticleDefinition* particle = theParticleIterator->value();
		G4ProcessManager* pmanager = particle->GetProcessManager();

		if(stepMaxProcess->IsApplicable(*particle) && pmanager)
		{
			pmanager->AddDiscreteProcess(stepMaxProcess);
		}
	}
}

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"

void MRKPhysicsList::SetCuts()
{
	// set cut values for gamma at first and for e- second and next for e+,
	// because some processes for e+/e- need cut values for gamma
	G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(250 * eV, 1 * GeV);
	SetCutValue(cutForGamma, "gamma");
	SetCutValue(cutForElectron, "e-");
	SetCutValue(cutForPositron, "e+");
	SetCutValue(cutForProton, "proton");

	if(verboseLevel > 0)
	{
		G4cout << "PhysicsList::SetCuts:";
	}

	if(verboseLevel > 0) DumpCutValuesTable();
}

void MRKPhysicsList::SetCutForGamma(G4double cut)
{
	cutForGamma = cut;
	SetParticleCuts(cutForGamma, G4Gamma::Gamma());
}

void MRKPhysicsList::SetCutForProton(G4double cut)
{
	cutForProton = cut;
	SetParticleCuts(cutForProton, G4Proton::Proton());
}

void MRKPhysicsList::SetCutForElectron(G4double cut)
{
	cutForElectron = cut;
	SetParticleCuts(cutForElectron, G4Electron::Electron());
}

void MRKPhysicsList::SetCutForPositron(G4double cut)
{
	cutForPositron = cut;
	SetParticleCuts(cutForPositron, G4Positron::Positron());
}

