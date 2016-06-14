#ifndef SRC_MRKGEOMETRY_HH_
#define SRC_MRKGEOMETRY_HH_

#include "G4VPhysicalVolume.hh"

#include "MRKMacroMessenger.hh"
#include "MRKGlobalField.hh"
#include "MRKMaterials.hh"

class MRKGeometry
{
public:
	MRKGeometry(MRKMacroMessenger*){};
	virtual G4VPhysicalVolume* Construct()=0;
	virtual ~MRKGeometry(){};
protected:
	MRKMaterials materials;
};

#endif /* SRC_MRKGEOMETRY_HH_ */
