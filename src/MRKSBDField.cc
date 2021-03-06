#include "globals.hh"

#include "G4GeometryManager.hh"

#include "MRKGlobalField.hh"

#include "MRKConstants.hh"
#include "MRKSBDField.hh"

MRKSBDField::MRKSBDField(G4LogicalVolume* lv, G4ThreeVector c, std::string fieldFileName, double inpPotential, G4ThreeVector inpFieldOffset) :
	MRKElementField(c, lv)
{
	potential = inpPotential;
	G4cout << "Loading SBD E field:" << G4endl;
	if(eField.loadField(fieldFileName.data(), potential * CLHEP::volt / CLHEP::m))
	{
		G4cout << "Error loading E field." << G4endl;
	}
	eFieldPrinted = false;

	fieldOffset.setVal(inpFieldOffset.getX() / CLHEP::m, inpFieldOffset.getY() / CLHEP::m, inpFieldOffset.getZ() / CLHEP::m);

	this->lvolume->SetVisAttributes(G4VisAttributes::Invisible);
}

MRKSBDField::~MRKSBDField()
{
	eField.reset();
}

//Method that we need to get the B field in
void MRKSBDField::addFieldValue(const G4double point[4], G4double field[6])
{

	vecOut.setVal(0., 0., 0.);
	posIn.setVal(point[0] / CLHEP::m, point[1] / CLHEP::m, point[2] / CLHEP::m);
	posIn -= fieldOffset;
	eField.linearInterpAs3D(posIn, vecOut);
	field[3] += vecOut.x;
	field[4] += vecOut.y;
	field[5] += vecOut.z;
}
