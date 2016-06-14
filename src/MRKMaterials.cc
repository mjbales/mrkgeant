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

#include "MRKMaterials.hh"

#include "../include/MRKText.hh"

MRKMaterials::MRKMaterials()
{
	nistMan = G4NistManager::Instance();

	nistMan->SetVerbose(2);

	El_C = El_Cl = El_H = El_O = nullptr;
	StretchedTeflonTape = StainlessSteel304 = G10 = EpoxyResin = CartridgeBrass = nullptr;
}

MRKMaterials::~MRKMaterials()
{

	delete El_O;
	delete El_C;
	delete El_H;
	delete El_Cl;
	delete El_Nd;
	delete El_Fe;
	delete El_B;

	delete EpoxyResin;
	delete G10;
	delete StainlessSteel304;
	delete CartridgeBrass;
	delete NeodymiumMagnet;
	delete StretchedTeflonTape;

}


G4Material* MRKMaterials::getMaterial(const G4String material)
{
	G4Material* mat = nistMan->FindOrBuildMaterial(material);

	if(!mat) mat = G4Material::GetMaterial(material);
	if(!mat)
	{
		G4cout << material << "Not Found, Please Retry" << G4endl;
	}

	return mat;
}

void MRKMaterials::createMaterials()
{

	//-------------------------------------
	// Element Definitions
	//-------------------------------------

	//Oxygen
	El_O = nistMan->FindOrBuildElement("O");
	//Carbon
	El_C = nistMan->FindOrBuildElement("C");
	//Hydrogen
	El_H = nistMan->FindOrBuildElement("H");
	//Chlorine
	El_Cl = nistMan->FindOrBuildElement("Cl");
	//Neodymium
	El_Nd = nistMan->FindOrBuildElement("Nd");
	//Iron
	El_Fe = nistMan->FindOrBuildElement("Fe");
	//Boron
	El_B = nistMan->FindOrBuildElement("B");

	//--------- Material definition ------
	//Nickel
	G4Material* Ni = getMaterial("G4_Ni");
	//Iron
	G4Material* Fe = getMaterial("G4_Fe");
	//Cromium
	G4Material* Cr = getMaterial("G4_Cr");
	//Manganese
	G4Material* Mn = getMaterial("G4_Mn");
	//Copper
	G4Material* Cu = getMaterial("G4_Cu");
	//Zinc
	G4Material* Zn = getMaterial("G4_Zn");
	//Carbon
	G4Material* Carbon = getMaterial("G4_C");
	//Fluorine
	G4Material* Fluorine = getMaterial("G4_F");

	int nAtoms;

	//Borosilicate Glass for G10 (Used PNNL-15870 - Compendium of Material Composition Data for Radiation Transport Modeling)
	BorosilicateGlass = getMaterial("G4_Pyrex_Glass");

	//Epoxy Resin for G10 (Guessed Using Approximate internet Numbers)
	EpoxyResin = new G4Material("EpoxyResin", 2.25 * g / cm3, 4);
	EpoxyResin->AddElement(El_C, nAtoms = 21);
	EpoxyResin->AddElement(El_H, nAtoms = 25);
	EpoxyResin->AddElement(El_Cl, nAtoms = 1);
	EpoxyResin->AddElement(El_O, nAtoms = 5);

	//G10 (Guessed using approximate internet numbers)
	G10 = new G4Material("G10", 1.8 * g / cm3, 2);
	G10->AddMaterial(EpoxyResin, 50 * perCent);
	G10->AddMaterial(BorosilicateGlass, 50 * perCent);

	//304 Stainless Steel (Used PNNL-15870 - Compendium of Material Composition Data for Radiation Transport Modeling)
	StainlessSteel304 = new G4Material("StainlessSteel304", 7.92 * g / cm3, 4);
	StainlessSteel304->AddMaterial(Cr, 19 * perCent);
	StainlessSteel304->AddMaterial(Mn, 2 * perCent);
	StainlessSteel304->AddMaterial(Fe, 69.5 * perCent);
	StainlessSteel304->AddMaterial(Ni, 9.5 * perCent);

	CartridgeBrass = new G4Material("CartridgeBrass", 8.53 * g / cm3, 2);
	CartridgeBrass->AddMaterial(Cu, 70 * perCent);
	CartridgeBrass->AddMaterial(Zn, 30 * perCent);

	NeodymiumMagnet = new G4Material("NeodymiumMagnet", 7.4 * g / cm3, 3);
	NeodymiumMagnet->AddElement(El_Nd, nAtoms = 2);
	NeodymiumMagnet->AddElement(El_Fe, nAtoms = 14);
	NeodymiumMagnet->AddElement(El_B, nAtoms = 1);

	StretchedTeflonTape = new G4Material("StretchedTeflonTape", 0.556 * g / cm3, 2);
	StretchedTeflonTape->AddMaterial(Carbon, 24.0183 * perCent);
	StretchedTeflonTape->AddMaterial(Fluorine, 75.9817 * perCent);

	// Print all the materials defined.
	//
	G4cout << G4endl<< "The materials defined are : " << G4endl << G4endl;
	G4cout << *(G4Material::GetMaterialTable()) << G4endl;

}
