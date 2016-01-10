/*
 *  cVector.cce
 *  Tracker
 *
 *  Created by Matthew Bales on 3/24/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "cVector.hh"



cVector3D::cVector3D(const double inpX, const double inpY, const double inpZ)
{
	x=inpX;
	y=inpY;
	z=inpZ;
}

cVector3D::cVector3D()
{
	x=0;
	y=0;
	z=0;
}

void cVector3D::get(double& outX, double& outY, double& outZ){
    outX=x;
    outY=y;
    outZ=z;
}

const cVector3D cVector3D::operator+(const cVector3D& rhs) const
{
	cVector3D result = *this;
	result += rhs;
	return result;
}

const cVector3D cVector3D::operator-(const cVector3D& rhs) const
{
	cVector3D result = *this;
	result -= rhs;
	return result;
}

const cVector3D& cVector3D::operator=(const cVector3D& inpVec)
{
	x = inpVec.x;
	y = inpVec.y;
	z = inpVec.z;
	return *this;
}

const cVector3D& cVector3D::operator += (const cVector3D& rhs)
{
	x +=rhs.x;
	y +=rhs.y;
	z +=rhs.z;
	return *this;
}
const cVector3D& cVector3D::operator -= (const cVector3D& rhs)
{
	x -=rhs.x;
	y -=rhs.y;
	z -=rhs.z;
	return *this;
}
const cVector3D& cVector3D::operator *= (double rhs)
{
	x *= rhs;
	y *= rhs;
	z *= rhs;
	return *this;
}

//Should throw exception if rhs is 0...but doesn't yet
const cVector3D& cVector3D::operator /= (double rhs)
{
	x /=rhs;
	y /=rhs;
	z /=rhs;
	return *this;
}


const cVector3D cVector3D::operator * (const double rhs) const
{
	cVector3D result = *this;
	result *= rhs;
	return result;
}

cVector3D operator* (const double& lhs, const cVector3D& rhs)
{
	cVector3D result = rhs;
	result *= lhs;
	return result;
}

const cVector3D cVector3D::operator / (const double rhs) const
{
	cVector3D result = *this;
	result /= rhs;
	return result;
}

bool cVector3D::operator== (const cVector3D& inpVec)
{
	if(x==inpVec.x && y==inpVec.y && z==inpVec.z){
		return true;
	}
	else{
		return false;
	}
}

bool cVector3D::operator != (const cVector3D& rhs)
{
	return !(*this == rhs);
}

double cVector3D::mag()
{
	return sqrt(x*x+y*y+z*z);
}

cVector3D cVector3D::crossProd(const cVector3D& v)
{
    cVector3D answer(y*v.z-v.y*z,-x*v.z+v.x*z,x*v.y-v.x*y);
    return answer;
}
double cVector3D::dotProd(const cVector3D& inpVec)
{
	return x*inpVec.x+y*inpVec.y+z*inpVec.z;
}

cVector3D cVector3D::scale(const double scaling)
{
	*this *= scaling;
	return *this;
}

double cVector3D::gamma()
{
	return 1./sqrt(1.-(x*x+y*y+z*z)*(INVCSQUARED));  //8.99875479107368E+18 = c^2
}
void cVector3D::setVal(const double inpX, const double inpY, const double inpZ)
{
	x=inpX;
	y=inpY;
	z=inpZ;
}

//To solve 3D equations easily (this is not a dot product or cross product
const cVector3D cVector3D::multComps(cVector3D inp) const
{
	cVector3D result = *this;
	result.x *= inp.x;
	result.y *= inp.y;
	result.z *= inp.z;
	return result;
}
//To solve 3D equations easily (this is not a dot product or cross product
const cVector3D cVector3D::divComps(cVector3D inp) const
{
	cVector3D result = *this;
	result.x /= inp.x;
	result.y /= inp.y;
	result.z /= inp.z;
	return result;
}

const cVector3D cVector3D::returnTransCoords() const //rotation of 9.5 degrees
{
	cVector3D result;
	result.x = -z*SIN_BEND+x*COS_BEND;
	result.z = z*COS_BEND+x*SIN_BEND;
	result.y = y;
	return result;
}
const cVector3D cVector3D::returnNormCoords() const  //rotation back 9.5 degrees
{
	cVector3D result;
	result.x = z*SIN_BEND+x*COS_BEND;
	result.z = z*COS_BEND-x*SIN_BEND;
	result.y = y;
	return result;
}

const cVector3D cVector3D::norm()
{
	cVector3D result;
	double oneOverMag = 1/this->mag();

	result.x = x * oneOverMag;
	result.y = y * oneOverMag;
	result.z = z * oneOverMag;
	return result;
}

double cVector3D::minComp()
{
	double min =std::abs(x);
	if(std::abs(y)<min)
		min=std::abs(y);
	if(std::abs(z)<min)
		min=std::abs(z);
	return min;
}

double cVector3D::maxComp()
{
	double max =std::abs(x);
	if(std::abs(y)>max)
		max=std::abs(y);
	if(std::abs(z)>max)
		max=std::abs(z);
	return max;
}

cVector3D zeroVec3D(){cVector3D v(0,0,0);return v;}

////////////////////////////////////////////////////////////////////////////////////////////////////////
cVector2D::cVector2D(const double inpR, const double inpZ)
{
	r=inpR;
	z=inpZ;
}

cVector2D::cVector2D()
{
	r=0;
	z=0;
}

const cVector2D cVector2D::operator+(const cVector2D& rhs) const
{
	cVector2D result = *this;
	result += rhs;
	return result;
}

const cVector2D cVector2D::operator-(const cVector2D& rhs) const
{
	cVector2D result = *this;
	result -= rhs;
	return result;
}

const cVector2D& cVector2D::operator=(const cVector2D& inpVec)
{
	r = inpVec.r;
	z = inpVec.z;
	return *this;
}

const cVector2D& cVector2D::operator += (const cVector2D& rhs)
{
	r +=rhs.r;
	z +=rhs.z;
	return *this;
}
const cVector2D& cVector2D::operator -= (const cVector2D& rhs)
{
	r -=rhs.r;
	z -=rhs.z;
	return *this;
}
const cVector2D& cVector2D::operator *= (double rhs)
{
	r *= rhs;
	z *= rhs;
	return *this;
}

//Should throw erception if rhs is 0...but doesn't yet
const cVector2D& cVector2D::operator /= (double rhs)
{
	r /=rhs;
	z /=rhs;
	return *this;
}


const cVector2D cVector2D::operator * (const double rhs) const
{
	cVector2D result = *this;
	result *= rhs;
	return result;
}

cVector2D operator* (const double& lhs, const cVector2D& rhs)
{
	cVector2D result = rhs;
	result *= lhs;
	return result;
}

const cVector2D cVector2D::operator / (const double rhs) const
{
	cVector2D result = *this;
	result /= rhs;
	return result;
}

bool cVector2D::operator== (const cVector2D& inpVec)
{
	if(r==inpVec.r && z==inpVec.z){
		return true;
	}
	else{
		return false;
	}
}

bool cVector2D::operator != (const cVector2D& rhs)
{
	return !(*this == rhs);
}

double cVector2D::mag()
{
	return sqrt(r*r+z*z);
}

double cVector2D::dotProd(const cVector2D& inpVec)
{
	return r*inpVec.r+z*inpVec.z;
}

cVector2D cVector2D::scale(const double scaling)
{
	*this *= scaling;
	return *this;
}

double cVector2D::gamma()
{
	return 1./sqrt(1.-(r*r+z*z)/(8.99875479107368E+18));  //8.99875479107368E+18 = c^2
}
void cVector2D::setVal(const double inpR, const double inpZ)
{
	r=inpR;
	z=inpZ;
}

//To solve 2D equations easily (this is not a dot product or cross product
const cVector2D cVector2D::multComps(cVector2D inp) const
{
	cVector2D result = *this;
	result.r *= inp.r;
	result.z *= inp.z;
	return result;
}
//To solve 2D equations easily (this is not a dot product or cross product
const cVector2D cVector2D::divComps(cVector2D inp) const
{
	cVector2D result = *this;
	result.r /= inp.r;
	result.z /= inp.z;
	return result;
}
cVector2D zeroVec2D(){cVector2D v(0,0); return v;}
