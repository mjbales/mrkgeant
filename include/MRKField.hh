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
//MRKField
//Stores and interpolates electric/magnetic field which is stored in V/m or tesla and m.
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef MRKField_h
#define MRKField_h 1

#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"

#include "MRKElementField.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"

#include "ROOTField.hh"
#include "TVector3.h"

#include <string>

class FieldSettings
{
    public:
    FieldSettings()
    {
        scalingValue=0;
        isMagnetic=false;
        spaceDim=fieldDim=3;
        angleX=angleY=angleZ=0;
        symmetry[0]=symmetry[1]=symmetry[2]=false;
        histName="field";
        extents=G4ThreeVector(1.*m,1.*m,1.*m);
        fieldFilePath="";
        useCubicInterpolation=false;
    };
    string fieldFilePath;
    string histName;

    G4ThreeVector centerPos;
    G4ThreeVector extents;
    G4ThreeVector offset;
    double scalingValue; //Zero scaling value will be the signal that no field should be loaded;
    int spaceDim;
    int fieldDim;
    double angleX;
    double angleY;
    double angleZ;
    bool isMagnetic; //false = electric
    bool symmetry[3];
    bool useCubicInterpolation;
};

class MRKField : public MRKElementField
{
  public:

    //  Constructor.
    MRKField(G4LogicalVolume* lv,FieldSettings inpFS);

    //  Destructor.
    ~MRKField();

    ///  getLength() returns the overall extents of the field (can make bigger than necessary)
    virtual G4double getLength() { return fs.extents[0]; }

    ///  getWidth() returns the overall extents of the field (can make bigger than necessary)
    virtual G4double getWidth() { return fs.extents[1]; }

    ///  getHeight() returns the overall extents of the field (can make bigger than necessary)
    virtual G4double getHeight() { return fs.extents[2]; }

    ///  addFieldValue() adds the field for this solenoid into field[].
    ///  point[] is in global coordinates.
    void addFieldValue(const G4double point[4], G4double field[6]);


  private:

    ROOTField theField;

    TVector3 posIn,vecOut,offset;

    FieldSettings fs;
    int g4FieldX,g4FieldY,g4FieldZ;



};

#endif
