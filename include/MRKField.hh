#ifndef MRKField_h
#define MRKField_h 1
#include <string>

#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"

#include "TVector3.h"

#include "MRKROOTField.hh"
#include "MRKElementField.hh"

class FieldSettings
{
public:
	FieldSettings()
	{
		scalingValue = 0;
		isMagnetic = false;
		spaceDim = fieldDim = 3;
		angleX = angleY = angleZ = 0;
		symmetry[0] = symmetry[1] = symmetry[2] = false;
		histName = "field";
		extents = G4ThreeVector(1. * CLHEP::m, 1. * CLHEP::m, 1. * CLHEP::m);
		fieldFilePath = "";
		useCubicInterpolation = false;
	}
	;
	std::string fieldFilePath;
	std::string histName;

	G4ThreeVector centerPos;
	G4ThreeVector extents;
	G4ThreeVector offset;
	double scalingValue; //Zero scaling value will be the signal that no field should be loaded;
	int spaceDim;
	int fieldDim;
	double angleX;
	double angleY;
	double angleZ;
	bool isMagnetic; //false = electric
	bool symmetry[3];
	bool useCubicInterpolation;
};

class MRKField: public MRKElementField
{
public:

	//  Constructor.
	MRKField(G4LogicalVolume* lv, FieldSettings inpFS);

	//  Destructor.
	~MRKField();

    ///  getLength() returns the overall extents of the field (can make bigger than necessary)
    virtual inline G4double getLength() { return fs.extents[0]; }

    ///  getWidth() returns the overall extents of the field (can make bigger than necessary)
    virtual inline G4double getWidth() { return fs.extents[1]; }

    ///  getHeight() returns the overall extents of the field (can make bigger than necessary)
    virtual inline G4double getHeight() { return fs.extents[2]; }

	///  addFieldValue() adds the field for this solenoid into field[].
	///  point[] is in global coordinates.
	void addFieldValue(const G4double point[4], G4double field[6]);

private:

	MRKROOTField theField;

	TVector3 posIn, vecOut, offset;

	FieldSettings fs;
	int g4FieldX, g4FieldY, g4FieldZ;

};

#endif
