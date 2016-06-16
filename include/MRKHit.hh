#ifndef MRKHit_h
#define MRKHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

#include "TObject.h"

#include "MRKConstants.hh"

#define MRKHITCODE_NOTHING 0
#define MRKHITCODE_DETIN 1
#define MRKHITCODE_DETOUT 2
#define MRKHITCODE_DETEDEP 3
#define MRKHITCODE_SIO2REJECT 4

#define MRKPARTYPE_NOTYPE 0
#define MRKPARTYPE_PROTON 1
#define MRKPARTYPE_ELECTRON 2
#define MRKPARTYPE_PHOTON 3

class MRKHit: public G4VHit
{
public:

	MRKHit();
	~MRKHit();
	MRKHit(const MRKHit &right);
	const MRKHit& operator=(const MRKHit &right);
	G4int operator==(const MRKHit &right) const;

	inline void *operator new(size_t);
	inline void operator delete(void *aHit);

	void Draw();
	void Print();

private:
	G4ThreeVector mom; //Momentum
	G4ThreeVector pos; //Position
	G4double time; //Time
	G4double eDep;  //Energy Deposit
	G4double kE;  //Kinetic Energy
	Char_t par;  //Particle type
	Char_t hitCode;  //hit direction

public:
	inline void SetHitCode(Char_t inpHitCode)
	{
		hitCode = inpHitCode;
	}
	inline Char_t GetHitCode()
	{
		return hitCode;
	}
	inline void SetTime(G4double inpTime)
	{
		time = inpTime;
	}
	inline G4double GetTime()
	{
		return time;
	}
	inline void SetKE(G4double inpKE)
	{
		kE = inpKE;
	}
	inline G4double GetKE()
	{
		return kE;
	}
	inline void SetParType(Char_t inpParType)
	{
		par = inpParType;
	}
	inline Char_t GetParType()
	{
		return par;
	}
	inline void SetEDep(G4double inpEDep)
	{
		eDep = inpEDep;
	}
	inline G4double GetEDep()
	{
		return eDep;
	}
	inline void SetPos(G4ThreeVector xyz)
	{
		pos = xyz;
	}
	inline G4ThreeVector GetPos()
	{
		return pos;
	}
	inline void SetMom(G4ThreeVector xyz)
	{
		mom = xyz;
	}
	inline G4ThreeVector GetMom()
	{
		return mom;
	}

};

typedef G4THitsCollection<MRKHit> MRKHitsCollection;

extern G4Allocator<MRKHit> MRKHitAllocator;

inline void* MRKHit::operator new(size_t)
{
	void *aHit;
	aHit = (void *) MRKHitAllocator.MallocSingle();
	return aHit;
}

inline void MRKHit::operator delete(void *aHit)
{
	MRKHitAllocator.FreeSingle((MRKHit*) aHit);
}

#endif
