#ifndef MRKMaterials_h
#define MRKMaterials_h 1

#include "globals.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"

/// Class which defines materials used in detector construction
class MRKMaterials
{
public:
	MRKMaterials();
	~MRKMaterials();

	G4Material* getMaterial(G4String);
	void createMaterials();

private:
	G4NistManager* nistMan;

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
