#ifndef MRKElementField_h
#define MRKElementField_h 1

#include "globals.hh"

#include "G4Navigator.hh"
#include "G4TransportationManager.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

#include "MRKConstants.hh"

//  class MRKElementField - interface for the EM field of one element

//  This is the interface class used by GlobalField to compute the field
//  value at a given point[].

//  An element that represents an element with an EM field will
//  derive a class from this one and implement the computation for the
//  element. The construct() function will add the derived object into
//  GlobalField.
class MRKElementField
{

private:

	MRKElementField& operator=(const MRKElementField&);

public:

	///  Constructor.
	MRKElementField(const G4ThreeVector, G4LogicalVolume*);

	/// the actual implementation constructs the MRKElementField
	void construct();

    ///  Destructor.
    virtual ~MRKElementField() { if (aNavigator) delete aNavigator; }

    /// setMaxStep(G4double) sets the max. step size
    void setMaxStep(G4double s)
    {
      maxStep = s;
      userLimits->SetMaxAllowedStep(maxStep);
      //userLimits->SetUserMaxTime(PROTON_MAX_TIME*second);
      lvolume->SetUserLimits(userLimits);
    }

    /// getMaxStep() returns the max. step size
    G4double getMaxStep() { return maxStep; }

    /// setColor(G4String) sets the color
    void setColor(G4String c)
    {
      color = c;
      lvolume->SetVisAttributes(getVisAttribute(color));
    }

    /// getColor() returns the color
    G4String getColor() { return color; }

    ///  getVisAttribute() returns the appropriate G4VisAttributes.
    static G4VisAttributes* getVisAttribute(G4String color);

    ///  setGlobalPoint() ensures that the point is within the global
    ///  bounding box of this ElementField's global coordinates.
    ///  Normally called 8 times for the corners of the local bounding
    ///  box, after a local->global coordinate transform.
    ///  If never called, the global bounding box is infinite.
    ///  BEWARE: if called only once, the bounding box is just a point.
    void setGlobalPoint(const G4double point[4])
    {
      if(minX == -DBL_MAX || minX > point[0]) minX = point[0];
      if(minY == -DBL_MAX || minY > point[1]) minY = point[1];
      if(minZ == -DBL_MAX || minZ > point[2]) minZ = point[2];
      if(maxX ==  DBL_MAX || maxX < point[0]) maxX = point[0];
      if(maxY ==  DBL_MAX || maxY < point[1]) maxY = point[1];
      if(maxZ ==  DBL_MAX || maxZ < point[2]) maxZ = point[2];
    }

    ///  isInBoundingBox() returns true if the point is within the
    ///  global bounding box - global coordinates.
    bool isInBoundingBox(const G4double point[4]) const
    {
      if(point[2] < minZ || point[2] > maxZ) return false;
      if(point[0] < minX || point[0] > maxX) return false;
      if(point[1] < minY || point[1] > maxY) return false;
      return true;
    }

    ///  addFieldValue() will add the field value for this element to field[].
    ///  Implementations must be sure to verify that point[] is within
    ///  the field region, and do nothing if not.
    ///  point[] is in global coordinates and geant4 units; x,y,z,t.
    ///  field[] is in geant4 units; Bx,By,Bz,Ex,Ey,Ez.
    ///  For efficiency, the caller may (but need not) call
    ///  isInBoundingBox(point), and only call this function if that
    ///  returns true.
    virtual void addFieldValue(const G4double point[4], G4double field[6]) = 0;

    virtual G4double getLength() = 0;
    virtual G4double getWidth()  = 0;
    virtual G4double getHeight() = 0;

protected:

	G4LogicalVolume* lvolume;

	G4AffineTransform global2local;

//    MRKElementField(const MRKElementField&);

private:

	static G4Navigator* aNavigator;

	G4String color;

	G4ThreeVector center;
	G4double minX, minY, minZ, maxX, maxY,maxZ;

	G4double maxStep;
	G4UserLimits* userLimits;

};

#endif
