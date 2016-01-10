/////////////////////////////////////////////////
//cVector- by Matthew Bales
//
//Defines 2D and 3D physics vector classes and allows
//normal operator manimulation of them
//in addition to standard vector operations
///////////////////////////////////////////////////
#ifndef CVECTOR_H_INCLUDED
#define CVECTOR_H_INCLUDED
#pragma once

//Standard Libraries
#include <cmath>
#include <math.h>

//Matt Libraries
#include "constants.hh"

class cVector3D{
	friend cVector3D operator * (const double&, const cVector3D&);
public:
	double x, y, z;


	cVector3D(const double,const double,const double);
	cVector3D();
	void get(double& outX, double& outY, double& outZ);
	const cVector3D operator + (const cVector3D&) const;
	const cVector3D operator - (const cVector3D&) const;
	const cVector3D& operator = (const cVector3D&);
	const cVector3D operator * (const double) const;
	const cVector3D operator / (const double) const;
	const cVector3D& operator += (const cVector3D&);
	const cVector3D& operator -= (const cVector3D&);
	const cVector3D& operator *= (const double);
	const cVector3D& operator /= (const double);
	bool operator == (const cVector3D&);
	bool operator != (const cVector3D&);
	double mag();
	double dotProd(const cVector3D&);
	cVector3D crossProd(const cVector3D&);
	cVector3D scale(double);
	double gamma(); //For velocity vectors
	void setVal(const double, const double, const double);
	const cVector3D multComps(cVector3D inp) const;
	const cVector3D divComps(cVector3D inp) const;
	const cVector3D returnTransCoords() const;  //Defined by RDK2
	const cVector3D returnNormCoords() const;  //Defined by RDK2
	const cVector3D norm();
	double minComp();
	double maxComp();
};

cVector3D zeroVec3D();

class cVector2D{
	friend cVector2D operator * (const double&, const cVector2D&);
public:
	double r,z;
	cVector2D(const double,const double);
	cVector2D();
	const cVector2D operator + (const cVector2D&) const;
	const cVector2D operator - (const cVector2D&) const;
	const cVector2D& operator = (const cVector2D&);
	const cVector2D operator * (const double) const;
	const cVector2D operator / (const double) const;
	const cVector2D& operator += (const cVector2D&);
	const cVector2D& operator -= (const cVector2D&);
	const cVector2D& operator *= (const double);
	const cVector2D& operator /= (const double);
	bool operator == (const cVector2D&);
	bool operator != (const cVector2D&);
	double mag();
	double dotProd(const cVector2D&);
	cVector2D scale(const double);
	double gamma(); //For velocity vectors
	void setVal(const double, const double);
	const cVector2D multComps(cVector2D inp) const;
	const cVector2D divComps(cVector2D inp) const;
};

cVector2D zeroVec2D();
#endif // CVECTOR_H_INCLUDED

