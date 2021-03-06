#include "globals.hh"

#include "G4GeometryManager.hh"

#include "MRKMirror.hh"
#include "MRKGlobalField.hh"
using namespace std;
MRKMirror::MRKMirror(G4LogicalVolume* lv, G4ThreeVector c, string fieldFilePath, double fieldScale, G4ThreeVector inpFieldOffset) :
	MRKElementField(c, lv)
{

	G4cout << "Loading mirror E field:" << G4endl;
	if(eField.loadField(fieldFilePath.data(),fieldScale*CLHEP::volt/CLHEP::m))
	{
		G4cout << "Error loading E field." << G4endl;
	}
	eFieldPrinted=false;
	this->lvolume->SetVisAttributes(G4VisAttributes::Invisible);

	fieldOffset.setVal(inpFieldOffset.getX()/CLHEP::m,inpFieldOffset.getY()/CLHEP::m,inpFieldOffset.getZ()/CLHEP::m);

}

MRKMirror::~MRKMirror()
{
	eField.reset();
}

//Method that we need to get the B field in
void MRKMirror::addFieldValue(const G4double point[4], G4double field[6])
{

	vecOut.setVal(0, 0, 0);
	posIn.setVal(point[0] / CLHEP::m, point[1] / CLHEP::m, point[2] / CLHEP::m);
	posIn -= fieldOffset;
	eField.linearInterpAs3D(posIn, vecOut);
	field[3] += vecOut.x;
	field[4] += vecOut.y;
	field[5] += vecOut.z;
}
