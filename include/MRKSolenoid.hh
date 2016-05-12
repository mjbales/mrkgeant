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
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef RDKSolenoid_h
#define RDKSolenoid_h 1

#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"

#include "MRKElementField.hh"
#include "MRKGlobalField.hh"
#include "globals.hh"

#include "MRKVector.hh"
#include "MRKVField.hh"


class MRKSolenoid : public MRKElementField
{
  public:

    ///  Default constructor.
    MRKSolenoid(G4LogicalVolume*, G4ThreeVector,string fieldFilePath,double fieldScale,G4ThreeVector inpFieldOffset);

    ///  Destructor.
    ~MRKSolenoid();

    ///  getLength() returns the length of the solenoid
    virtual G4double getLength() { return 17.6*cm; }

    ///  getWidth() returns the solenoid diameter
    virtual G4double getWidth() { return 12.*cm; }

    ///  getHeight() returns the solenoid diameter
    virtual G4double getHeight() { return 100.*cm; }


    ///  addFieldValue() adds the field for this solenoid into field[].
    ///  point[] is in global coordinates.
    void addFieldValue(const G4double point[4], G4double field[6]);

  private:

    cVField3D bField;
    bool bFieldPrinted;
    MRKVector3D vecOut,posIn;
    MRKVector3D fieldOffset;
    //G4ThreeVector globalPos;


};

#endif
