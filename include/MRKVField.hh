/////////////////////////////////////////////////
//cVField- by Matthew Bales
//
//Should be really renamed cField as I now use more than just
//a vector field.
//
//Contains classes used to define 2D and 3D scalar and vector fields
//Since ROOT doesn't like "complicated" STL definitions, I couldn't do
//exactly the most beautiful implementation I wanted...but oh well
///////////////////////////////////////////////////
#pragma once
#ifndef CVFIELD_H_INCLUDED
#define CVFIELD_H_INCLUDED

//Standard Libraries
#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string.h>
#include <cmath>
#include <vector>

#include "MRKConstants.hh"
#include "MRKText.hh"
#include "MRKVector.hh" //Math/Physics vectors

//File should be called cField.h  but I haven't renamed it yet


//Structure that contains/calculates vectors and derivatives at point (for cubic interpolation)
struct sVandD3D
{
	MRKVector3D val;
	MRKVector3D dx,dy,dz,dxy,dxz,dyz,dxyz;

};

struct sVandD2D
{
	MRKVector2D val;
	MRKVector2D dr,dz,drz;
};

/////////////////////////////////////////////////////////////////////////////////////////////////

//Base class for field that is in three dimensial space
class cField3D{
protected:
    int rows;			//x
    int columns;		//y
    int layers;			//z
	double xStart;
	double yStart;
	double zStart;
	double xSpacing;
	double ySpacing;
	double zSpacing;
	double xFinal;
	double yFinal;
	double zFinal;
    int arraySize;		//rows*columns*layers
	bool fileLoaded;
public:
    cField3D();
    virtual ~cField3D(){};
    virtual void reset(){};
    void calcPositionLimits();
    bool isPositionOutsideField(MRKVector3D pos);
   // virtual int loadField(const char*,double scalingValue){return 0;};
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Scalar field in 3D
class cSField3D:public cField3D{
protected:
	std::vector<std::vector<std::vector<double> > > fArray;

    //Linear Interp
    double xFloor,yFloor,zFloor;
	double xMod,yMod,zMod;
	double result;
    int xF,yF,zF;

public:
	cSField3D();
	~cSField3D();
	void reset();
	int loadField(const char*,double scalingValue);
	double getValue( int, int, int);
	void linearInterp(const MRKVector3D& pos,double& valOut);
	void cubicInterp(const MRKVector3D& pos,double& valOut){linearInterp(pos,valOut);}//Currently just wrapping linear till I update it
	void setSize( int, int, int); //Clears values and then resizes
	void setValue( int inpRow, int inpColumn, int inpLayer,double inpVal);
};


//////////////////////////////////////////////////////////////////////////////////////////////////

//Vector field in 3D
class cVField3D:public cField3D{
protected:
	std::vector<std::vector<std::vector<double> > > xArray;
	std::vector<std::vector<std::vector<double> > > yArray;
	std::vector<std::vector<std::vector<double> > > zArray;

    //Linear Interp
    double xFloor,yFloor,zFloor;
	double xMod,yMod,zMod,xModInv,yModInv,zModInv;
	double xResult,yResult,zResult;
    int xF,yF,zF,xF2,yF2,zF2;
    double oldFieldBox[3][2][2][2]={{{{}}}};
    bool symmetryInY;  //is the y dimension mirrored

	//Cubic Interpolation
	MRKVector3D a0,a1,a2,a3,df1D2,df2D2; //Variables used in cubic interpolation. Global in class for speed.
	double mu2, mu3; //Variables used in cubic interpolation.  Global in class for speed.
	sVandD3D vAndD[14];  //Variable for cubic interpolation. Global in class for speed.

public:
	cVField3D();
	~cVField3D();
	void reset();
	int loadField(const char*,double scalingValue);
	const MRKVector3D getVector( int, int, int);
	void linearInterp(const MRKVector3D& pos,MRKVector3D& vecOut);
	void linearInterp2(const MRKVector3D& pos,MRKVector3D& vecOut);
	void cubicInterp(const MRKVector3D& pos,MRKVector3D& vecOut);
	void setSize( int, int, int); //Clears values and then resizes
	void setVector( int inpRow, int inpColumn, int inpLayer,MRKVector3D inpVec);
	inline void setSymmetryInY(bool inp){symmetryInY=inp;};
	void convertToROOTFile(std::string rootPathOut);


private:
    //cubic interpolation
    void derivatives(int i, int j, int k, MRKVector3D& dx, MRKVector3D& dy, MRKVector3D& dz, MRKVector3D& dxy, MRKVector3D& dxz, MRKVector3D& dyz, MRKVector3D& dxyz);
    MRKVector3D derivative(MRKVector3D start, MRKVector3D finish);
    MRKVector3D cubicInterpolate1DVec(double mu,const MRKVector3D& f1,const MRKVector3D& f2,const MRKVector3D& df1,const MRKVector3D& df2);



};


////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Base class for field that is in 2 dimensial space
class cField2D{
protected:
    int rows;			//x
    int columns;		//y
    double rStart;
    double zStart;
    double rFinal;
    double zFinal;
    double rSpacing;
    double zSpacing;
    int arraySize;		//rows*columns*layers
	bool fileLoaded;

	//RDK only varialbes
	bool transposedField;  //For RDK since the SBD is at an angle
	MRKVector3D posInField;  //For storing tranposed vector..stored here for speed
public:
    cField2D();
    virtual ~cField2D(){};
    virtual void reset(){};
   // virtual int loadField(const char*,double scalingValue){return 0;};
    int getRows(){return rows;};
    int getColumns(){return columns;};
    int getArraySize(){return arraySize;};
    double getRStart(){return rStart;};
    double getZStart(){return zStart;};
    double getRSpacing(){return rSpacing;};
    double getZSpacing(){return zSpacing;};
    bool getFileLoaded(){return fileLoaded;};
    void setTransposedField(bool inp){transposedField=inp;};

    void calcPositionLimits();
    bool isPositionOutsideField(double zPosition, double radius);

};

class cSField2D:public cField2D{
protected:
	std::vector<std::vector<double> > fArray;  //Multidimensional storage vector of math/physics vectors...confusing I know

	//Linear Interp
	double rFloor,zFloor;
	double rMod,zMod;
	double rResult,zResult;
    int rF,zF;
	double radius;
	double result;

public:
	cSField2D();
	~cSField2D();
	void reset();
	int loadField(const char*,double scalingValue);
	double getValue( int, int);
	void linearInterpAs3D(const MRKVector3D& pos,double& valOut);
	void linearInterp(const MRKVector2D& pos, double& valOut);
	void cubicInterpAs3D(const MRKVector3D& pos,double& valOut){linearInterpAs3D(pos,valOut);}//Currently just wrapping linear till I update it
	void setValue( int inpRow, int inpColumn, double inpVal);
    void setSize( int, int); //Clears values and then resizes


};


//For cylindrically symmetric fields
class cVField2D:public cField2D{
protected:
	std::vector<std::vector<double> > rArray;
	std::vector<std::vector<double> > zArray;

    //2D Interp
    double rFloor,zFloor;
	double rMod,zMod;
	double rResult,zResult;
    int rF,zF;
	double radius;




public:
	cVField2D();
	~cVField2D();
	void reset();
	int loadField(const char*,double);
	MRKVector2D getVector( int, int);
	void linearInterpAs3D(const MRKVector3D& pos,MRKVector3D& vecOut);
	void linearInterpAs3D2(const MRKVector3D& pos,MRKVector3D& vecOut);
	void linearInterpAs3DTransposed2(const MRKVector3D& pos,MRKVector3D& vecOut);
	void cubicInterpAs3D(const MRKVector3D& pos,MRKVector3D& vecOut){linearInterpAs3D(pos,vecOut);}//Currently just wrapping linear till I update it
	void changeScalingPotential(double inpPotential);
	void setVector( int inpRow, int inpColumn,MRKVector2D inpVec);
    void setSize( int, int); //Clears values and then resizes

};


#endif // CVFIELD_H_INCLUDED
