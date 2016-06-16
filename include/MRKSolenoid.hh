#ifndef RDKSolenoid_h
#define RDKSolenoid_h 1

#include <string>

#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "globals.hh"

#include "MRKElementField.hh"
#include "MRKGlobalField.hh"
#include "MRKVector.hh"
#include "MRKVField.hh"

class MRKSolenoid: public MRKElementField
{
public:

	///  Default constructor.
	MRKSolenoid(G4LogicalVolume*, G4ThreeVector, std::string fieldFilePath, double fieldScale, G4ThreeVector inpFieldOffset);

	///  Destructor.
	~MRKSolenoid();

    ///  getLength() returns the length of the solenoid
    inline virtual G4double getLength() { return 17.6*cm; }

    ///  getWidth() returns the solenoid diameter
    inline virtual G4double getWidth() { return 12.*cm; }

    ///  getHeight() returns the solenoid diameter
    inline virtual G4double getHeight() { return 100.*cm; }

	///  addFieldValue() adds the field for this solenoid into field[].
	///  point[] is in global coordinates.
	void addFieldValue(const G4double point[4], G4double field[6]);

private:

	cVField3D bField;
	bool bFieldPrinted;
	MRKVector3D vecOut, posIn;
	MRKVector3D fieldOffset;
	//G4ThreeVector globalPos;

};

#endif
