#ifndef MRKDetectorConstruction_h
#define MRKDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"

#include "MRKConstants.hh"
#include "MRKMacroMessenger.hh"
#include "MRKGlobalField.hh"
#include "MRKGeometry.hh"

enum ExperimentModel
{
	MODEL_RDK2
};

class MRKDetectorConstruction: public G4VUserDetectorConstruction
{
public:
	MRKDetectorConstruction(MRKMacroMessenger* inpMacroMessenger);
	~MRKDetectorConstruction();

public:

	G4VPhysicalVolume* Construct();

	void defineMacroCommands(MRKMacroMessenger* inpMacroMessenger);

	void setExperimentModel(string modelName);

	inline const G4VPhysicalVolume* GetWorld()      {return physiWorld;};
	inline ExperimentModel getExperimentModel(){ return experimentModel;}
	inline void setExperimentModel(G4String inpModelString);
	inline void setExperimentModel(ExperimentModel inpModel){ G4cout << "Setting experimental model for detector construction." << G4endl;experimentModel=inpModel;}

	inline bool getUseSBDDetector(){return theGeometry->getUseSBDDetector();};
	inline bool getUseBAPDDetectors(){return theGeometry->getUseBAPDDetectors();};
	inline bool getUseBGODetectors(){return theGeometry->getUseBGODetectors();};
	inline int getNumBGOsUsed(){return theGeometry->getNumBGOsUsed();}

private:
	G4VPhysicalVolume* physiWorld;    // pointer to the physical envelope

	MRKGlobalField* theGlobalField;  //The electric/magnetic field

	MRKMacroMessenger* theMacroMessenger; //The macro messenger

	MRKGeometry* theGeometry; //Geometry - Owned by this

	ExperimentModel experimentModel;

};

#endif
