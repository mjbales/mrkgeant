#include "MRKRun.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"

#include "MRKText.hh"
#include "MRKDetectorConstruction.hh"

MRKRun::MRKRun(MRKPrimaryGeneratorAction* generator)
{
	theGenerator = generator;
	fileIsOpen = false;
	BAPDHitCount = 0;
	macroFileName = "";
	geantResultsFile = nullptr;
	useFluxTree = false;

	SBDEDepTotal = SBDKEFirst = SBDTimeFirst = 0.;
	SBDPosXFirst = SBDPosYFirst = SBDPosZFirst = SBDMomXFirst = SBDMomYFirst = SBDMomZFirst = 0.;
	MomX = MomY = MomZ = PosX = PosY = PosZ = Time = EDep = KE = 0.;
	Particle = 0;
	Clocks = 0;
	HitCode = 0;
	geantResultsTree = nullptr;
	geantSBDFluxTree = nullptr;
	evtMap = nullptr;
	SBDCollectionID = 0;
	SBDHitsCollection = nullptr;
	useSBDDetector = true;
	useBAPDDetectors = true;
	useBGODetectors = true;
	eventNumber = 0;

}

MRKRun::~MRKRun()
{
	G4cout << "Beginning to delete Run...";
	if(geantResultsFile != nullptr) delete geantResultsFile;
	G4cout << "COMPLETED!" << G4endl;

}

void MRKRun::setOutputFile(string outputFileName)
{
	if(outputFileName == "")
		this->outputFileName = "mattOut.root";
	else
		this->outputFileName = outputFileName;
	G4cout << "Begin Run.  File name: " << outputFileName << G4endl;
}

void MRKRun::openOutputFile()
{
	string branchName, branchNameAndVar, treeName;

	useSBDDetector = theGenerator->getUseSBDDetector();
	useBAPDDetectors = theGenerator->getUseBAPDDetectors();
	useBGODetectors = theGenerator->getUseBGODetectors();

	G4SDManager* SDMan = G4SDManager::GetSDMpointer();
	G4String fullName;

	if(useSBDDetector)
	{
		fullName = "SBD_det/SBD_det_collection";
		SBDCollectionID = SDMan->GetCollectionID(fullName);
		G4cout << SBDCollectionID << G4endl;
		string temp;
		cin >> temp;
	}

	if(useBAPDDetectors)
	{
		for (int i = 0; i < NUM_BAPDS; i++)
		{
			fullName = "BAPD_" + int2str(i + 1) + "_det/BAPD_" + int2str(i + 1) + "_det_collection";
			BAPDCollectionID[i] = SDMan->GetCollectionID(fullName);

		}

	}
	if(useBGODetectors)
	{
		for (int i = 0; i < theGenerator->getNumBGOsUsed(); i++)
		{
			fullName = "BGO_" + int2str(i + 1) + "_det/BGO_" + int2str(i + 1) + "_det_collection";
			BGOCollectionID[i] = SDMan->GetCollectionID(fullName);
		}
	}

	geantResultsFile = new TFile(this->outputFileName.data(), "recreate");
	if(macroFileName != "")
	{
		//Add macro file to results file as record
		TMacro macroFile(macroFileName.data());
		macroFile.Write();
	}

	geantResultsTree = new TTree(DEFAULT_GEANTRESULTS_TREENAME, "Geant Generated Derived Results");
	geantResultsTree->Branch("Event", &eventNumber, "Event/I");
	if(theGenerator->getVerbose())
	{
		geantResultsTree->Branch("Clocks", &Clocks, "Clocks/I");
	}
	if(useSBDDetector)
	{
		if(useFluxTree)
		{
			geantSBDFluxTree = new TTree("SBDFluxTree", "Geant Entry and Exit Results");
			geantSBDFluxTree->Branch("Event", &eventNumber, "Event/I");
			geantSBDFluxTree->Branch("HitCode", &HitCode, "HitCode/B");
			geantSBDFluxTree->Branch("Particle", &Particle, "Particle/B");
			geantSBDFluxTree->Branch("Time", &Time, "Time/D");
			geantSBDFluxTree->Branch("PosX", &PosX, "PosX/D");
			geantSBDFluxTree->Branch("PosY", &PosY, "PosY/D");
			geantSBDFluxTree->Branch("PosZ", &PosZ, "PosZ/D");
			geantSBDFluxTree->Branch("MomX", &MomX, "MomX/D");
			geantSBDFluxTree->Branch("MomY", &MomY, "MomY/D");
			geantSBDFluxTree->Branch("MomZ", &MomZ, "MomZ/D");
			geantSBDFluxTree->Branch("KE", &KE, "KE/D");
		}

		geantResultsTree->Branch("SBDEDepTotal", &SBDEDepTotal, "SBDEDepTotal/D");
		geantResultsTree->Branch("SBDTimeFirst", &SBDTimeFirst, "SBDTimeFirst/D");
		geantResultsTree->Branch("SBDKEFirst", &SBDKEFirst, "SBDKEFirst/D");
		geantResultsTree->Branch("SBDPosXFirst", &SBDPosXFirst, "SBDPosXFirst/D");
		geantResultsTree->Branch("SBDPosYFirst", &SBDPosYFirst, "SBDPosYFirst/D");
		geantResultsTree->Branch("SBDPosZFirst", &SBDPosZFirst, "SBDPosZFirst/D");
		geantResultsTree->Branch("SBDMomXFirst", &SBDMomXFirst, "SBDMomXFirst/D");
		geantResultsTree->Branch("SBDMomYFirst", &SBDMomYFirst, "SBDMomYFirst/D");
		geantResultsTree->Branch("SBDMomZFirst", &SBDMomZFirst, "SBDMomZFirst/D");
	}

	if(useBGODetectors)
	{

		for (int i = 0; i < theGenerator->getNumBGOsUsed(); i++)
		{
			branchName = "BGO" + int2str(i + 1) + "EDepTotal";
			branchNameAndVar = branchName + "/D";
			geantResultsTree->Branch(branchName.data(), &BGOEDepTotal[i], branchNameAndVar.data());

			branchName = "BGO" + int2str(i + 1) + "KEFirst";
			branchNameAndVar = branchName + "/D";
			geantResultsTree->Branch(branchName.data(), &BGOKEFirst[i], branchNameAndVar.data());

			if(useFluxTree)
			{
				treeName = "BGOFluxTree" + int2str(i + 1);
				geantBGOFluxTree[i] = new TTree(treeName.data(), "Geant Entry and Exit Results");
				geantBGOFluxTree[i]->Branch("Event", &eventNumber, "Event/I");
				geantBGOFluxTree[i]->Branch("HitCode", &HitCode, "HitCode/B");
				geantBGOFluxTree[i]->Branch("Particle", &Particle, "Particle/B");
				geantBGOFluxTree[i]->Branch("Time", &Time, "Time/D");
				geantBGOFluxTree[i]->Branch("PosX", &PosX, "PosX/D");
				geantBGOFluxTree[i]->Branch("PosY", &PosY, "PosY/D");
				geantBGOFluxTree[i]->Branch("PosZ", &PosZ, "PosZ/D");
				geantBGOFluxTree[i]->Branch("MomX", &MomX, "MomX/D");
				geantBGOFluxTree[i]->Branch("MomY", &MomY, "MomY/D");
				geantBGOFluxTree[i]->Branch("MomZ", &MomZ, "MomZ/D");
				geantBGOFluxTree[i]->Branch("KE", &KE, "KE/D");
			}
		}

	}

	if(useBAPDDetectors)
	{

		for (int i = 0; i < NUM_BAPDS; i++)
		{
			branchName = "BAPD" + int2str(i + 1) + "EDepTotal";
			branchNameAndVar = branchName + "/D";
			geantResultsTree->Branch(branchName.data(), &BAPDEDepTotal[i], branchNameAndVar.data());

			branchName = "BAPD" + int2str(i + 1) + "KEFirst";
			branchNameAndVar = branchName + "/D";
			geantResultsTree->Branch(branchName.data(), &BAPDKEFirst[i], branchNameAndVar.data());

			if(useFluxTree)
			{
				treeName = "BAPDFluxTree" + int2str(i + 1);
				geantBAPDFluxTree[i] = new TTree(treeName.data(), "Geant Entry and Exit Results");
				geantBAPDFluxTree[i]->Branch("Event", &eventNumber, "Event/I");
				geantBAPDFluxTree[i]->Branch("HitCode", &HitCode, "HitCode/B");
				geantBAPDFluxTree[i]->Branch("Particle", &Particle, "Particle/B");
				geantBAPDFluxTree[i]->Branch("Time", &Time, "Time/D");
				geantBAPDFluxTree[i]->Branch("PosX", &PosX, "PosX/D");
				geantBAPDFluxTree[i]->Branch("PosY", &PosY, "PosY/D");
				geantBAPDFluxTree[i]->Branch("PosZ", &PosZ, "PosZ/D");
				geantBAPDFluxTree[i]->Branch("MomX", &MomX, "MomX/D");
				geantBAPDFluxTree[i]->Branch("MomY", &MomY, "MomY/D");
				geantBAPDFluxTree[i]->Branch("MomZ", &MomZ, "MomZ/D");
				geantBAPDFluxTree[i]->Branch("KE", &KE, "KE/D");
			}

		}
	}

	fileIsOpen = true;

}

void MRKRun::closeAndWrite()
{
	G4cout << "Writing and closing file: " << outputFileName << G4endl;
	geantResultsFile->Write();
	geantResultsFile->Close();
	fileIsOpen=false;

	delete geantResultsFile;
	geantResultsFile=nullptr;
	BAPDHitCount=0;
}

void MRKRun::RecordEvent(const G4Event* evt)
{
	SBDKEFirst = 0;
	if(!fileIsOpen) openOutputFile();
	G4HCofThisEvent* HCE = evt->GetHCofThisEvent();

	if(!HCE) return;

	SBDEDepTotal = SBDMomZFirst = SBDMomYFirst = SBDMomXFirst = SBDPosXFirst = SBDPosYFirst = SBDPosZFirst = SBDTimeFirst = 0;

	int numEntries;
	MRKHit* theHit;
	SBDHitsCollection = 0;

	if(useSBDDetector)
	{
		SBDHitsCollection = (MRKHitsCollection*) HCE->GetHC(SBDCollectionID);

		if(SBDHitsCollection)
		{

			numEntries = SBDHitsCollection->entries();

			eventNumber = theGenerator->getCurrentEventNumber();
			for (int i = 0; i < numEntries; i++)
			{

				theHit = (MRKHit*) SBDHitsCollection->GetHit(i);
				HitCode = theHit->GetHitCode();

				//Does this belong in the flux tree
				if(HitCode != MRKHITCODE_NOTHING && HitCode != MRKHITCODE_DETEDEP)
				{
					HitCode = theHit->GetHitCode();
					Particle = theHit->GetParType();
					Time = theHit->GetTime() / second;
					PosX = theHit->GetPos().getX() / meter;
					PosY = theHit->GetPos().getY() / meter;
					PosZ = theHit->GetPos().getZ() / meter;
					MomX = theHit->GetMom().getX() / keV;
					MomY = theHit->GetMom().getY() / keV;
					MomZ = theHit->GetMom().getZ() / keV;
					KE = theHit->GetKE() / keV;
					if(useFluxTree) geantSBDFluxTree->Fill();

					if(SBDTimeFirst == 0)
					{

						SBDTimeFirst = Time;
						SBDPosXFirst = PosX;
						SBDPosYFirst = PosY;
						SBDPosZFirst = PosZ;
						SBDMomXFirst = MomX;
						SBDMomYFirst = MomY;
						SBDMomZFirst = MomZ;
						SBDKEFirst = KE;
					}
				}
				//Or add for EDeposit tree
				else if(HitCode == MRKHITCODE_DETEDEP)
				{
					EDep = theHit->GetEDep() / keV;
					SBDEDepTotal += EDep;
				}

			}
		}
	}

	bool BAPDEnergyDeposited = false;
	if(useBAPDDetectors)
	{
		double earliestBAPDFluxEventTime = 1e55; //really large time
		for (int j = 0; j < NUM_BAPDS; j++)
		{
			BAPDKEFirst[j] = 0;

			BAPDEDepTotal[j] = 0;
			BAPDHitsCollection[j] = 0;
			BAPDHitsCollection[j] = (MRKHitsCollection*) HCE->GetHC(BAPDCollectionID[j]);

			if(BAPDHitsCollection[j])
			{
				numEntries = (BAPDHitsCollection[j])->entries();
				eventNumber = theGenerator->getCurrentEventNumber();
				for (int i = 0; i < numEntries; i++)
				{
					theHit = (MRKHit*) BAPDHitsCollection[j]->GetHit(i);
					HitCode = theHit->GetHitCode();

					//Does this belong in the flux tree
					if((HitCode != MRKHITCODE_NOTHING && HitCode != MRKHITCODE_DETEDEP))
					{

						KE = theHit->GetKE() / keV;
						Time = theHit->GetTime() / second;

						if(Time < earliestBAPDFluxEventTime && (HitCode == MRKHITCODE_DETIN || HitCode == MRKHITCODE_SIO2REJECT))
						{
							earliestBAPDFluxEventTime = Time;
							BAPDKEFirst[j] = KE;
						}
						if(useFluxTree)
						{
							Particle = theHit->GetParType();
							Time = theHit->GetTime() / second;
							PosX = theHit->GetPos().getX() / meter;
							PosY = theHit->GetPos().getY() / meter;
							PosZ = theHit->GetPos().getZ() / meter;
							MomX = theHit->GetMom().getX() / keV;
							MomY = theHit->GetMom().getY() / keV;
							MomZ = theHit->GetMom().getZ() / keV;
							geantBAPDFluxTree[j]->Fill();
						}

					}
					//Or add for EDeposit tree
					else if(HitCode == MRKHITCODE_DETEDEP)
					{
						EDep = theHit->GetEDep() / keV;
						BAPDEDepTotal[j] += EDep;
						BAPDEnergyDeposited = true;

					}

				}
			}
		}
	}

	bool BGOEnergyDeposited = false;
	if(useBGODetectors)
	{
		double earliestBGOFluxEventTime = 1e55; //really large time

		for (int j = 0; j < theGenerator->getNumBGOsUsed(); j++)
		{

			BGOKEFirst[j] = 0;
			BGOEDepTotal[j] = 0;
			BGOHitsCollection[j] = 0;
			BGOHitsCollection[j] = (MRKHitsCollection*) HCE->GetHC(BGOCollectionID[j]);

			if(BGOHitsCollection[j])
			{
				numEntries = (BGOHitsCollection[j])->entries();
				eventNumber = theGenerator->getCurrentEventNumber();
				for (int i = 0; i < numEntries; i++)
				{
					theHit = (MRKHit*) BGOHitsCollection[j]->GetHit(i);
					HitCode = theHit->GetHitCode();

					//Does this belong in the flux tree
					if(HitCode != MRKHITCODE_NOTHING && HitCode != MRKHITCODE_DETEDEP)
					{

						KE = theHit->GetKE() / keV;
						Time = theHit->GetTime() / second;
						if(Time < earliestBGOFluxEventTime && HitCode == MRKHITCODE_DETIN)
						{
							earliestBGOFluxEventTime = Time;
							BGOKEFirst[j] = KE;
						}

						if(useFluxTree)
						{
							HitCode = theHit->GetHitCode();
							Particle = theHit->GetParType();

							PosX = theHit->GetPos().getX() / meter;
							PosY = theHit->GetPos().getY() / meter;
							PosZ = theHit->GetPos().getZ() / meter;
							MomX = theHit->GetMom().getX() / keV;
							MomY = theHit->GetMom().getY() / keV;
							MomZ = theHit->GetMom().getZ() / keV;
							geantBGOFluxTree[j]->Fill();
						}

					}
					//Or add for EDeposit tree
					else if(HitCode == MRKHITCODE_DETEDEP)
					{
						//   G4cout << "EDep hit!" << G4endl;
						EDep = theHit->GetEDep() / keV;
						BGOEDepTotal[j] += EDep;
						BGOEnergyDeposited = true;

					}

				}
			}
		}
	}

	for (int i = 0; i < 3; i++)
	{
		if(BAPDEDepTotal[i] > .3) BAPDHitCount++;
	}

	if(theGenerator->getVerbose())
	{
		int endEventClock = clock();
		Clocks = endEventClock - theGenerator->getEventStartClock();
	}

	geantResultsTree->Fill();

	if(theGenerator->getVerbose())
	{
		G4cout << "------------------Calculated in " << Clocks << " Clocks------------------------" << G4endl;
		if(useSBDDetector)
		{
			if(SBDTimeFirst==0)
			G4cout << "RDK Event Number: " << eventNumber << " missed the SBD" << G4endl;
			else
			{
				G4cout << "RDK Event Number: " << eventNumber << " hit the SBD @ ("<< SBDPosXFirst <<","<< SBDPosYFirst <<","<< SBDPosZFirst <<" m) after "<< SBDTimeFirst<<" s" << G4endl;
				G4cout << "     with a KE of " << SBDKEFirst << " in the direction ("<< SBDMomXFirst << ", "<< SBDMomXFirst << ", "<< SBDMomXFirst <<") and deposited " << SBDEDepTotal << " keV." << G4endl;
			}
		}
		if(useBAPDDetectors)
		{

			if(BAPDEnergyDeposited)
			{
				for(int i=0;i<NUM_BAPDS;i++)
				{
					G4cout << "RDK Event Number: " << eventNumber <<"  BAPD " << i+1 << " Energy Deposited: " << BAPDEDepTotal[i] <<" keV "<<G4endl;
				}
			}
			else
			G4cout << "RDK Event Number: " << eventNumber << " missed all BAPDs" << G4endl;

		}
		if(useBGODetectors)
		{
			if(BGOEnergyDeposited)
			{
				for(int i=0;i<theGenerator->getNumBGOsUsed();i++)
				{
					G4cout << "RDK Event Number: " << eventNumber <<"   BGO " << i+1 << " Energy Deposited: " << BGOEDepTotal[i] <<" keV "<<G4endl;
				}
			}
			else
			G4cout << "RDK Event Number: " << eventNumber << " missed all BGOs" << G4endl;
		}

	}

	theGenerator->setCurrentEventNumber(eventNumber + 1);

}

G4double MRKRun::GetTotal(const G4THitsMap<G4double> &map) const
{
	G4double tot = 0.;
	std::map<G4int, G4double*>::iterator itr = map.GetMap()->begin();
	for (; itr != map.GetMap()->end(); itr++)
	{
		tot += *(itr->second);
	}
	return tot;
}

/*
 void MRKRun::setupDetector(string detName)
 {
 string branchName,branchNameAndVar,treeName;

 G4SDManager* SDMan = G4SDManager::GetSDMpointer();
 G4String fullName=detName+"/"+detName+"_collection";

 G4int collectionID = SDMan->GetCollectionID(fullName.data());


 string fluxTreeName=detName+"FluxTree";
 if(useFluxTree)
 {
 geantSBDFluxTree= new TTree(fluxTreeName.data(),"Geant Entry and Exit Results");
 geantSBDFluxTree->Branch("Event",&eventNumber,"Event/I");
 geantSBDFluxTree->Branch("HitCode",&HitCode,"HitCode/B");
 geantSBDFluxTree->Branch("Particle",&Particle,"Particle/B");
 geantSBDFluxTree->Branch("Time",&Time,"Time/D");
 geantSBDFluxTree->Branch("PosX",&PosX,"PosX/D");
 geantSBDFluxTree->Branch("PosY",&PosY,"PosY/D");
 geantSBDFluxTree->Branch("PosZ",&PosZ,"PosZ/D");
 geantSBDFluxTree->Branch("MomX",&MomX,"MomX/D");
 geantSBDFluxTree->Branch("MomY",&MomY,"MomY/D");
 geantSBDFluxTree->Branch("MomZ",&MomZ,"MomZ/D");
 geantSBDFluxTree->Branch("KE",&KE,"KE/D");
 }

 geantResultsTree->Branch("SBDEDepTotal",&SBDEDepTotal,"SBDEDepTotal/D");
 geantResultsTree->Branch("SBDTimeFirst",&SBDTimeFirst,"SBDTimeFirst/D");
 geantResultsTree->Branch("SBDKEFirst",&SBDKEFirst,"SBDKEFirst/D");
 geantResultsTree->Branch("SBDPosXFirst",&SBDPosXFirst,"SBDPosXFirst/D");
 geantResultsTree->Branch("SBDPosYFirst",&SBDPosYFirst,"SBDPosYFirst/D");
 geantResultsTree->Branch("SBDPosZFirst",&SBDPosZFirst,"SBDPosZFirst/D");
 geantResultsTree->Branch("SBDMomXFirst",&SBDMomXFirst,"SBDMomXFirst/D");
 geantResultsTree->Branch("SBDMomYFirst",&SBDMomYFirst,"SBDMomYFirst/D");
 geantResultsTree->Branch("SBDMomZFirst",&SBDMomZFirst,"SBDMomZFirst/D");



 if(useBGODetectors)
 {
 for(int i=0;i<theGenerator->getNumBGOsUsed();i++)
 {
 branchName="BGO"+int2str(i+1)+"EDepTotal";
 branchNameAndVar=branchName+"/D";
 geantResultsTree->Branch(branchName.data(),&BGOEDepTotal[i],branchNameAndVar.data());

 branchName="BGO"+int2str(i+1)+"KEFirst";
 branchNameAndVar=branchName+"/D";
 geantResultsTree->Branch(branchName.data(),&BGOKEFirst[i],branchNameAndVar.data());

 if(useFluxTree)
 {
 treeName="BGOFluxTree"+int2str(i+1);
 geantBGOFluxTree[i]= new TTree(treeName.data(),"Geant Entry and Exit Results");
 geantBGOFluxTree[i]->Branch("Event",&eventNumber,"Event/I");
 geantBGOFluxTree[i]->Branch("HitCode",&HitCode,"HitCode/B");
 geantBGOFluxTree[i]->Branch("Particle",&Particle,"Particle/B");
 geantBGOFluxTree[i]->Branch("Time",&Time,"Time/D");
 geantBGOFluxTree[i]->Branch("PosX",&PosX,"PosX/D");
 geantBGOFluxTree[i]->Branch("PosY",&PosY,"PosY/D");
 geantBGOFluxTree[i]->Branch("PosZ",&PosZ,"PosZ/D");
 geantBGOFluxTree[i]->Branch("MomX",&MomX,"MomX/D");
 geantBGOFluxTree[i]->Branch("MomY",&MomY,"MomY/D");
 geantBGOFluxTree[i]->Branch("MomZ",&MomZ,"MomZ/D");
 geantBGOFluxTree[i]->Branch("KE",&KE,"KE/D");
 }
 }

 }


 if(useBAPDDetectors)
 {

 for(int i=0;i<NUM_BAPDS;i++)
 {
 branchName="BAPD"+int2str(i+1)+"EDepTotal";
 branchNameAndVar=branchName+"/D";
 geantResultsTree->Branch(branchName.data(),&BAPDEDepTotal[i],branchNameAndVar.data());

 branchName="BAPD"+int2str(i+1)+"KEFirst";
 branchNameAndVar=branchName+"/D";
 geantResultsTree->Branch(branchName.data(),&BAPDKEFirst[i],branchNameAndVar.data());

 if(useFluxTree)
 {
 treeName="BAPDFluxTree"+int2str(i+1);
 geantBAPDFluxTree[i]= new TTree(treeName.data(),"Geant Entry and Exit Results");
 geantBAPDFluxTree[i]->Branch("Event",&eventNumber,"Event/I");
 geantBAPDFluxTree[i]->Branch("HitCode",&HitCode,"HitCode/B");
 geantBAPDFluxTree[i]->Branch("Particle",&Particle,"Particle/B");
 geantBAPDFluxTree[i]->Branch("Time",&Time,"Time/D");
 geantBAPDFluxTree[i]->Branch("PosX",&PosX,"PosX/D");
 geantBAPDFluxTree[i]->Branch("PosY",&PosY,"PosY/D");
 geantBAPDFluxTree[i]->Branch("PosZ",&PosZ,"PosZ/D");
 geantBAPDFluxTree[i]->Branch("MomX",&MomX,"MomX/D");
 geantBAPDFluxTree[i]->Branch("MomY",&MomY,"MomY/D");
 geantBAPDFluxTree[i]->Branch("MomZ",&MomZ,"MomZ/D");
 geantBAPDFluxTree[i]->Branch("KE",&KE,"KE/D");
 }

 }
 }

 fileIsOpen=true;
 }
 */
