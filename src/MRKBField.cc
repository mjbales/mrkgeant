#include "MRKBField.hh"

#include "globals.hh"
#include "G4GeometryManager.hh"

#include "MRKGlobalField.hh"
#include "MRKConstants.hh"

MRKBField::MRKBField(G4LogicalVolume* lv, G4ThreeVector centerPos, std::string fieldFileName, std::string inpHistName, double scalingValue, int inpSpaceDim, int inpFieldDim, double angleX, double angleY, double angleZ, G4ThreeVector inpFieldOffset) :
	MRKElementField(centerPos, lv)
{
	fieldScale = scalingValue;
	histName = inpHistName;
	G4cout << "Loading B field: " << fieldFileName << "... ";
	if(bField.loadFieldFromFile(fieldFileName, histName, scalingValue * tesla, inpSpaceDim, inpFieldDim))  //presume file is in V/m
		G4cout << "Error loading B field." << G4endl;
	G4cout << " loading complete." << G4endl;

	bField.setRotation(angleX, angleY, angleZ);

	fieldOffset.SetXYZ(inpFieldOffset.getX() / m, inpFieldOffset.getY() / m, inpFieldOffset.getZ() / m);  //let's pre calculate the field offset in terms of meters

	this->lvolume->SetVisAttributes(G4VisAttributes::Invisible);
}

MRKBField::~MRKBField()
{
	bField.reset();

}

//Method that we need to get the B field in
void MRKBField::addFieldValue(const G4double point[4], G4double field[6])
{

	vecOut.SetXYZ(0., 0., 0.);
	posIn.SetXYZ(point[0] / m, point[1] / m, point[2] / m);
	posIn -= fieldOffset;
	bField.linearInterp3D(posIn, vecOut);
	field[0] += vecOut.x();
	field[1] += vecOut.y();
	field[2] += vecOut.z();

}
