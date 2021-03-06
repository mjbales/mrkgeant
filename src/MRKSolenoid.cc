#include "globals.hh"

#include "G4GeometryManager.hh"

#include "MRKSolenoid.hh"
#include "MRKGlobalField.hh"

MRKSolenoid::MRKSolenoid(G4LogicalVolume* lv, G4ThreeVector c, std::string fieldFilePath, double fieldScale, G4ThreeVector inpFieldOffset) :
	MRKElementField(c, lv)
{

	G4cout << "Loading B field:" << G4endl;
	if(bField.loadField(fieldFilePath.data(),fieldScale*CLHEP::tesla))
	{
		G4cout << "Error loading B field." << G4endl;
	}
	bFieldPrinted=false;
	this->lvolume->SetVisAttributes(G4VisAttributes::Invisible);

	fieldOffset.setVal(inpFieldOffset.getX()/CLHEP::m,inpFieldOffset.getY()/CLHEP::m,inpFieldOffset.getZ()/CLHEP::m);

}

MRKSolenoid::~MRKSolenoid()
{
	bField.reset();
}

//Method that we need to get the B field in
void MRKSolenoid::addFieldValue(const G4double point[4], G4double field[6])
{

	vecOut.setVal(0., 0., 0.);
	posIn.setVal(point[0] / CLHEP::m, point[1] / CLHEP::m, point[2] / CLHEP::m);
	posIn -= fieldOffset;
	bField.linearInterp(posIn, vecOut);

	field[0] += vecOut.x;
	field[1] += vecOut.y;
	field[2] += vecOut.z;
}
