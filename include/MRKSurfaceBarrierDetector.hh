#ifndef MRKSurfaceBarrierDetector_h
#define MRKSurfaceBarrierDetector_h 1

#include "G4VSensitiveDetector.hh"
#include "MRKHit.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "TRandom3.h"

#include "MRKText.hh"
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

/// Defines the sensitive detector for the surface barrier detector
class MRKSurfaceBarrierDetector : public G4VSensitiveDetector
{

  public:

      MRKSurfaceBarrierDetector(G4String name);
      ~MRKSurfaceBarrierDetector();

      void Initialize(G4HCofThisEvent* HCE);
      G4bool ProcessHits(G4Step*aStep,G4TouchableHistory* ROhist);
      void EndOfEvent(G4HCofThisEvent *HCE);
      void clear();
      void DrawAll();
      void PrintAll();

  private:
      MRKHitsCollection* mrkCollection;
      G4ParticleDefinition* protonParDef;
      G4ParticleDefinition* electronParDef;
      G4ParticleDefinition* photonParDef;

      int HCID;


  public:
};




#endif
