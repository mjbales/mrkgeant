//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
//

#include "globals.hh"

#include "G4GeometryManager.hh"

#include "MRKElementField.hh"

#include "MRKField.hh"

#include "../include/MRKConstants.hh"

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
