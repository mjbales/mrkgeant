#ifndef MRKBField_h
#define MRKBField_h 1

#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"

#include "MRKElementField.hh"
#include "MRKGlobalField.hh"
#include "globals.hh"

#include "TVector3.h"
#include "MRKROOTField.hh"

/// Magnetic field class.  Values are stored in the ROOT field data structure.
class MRKBField: public MRKElementField
{
public:

	//  Default constructor.
	MRKBField(G4LogicalVolume* lv, G4ThreeVector centerPos, std::string fieldFileName, std::string inpHistName, double scalingValue = 1, int inpSpaceDim = 3, int inpFieldDim = 3, double angleX = 0, double angleY = 0, double angleZ = 0, G4ThreeVector inpFieldOffset = G4ThreeVector());

	//  Destructor.
	~MRKBField();

    ///  getLength() returns the overall extents of the field diameter (bigger than necessary)
    virtual G4double getLength() { return 1.*CLHEP::m; }

    ///  getWidth() returns the overall extents of the field diameter (bigger than necessary)
    virtual G4double getWidth() { return 1.*CLHEP::m; }

    ///  getHeight() returns the overall extents of the field diameter (bigger than necessary)
    virtual G4double getHeight() { return 1.*CLHEP::m; }

	///  addFieldValue() adds the magnetic field into field[].
	///  point[] is in global coordinates.
	void addFieldValue(const G4double point[4], G4double field[6]);

    inline void setSymmetry(bool inpX, bool inpY=false, bool inpZ=false){bField.setSymmetry(inpX,inpY,inpZ);};

private:

	MRKROOTField bField; //Field vectors defined here
	TVector3 posIn, vecOut;
	TVector3 fieldOffset;
	double fieldScale;
	std::string histName;

};

#endif
