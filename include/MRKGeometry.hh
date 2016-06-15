#ifndef SRC_MRKGEOMETRY_HH_
#define SRC_MRKGEOMETRY_HH_

#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"

#include "MRKMacroMessenger.hh"
#include "MRKGlobalField.hh"
#include "MRKMaterials.hh"

class MRKGeometry
{
public:
	MRKGeometry(MRKMacroMessenger*){};
	virtual G4VPhysicalVolume* Construct()=0;
	inline G4LogicalVolume* getWorldLogicalVolume(){return logicWorld;}
	virtual ~MRKGeometry(){};

	virtual bool getUseSBDDetector()=0;
	virtual bool getUseBAPDDetectors()=0;
	virtual bool getUseBGODetectors()=0;
	virtual int getNumBGOsUsed()=0;
protected:
	MRKMaterials materials;
	G4LogicalVolume* logicWorld=nullptr;
	G4VPhysicalVolume* physiWorld=nullptr;    // pointer to the physical envelope

};

#endif /* SRC_MRKGEOMETRY_HH_ */
