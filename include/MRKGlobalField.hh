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

#ifndef MRKGlobalField_h
#define MRKGlobalField_h 1

#include <vector>
#include <string>

#include "G4FieldManager.hh"
#include "G4PropagatorInField.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ChordFinder.hh"

#include "G4MagneticField.hh"
#include "G4ElectroMagneticField.hh"

#include "G4Mag_EqRhs.hh"
#include "G4Mag_SpinEqRhs.hh"

#include "G4EqMagElectricField.hh"
#include "G4EqEMFieldWithSpin.hh"

#include "MRKFieldMessenger.hh"
#include "MRKElementField.hh"
#include "MRKField.hh"
#include "G4LogicalVolume.hh"

//  MRKGlobalField - handles the global ElectroMagnetic field
//
//  There is a single G04GlobalField object.
//
//  The field from each individual beamline element is given by a
//  ElementField object. Any number of overlapping ElementField
//  objects can be added to the global field. Any element that
//  represents an element with an EM field must add the appropriate
//  ElementField to the global GlobalField object.

typedef std::vector<MRKElementField*> FieldList;
const int MAX_MRK_NUM_FIELDS = 1000;

class MRKGlobalField : public G4ElectroMagneticField {
//class MRKGlobalField : public G4MagneticField {

private:

  MRKGlobalField();
  MRKGlobalField(const MRKGlobalField&);

  ~MRKGlobalField();

  MRKGlobalField& operator=(const MRKGlobalField&);

  void setupArray();

public:

  /// getObject() returns the single MRKGlobalField object.
  /// It is constructed, if necessary.
  static MRKGlobalField* getObject();

  /// GetFieldValue() returns the field value at a given point[].
  /// field is really field[6]: Bx,By,Bz,Ex,Ey,Ez.
  /// point[] is in global coordinates: x,y,z,t.
  void GetFieldValue(const G4double* point, G4double* field) const;

  /// DoesFieldChangeEnergy() returns true.
  G4bool DoesFieldChangeEnergy() const { return true; }

  /// addElementField() adds the ElementField object for a single
  /// element to the global field.
  void addElementField(MRKElementField* f) { if (theFieldList) theFieldList->push_back(f); }

  /// clear() removes all ElementField-s from the global object,
  /// and destroys them. Used before the geometry is completely
  /// re-created.
  void clear();

  /// updates all field tracking objects and clear()
  void updateField();

  /// Set the Stepper types
  void SetStepperType( G4int i ) { fStepperType = i; }

  /// Set the Stepper
  void SetStepper();

  /// Set the minimum step length
  void SetMinStep(G4double s) { minStep = s; }

  /// Set the delta chord length
  void SetDeltaChord(G4double s) { deltaChord = s; }

  /// Set the delta one step length
  void SetDeltaOneStep(G4double s) { deltaOneStep = s; }

  /// Set the delta intersection length
  void SetDeltaIntersection(G4double s) { deltaIntersection = s; }

  /// Set the minimum eps length
  void SetEpsMin(G4double s) { epsMin = s; }

  /// Set the maximum eps length
  void SetEpsMax(G4double s) { epsMax = s; }

  ///Set the largest acceptable stepsize
  void SetLargestStepSize(G4double s) { largestStepSize = s; }

  /// Return the list of Element Fields
  FieldList* getFields() { return theFieldList; }

  ///Creates/loads/adds all fields from fieldSettingsToLoad vector
  void constructFields(G4LogicalVolume* worldLogicVolume);

  void addNamedField(string fieldSettingsName);

   //Field commands for messenger
    void setCurrentFieldSettingsToModify(int inp);
    inline void setFieldPath(string inp){if(currentFieldSettingsToModify >= 0) fieldSettingsToLoad[currentFieldSettingsToModify].fieldFilePath=inp;}
    inline void setFieldHistName(string inp){if(currentFieldSettingsToModify >= 0) fieldSettingsToLoad[currentFieldSettingsToModify].histName=inp;}
    inline void setFieldExtents(G4ThreeVector inp){if(currentFieldSettingsToModify >= 0) fieldSettingsToLoad[currentFieldSettingsToModify].extents=inp;}
    inline void setFieldScalingValue(double inp){if(currentFieldSettingsToModify >= 0) fieldSettingsToLoad[currentFieldSettingsToModify].scalingValue=inp;}
    inline void setFieldSpaceDim(int inp){if(currentFieldSettingsToModify >= 0) fieldSettingsToLoad[currentFieldSettingsToModify].spaceDim=inp;}
    inline void setFieldFieldDim(int inp){if(currentFieldSettingsToModify >= 0) fieldSettingsToLoad[currentFieldSettingsToModify].fieldDim=inp;}
    inline void setFieldAngleX(double inp){if(currentFieldSettingsToModify >= 0) fieldSettingsToLoad[currentFieldSettingsToModify].angleX=inp;}
    inline void setFieldAngleY(double inp){if(currentFieldSettingsToModify >= 0) fieldSettingsToLoad[currentFieldSettingsToModify].angleY=inp;}
    inline void setFieldAngleZ(double inp){if(currentFieldSettingsToModify >= 0) fieldSettingsToLoad[currentFieldSettingsToModify].angleZ=inp;}
    inline void setFieldIsMagnetic(bool inp){if(currentFieldSettingsToModify >= 0) fieldSettingsToLoad[currentFieldSettingsToModify].isMagnetic=inp;}
    inline void setFieldSymmetryX(bool inp){if(currentFieldSettingsToModify >= 0) fieldSettingsToLoad[currentFieldSettingsToModify].symmetry[0]=inp;}
    inline void setFieldSymmetryY(bool inp){if(currentFieldSettingsToModify >= 0) fieldSettingsToLoad[currentFieldSettingsToModify].symmetry[1]=inp;}
    inline void setFieldSymmetryZ(bool inp){if(currentFieldSettingsToModify >= 0) fieldSettingsToLoad[currentFieldSettingsToModify].symmetry[2]=inp;}
    inline void setFieldUseCubicInterpolation(bool inp){if(currentFieldSettingsToModify >= 0) fieldSettingsToLoad[currentFieldSettingsToModify].useCubicInterpolation=inp;}

    inline void setFieldOffset(G4ThreeVector inp){if(currentFieldSettingsToModify >= 0) {fieldSettingsToLoad[currentFieldSettingsToModify].offset=inp;fieldSettingsToLoad[currentFieldSettingsToModify].centerPos+=inp;}}



protected:

  /// Get the global field manager
  G4FieldManager* GetGlobalFieldManager();

private:

  static MRKGlobalField* object;

  G4int numElementFields;
  G4bool first;

  FieldList* theFieldList;

  MRKElementField** elementFieldArray;

  vector<FieldSettings> fieldSettingsToLoad; //Settings to load magnetic or electric fields (only created during initialization);
  int currentFieldSettingsToModify;  //Current fieldSettingsToLoad to modify with macro commands...stupid macro command limitations

private:

  G4int fStepperType;

  G4double minStep;
  G4double deltaChord;
  G4double deltaOneStep;
  G4double deltaIntersection;
  G4double epsMin;
  G4double epsMax;
  G4double largestStepSize;

//  G4Mag_EqRhs*            fEquation;
//  G4Mag_SpinEqRhs*        fEquation;

//  G4EqMagElectricField*   fEquation;
  G4EqMagElectricField*    fEquation;

  G4FieldManager*         fFieldManager;
  G4PropagatorInField*    fFieldPropagator;
  G4MagIntegratorStepper* fStepper;
  G4ChordFinder*          fChordFinder;

  MRKFieldMessenger*         fFieldMessenger;

};

#endif
