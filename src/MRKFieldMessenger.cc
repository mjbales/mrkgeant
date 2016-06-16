#include "MRKFieldMessenger.hh"

#include "MRKGlobalField.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"

MRKFieldMessenger::MRKFieldMessenger(MRKGlobalField* pEMfield) :
	fGlobalField(pEMfield)
{
	detDir = new G4UIdirectory("/MRK/field/");
	detDir->SetGuidance(" Field tracking control ");

	fStepperCMD = new G4UIcmdWithAnInteger("/MRK/field/setStepperType", this);
	fStepperCMD->SetGuidance("Select stepper type for field");
	fStepperCMD->SetParameterName("choice", true);
	fStepperCMD->SetDefaultValue(4);
	fStepperCMD->AvailableForStates(G4State_PreInit, G4State_Idle);

	fUpdateCMD = new G4UIcmdWithoutParameter("/MRK/field/update", this);
	fUpdateCMD->SetGuidance("Update Field");
	fUpdateCMD->SetGuidance("This command MUST be applied before \"beamOn\" ");
	fUpdateCMD->SetGuidance("if you changed field settings.");
	fUpdateCMD->AvailableForStates(G4State_PreInit, G4State_Idle);

	fMinStepCMD = new G4UIcmdWithADoubleAndUnit("/MRK/field/setMinStep", this);
	fMinStepCMD->SetGuidance("Define minimal step");
	fMinStepCMD->SetParameterName("min step", false, false);
	fMinStepCMD->SetDefaultUnit("mm");
	fMinStepCMD->AvailableForStates(G4State_PreInit, G4State_Idle);

	fDeltaChordCMD = new G4UIcmdWithADoubleAndUnit("/MRK/field/setDeltaChord", this);
	fDeltaChordCMD->SetGuidance("Define delta chord");
	fDeltaChordCMD->SetParameterName("delta chord", false, false);
	fDeltaChordCMD->SetDefaultUnit("mm");
	fDeltaChordCMD->AvailableForStates(G4State_PreInit, G4State_Idle);

	fDeltaOneStepCMD = new G4UIcmdWithADoubleAndUnit("/MRK/field/setDeltaOneStep", this);
	fDeltaOneStepCMD->SetGuidance("Define delta one step");
	fDeltaOneStepCMD->SetParameterName("delta one step", false, false);
	fDeltaOneStepCMD->SetDefaultUnit("mm");
	fDeltaOneStepCMD->AvailableForStates(G4State_PreInit, G4State_Idle);

	fDeltaIntersectionCMD = new G4UIcmdWithADoubleAndUnit("/MRK/field/setDeltaIntersection", this);
	fDeltaIntersectionCMD->SetGuidance("Define delta intersection");
	fDeltaIntersectionCMD->SetParameterName("delta intersection", false, false);
	fDeltaIntersectionCMD->SetDefaultUnit("mm");
	fDeltaIntersectionCMD->AvailableForStates(G4State_PreInit, G4State_Idle);

	fEpsMinCMD = new G4UIcmdWithADoubleAndUnit("/MRK/field/setEpsMin", this);
	fEpsMinCMD->SetGuidance("Define eps min");
	fEpsMinCMD->SetParameterName("eps min", false, false);
	fEpsMinCMD->SetDefaultUnit("mm");
	fEpsMinCMD->AvailableForStates(G4State_PreInit, G4State_Idle);

	fEpsMaxCMD = new G4UIcmdWithADoubleAndUnit("/MRK/field/setEpsMax", this);
	fEpsMaxCMD->SetGuidance("Define eps max");
	fEpsMaxCMD->SetParameterName("eps max", false, false);
	fEpsMaxCMD->SetDefaultUnit("mm");
	fEpsMaxCMD->AvailableForStates(G4State_PreInit, G4State_Idle);

	fLargestAcceptableStepCMD = new G4UIcmdWithADoubleAndUnit("/MRK/field/setLargestStep", this);
	fLargestAcceptableStepCMD->SetGuidance("Define largest acceptable step");
	fLargestAcceptableStepCMD->SetParameterName("Largest Acceptable Step", false, false);
	fLargestAcceptableStepCMD->SetDefaultUnit("mm");
	fLargestAcceptableStepCMD->AvailableForStates(G4State_PreInit, G4State_Idle);

	setCurrentFieldSettingsToModifyCmd = new G4UIcmdWithAnInteger("/MRK/field/setCurrentFieldToModify", this);
	setCurrentFieldSettingsToModifyCmd->SetGuidance("Sets the current field to modify");
	setCurrentFieldSettingsToModifyCmd->SetParameterName("currentFieldNumber", false);
	setCurrentFieldSettingsToModifyCmd->AvailableForStates(G4State_PreInit);

	setFieldPathCmd = new G4UIcmdWithAString("/MRK/field/setFieldPath", this);
	setFieldPathCmd->SetGuidance("Sets the current field path");
	setFieldPathCmd->SetParameterName("fieldPath", false);
	setFieldPathCmd->AvailableForStates(G4State_PreInit);

	setFieldHistNameCmd = new G4UIcmdWithAString("/MRK/field/setFieldHistName", this);
	setFieldHistNameCmd->SetGuidance("Sets the ROOT histname for the field file");
	setFieldHistNameCmd->SetParameterName("fieldHistName", false);
	setFieldHistNameCmd->AvailableForStates(G4State_PreInit);

	setFieldOffsetCmd = new G4UIcmdWith3VectorAndUnit("/MRK/field/setFieldOffset", this);
	setFieldOffsetCmd->SetGuidance("Sets the current field offset");
	setFieldOffsetCmd->SetParameterName("fieldCenterPosX", "fieldCenterPosY", "fieldCenterPosZ", false);
	setFieldOffsetCmd->AvailableForStates(G4State_PreInit);

	setFieldExtentsCmd = new G4UIcmdWith3VectorAndUnit("/MRK/field/setFieldExtents", this);
	setFieldExtentsCmd->SetGuidance("Sets the current field extents");
	setFieldExtentsCmd->SetParameterName("fieldExtentsX", "fieldExtentsY", "fieldExtentsZ", false);
	setFieldExtentsCmd->AvailableForStates(G4State_PreInit);

	setFieldScalingValueCmd = new G4UIcmdWithADouble("/MRK/field/setFieldScalingValue", this);
	setFieldScalingValueCmd->SetGuidance("Sets the current field scaling value");
	setFieldScalingValueCmd->SetParameterName("fieldScalingvalue", false);
	setFieldScalingValueCmd->AvailableForStates(G4State_PreInit);

	setFieldSpaceDimCmd = new G4UIcmdWithAnInteger("/MRK/field/setFieldSpaceDim", this);
	setFieldSpaceDimCmd->SetGuidance("Sets the current field space dimension");
	setFieldSpaceDimCmd->SetParameterName("fieldSpaceDim", false);
	setFieldSpaceDimCmd->AvailableForStates(G4State_PreInit);

	setFieldFieldDimCmd = new G4UIcmdWithAnInteger("/MRK/field/setFieldFieldDim", this);
	setFieldFieldDimCmd->SetGuidance("Sets the current field's dimension (vector/scalar)");
	setFieldFieldDimCmd->SetParameterName("fieldFieldDim", false);
	setFieldFieldDimCmd->AvailableForStates(G4State_PreInit);

	setFieldAngleXCmd = new G4UIcmdWithADouble("/MRK/field/setFieldAngleX", this);
	setFieldAngleXCmd->SetGuidance("Sets the rotation angle of the field around X axis");
	setFieldAngleXCmd->SetParameterName("fieldAngleX", false);
	setFieldAngleXCmd->AvailableForStates(G4State_PreInit);

	setFieldAngleYCmd = new G4UIcmdWithADouble("/MRK/field/setFieldAngleY", this);
	setFieldAngleYCmd->SetGuidance("Sets the rotation angle of the field around Y axis");
	setFieldAngleYCmd->SetParameterName("fieldAngleY", false);
	setFieldAngleYCmd->AvailableForStates(G4State_PreInit);

	setFieldAngleZCmd = new G4UIcmdWithADouble("/MRK/field/setFieldAngleZ", this);
	setFieldAngleZCmd->SetGuidance("Sets the rotation angle of the field around Z axis");
	setFieldAngleZCmd->SetParameterName("fieldAngleZ", false);
	setFieldAngleZCmd->AvailableForStates(G4State_PreInit);

	setFieldIsMagneticCmd = new G4UIcmdWithABool("/MRK/field/setFieldIsMagnetic", this);
	setFieldIsMagneticCmd->SetGuidance("Sets whether field is magnetic");
	setFieldIsMagneticCmd->SetParameterName("fieldIsMagnetic", false);
	setFieldIsMagneticCmd->AvailableForStates(G4State_PreInit);

	setUseCubicInterpolationCmd = new G4UIcmdWithABool("/MRK/field/setUseCubicInterpolation", this);
	setUseCubicInterpolationCmd->SetGuidance("Sets whether field should use cubic interpolation (default is linear)");
	setUseCubicInterpolationCmd->SetParameterName("useCubicInterpolation", false);
	setUseCubicInterpolationCmd->AvailableForStates(G4State_PreInit);

	setFieldSymmetryXCmd = new G4UIcmdWithABool("/MRK/field/setFieldSymmetryX", this);
	setFieldSymmetryXCmd->SetGuidance("Sets whether field is symmetric about X axis");
	setFieldSymmetryXCmd->SetParameterName("fieldSymmetryX", false);
	setFieldSymmetryXCmd->AvailableForStates(G4State_PreInit);

	setFieldSymmetryYCmd = new G4UIcmdWithABool("/MRK/field/setFieldSymmetryY", this);
	setFieldSymmetryYCmd->SetGuidance("Sets whether field is symmetric about Y axis");
	setFieldSymmetryYCmd->SetParameterName("fieldSymmetryY", false);
	setFieldSymmetryYCmd->AvailableForStates(G4State_PreInit);

	setFieldSymmetryZCmd = new G4UIcmdWithABool("/MRK/field/setFieldSymmetryZ", this);
	setFieldSymmetryZCmd->SetGuidance("Sets whether field is symmetric about Z axis");
	setFieldSymmetryZCmd->SetParameterName("fieldSymmetryZ", false);
	setFieldSymmetryZCmd->AvailableForStates(G4State_PreInit);

	addNamedFieldsCmd = new G4UIcmdWithAString("/MRK/field/addNamedFields", this);
	addNamedFieldsCmd->SetGuidance("Sets predefined fields to be used");
	addNamedFieldsCmd->SetParameterName("NamedFieldsString", false);
	addNamedFieldsCmd->AvailableForStates(G4State_PreInit);
}

MRKFieldMessenger::~MRKFieldMessenger()
{
	delete detDir;

	delete fStepperCMD;
	delete fMinStepCMD;
	delete fDeltaChordCMD;
	delete fDeltaOneStepCMD;
	delete fDeltaIntersectionCMD;
	delete fEpsMinCMD;
	delete fEpsMaxCMD;
	delete fUpdateCMD;
	delete fLargestAcceptableStepCMD;

	delete setCurrentFieldSettingsToModifyCmd;
	delete setFieldPathCmd;
	delete setFieldHistNameCmd;
	delete setFieldOffsetCmd;
	delete setFieldExtentsCmd;
	delete setFieldScalingValueCmd;
	delete setFieldSpaceDimCmd;
	delete setFieldFieldDimCmd;
	delete setFieldAngleXCmd;
	delete setFieldAngleYCmd;
	delete setFieldAngleZCmd;
	delete setFieldIsMagneticCmd;
	delete setFieldSymmetryXCmd;
	delete setFieldSymmetryYCmd;
	delete setFieldSymmetryZCmd;
	delete addNamedFieldsCmd;
	delete setUseCubicInterpolationCmd;

}

void MRKFieldMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if(command == fStepperCMD)
	{
		fGlobalField->SetStepperType(fStepperCMD->GetNewIntValue(newValue));
	}
	if(command == fUpdateCMD)
	{
		fGlobalField->updateField();
	}
	if(command == fMinStepCMD)
	{
		fGlobalField->SetMinStep(fMinStepCMD->GetNewDoubleValue(newValue));
	}
	if(command == fDeltaChordCMD)
	{
		fGlobalField->SetDeltaChord(fDeltaChordCMD->GetNewDoubleValue(newValue));
	}
	if(command == fDeltaOneStepCMD)
	{
		fGlobalField->SetDeltaOneStep(fDeltaOneStepCMD->GetNewDoubleValue(newValue));
	}
	if(command == fDeltaIntersectionCMD)
	{
		fGlobalField->SetDeltaIntersection(fDeltaIntersectionCMD->GetNewDoubleValue(newValue));
	}
	if(command == fEpsMinCMD)
	{
		fGlobalField->SetEpsMin(fEpsMinCMD->GetNewDoubleValue(newValue));
	}
	if(command == fEpsMaxCMD)
	{
		fGlobalField->SetEpsMax(fEpsMaxCMD->GetNewDoubleValue(newValue));
	}
	if(command == fLargestAcceptableStepCMD)
	{
		fGlobalField->SetLargestStepSize(fLargestAcceptableStepCMD->GetNewDoubleValue(newValue));
	}

	if(command == setCurrentFieldSettingsToModifyCmd)
	{
		fGlobalField->setCurrentFieldSettingsToModify(setCurrentFieldSettingsToModifyCmd->GetNewIntValue(newValue));
	}
	if(command == setFieldPathCmd)
	{
		fGlobalField->setFieldPath(newValue);
	}
	if(command == setFieldHistNameCmd)
	{
		fGlobalField->setFieldHistName(newValue);
	}
	if(command == setFieldOffsetCmd)
	{
		fGlobalField->setFieldOffset(setFieldOffsetCmd->GetNew3VectorValue(newValue));
	}
	if(command == setFieldExtentsCmd)
	{
		fGlobalField->setFieldExtents(setFieldExtentsCmd->GetNew3VectorValue(newValue));
	}
	if(command == setFieldScalingValueCmd)
	{
		fGlobalField->setFieldScalingValue(setFieldScalingValueCmd->GetNewDoubleValue(newValue));
	}
	if(command == setFieldSpaceDimCmd)
	{
		fGlobalField->setFieldSpaceDim(setFieldSpaceDimCmd->GetNewIntValue(newValue));
	}
	if(command == setFieldFieldDimCmd)
	{
		fGlobalField->setFieldFieldDim(setFieldFieldDimCmd->GetNewIntValue(newValue));
	}
	if(command == setFieldAngleXCmd)
	{
		fGlobalField->setFieldAngleX(setFieldAngleXCmd->GetNewDoubleValue(newValue));
	}
	if(command == setFieldAngleYCmd)
	{
		fGlobalField->setFieldAngleY(setFieldAngleYCmd->GetNewDoubleValue(newValue));
	}
	if(command == setFieldAngleZCmd)
	{
		fGlobalField->setFieldAngleZ(setFieldAngleZCmd->GetNewDoubleValue(newValue));
	}
	if(command == setFieldIsMagneticCmd)
	{
		fGlobalField->setFieldIsMagnetic(setFieldIsMagneticCmd->GetNewBoolValue(newValue));
	}
	if(command == setUseCubicInterpolationCmd)
	{
		fGlobalField->setFieldUseCubicInterpolation(setUseCubicInterpolationCmd->GetNewBoolValue(newValue));
	}
	if(command == setFieldSymmetryXCmd)
	{
		fGlobalField->setFieldSymmetryX(setFieldSymmetryXCmd->GetNewBoolValue(newValue));
	}
	if(command == setFieldSymmetryYCmd)
	{
		fGlobalField->setFieldSymmetryY(setFieldSymmetryYCmd->GetNewBoolValue(newValue));
	}
	if(command == setFieldSymmetryZCmd)
	{
		fGlobalField->setFieldSymmetryZ(setFieldSymmetryZCmd->GetNewBoolValue(newValue));
	}
	if(command == addNamedFieldsCmd)
	{
		fGlobalField->addNamedField(newValue);
	}

}
