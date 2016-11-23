#ifndef MRKMirror_h
#define MRKMirror_h 1

#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"

#include "MRKElementField.hh"
#include "MRKGlobalField.hh"
#include "globals.hh"

#include <string.h>
#include "MRKVector.hh"
#include "MRKVField.hh"

/// Class which defines the field for the electrostatic mirror
class MRKMirror: public MRKElementField
{
public:

	///  Default constructor.
	MRKMirror(G4LogicalVolume*, G4ThreeVector, std::string fieldFilePath, double fieldScale, G4ThreeVector inpFieldOffset);

	///  Destructor.
	~MRKMirror();

    ///  getLength() returns the length of the solenoid
    inline virtual G4double getLength() { return 17.6*CLHEP::cm; }

    ///  getWidth() returns the solenoid diameter
    inline virtual G4double getWidth() { return 12.*CLHEP::cm; }

    ///  getHeight() returns the solenoid diameter
    inline virtual G4double getHeight() { return 100.*CLHEP::cm; }

	///  addFieldValue() adds the field for this solenoid into field[].
	///  point[] is in global coordinates.
	void addFieldValue(const G4double point[4], G4double field[6]);

private:

	cVField2D eField;
	bool eFieldPrinted;
	MRKVector3D posIn, vecOut;
	MRKVector3D fieldOffset;
	//G4ThreeVector globalPos;

};

#endif
