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

#ifndef MRKMaterials_h
#define MRKMaterials_h 1

#include "globals.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"


class MRKMaterials
{
  public:

    ~MRKMaterials();

    static MRKMaterials* GetInstance();

    G4Material* GetMaterial(G4String);

  private:

    MRKMaterials();

    void CreateMaterials();

  private:

  static MRKMaterials* instance;

  G4NistManager*     nistMan;


    G4Element* El_O;
    G4Element* El_C;
    G4Element* El_H;
    G4Element* El_Cl;
    G4Element* El_Nd;
    G4Element* El_Fe;
    G4Element* El_B;



    G4Material* EpoxyResin;
    G4Material* G10;
    G4Material* StainlessSteel304;
    G4Material* CartridgeBrass;
    G4Material* BorosilicateGlass;
    G4Material* NeodymiumMagnet;
    G4Material* AluminumOxide;
    G4Material* StretchedTeflonTape;


};

#endif /*MRKMaterials_h*/
