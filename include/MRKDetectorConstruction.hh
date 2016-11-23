#ifndef MRKDetectorConstruction_h
#define MRKDetectorConstruction_h 1

#include <string>

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"

#include "MRKConstants.hh"
#include "MRKMacroMessenger.hh"
#include "MRKGlobalField.hh"
#include "MRKGeometry.hh"

enum class MRKExperimentModel
{
	MODEL_RDK2, MODEL_SBDONLY
};

//Detector construction class, allows for selection of different RDK models
class MRKDetectorConstruction: public G4VUserDetectorConstruction
{
public:
	MRKDetectorConstruction(MRKMacroMessenger* inpMacroMessenger);
	~MRKDetectorConstruction();

public:

	G4VPhysicalVolume* Construct();

	/// Defines the macro commands related to detector construction
	void defineMacroCommands(MRKMacroMessenger* inpMacroMessenger);

	/// Sets which experimental model to use
	void setExperimentModel(std::string modelName);

	/// Sets which experimental model to use
	void setExperimentModel(G4String inpModelString);

	inline const G4VPhysicalVolume* GetWorld()      {return physiWorld;};
	inline MRKExperimentModel getExperimentModel(){ return experimentModel;}


	inline bool getUseSBDDetector(){return theGeometry->getUseSBDDetector();};
	inline bool getUseBAPDDetectors(){return theGeometry->getUseBAPDDetectors();};
	inline bool getUseBGODetectors(){return theGeometry->getUseBGODetectors();};
	inline int getNumBGOsUsed(){return theGeometry->getNumBGOsUsed();}

private:
	G4VPhysicalVolume* physiWorld;    // pointer to the physical envelope

	MRKGlobalField* theGlobalField;  //The electric/magnetic field

	MRKMacroMessenger* theMacroMessenger; //The macro messenger

	MRKGeometry* theGeometry; //Geometry - Owned by this

	MRKExperimentModel experimentModel;

};

#endif
