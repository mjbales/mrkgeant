#include "MRKEField.hh"

#include "globals.hh"
#include "G4GeometryManager.hh"

#include "MRKGlobalField.hh"
#include "MRKConstants.hh"

MRKEField::MRKEField(G4LogicalVolume* lv, G4ThreeVector centerPos, std::string fieldFileName, std::string inpHistName, double scalingValue, int inpSpaceDim, int inpFieldDim, double angleX, double angleY, double angleZ, G4ThreeVector inpFieldOffset) :
	MRKElementField(centerPos, lv)
{
	fieldScale = scalingValue;
	histName = inpHistName;
	G4cout << "Loading E field: " << fieldFileName << "... ";
	if(eField.loadFieldFromFile(fieldFileName, histName, scalingValue * volt / m, inpSpaceDim, inpFieldDim))  //presume file is in V/m
		G4cout << "error loading E field!" << G4endl;
	G4cout << " loading complete." << G4endl;

	eField.setRotation(angleX, angleY, angleZ);

	fieldOffset.SetXYZ(inpFieldOffset.getX() / m, inpFieldOffset.getY() / m, inpFieldOffset.getZ() / m);  //let's pre calculate the field offset in terms of meters

	this->lvolume->SetVisAttributes(G4VisAttributes::Invisible);
}

MRKEField::~MRKEField()
{
	eField.reset();

}

//Method that we need to get the E field in
void MRKEField::addFieldValue(const G4double point[4], G4double field[6])
{
	posIn.SetXYZ(point[0] / m, point[1] / m, point[2] / m);
	posIn -= fieldOffset;
	vecOut.SetXYZ(0, 0, 0);
	eField.linearInterp3D(posIn, vecOut);
	field[3] += vecOut.x();
	field[4] += vecOut.y();
	field[5] += vecOut.z();

}
