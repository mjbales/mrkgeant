#include "MRKField.hh"

#include "globals.hh"
#include "G4GeometryManager.hh"

#include "MRKElementField.hh"
#include "MRKConstants.hh"

MRKField::MRKField(G4LogicalVolume* lv,FieldSettings inpFS): MRKElementField(inpFS.centerPos,lv)
{
    fs=inpFS;

    G4cout << "Loading field: " << fs.fieldFilePath << "... ";
    double tempScaling=fs.scalingValue;
    if(fs.isMagnetic)
    {
        tempScaling*=tesla;
        g4FieldX=0;
        g4FieldY=1;
        g4FieldZ=2;
    }
    else
    {
        tempScaling*=volt/m;
        g4FieldX=3;
        g4FieldY=4;
        g4FieldZ=5;
    }
    offset.SetXYZ(fs.offset[0]/m,fs.offset[1]/m,fs.offset[2]/m); //Convert to ROOT vector format
    if(theField.loadFieldFromFile(fs.fieldFilePath, fs.histName, tempScaling, fs.spaceDim,  fs.fieldDim))
        G4cout << "error loading field!" << G4endl;
    G4cout<<" loading complete."<< G4endl;

    theField.setRotation(fs.angleX,fs.angleY,fs.angleZ);
    theField.setSymmetry(fs.symmetry[0],fs.symmetry[1],fs.symmetry[2]);

    //Setting global bounding box in the weird way that Geant4 wants
    G4double tempPoint[4];
    for (int i = 0;i< 3;i++)
    {
    	tempPoint[i]=fs.centerPos[i] + 0.5*fs.extents[i];
    }
    setGlobalPoint(tempPoint);
    for (int i = 0;i< 3;i++)
    {
    	tempPoint[i]=fs.centerPos[i] - 0.5*fs.extents[i];
    }
    setGlobalPoint(tempPoint);

    this->lvolume->SetVisAttributes(G4VisAttributes::Invisible);
}


MRKField::~MRKField()
{
    theField.reset();

}

//Method that we need to get the E field in
void MRKField::addFieldValue(const G4double point[4],G4double field[6])
{
    if(!isInBoundingBox(point))
    {
        return;
    }
    posIn.SetXYZ(point[0]/m,point[1]/m,point[2]/m);
    vecOut.SetXYZ(0,0,0);
    if(fs.useCubicInterpolation)
    {
        theField.cubicInterp3D(posIn-offset,vecOut);
    }
    else
    {
        theField.linearInterp3D(posIn-offset,vecOut);
    }
    field[g4FieldX] += vecOut.x();
    field[g4FieldY] += vecOut.y();
    field[g4FieldZ] += vecOut.z();
}
