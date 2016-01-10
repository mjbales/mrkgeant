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

#include <time.h>

#include "Randomize.hh"
#include "G4TransportationManager.hh"

#include "G4ExplicitEuler.hh"
#include "G4ImplicitEuler.hh"
#include "G4SimpleRunge.hh"
#include "G4SimpleHeum.hh"
#include "G4ClassicalRK4.hh"
#include "G4CashKarpRKF45.hh"

#include "MRKGlobalField.hh"

MRKGlobalField* MRKGlobalField::object = 0;
/*
MRKGlobalField::MRKGlobalField() : G4ElectroMagneticField(),
                             minStep(0.0001*mm), deltaChord(3.0*mm),
                             deltaOneStep(0.0001*mm), deltaIntersection(0.0001*mm),
                             epsMin(2.5e-7*mm),
                             epsMax(0.0001*mm),
                             //epsMax(*mm),
                             fEquation(0), fFieldManager(0),
                             fFieldPropagator(0), fStepper(0), fChordFinder(0)
*/
MRKGlobalField::MRKGlobalField() : G4ElectroMagneticField(),
                             minStep(0.001*mm), deltaChord(0.1*mm),
                             deltaOneStep(0.0001*mm), deltaIntersection(0.0001*mm),
                             epsMin(2.5e-7*mm), epsMax(0.001*mm), largestStepSize(1*mm),
                             fEquation(0), fFieldManager(0),
                             fFieldPropagator(0), fStepper(0), fChordFinder(0)
{
  fFieldMessenger = new MRKFieldMessenger(this);

  theFieldList = new FieldList();

  fStepperType = 4;       // ClassicalRK4 is default stepper

  //  set object

  object = this;

  updateField();
  currentFieldSettingsToModify=-1;
}

MRKGlobalField::~MRKGlobalField()
{
    G4cout << "Beginning to delete Global Field...";
  clear();

  delete fFieldMessenger;

  if (fEquation)        delete fEquation;
  if (fFieldManager)    delete fFieldManager;
  if (fFieldPropagator) delete fFieldPropagator;
  if (fStepper)         delete fStepper;
  if (fChordFinder)     delete fChordFinder;
  G4cout << "COMPLETED!" << G4endl;
}

void MRKGlobalField::updateField()
{
  first = true;

  numElementFields = 0;
  elementFieldArray = 0;

  clear();

  //  Construct equ. of motion of particles through B fields
//  fEquation = new G4Mag_EqRhs(this);
  //  Construct equ. of motion of particles through e.m. fields
  fEquation = new G4EqMagElectricField(this);
  //  Construct equ. of motion of particles including spin through B fields
//  fEquation = new G4Mag_SpinEqRhs(this);
  //  Construct equ. of motion of particles including spin through e.m. fields
//  fEquation = new G4EqEMFieldWithSpin(this);

  //  Get transportation, field, and propagator managers
  G4TransportationManager* fTransportManager =
         G4TransportationManager::GetTransportationManager();

  fFieldManager = GetGlobalFieldManager();

  fFieldPropagator = fTransportManager->GetPropagatorInField();

  //  Need to SetFieldChangesEnergy to account for a time varying electric
  //  field (r.f. fields)
  fFieldManager->SetFieldChangesEnergy(true);

  //  Set the field
  fFieldManager->SetDetectorField(this);

  //  Choose a stepper for integration of the equation of motion
  SetStepper();

  //  Create a cord finder providing the (global field, min step length,
  //  a pointer to the stepper)
  fChordFinder = new G4ChordFinder((G4MagneticField*)this,minStep,fStepper);

  // Set accuracy parameters
  fChordFinder->SetDeltaChord( deltaChord );

  fFieldManager->SetAccuraciesWithDeltaOneStep(deltaOneStep);

  fFieldManager->SetDeltaIntersection(deltaIntersection);

  fFieldManager->SetMinimumEpsilonStep(epsMin);
  fFieldManager->SetMaximumEpsilonStep(epsMax);
  fFieldPropagator->SetLargestAcceptableStep(largestStepSize);

  G4cout << "Accuracy Parameters:" <<

            " MinStep=" << minStep <<
            " DeltaChord=" << deltaChord <<
            " DeltaOneStep=" << deltaOneStep << G4endl;
  G4cout << "                    " <<
            " DeltaIntersection=" << deltaIntersection <<
            " EpsMin=" << epsMin <<
            " EpsMax=" << epsMax <<  G4endl <<
            " LargestStepSize=" << largestStepSize <<  G4endl;

  fFieldManager->SetChordFinder(fChordFinder);

}

MRKGlobalField* MRKGlobalField::getObject()
{
  if (!object) new MRKGlobalField();
  return object;
}

void MRKGlobalField::SetStepper()
{
  if(fStepper) delete fStepper;

  switch ( fStepperType )
  {
    case 0:
      fStepper = new G4ExplicitEuler( fEquation, 8 ); // no spin tracking
//      fStepper = new G4ExplicitEuler( fEquation, 12 ); // with spin tracking
      G4cout << "G4ExplicitEuler is called" << G4endl;
      break;
    case 1:
      fStepper = new G4ImplicitEuler( fEquation, 8 ); // no spin tracking
//      fStepper = new G4ImplicitEuler( fEquation, 12 ); // with spin tracking
      G4cout << "G4ImplicitEuler is called" << G4endl;
      break;
    case 2:
      fStepper = new G4SimpleRunge( fEquation, 8 ); // no spin tracking
//      fStepper = new G4SimpleRunge( fEquation, 12 ); // with spin tracking
      G4cout << "G4SimpleRunge is called" << G4endl;
      break;
    case 3:
      fStepper = new G4SimpleHeum( fEquation, 8 ); // no spin tracking
//      fStepper = new G4SimpleHeum( fEquation, 12 ); // with spin tracking
      G4cout << "G4SimpleHeum is called" << G4endl;
      break;
    case 4:
      fStepper = new G4ClassicalRK4( fEquation,8); // no spin tracking
//      fStepper = new G4ClassicalRK4( fEquation, 12 ); // with spin tracking
      G4cout << "G4ClassicalRK4 (default) is called" << G4endl;
      break;
    case 5:
      fStepper = new G4CashKarpRKF45( fEquation, 8 ); // no spin tracking
//      fStepper = new G4CashKarpRKF45( fEquation, 12 ); // with spin tracking
      G4cout << "G4CashKarpRKF45 is called" << G4endl;
      break;
    default: fStepper = 0;
  }
}

G4FieldManager* MRKGlobalField::GetGlobalFieldManager()
{
  return G4TransportationManager::GetTransportationManager()
                                ->GetFieldManager();
}

void MRKGlobalField::GetFieldValue(const G4double* point, G4double* outField) const
{
  // NOTE: this routine dominates the CPU time for tracking.
  // Using the simple array outField[] instead of fields[]
  // directly sped it up
  outField[0]=0;
  outField[1]=0;
  outField[2]=0;
  outField[3]=0;
  outField[4]=0;
  outField[5]=0;


  // protect against Geant4 bug that calls us with point[] NaN.
  if(point[0] != point[0]) return;

  // (can't use nfp or outField, as they may change)
  if (first) ((MRKGlobalField*)this)->setupArray();   // (cast away const)

  for (int i=0; i<numElementFields; ++i) {
      MRKElementField* theElementField = elementFieldArray[i];

      //if (theElementField->isInBoundingBox(point)) {
         theElementField->addFieldValue(point,outField);
      //}
  }

}

void MRKGlobalField::clear()
{
  if (theFieldList) {
     if (theFieldList->size()>0) {
        FieldList::iterator i;
        for (i=theFieldList->begin(); i!=theFieldList->end(); ++i) delete *i;
        theFieldList->clear();
     }
  }

  if (elementFieldArray) delete[] elementFieldArray;

  first = true;

  numElementFields = 0;
  elementFieldArray = NULL;
}

void MRKGlobalField::setupArray()
{
  first = false;
  numElementFields = theFieldList->size();
  elementFieldArray = new MRKElementField* [numElementFields+1]; // add 1 so it's never 0
  for (int i=0; i<numElementFields; ++i) elementFieldArray[i] = (*theFieldList)[i];
}

void MRKGlobalField::addNamedField(string fieldSettingsName)
{
    FieldSettings oldSolenoidFieldSettings;
    oldSolenoidFieldSettings.isMagnetic=true;
    oldSolenoidFieldSettings.fieldFilePath="Bfield_RDK2_Matt2.root";
    oldSolenoidFieldSettings.histName="solenoid";
    oldSolenoidFieldSettings.scalingValue=1;
    oldSolenoidFieldSettings.symmetry[1]=true;

    FieldSettings oldMirrorFieldSettings;
    oldMirrorFieldSettings.isMagnetic=false;
    oldMirrorFieldSettings.fieldFilePath="Efield_RDK2_Mirror_Matt4.root";
    oldMirrorFieldSettings.histName="mirror";
    oldMirrorFieldSettings.scalingValue=1400;
    oldMirrorFieldSettings.spaceDim=2;
    oldMirrorFieldSettings.fieldDim=2;

    FieldSettings oldSBDFieldSettings;
    oldSBDFieldSettings.isMagnetic=false;
    oldSBDFieldSettings.fieldFilePath="Efield_RDK2_SBD_Matt5.root";
    oldSBDFieldSettings.histName="sbd";
    oldSBDFieldSettings.scalingValue=-25000;
    oldSBDFieldSettings.spaceDim=2;
    oldSBDFieldSettings.fieldDim=2;
    oldSBDFieldSettings.angleY=-9.5*PI/180.;

    FieldSettings mattMirrorFieldSettings;
    mattMirrorFieldSettings.isMagnetic=false;
    mattMirrorFieldSettings.fieldFilePath="COMSOL_MirrorElectricField_Fine130515.root";
    mattMirrorFieldSettings.histName="mirror";
    mattMirrorFieldSettings.scalingValue=1400;

    FieldSettings mattSBDFieldSettings;
    mattSBDFieldSettings.isMagnetic=false;
    mattSBDFieldSettings.fieldFilePath="COMSOL_SBDElectricField_Fine130515.root";
    mattSBDFieldSettings.histName="sbd";
    mattSBDFieldSettings.scalingValue=-25000;
    mattSBDFieldSettings.offset = G4ThreeVector( -0.31359045121*mm,0,-1.87394264285*mm);

    FieldSettings mattBAPDFieldSettings;
    mattBAPDFieldSettings.isMagnetic=false;
    mattBAPDFieldSettings.fieldFilePath="COMSOL_BAPDOnlyField_A_1350V_140418.root";
    mattBAPDFieldSettings.histName="bapd";
    mattBAPDFieldSettings.scalingValue=1;
    oldSolenoidFieldSettings.symmetry[1]=true;  //should recalc as it isn't completely symmetric

    FieldSettings bryceSBDMirrorBAPD_1;
    bryceSBDMirrorBAPD_1.isMagnetic=false;
    bryceSBDMirrorBAPD_1.fieldFilePath="COMSOL_SBD_Mirror_BAPDElectricField_SuperFine130724_1.root";
    bryceSBDMirrorBAPD_1.histName="total1";
    bryceSBDMirrorBAPD_1.scalingValue=1;
    bryceSBDMirrorBAPD_1.symmetry[1]=true;
    bryceSBDMirrorBAPD_1.centerPos=G4ThreeVector(2,0,-17.5);
    bryceSBDMirrorBAPD_1.extents=G4ThreeVector(20,12,25);

    FieldSettings bryceSBDMirrorBAPD_2;
    bryceSBDMirrorBAPD_2.isMagnetic=false;
    bryceSBDMirrorBAPD_2.fieldFilePath="COMSOL_SBD_Mirror_BAPDElectricField_SuperFine130724_2.root";
    bryceSBDMirrorBAPD_2.histName="total2";
    bryceSBDMirrorBAPD_2.scalingValue=1;
    bryceSBDMirrorBAPD_2.symmetry[1]=true;
    bryceSBDMirrorBAPD_2.centerPos=G4ThreeVector(2,0,7.525);
    bryceSBDMirrorBAPD_2.extents=G4ThreeVector(20,12,24.95);

    FieldSettings bryceSBDMirrorBAPD_3;
    bryceSBDMirrorBAPD_3.isMagnetic=false;
    bryceSBDMirrorBAPD_3.fieldFilePath="COMSOL_SBD_Mirror_BAPDElectricField_SuperFine130724_3.root";
    bryceSBDMirrorBAPD_3.histName="total3";
    bryceSBDMirrorBAPD_3.scalingValue=1;
    bryceSBDMirrorBAPD_3.symmetry[1]=true;
    bryceSBDMirrorBAPD_3.centerPos=G4ThreeVector(2,0,32.525);
    bryceSBDMirrorBAPD_3.extents=G4ThreeVector(20,12,24.95);

    FieldSettings bryceSBDMirrorBAPD_4;
    bryceSBDMirrorBAPD_4.isMagnetic=false;
    bryceSBDMirrorBAPD_4.fieldFilePath="COMSOL_SBD_Mirror_BAPDElectricField_SuperFine130724_4.root";
    bryceSBDMirrorBAPD_4.histName="total4";
    bryceSBDMirrorBAPD_4.scalingValue=1;
    bryceSBDMirrorBAPD_4.symmetry[1]=true;
    bryceSBDMirrorBAPD_4.centerPos=G4ThreeVector(2,0,57.525);
    bryceSBDMirrorBAPD_4.extents=G4ThreeVector(20,12,24.95);


    if(fieldSettingsName == "OLD_RDK2")
    {
        cout << "Using Old RDK Fields" << endl;
        fieldSettingsToLoad.push_back(oldSolenoidFieldSettings);
        fieldSettingsToLoad.push_back(oldMirrorFieldSettings);
        fieldSettingsToLoad.push_back(oldSBDFieldSettings);
    }
    else if(fieldSettingsName == "NEW_RDK2_MATT")
    {
        cout << "Using New RDK Fields" << endl;
        //Still use old magnetic field for now
        fieldSettingsToLoad.push_back(oldSolenoidFieldSettings);
        fieldSettingsToLoad.push_back(mattMirrorFieldSettings);
        fieldSettingsToLoad.push_back(mattSBDFieldSettings);
    }
    else if(fieldSettingsName=="NEW_RDK2_MATT_WBAPD")
    {
        fieldSettingsToLoad.push_back(oldSolenoidFieldSettings);
        fieldSettingsToLoad.push_back(mattMirrorFieldSettings);
        fieldSettingsToLoad.push_back(mattSBDFieldSettings);
        fieldSettingsToLoad.push_back(mattBAPDFieldSettings);

    }
    else if(fieldSettingsName == "NEW_RDK2_BRYCE")
    {
        cout << "Using New RDK Fields: Bryce edition" << endl;
          //Still use old magnetic field for now
        fieldSettingsToLoad.push_back(oldSolenoidFieldSettings);
        fieldSettingsToLoad.push_back(bryceSBDMirrorBAPD_1);
        fieldSettingsToLoad.push_back(bryceSBDMirrorBAPD_2);
        fieldSettingsToLoad.push_back(bryceSBDMirrorBAPD_3);
        fieldSettingsToLoad.push_back(bryceSBDMirrorBAPD_4);
    }
    if(fieldSettingsName == "SIMPLE_LIFETIME")
    {
        cout << "Using Old RDK Fields for lifetime" << endl;
        fieldSettingsToLoad.push_back(oldSolenoidFieldSettings);
        fieldSettingsToLoad.push_back(oldSBDFieldSettings);
    }
}

void MRKGlobalField::setCurrentFieldSettingsToModify(int inp)
{
    if(inp >=0 && inp < MAX_MRK_NUM_FIELDS) //Is the number within a reasonable range
    {
        currentFieldSettingsToModify=inp;
        if(fieldSettingsToLoad.size() <=(unsigned int) inp) //Do we need to grow the vector
        {
            fieldSettingsToLoad.resize(inp+1); //New FieldSettings that are created are not loaded because their scaling value is set to zero by default
        }
    }
    else
    {
        G4cout << "Field to modify not within appropriate range of 0 to " << MAX_MRK_NUM_FIELDS << "." << G4endl;
    }
}

void MRKGlobalField::constructFields(G4LogicalVolume* worldLogicVolume)
{

    for (unsigned int i = 0;i< fieldSettingsToLoad.size();i++)
    {
        if(fieldSettingsToLoad[i].scalingValue != 0) //Only load fields if their scaling values are not zero
        {
//            addElementField(new MRKField(worldLogicVolume,fieldSettingsToLoad[i]));
            new MRKField(worldLogicVolume,fieldSettingsToLoad[i]);
        }
    }
    return;
}
