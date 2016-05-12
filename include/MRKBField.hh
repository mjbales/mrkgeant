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
//MRKBField
//Stores and interpolates electric field which is stored in tesla and m.
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef MRKBField_h
#define MRKBField_h 1

#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"

#include "MRKElementField.hh"
#include "MRKGlobalField.hh"
#include "globals.hh"

#include "TVector3.h"
#include "MRKROOTField.hh"




class MRKBField : public MRKElementField
{
  public:

    //  Default constructor.
    MRKBField(G4LogicalVolume* lv,G4ThreeVector centerPos,string fieldFileName,string inpHistName,double scalingValue=1,int inpSpaceDim=3, int inpFieldDim=3,double angleX=0, double angleY=0, double angleZ=0,G4ThreeVector inpFieldOffset=G4ThreeVector());

    //  Destructor.
    ~MRKBField();

    ///  getLength() returns the overall extents of the field diameter (bigger than necessary)
    virtual G4double getLength() { return 17.6*cm; }

    ///  getWidth() returns the overall extents of the field diameter (bigger than necessary)
    virtual G4double getWidth() { return 12.*cm; }

    ///  getHeight() returns the overall extents of the field diameter (bigger than necessary)
    virtual G4double getHeight() { return 100.*cm; }


    ///  addFieldValue() adds the field for this solenoid into field[].
    ///  point[] is in global coordinates.
    void addFieldValue(const G4double point[4], G4double field[6]);

    inline void setSymmetry(bool inpX, bool inpY=false, bool inpZ=false){bField.setSymmetry(inpX,inpY,inpZ);};

  private:

    MRKROOTField bField;
    TVector3 posIn,vecOut;
    TVector3 fieldOffset;
    double fieldScale;
    string histName;



};

#endif
