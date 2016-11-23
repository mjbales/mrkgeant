#ifndef MRKFieldMessenger_h
#define MRKFieldMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class MRKGlobalField;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;
class G4UIcmdWith3VectorAndUnit;

/// Class which defines macro commands relating to magnetic and electric fields
class MRKFieldMessenger: public G4UImessenger
{
public:
	MRKFieldMessenger(MRKGlobalField*);
	~MRKFieldMessenger();

	void SetNewValue(G4UIcommand*, G4String);
	void SetNewValue(G4UIcommand*, G4int);

private:

	MRKGlobalField* fGlobalField;

	G4UIdirectory* detDir;

	G4UIcmdWithAnInteger* fStepperCMD;
	G4UIcmdWithADoubleAndUnit* fMinStepCMD;
	G4UIcmdWithADoubleAndUnit* fDeltaChordCMD;
	G4UIcmdWithADoubleAndUnit* fDeltaOneStepCMD;
	G4UIcmdWithADoubleAndUnit* fDeltaIntersectionCMD;
	G4UIcmdWithADoubleAndUnit* fEpsMinCMD;
	G4UIcmdWithADoubleAndUnit* fEpsMaxCMD;
	G4UIcmdWithADoubleAndUnit* fLargestAcceptableStepCMD;
	G4UIcmdWithoutParameter* fUpdateCMD;

	G4UIcmdWithAnInteger* setCurrentFieldSettingsToModifyCmd;
	G4UIcmdWithAString* setFieldPathCmd;
	G4UIcmdWithAString* setFieldHistNameCmd;
	G4UIcmdWith3VectorAndUnit* setFieldOffsetCmd;
	G4UIcmdWith3VectorAndUnit* setFieldExtentsCmd;
	G4UIcmdWithADouble* setFieldScalingValueCmd;
	G4UIcmdWithAnInteger* setFieldSpaceDimCmd;
	G4UIcmdWithAnInteger* setFieldFieldDimCmd;
	G4UIcmdWithADouble* setFieldAngleXCmd;
	G4UIcmdWithADouble* setFieldAngleYCmd;
	G4UIcmdWithADouble* setFieldAngleZCmd;
	G4UIcmdWithABool* setFieldIsMagneticCmd;
	G4UIcmdWithABool* setFieldSymmetryXCmd;
	G4UIcmdWithABool* setFieldSymmetryYCmd;
	G4UIcmdWithABool* setFieldSymmetryZCmd;
	G4UIcmdWithAString* addNamedFieldsCmd;
	G4UIcmdWithABool* setUseCubicInterpolationCmd;

};

#endif

