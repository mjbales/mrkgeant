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

#include "MRKGlobalField.hh"

#include "RDKSBDField.hh"

#include "constants.hh"

RDKSBDField::RDKSBDField(G4LogicalVolume* lv,G4ThreeVector c,string fieldFileName,double inpPotential, G4ThreeVector inpFieldOffset) : MRKElementField(c,lv)
{
    potential=inpPotential;
    G4cout << "Loading SBD E field:" << G4endl;
    if(eField.loadField(fieldFileName.data(),potential*volt/m))
        G4cout << "Error loading E field." << G4endl;
    eFieldPrinted=false;

    fieldOffset.setVal(inpFieldOffset.getX()/m,inpFieldOffset.getY()/m,inpFieldOffset.getZ()/m);


    this->lvolume->SetVisAttributes(G4VisAttributes::Invisible);
}

RDKSBDField::~RDKSBDField()
{
    eField.reset();
}

//Method that we need to get the B field in
void RDKSBDField::addFieldValue(const G4double point[4],
                                         G4double field[6])
{

    vecOut.setVal(0.,0.,0.);
    posIn.setVal(point[0]/m,point[1]/m,point[2]/m);
    posIn-=fieldOffset;
    eField.linearInterpAs3D(posIn,vecOut);
    field[3] += vecOut.x;
    field[4] += vecOut.y;
    field[5] += vecOut.z;
}
