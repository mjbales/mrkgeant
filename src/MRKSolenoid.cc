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

#include "../include/MRKSolenoid.hh"
#include "MRKGlobalField.hh"


MRKSolenoid::MRKSolenoid(G4LogicalVolume* lv,G4ThreeVector c,string fieldFilePath,double fieldScale,G4ThreeVector inpFieldOffset) : MRKElementField(c,lv)
{

    G4cout << "Loading B field:" << G4endl;
    if(bField.loadField(fieldFilePath.data(),fieldScale*tesla))
        G4cout << "Error loading B field." << G4endl;
    bFieldPrinted=false;
    this->lvolume->SetVisAttributes(G4VisAttributes::Invisible);

    fieldOffset.setVal(inpFieldOffset.getX()/m,inpFieldOffset.getY()/m,inpFieldOffset.getZ()/m);

}

MRKSolenoid::~MRKSolenoid()
{
    bField.reset();
}

//Method that we need to get the B field in
void MRKSolenoid::addFieldValue(const G4double point[4],
                                         G4double field[6])
{

    vecOut.setVal(0.,0.,0.);
    posIn.setVal(point[0]/m,point[1]/m,point[2]/m);
    posIn-=fieldOffset;
    bField.linearInterp(posIn,vecOut);

    field[0] += vecOut.x;
    field[1] += vecOut.y;
    field[2] += vecOut.z;
}
