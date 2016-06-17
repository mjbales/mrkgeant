#ifndef RDKSBDField_h
#define RDKSBDField_h 1

#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "globals.hh"

#include "MRKElementField.hh"
#include "MRKGlobalField.hh"

#include "MRKVector.hh"
#include "MRKVField.hh"

class MRKSBDField: public MRKElementField
{
public:

	///  Default constructor.
	MRKSBDField(G4LogicalVolume*, G4ThreeVector, std::string fieldFileName, double inpPotential, G4ThreeVector inpFieldOffset);

	///  Destructor.
	~MRKSBDField();

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
	MRKVector3D posIn, vecOut;
	MRKVector3D fieldOffset;
	bool eFieldPrinted;
	double potential;
	//G4ThreeVector globalPos;

};

#endif
