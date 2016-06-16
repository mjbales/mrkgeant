/////////////////////////////////////////////////
//cVector- by Matthew Bales
//
//Defines 2D and 3D physics vector classes and allows
//normal operator manimulation of them
//in addition to standard vector operations
///////////////////////////////////////////////////
#ifndef CVECTOR_H_INCLUDED
#define CVECTOR_H_INCLUDED

//Standard Libraries
#include <cmath>
#include <math.h>

//Matt Libraries
#include "MRKConstants.hh"

class MRKVector3D
{
	friend MRKVector3D operator *(const double&, const MRKVector3D&);
public:
	double x, y, z;

	MRKVector3D(const double, const double, const double);
	MRKVector3D();
	void get(double& outX, double& outY, double& outZ);
	const MRKVector3D operator +(const MRKVector3D&) const;
	const MRKVector3D operator -(const MRKVector3D&) const;
	const MRKVector3D& operator =(const MRKVector3D&);
	const MRKVector3D operator *(const double) const;
	const MRKVector3D operator /(const double) const;
	const MRKVector3D& operator +=(const MRKVector3D&);
	const MRKVector3D& operator -=(const MRKVector3D&);
	const MRKVector3D& operator *=(const double);
	const MRKVector3D& operator /=(const double);
	bool operator ==(const MRKVector3D&);
	bool operator !=(const MRKVector3D&);
	double mag();
	double dotProd(const MRKVector3D&);
	MRKVector3D crossProd(const MRKVector3D&);
	MRKVector3D scale(double);
	double gamma(); //For velocity vectors
	void setVal(const double, const double, const double);
	const MRKVector3D multComps(MRKVector3D inp) const;
	const MRKVector3D divComps(MRKVector3D inp) const;
	const MRKVector3D returnTransCoords() const;  //Defined by RDK2
	const MRKVector3D returnNormCoords() const;  //Defined by RDK2
	const MRKVector3D norm();
	double minComp();
	double maxComp();
};

MRKVector3D zeroVec3D();

class MRKVector2D
{
	friend MRKVector2D operator *(const double&, const MRKVector2D&);
public:
	double r, z;
	MRKVector2D(const double, const double);
	MRKVector2D();
	const MRKVector2D operator +(const MRKVector2D&) const;
	const MRKVector2D operator -(const MRKVector2D&) const;
	const MRKVector2D& operator =(const MRKVector2D&);
	const MRKVector2D operator *(const double) const;
	const MRKVector2D operator /(const double) const;
	const MRKVector2D& operator +=(const MRKVector2D&);
	const MRKVector2D& operator -=(const MRKVector2D&);
	const MRKVector2D& operator *=(const double);
	const MRKVector2D& operator /=(const double);
	bool operator ==(const MRKVector2D&);
	bool operator !=(const MRKVector2D&);
	double mag();
	double dotProd(const MRKVector2D&);
	MRKVector2D scale(const double);
	double gamma(); //For velocity vectors
	void setVal(const double, const double);
	const MRKVector2D multComps(MRKVector2D inp) const;
	const MRKVector2D divComps(MRKVector2D inp) const;
};

MRKVector2D zeroVec2D();
#endif // CVECTOR_H_INCLUDED

