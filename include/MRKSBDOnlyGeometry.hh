#ifndef SRC_MRKSBDONLYGEOMETRY_HH_
#define SRC_MRKSBDONLYGEOMETRY_HH_

#include <vector>

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4UserLimits.hh"

#include "MRKMaterials.hh"
#include "MRKGeometry.hh"
#include "MRKMacroMessenger.hh"

class MRKSBDOnlyGeometry: public MRKGeometry
{
public:
	MRKSBDOnlyGeometry(MRKMacroMessenger* inpMacroMessenger);
	~MRKSBDOnlyGeometry();

	G4VPhysicalVolume* Construct();

	inline bool getUseSBDDetector(){return true;}
	inline bool getUseBAPDDetectors(){return false;}
	inline bool getUseBGODetectors(){return false;}
	inline int getNumBGOsUsed(){return 0;}
	inline void setUse1_5mmSBD(bool inp){use1_5mmSBD=inp;}
	inline void setSBDGoldLayerOff(bool inp){SBDGoldLayerOff=inp;}
	inline void setSiDeadLayerLength(double inp){SiDeadLayerLength=inp;}
	inline void setSBDCanOff(bool inp){SBDCanOff=inp;}
private:
	void defineMacroCommands(MRKMacroMessenger* inpMacroMessenger);
	void defineDistancesAndRotationsRDK2();

	//Volume construction methods
	void constructWorld();
	G4LogicalVolume* constructSBDDetectorEnvelope();
	G4LogicalVolume* constructRDK2SBDSilicon();
	G4LogicalVolume* constructRDK2SBDSiliconHolder();
	G4LogicalVolume* constructRDK2SBDDeadLayer();
	G4LogicalVolume* constructRDK2SBDGoldCoating();
	G4LogicalVolume* constructRDK2SBDAluminumCoating();
	G4LogicalVolume* constructRDK2SBDPlasticLining();
	G4LogicalVolume* constructRDK2SBDBackBrassRings();
	G4LogicalVolume* constructRDK2SBDInternalBrassDisc();
	G4LogicalVolume* constructRDK2SBDSpring();
	G4LogicalVolume* constructRDK2SBDHexScrew();
	G4LogicalVolume* constructRDK2SBDFrontCase();
	G4LogicalVolume* constructRDK2SBDMidCase();
	G4LogicalVolume* constructRDK2SBDBackCase();

	void constructSBDEpoxy();

	G4LogicalVolume* logicSBDDetectionVolume;
	G4LogicalVolume* logicSBDDetectorEnvelope;

	double SiDeadLayerLength;

	bool useSBDDetector;
	bool SBDGoldLayerOff;
	bool SBDCanOff;

	G4ThreeVector sbdSiliconFrontCenter;

	bool use1_5mmSBD; //true =1.5 mm det    false = 1.0 mm det
	double sbdThicknessModifier;
	G4ThreeVector sbdThicknessModifierVector;

	G4VisAttributes* VisStainlessSteel304;
	G4VisAttributes* VisG4_Al;
	G4VisAttributes* VisG4_Galactic;
	G4VisAttributes* VisG4_POLYCARBONATE;
	G4VisAttributes* VisG4_Si;
	G4VisAttributes* VisG4_ALUMINUM_OXIDE;
	G4VisAttributes* VisG4_Au;
	G4VisAttributes* VisCartridgeBrass;
	G4VisAttributes* VisMylar;
	G4VisAttributes* VisEpoxy;
};

#endif /* SRC_MRKSBDONLYGEOMETRY_HH_ */
