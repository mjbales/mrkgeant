#include "MRKVector.hh"



MRKVector3D::MRKVector3D(const double inpX, const double inpY, const double inpZ)
{
	x=inpX;
	y=inpY;
	z=inpZ;
}

MRKVector3D::MRKVector3D()
{
	x=0;
	y=0;
	z=0;
}

void MRKVector3D::get(double& outX, double& outY, double& outZ){
    outX=x;
    outY=y;
    outZ=z;
}

const MRKVector3D MRKVector3D::operator+(const MRKVector3D& rhs) const
{
	MRKVector3D result = *this;
	result += rhs;
	return result;
}

const MRKVector3D MRKVector3D::operator-(const MRKVector3D& rhs) const
{
	MRKVector3D result = *this;
	result -= rhs;
	return result;
}

const MRKVector3D& MRKVector3D::operator=(const MRKVector3D& inpVec)
{
	x = inpVec.x;
	y = inpVec.y;
	z = inpVec.z;
	return *this;
}

const MRKVector3D& MRKVector3D::operator += (const MRKVector3D& rhs)
{
	x +=rhs.x;
	y +=rhs.y;
	z +=rhs.z;
	return *this;
}
const MRKVector3D& MRKVector3D::operator -= (const MRKVector3D& rhs)
{
	x -=rhs.x;
	y -=rhs.y;
	z -=rhs.z;
	return *this;
}
const MRKVector3D& MRKVector3D::operator *= (double rhs)
{
	x *= rhs;
	y *= rhs;
	z *= rhs;
	return *this;
}

//Should throw exception if rhs is 0...but doesn't yet
const MRKVector3D& MRKVector3D::operator /= (double rhs)
{
	x /=rhs;
	y /=rhs;
	z /=rhs;
	return *this;
}


const MRKVector3D MRKVector3D::operator * (const double rhs) const
{
	MRKVector3D result = *this;
	result *= rhs;
	return result;
}

MRKVector3D operator* (const double& lhs, const MRKVector3D& rhs)
{
	MRKVector3D result = rhs;
	result *= lhs;
	return result;
}

const MRKVector3D MRKVector3D::operator / (const double rhs) const
{
	MRKVector3D result = *this;
	result /= rhs;
	return result;
}

bool MRKVector3D::operator== (const MRKVector3D& inpVec)
{
	if(x==inpVec.x && y==inpVec.y && z==inpVec.z){
		return true;
	}
	else{
		return false;
	}
}

bool MRKVector3D::operator != (const MRKVector3D& rhs)
{
	return !(*this == rhs);
}

double MRKVector3D::mag()
{
	return sqrt(x*x+y*y+z*z);
}

MRKVector3D MRKVector3D::crossProd(const MRKVector3D& v)
{
    MRKVector3D answer(y*v.z-v.y*z,-x*v.z+v.x*z,x*v.y-v.x*y);
    return answer;
}
double MRKVector3D::dotProd(const MRKVector3D& inpVec)
{
	return x*inpVec.x+y*inpVec.y+z*inpVec.z;
}

MRKVector3D MRKVector3D::scale(const double scaling)
{
	*this *= scaling;
	return *this;
}

double MRKVector3D::gamma()
{
	return 1./sqrt(1.-(x*x+y*y+z*z)*(INVCSQUARED));  //8.99875479107368E+18 = c^2
}
void MRKVector3D::setVal(const double inpX, const double inpY, const double inpZ)
{
	x=inpX;
	y=inpY;
	z=inpZ;
}

//To solve 3D equations easily (this is not a dot product or cross product
const MRKVector3D MRKVector3D::multComps(MRKVector3D inp) const
{
	MRKVector3D result = *this;
	result.x *= inp.x;
	result.y *= inp.y;
	result.z *= inp.z;
	return result;
}
//To solve 3D equations easily (this is not a dot product or cross product
const MRKVector3D MRKVector3D::divComps(MRKVector3D inp) const
{
	MRKVector3D result = *this;
	result.x /= inp.x;
	result.y /= inp.y;
	result.z /= inp.z;
	return result;
}

const MRKVector3D MRKVector3D::returnTransCoords() const //rotation of 9.5 degrees
{
	MRKVector3D result;
	result.x = -z*SIN_BEND+x*COS_BEND;
	result.z = z*COS_BEND+x*SIN_BEND;
	result.y = y;
	return result;
}
const MRKVector3D MRKVector3D::returnNormCoords() const  //rotation back 9.5 degrees
{
	MRKVector3D result;
	result.x = z*SIN_BEND+x*COS_BEND;
	result.z = z*COS_BEND-x*SIN_BEND;
	result.y = y;
	return result;
}

const MRKVector3D MRKVector3D::norm()
{
	MRKVector3D result;
	double oneOverMag = 1/this->mag();

	result.x = x * oneOverMag;
	result.y = y * oneOverMag;
	result.z = z * oneOverMag;
	return result;
}

double MRKVector3D::minComp()
{
	double min =std::abs(x);
	if(std::abs(y)<min)
		min=std::abs(y);
	if(std::abs(z)<min)
		min=std::abs(z);
	return min;
}

double MRKVector3D::maxComp()
{
	double max =std::abs(x);
	if(std::abs(y)>max)
		max=std::abs(y);
	if(std::abs(z)>max)
		max=std::abs(z);
	return max;
}

MRKVector3D zeroVec3D(){MRKVector3D v(0,0,0);return v;}

////////////////////////////////////////////////////////////////////////////////////////////////////////
MRKVector2D::MRKVector2D(const double inpR, const double inpZ)
{
	r=inpR;
	z=inpZ;
}

MRKVector2D::MRKVector2D()
{
	r=0;
	z=0;
}

const MRKVector2D MRKVector2D::operator+(const MRKVector2D& rhs) const
{
	MRKVector2D result = *this;
	result += rhs;
	return result;
}

const MRKVector2D MRKVector2D::operator-(const MRKVector2D& rhs) const
{
	MRKVector2D result = *this;
	result -= rhs;
	return result;
}

const MRKVector2D& MRKVector2D::operator=(const MRKVector2D& inpVec)
{
	r = inpVec.r;
	z = inpVec.z;
	return *this;
}

const MRKVector2D& MRKVector2D::operator += (const MRKVector2D& rhs)
{
	r +=rhs.r;
	z +=rhs.z;
	return *this;
}
const MRKVector2D& MRKVector2D::operator -= (const MRKVector2D& rhs)
{
	r -=rhs.r;
	z -=rhs.z;
	return *this;
}
const MRKVector2D& MRKVector2D::operator *= (double rhs)
{
	r *= rhs;
	z *= rhs;
	return *this;
}

//Should throw erception if rhs is 0...but doesn't yet
const MRKVector2D& MRKVector2D::operator /= (double rhs)
{
	r /=rhs;
	z /=rhs;
	return *this;
}


const MRKVector2D MRKVector2D::operator * (const double rhs) const
{
	MRKVector2D result = *this;
	result *= rhs;
	return result;
}

MRKVector2D operator* (const double& lhs, const MRKVector2D& rhs)
{
	MRKVector2D result = rhs;
	result *= lhs;
	return result;
}

const MRKVector2D MRKVector2D::operator / (const double rhs) const
{
	MRKVector2D result = *this;
	result /= rhs;
	return result;
}

bool MRKVector2D::operator== (const MRKVector2D& inpVec)
{
	if(r==inpVec.r && z==inpVec.z){
		return true;
	}
	else{
		return false;
	}
}

bool MRKVector2D::operator != (const MRKVector2D& rhs)
{
	return !(*this == rhs);
}

double MRKVector2D::mag()
{
	return sqrt(r*r+z*z);
}

double MRKVector2D::dotProd(const MRKVector2D& inpVec)
{
	return r*inpVec.r+z*inpVec.z;
}

MRKVector2D MRKVector2D::scale(const double scaling)
{
	*this *= scaling;
	return *this;
}

double MRKVector2D::gamma()
{
	return 1./sqrt(1.-(r*r+z*z)/(8.99875479107368E+18));  //8.99875479107368E+18 = c^2
}
void MRKVector2D::setVal(const double inpR, const double inpZ)
{
	r=inpR;
	z=inpZ;
}

//To solve 2D equations easily (this is not a dot product or cross product
const MRKVector2D MRKVector2D::multComps(MRKVector2D inp) const
{
	MRKVector2D result = *this;
	result.r *= inp.r;
	result.z *= inp.z;
	return result;
}
//To solve 2D equations easily (this is not a dot product or cross product
const MRKVector2D MRKVector2D::divComps(MRKVector2D inp) const
{
	MRKVector2D result = *this;
	result.r /= inp.r;
	result.z /= inp.z;
	return result;
}
MRKVector2D zeroVec2D(){MRKVector2D v(0,0); return v;}
