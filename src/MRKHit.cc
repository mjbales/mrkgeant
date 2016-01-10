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

#include "MRKHit.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4UnitsTable.hh"
#include "G4AttValue.hh"
#include "G4AttDef.hh"
#include "G4AttCheck.hh"

G4Allocator<MRKHit> MRKHitAllocator;

MRKHit::MRKHit()
{
    eDep=0;
    time=0;
    par=MRKPARTYPE_NOTYPE;
    hitCode=0;
    kE=0;
}

MRKHit::~MRKHit()
{;}

MRKHit::MRKHit(const MRKHit &right)
  : G4VHit()
{
  eDep = right.eDep;
  time = right.time;
  pos = right.pos;
  mom = right.mom;
  par = right.par;
  hitCode = right.hitCode;
  kE=right.kE;
}

const MRKHit& MRKHit::operator=(const MRKHit &right)
{
  eDep = right.eDep;
  pos = right.pos;
  mom = right.mom;
  time = right.time;
  par = right.par;
  hitCode=right.hitCode;
  kE=right.kE;
  return *this;
}

G4int MRKHit::operator==(const MRKHit &right) const
{
  return (this==&right) ? 1 : 0;
}


void MRKHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle(pos);
    circle.SetScreenSize(0.04);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1.,0.,0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}



void MRKHit::Print()
{;}

