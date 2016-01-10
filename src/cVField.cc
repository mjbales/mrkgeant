#include "cVField.hh"
////////////////////////////////////////////////////////////////////////////////////////////////////
//cField3D
////////////////////////////////////////////////////////////////////////////////////////////////////
cField3D::cField3D()
{
	rows=columns=layers=arraySize=0;
	xStart=yStart=zStart=xSpacing=ySpacing=zSpacing=0.0;
	fileLoaded = false;
}

void cField3D::calcPositionLimits()
{
    xFinal=xStart+(rows-1)*xSpacing;
    yFinal=yStart+(columns-1)*ySpacing;
    zFinal=zStart+(layers-1)*zSpacing;
}

bool cField3D::isPositionOutsideField(cVector3D pos) // Assumes Y field is mirrored
{
    if(pos.z > zStart && pos.z < zFinal)
    {
        if(abs(pos.y) > yStart && abs(pos.y) < yFinal)
        {
            if(pos.x > xStart && pos.x < xFinal)
            {
                return false;
            }

        }
    }
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//cSField3D
/////////////////////////////////////////////////////////////////////////////////////////////////////
cSField3D::cSField3D(){
}

cSField3D::~cSField3D()
{
	reset();
}


void cSField3D::reset(){

	setSize(0,0,0);
	xStart=yStart=zStart=xSpacing=ySpacing=zSpacing=0.0;
	fileLoaded = false;
}

void cSField3D::setSize( int inpRows, int inpColumns, int inpLayers)
{
    int i,j;

    if(rows >0 && columns >0 && layers >0){
        for(i=0;i<rows;i++)
        {
            for(j=0;j<columns;j++)
            {
                fArray[i][j].clear();
            }
            fArray[i].clear();
        }
        fArray.clear();
    }
    rows=columns=layers=arraySize=0;

    if(inpRows>0 && inpColumns>0 && inpLayers>0)
    {
        rows=inpRows;
        columns=inpColumns;
        layers=inpLayers;
        arraySize=rows*columns*layers;

        fArray.resize(rows);
		for(i=0;i<rows;i++)
        {
            fArray[i].resize(columns);
            for(j=0;j<columns;j++)
            {
                fArray[i][j].resize(layers);
            }
        }


    }
}




//Assumes cm input but stores as meter
int cSField3D::loadField(const char* inpFileName, double scalingValue){

	double d1, d2, d3, d4;
	 int i,j,k,inpRows,inpColumns,inpLayers;
	ifstream fieldFile;


	fieldFile.open(inpFileName);

	if(!fieldFile.is_open())
	{
	    cout << "Could not find file: "  << inpFileName << endl;
		return 1;
	}
	else
	{
		//First 3 s
		skipCommentLines(fieldFile);
		fieldFile >> d1 >> d2 >> inpRows;
		xStart=d1/100.; xSpacing=d2/100.;
		fieldFile.ignore(1024,'\n');

		skipCommentLines(fieldFile);
		fieldFile >> d1 >> d2 >> inpColumns;
		yStart=d1/100.; ySpacing=d2/100.;
		fieldFile.ignore(1024,'\n');

		skipCommentLines(fieldFile);
		fieldFile >> d1 >> d2 >> inpLayers;
		zStart=d1/100.; zSpacing=d2/100.;

		if(inpRows <= 0 || inpColumns <=0 || inpLayers <=0)
            return -1;

		setSize(inpRows,inpColumns,inpLayers);


		for (i=0; i< rows; i++){
			for (j=0; j< columns; j++){
				for (k=0; k< layers; k++){

					fieldFile.ignore(1024,'\n');
					//Check for comment Lines
					skipCommentLines(fieldFile);
					fieldFile >> d1 >> d2  >> d3  >> d4;
					fArray[i][j][k]=d4*scalingValue;
				}
			}
		}
	}
	calcPositionLimits();
	fileLoaded=true;
	return 0;
}




double cSField3D::getValue( int xIn, int yIn, int zIn)
{
	if(xIn >= rows || yIn >= columns || zIn >= layers){
		return 0;
	}
	return fArray[xIn][yIn][zIn];
}

void cSField3D::setValue( int inpRow, int inpColumn, int inpLayer,double inpVal)
{
    fArray[inpRow][inpColumn][inpLayer]=inpVal;

}




//Assumes Y field is mirrored!!!!!!!!!!!!
//Adds interpolated field to vecOut
void cSField3D::linearInterp(const cVector3D& pos,double& valOut){

	xMod = modf((pos.x-xStart)/xSpacing,&xFloor);
	yMod = modf((abs(pos.y)-yStart)/ySpacing,&yFloor);
	zMod = modf((pos.z-zStart)/zSpacing,&zFloor);
	xF=int(xFloor);
	yF=int(yFloor);
	zF=int(zFloor);
	if(xF >= rows - 1 || xF <0 || yF >= columns - 1|| yF <0 || zF >= layers - 1 || zF <0){
		return;
	}
	result=fArray[xF][yF][zF]*(1.-xMod)*(1.-yMod)*(1.-zMod)+
		fArray[xF][yF][zF+1]*(1.-xMod)*(1.-yMod)*(zMod)+
		fArray[xF][yF+1][zF]*(1.-xMod)*(yMod)*(1.-zMod)+
		fArray[xF][yF+1][zF+1]*(1.-xMod)*(yMod)*(zMod)+
		fArray[xF+1][yF][zF]*(xMod)*(1.-yMod)*(1.-zMod)+
		fArray[xF+1][yF][zF+1]*(xMod)*(1.-yMod)*(zMod)+
		fArray[xF+1][yF+1][zF]*(xMod)*(yMod)*(1.-zMod)+
		fArray[xF+1][yF+1][zF+1]*(xMod)*(yMod)*(zMod);

	valOut+=result;


	return;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//cVField3D
/////////////////////////////////////////////////////////////////////////////////////////////////////
cVField3D::cVField3D()
{
    xF=yF=zF=-999999;
    symmetryInY=false;
}

cVField3D::~cVField3D()
{
	reset();
}

void cVField3D::reset(){

    setSize(0,0,0);
	xStart=yStart=zStart=xSpacing=ySpacing=zSpacing=0.0;
	fileLoaded = false;
}

void cVField3D::setSize( int inpRows, int inpColumns, int inpLayers)
{
     int i,j;

    if(rows >0 && columns >0 && layers){
        for(i=0;i<rows;i++)
        {
            for(j=0;j<columns;j++)
            {
                xArray[i][j].clear();
                yArray[i][j].clear();
                zArray[i][j].clear();
            }
            xArray[i].clear();
            yArray[i].clear();
            zArray[i].clear();
        }
        xArray.clear();
        yArray.clear();
        zArray.clear();
    }
    rows=columns=layers=arraySize=0;

    if(inpRows>0 && inpColumns>0 && inpLayers>0)
    {
        rows=inpRows;
        columns=inpColumns;
        layers=inpLayers;
        arraySize=rows*columns*layers;

        xArray.resize(rows);
        yArray.resize(rows);
        zArray.resize(rows);
		for(i=0;i<rows;i++)
        {
            xArray[i].resize(columns);
            yArray[i].resize(columns);
            zArray[i].resize(columns);
            for(j=0;j<columns;j++)
            {
                xArray[i][j].resize(layers);
                yArray[i][j].resize(layers);
                zArray[i][j].resize(layers);
            }
        }


    }
}




int cVField3D::loadField(const char* inpFileName, double scalingValue){ //Assumes cm input but stores as meter

	double d1, d2, d3, d4, d5, d6,d7;
	 int inpRows,inpColumns,inpLayers,i,j,k;
	ifstream fieldFile;
	bool isComsolFileFormat=false;


	fieldFile.open(inpFileName);

	if(!fieldFile.is_open())
	{
		return 1;
	}
	else
	{
        isComsolFileFormat=(fieldFile.peek()=='%');
		//First 3 s
		skipCommentLines(fieldFile);
		fieldFile >> d1 >> d2 >> inpRows;
		xStart=d1/100.; xSpacing=d2/100.;
		fieldFile.ignore(1024,'\n');

		skipCommentLines(fieldFile);
		fieldFile >> d1 >> d2 >> inpColumns;
		yStart=d1/100.; ySpacing=d2/100.;
		fieldFile.ignore(1024,'\n');

		skipCommentLines(fieldFile);
		fieldFile >> d1 >> d2 >> inpLayers;
		zStart=d1/100.; zSpacing=d2/100.;

		setSize(inpRows,inpColumns,inpLayers);

		if(inpRows <= 0 || inpColumns <=0 || inpLayers <=0)
            return -1;

        if(isComsolFileFormat)
        {
            for (k=0; k< layers; k++){
                for (j=0; j< columns; j++){
                    for (i=0; i< rows; i++){

                        fieldFile.ignore(1024,'\n');
                        //Check for comment Lines
                        skipCommentLines(fieldFile);
                        fieldFile >> d1 >> d2  >> d3  >> d7 >> d4 >> d5 >> d6; //COMSOL has voltage listed as well
                        xArray[i][j][k]=d4*scalingValue;
                        yArray[i][j][k]=d5*scalingValue;
                        zArray[i][j][k]=d6*scalingValue;
                    }
                }
            }
        }
        else
        {
            for (i=0; i< rows; i++){
                for (j=0; j< columns; j++){
                    for (k=0; k< layers; k++){

                        fieldFile.ignore(1024,'\n');
                        //Check for comment Lines
                        skipCommentLines(fieldFile);
                        fieldFile >> d1 >> d2  >> d3  >> d4 >> d5 >> d6;
                        xArray[i][j][k]=d4*scalingValue;
                        yArray[i][j][k]=d5*scalingValue;
                        zArray[i][j][k]=d6*scalingValue;
                    }
                }
            }
        }
	}
	calcPositionLimits();
	fileLoaded=true;
	return 0;
}


const cVector3D cVField3D::getVector( int xIn, int yIn, int zIn){

	cVector3D outVec;
	//if(xIn >= rows || yIn >= columns || zIn >= layers || !fileLoaded){
	//	return outVec;
	//}
    outVec.setVal(xArray[xIn][yIn][zIn],yArray[xIn][yIn][zIn],zArray[xIn][yIn][zIn]);
	return outVec;
}

void cVField3D::setVector( int inpRow, int inpColumn, int inpLayer,cVector3D inpVec)
{
    xArray[inpRow][inpColumn][inpLayer]=inpVec.x;
    yArray[inpRow][inpColumn][inpLayer]=inpVec.y;
    zArray[inpRow][inpColumn][inpLayer]=inpVec.z;
}



//Adds interpolated field to vecOut
void cVField3D::linearInterp(const cVector3D& pos,cVector3D& vecOut)
{
	xMod = modf((pos.x-xStart)/xSpacing,&xFloor);

	if(symmetryInY)
        yMod = modf((abs(pos.y)-yStart)/ySpacing,&yFloor);
    else
        yMod = modf((pos.y-yStart)/ySpacing,&yFloor);

	zMod = modf((pos.z-zStart)/zSpacing,&zFloor);
	xF=int(xFloor);
	yF=int(yFloor);
	zF=int(zFloor);
	if(!fileLoaded)
		return;
	if(xF >= rows - 1 || xF <0 || yF >= columns - 1|| yF <0 || zF >= layers - 1 || zF <0)
	{
		return;
	}

	xResult=xArray[xF][yF][zF]*(1.-xMod)*(1.-yMod)*(1.-zMod)+
		xArray[xF][yF][zF+1]*(1.-xMod)*(1.-yMod)*(zMod)+
		xArray[xF][yF+1][zF]*(1.-xMod)*(yMod)*(1.-zMod)+
		xArray[xF][yF+1][zF+1]*(1.-xMod)*(yMod)*(zMod)+
		xArray[xF+1][yF][zF]*(xMod)*(1.-yMod)*(1.-zMod)+
		xArray[xF+1][yF][zF+1]*(xMod)*(1.-yMod)*(zMod)+
		xArray[xF+1][yF+1][zF]*(xMod)*(yMod)*(1.-zMod)+
		xArray[xF+1][yF+1][zF+1]*(xMod)*(yMod)*(zMod);
	yResult=yArray[xF][yF][zF]*(1.-xMod)*(1.-yMod)*(1.-zMod)+
		yArray[xF][yF][zF+1]*(1.-xMod)*(1.-yMod)*(zMod)+
		yArray[xF][yF+1][zF]*(1.-xMod)*(yMod)*(1.-zMod)+
		yArray[xF][yF+1][zF+1]*(1.-xMod)*(yMod)*(zMod)+
		yArray[xF+1][yF][zF]*(xMod)*(1.-yMod)*(1.-zMod)+
		yArray[xF+1][yF][zF+1]*(xMod)*(1.-yMod)*(zMod)+
		yArray[xF+1][yF+1][zF]*(xMod)*(yMod)*(1.-zMod)+
		yArray[xF+1][yF+1][zF+1]*(xMod)*(yMod)*(zMod);
	zResult=zArray[xF][yF][zF]*(1.-xMod)*(1.-yMod)*(1.-zMod)+
		zArray[xF][yF][zF+1]*(1.-xMod)*(1.-yMod)*(zMod)+
		zArray[xF][yF+1][zF]*(1.-xMod)*(yMod)*(1.-zMod)+
		zArray[xF][yF+1][zF+1]*(1.-xMod)*(yMod)*(zMod)+
		zArray[xF+1][yF][zF]*(xMod)*(1.-yMod)*(1.-zMod)+
		zArray[xF+1][yF][zF+1]*(xMod)*(1.-yMod)*(zMod)+
		zArray[xF+1][yF+1][zF]*(xMod)*(yMod)*(1.-zMod)+
		zArray[xF+1][yF+1][zF+1]*(xMod)*(yMod)*(zMod);


	if(symmetryInY && pos.y<0)
		yResult=-yResult;

	vecOut.x += xResult;
	vecOut.y += yResult;
	vecOut.z += zResult;


	return;
}


//Assumes Y field is mirrored!!!!!!!!!!!!
//Adds interpolated field to vecOut
void cVField3D::linearInterp2(const cVector3D& pos,cVector3D& vecOut)
{
    if(isPositionOutsideField(pos))
    {
        xF=yF=zF=-9999999;
        return;
    }
	xMod = modf((pos.x-xStart)/xSpacing,&xFloor);
	yMod = modf((abs(pos.y)-yStart)/ySpacing,&yFloor);
	zMod = modf((pos.z-zStart)/zSpacing,&zFloor);

	if(xF != int(xFloor) || yF != int(yFloor) || zF != int(zFloor))
	{
        xF=int(xFloor);
        yF=int(yFloor);
        zF=int(zFloor);
        xF2=xF+1;
        yF2=yF+1;
        zF2=zF+1;

        oldFieldBox[0][0][0][0]=xArray[xF][yF][zF];
        oldFieldBox[0][0][0][1]=xArray[xF][yF][zF2];
        oldFieldBox[0][0][1][0]=xArray[xF][yF2][zF];
        oldFieldBox[0][0][1][1]=xArray[xF][yF2][zF2];
        oldFieldBox[0][1][0][0]=xArray[xF2][yF][zF];
        oldFieldBox[0][1][0][1]=xArray[xF2][yF][zF2];
        oldFieldBox[0][1][1][0]=xArray[xF2][yF2][zF];
        oldFieldBox[0][1][1][1]=xArray[xF2][yF2][zF2];
        oldFieldBox[1][0][0][0]=yArray[xF][yF][zF];
        oldFieldBox[1][0][0][1]=yArray[xF][yF][zF2];
        oldFieldBox[1][0][1][0]=yArray[xF][yF2][zF];
        oldFieldBox[1][0][1][1]=yArray[xF][yF2][zF2];
        oldFieldBox[1][1][0][0]=yArray[xF2][yF][zF];
        oldFieldBox[1][1][0][1]=yArray[xF2][yF][zF2];
        oldFieldBox[1][1][1][0]=yArray[xF2][yF2][zF];
        oldFieldBox[1][1][1][1]=yArray[xF2][yF2][zF2];
        oldFieldBox[2][0][0][0]=zArray[xF][yF][zF];
        oldFieldBox[2][0][0][1]=zArray[xF][yF][zF2];
        oldFieldBox[2][0][1][0]=zArray[xF][yF2][zF];
        oldFieldBox[2][0][1][1]=zArray[xF][yF2][zF2];
        oldFieldBox[2][1][0][0]=zArray[xF2][yF][zF];
        oldFieldBox[2][1][0][1]=zArray[xF2][yF][zF2];
        oldFieldBox[2][1][1][0]=zArray[xF2][yF2][zF];
        oldFieldBox[2][1][1][1]=zArray[xF2][yF2][zF2];

	}
	xModInv=1.-xMod;
    yModInv=1.-yMod;
    zModInv=1.-zMod;

	xResult=oldFieldBox[0][0][0][0]*(xModInv)*(yModInv)*(zModInv)+
		oldFieldBox[0][0][0][1]*(xModInv)*(yModInv)*(zMod)+
		oldFieldBox[0][0][1][0]*(xModInv)*(yMod)*(zModInv)+
		oldFieldBox[0][0][1][1]*(xModInv)*(yMod)*(zMod)+
		oldFieldBox[0][1][0][0]*(xMod)*(yModInv)*(zModInv)+
		oldFieldBox[0][1][0][1]*(xMod)*(yModInv)*(zMod)+
		oldFieldBox[0][1][1][0]*(xMod)*(yMod)*(zModInv)+
		oldFieldBox[0][1][1][1]*(xMod)*(yMod)*(zMod);
	yResult=oldFieldBox[1][0][0][0]*(xModInv)*(yModInv)*(zModInv)+
		oldFieldBox[1][0][0][1]*(xModInv)*(yModInv)*(zMod)+
		oldFieldBox[1][0][1][0]*(xModInv)*(yMod)*(zModInv)+
		oldFieldBox[1][0][1][1]*(xModInv)*(yMod)*(zMod)+
		oldFieldBox[1][1][0][0]*(xMod)*(yModInv)*(zModInv)+
		oldFieldBox[1][1][0][1]*(xMod)*(yModInv)*(zMod)+
		oldFieldBox[1][1][1][0]*(xMod)*(yMod)*(zModInv)+
		oldFieldBox[1][1][1][1]*(xMod)*(yMod)*(zMod);
	zResult=oldFieldBox[2][0][0][0]*(xModInv)*(yModInv)*(zModInv)+
		oldFieldBox[2][0][0][1]*(xModInv)*(yModInv)*(zMod)+
		oldFieldBox[2][0][1][0]*(xModInv)*(yMod)*(zModInv)+
		oldFieldBox[2][0][1][1]*(xModInv)*(yMod)*(zMod)+
		oldFieldBox[2][1][0][0]*(xMod)*(yModInv)*(zModInv)+
		oldFieldBox[2][1][0][1]*(xMod)*(yModInv)*(zMod)+
		oldFieldBox[2][1][1][0]*(xMod)*(yMod)*(zModInv)+
		oldFieldBox[2][1][1][1]*(xMod)*(yMod)*(zMod);


	if(pos.y<0)
		yResult=-yResult;

	vecOut.x += xResult;
	vecOut.y += yResult;
	vecOut.z += zResult;


	return;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////
//cField2D
/////////////////////////////////////////////////////////////////////////////////////////////////////
cField2D::cField2D()
{
	rows=columns=arraySize=0;
	rStart=zStart=rSpacing=zSpacing=0.0;
	fileLoaded=false;
}

void cField2D::calcPositionLimits()
{
    rFinal=rStart+(rows-1)*rSpacing;
    zFinal=zStart+(columns-1)*zSpacing;
}

bool cField2D::isPositionOutsideField(double zPosition, double radius)
{
    if(zPosition > zStart && zPosition < zFinal)
    {
        if(radius > rStart && radius < rFinal)
        {

            return false;

        }
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cSField2D
/////////////////////////////////////////////////////////////////////////////////////////////////////


cSField2D::cSField2D(){
}

cSField2D::~cSField2D()
{
	reset();
}


void cSField2D::reset(){

	setSize(0,0);
	rStart=zStart=rSpacing=zSpacing=0.0;
	fileLoaded = false;
}

void cSField2D::setSize( int inpRows, int inpColumns)
{

     int i;


    if(rows >0 && columns >0){
        for(i=0;i<rows;i++)
        {
            fArray[i].clear();
        }
        fArray.clear();

    }

    rows=columns=arraySize=0;

    if(inpRows>0 && inpColumns>0)
    {
        rows=inpRows;
        columns=inpColumns;
        arraySize=rows*columns;

        fArray.resize(rows);
		for(i=0;i<rows;i++)
        {
            fArray[i].resize(columns);
        }


    }
}




//Assumes cm input but stores as meter
int cSField2D::loadField(const char* inpFileName, double scalingValue){

	double d1, d2, d3;
	int i1;
	 int i,j,inpRows=0,inpColumns=0;
	ifstream fieldFile;


	fieldFile.open(inpFileName);

	if(!fieldFile.is_open())
	{
		return -1;
	}
	else
	{
		skipCommentLines(fieldFile);
		fieldFile >> i1;
		fieldFile.ignore(1024,'\n');
		transposedField = i1;

		skipCommentLines(fieldFile);
		fieldFile >> d1 >> d2 >> inpRows;
		rStart=d1/100.; rSpacing=d2/100.;
		fieldFile.ignore(1024,'\n');


		skipCommentLines(fieldFile);
		fieldFile >> d1 >> d2 >> inpColumns;
		zStart=d1/100.; zSpacing=d2/100.;

		if(inpRows <= 0 || inpColumns <=0)
            return -1;

		setSize(inpRows,inpColumns);


		for (i=0; i < rows; i++){
			for (j=0; j< columns; j++){
			    if(fieldFile.bad() || fieldFile.eof() || fieldFile.fail())
			    {
			        cout << "End of field file reached early" << endl;
                    return -1;
			    }

                fieldFile.ignore(1024,'\n');
                //Check for comment Lines
                skipCommentLines(fieldFile);
                fieldFile >> d1 >> d2  >> d3;
                fArray[i][j]=d3*scalingValue;

			}
		}
	}
	calcPositionLimits();
	fileLoaded=true;
	return 0;
}




double cSField2D::getValue( int rIn, int zIn)
{
	if(rIn >= rows || zIn >= columns){
		return 0;
	}
	return fArray[rIn][zIn];
}

void cSField2D::setValue( int inpRow, int inpColumn,double inpVal)
{
    fArray[inpRow][inpColumn]=inpVal;

}


void cSField2D::linearInterpAs3D(const cVector3D& pos,double& valOut){
	if(!fileLoaded)
		return;
	if(transposedField)
		posInField = pos.returnTransCoords();
	else
		posInField = pos;
	radius = sqrt(pow(posInField.x,2)+pow(posInField.y,2));
	rMod = modf((radius-rStart)/rSpacing,&rFloor);
	zMod = modf((posInField.z-zStart)/zSpacing,&zFloor);
	rF=int(rFloor);
	zF=int(zFloor);

	if(rF >= rows - 1 || rF <0 || zF >= columns - 1 || zF <0){
		return;
	}

	result=fArray[rF][zF]*(1-rMod)*(1-zMod)+
		fArray[rF][zF+1]*(1-rMod)*(zMod)+
		fArray[rF+1][zF]*(rMod)*(1-zMod)+
		fArray[rF+1][zF+1]*(rMod)*(zMod);


	valOut+=result;
	return;
}

//Still use r and z because I'm copy an paste lazy
void cSField2D::linearInterp(const cVector2D& pos,double& valOut){
	if(!fileLoaded)
		return;
	rMod = modf((pos.r-rStart)/rSpacing,&rFloor);
	zMod = modf((pos.z-zStart)/zSpacing,&zFloor);
	rF=int(rFloor);
	zF=int(zFloor);

	if(rF >= rows - 1 || rF <0 || zF >= columns - 1 || zF <0){
		return;
	}

	result=fArray[rF][zF]*(1-rMod)*(1-zMod)+
		fArray[rF][zF+1]*(1-rMod)*(zMod)+
		fArray[rF+1][zF]*(rMod)*(1-zMod)+
		fArray[rF+1][zF+1]*(rMod)*(zMod);


	valOut+=result;
	return;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
//cVField2D
/////////////////////////////////////////////////////////////////////////////////////////////////////
cVField2D::cVField2D()
{
}

cVField2D::~cVField2D()
{
	reset();
}

void cVField2D::reset(){

    setSize(0,0);
	rStart=zStart=rSpacing=zSpacing=0.0;
	fileLoaded = false;
}

void cVField2D::setSize( int inpRows, int inpColumns)
{
     int i;
    if(rows >0 && columns >0){
        for(i=0;i<rows;i++)
        {
            rArray[i].clear();
            zArray[i].clear();
        }
        rArray.clear();
        zArray.clear();
    }
    rows=columns=arraySize=0;

    if(inpRows>0 && inpColumns>0)
    {
        rows=inpRows;
        columns=inpColumns;
        arraySize=rows*columns;

        rArray.resize(rows);
        zArray.resize(rows);
		for(i=0;i<rows;i++)
        {
            rArray[i].resize(columns);
            zArray[i].resize(columns);

        }


    }
}


int cVField2D::loadField(const char* inpFileName, double scalingValue){ //Assumes cm input but stores as meter

	double d1, d2, d3, d4;
	int i1;
	 int inpRows,inpColumns,i,j;
	ifstream fieldFile;


	fieldFile.open(inpFileName);

	if(!fieldFile.is_open())
	{
		return 1;
	}
	else
	{
		skipCommentLines(fieldFile);
		fieldFile >> i1;
		fieldFile.ignore(1024,'\n');
		transposedField = i1;

		skipCommentLines(fieldFile);
		fieldFile >> d1 >> d2 >> inpRows;
		rStart=d1/100.; rSpacing=d2/100.;
		fieldFile.ignore(1024,'\n');


		skipCommentLines(fieldFile);
		fieldFile >> d1 >> d2 >> inpColumns;
		zStart=d1/100.; zSpacing=d2/100.;

		if(inpRows <= 0 || inpColumns <=0)
            return -1;

		setSize(inpRows,inpColumns);


		for (i=0; i< rows; i++){
			for (j=0; j< columns; j++){


                fieldFile.ignore(1024,'\n');
                //Check for comment Lines
                skipCommentLines(fieldFile);
                fieldFile >> d1 >> d2  >> d3 >> d4;
                rArray[i][j]=d3*scalingValue;
                zArray[i][j]=d4*scalingValue;

			}
		}
	}
	calcPositionLimits();
	fileLoaded=true;
	return 0;
}




cVector2D cVField2D::getVector( int rIn, int zIn){

    cVector2D outVec;
	if(rIn >= rows || zIn >= columns || !fileLoaded){
		return outVec;
	}
    outVec.setVal(rArray[rIn][zIn],zArray[rIn][zIn]);
	return outVec;
}

void cVField2D::setVector( int inpRow, int inpColumn,cVector2D inpVec)
{
    rArray[inpRow][inpColumn]=inpVec.r;
    zArray[inpRow][inpColumn]=inpVec.z;
}


void cVField2D::linearInterpAs3D(const cVector3D& pos,cVector3D& vecOut){
	if(!fileLoaded)
		return;
	if(transposedField)
	{
		posInField = pos.returnTransCoords();
	}
	else
	{
		posInField = pos;
	}
	radius = sqrt(pow(posInField.x,2)+pow(posInField.y,2));
	rMod = modf((radius-rStart)/rSpacing,&rFloor);
	zMod = modf((posInField.z-zStart)/zSpacing,&zFloor);
	rF=int(rFloor);
	zF=int(zFloor);

	if(rF >= rows - 1 || rF <0 || zF >= columns - 1 || zF <0){
		return;
	}

	rResult=rArray[rF][zF]*(1-rMod)*(1-zMod)+
		rArray[rF][zF+1]*(1-rMod)*(zMod)+
		rArray[rF+1][zF]*(rMod)*(1-zMod)+
		rArray[rF+1][zF+1]*(rMod)*(zMod);
	zResult=zArray[rF][zF]*(1-rMod)*(1-zMod)+
		zArray[rF][zF+1]*(1-rMod)*(zMod)+
		zArray[rF+1][zF]*(rMod)*(1-zMod)+
		zArray[rF+1][zF+1]*(rMod)*(zMod);

    cout << "r,z " << rResult << " " << zResult << endl;

    if(radius == 0)
        vecOut.setVal(0,0,zResult+vecOut.z);
    else
        vecOut.setVal((rResult*posInField.x/radius)+vecOut.x,(rResult*posInField.y/radius)+vecOut.y,zResult+vecOut.z);
	if(transposedField)
	{
		vecOut = vecOut.returnNormCoords();
	}
	return;
}

void cVField2D::linearInterpAs3D2(const cVector3D& pos,cVector3D& vecOut)
{
	radius = sqrt(pow(pos.x,2)+pow(pos.y,2));
	if(isPositionOutsideField(pos.z,radius))
	{
        return;
	}
	rMod = modf((radius-rStart)/rSpacing,&rFloor);
	zMod = modf((pos.z-zStart)/zSpacing,&zFloor);
	rF=int(rFloor);
	zF=int(zFloor);

	rResult=rArray[rF][zF]*(1-rMod)*(1-zMod)+
		rArray[rF][zF+1]*(1-rMod)*(zMod)+
		rArray[rF+1][zF]*(rMod)*(1-zMod)+
		rArray[rF+1][zF+1]*(rMod)*(zMod);
	zResult=zArray[rF][zF]*(1-rMod)*(1-zMod)+
		zArray[rF][zF+1]*(1-rMod)*(zMod)+
		zArray[rF+1][zF]*(rMod)*(1-zMod)+
		zArray[rF+1][zF+1]*(rMod)*(zMod);

    double rResultDivRadius=rResult/radius;

    if(radius == 0)
        vecOut.setVal(vecOut.x,vecOut.y,zResult+vecOut.z);
    else
        vecOut.setVal((rResultDivRadius*pos.x)+vecOut.x,(rResultDivRadius*pos.y)+vecOut.y,zResult+vecOut.z);

	return;
}

void cVField2D::linearInterpAs3DTransposed2(const cVector3D& pos,cVector3D& vecOut)
{

    posInField = pos.returnTransCoords();
	linearInterpAs3D2(posInField,vecOut);
	vecOut = vecOut.returnNormCoords();
}


///////////////////////////////////////////////////////////////////////////////////////////////
// Cubic Interpolation that needs to be updated



//Interpolates over distance D where mu varies from 0 to 1, 0 being at f1, 1 being at f2 and 1st derivatives df1 and df2
cVector3D cVField3D::cubicInterpolate1DVec(double mu,
		const cVector3D& f1,const cVector3D& f2,const cVector3D& df1,const cVector3D& df2)
{
	//All variable are declared in the field as they will be used often!

	mu2 = mu*mu;
	mu3 = mu2*mu;

	a0 = 2*f1-2*f2 + df1 + df2;
	a1 = -3*f1 +3*f2-2*df1 -df2;
	a2 = df1;
	a3 = f1;



   return(a0*mu3+a1*mu2+a2*mu+a3);
}

//Calculates derivative based on centered differencing...inputs are one increment less then point and one increment greater than point and the spacing
//This is a derivative with respect to mu.
cVector3D cVField3D::derivative(cVector3D start, cVector3D finish)
{
	return (finish - start)/(2.);
}


//Calculates derivatives based on centered differencing
void cVField3D::derivatives(int i, int j, int k, cVector3D& dx, cVector3D& dy, cVector3D& dz, cVector3D& dxy, cVector3D& dxz, cVector3D& dyz, cVector3D& dxyz)
{
	int im=i,ip=i,jm=j,jp=j,km=k,kp=k;
	cVector3D temp1, temp2,temp3, temp4;

	//Check to see if we're not at the borders of the array.  If we are then assume values are the same.  IE 1st derivative doesn't change
	if(i > 0) im--;
	if(i < rows - 1) ip++;
	if(j > 0) jm--;
	if(j < columns - 1) jp++;
	if(k > 0) km--;
	if(k < layers - 1) kp++;


	//Calculating dx,dy,dz
	dx=derivative(getVector(im,j,k),getVector(ip,j,k));
	dy=derivative(getVector(i,jm,k),getVector(i,jp,k));
	dz=derivative(getVector(i,j,km),getVector(i,j,kp));

	//Calculating dxy
	temp1 = derivative(getVector(im,jm,k),getVector(ip,jm,k));
	temp2 = derivative(getVector(im,jp,k),getVector(ip,jp,k));
	dxy = derivative(temp1,temp2);

	//Calculating dxz
	temp1 = derivative(getVector(im,j,km),getVector(ip,j,km));
	temp2 = derivative(getVector(im,j,kp),getVector(ip,j,kp));
	dxz = derivative(temp1,temp2);

	//Calculating dyz
	temp1 = derivative(getVector(i,jm,km),getVector(i,jp,km));
	temp2 = derivative(getVector(i,jm,kp),getVector(i,jp,kp));
	dyz = derivative(temp1,temp2);

	//Calculating dxyz
	temp1 = derivative(getVector(im,jm,km),getVector(ip,jm,km));
	temp2 = derivative(getVector(im,jp,km),getVector(ip,jp,km));
	temp3 = derivative(temp1,temp2);

	temp1 = derivative(getVector(im,jm,kp),getVector(ip,jm,kp));
	temp2 = derivative(getVector(im,jp,kp),getVector(ip,jp,kp));
	temp4 = derivative(temp1,temp2);

	dxyz = derivative(temp3,temp4);
}


//Assumes field is mirrored in the y direction
void cVField3D::cubicInterp(const cVector3D& pos,cVector3D& vecOut)
{
	double xFloor,yFloor,zFloor;
	cVector3D mu, temp;
	int xF,yF,zF;
	if(!fileLoaded)
		return;



	mu.x = modf((pos.x-xStart)/xSpacing,&xFloor);
	mu.y = modf((abs(pos.y)-yStart)/ySpacing,&yFloor);
	mu.z = modf((pos.z-zStart)/zSpacing,&zFloor);
	xF=int(xFloor);
	yF=int(yFloor);
	zF=int(zFloor);

	//ensure we are not on the boundary of the array
	if(xF >= rows - 1 || xF <0 || yF >= columns - 1|| yF <0 || zF >= layers - 1 || zF <0){

		return;
	}

	vAndD[0].val=getVector(xF,yF,zF);
	vAndD[1].val=getVector(xF+1,yF,zF);
	vAndD[2].val=getVector(xF,yF+1,zF);
	vAndD[3].val=getVector(xF+1,yF+1,zF);
	vAndD[4].val=getVector(xF,yF,zF+1);
	vAndD[5].val=getVector(xF+1,yF,zF+1);
	vAndD[6].val=getVector(xF,yF+1,zF+1);
	vAndD[7].val=getVector(xF+1,yF+1,zF+1);

	derivatives(xF,yF,zF,vAndD[0].dx,vAndD[0].dy,vAndD[0].dz,vAndD[0].dxy,vAndD[0].dxz,vAndD[0].dyz,vAndD[0].dxyz);
	derivatives(xF+1,yF,zF,vAndD[1].dx,vAndD[1].dy,vAndD[1].dz,vAndD[1].dxy,vAndD[1].dxz,vAndD[1].dyz,vAndD[1].dxyz);
	derivatives(xF,yF+1,zF,vAndD[2].dx,vAndD[2].dy,vAndD[2].dz,vAndD[2].dxy,vAndD[2].dxz,vAndD[2].dyz,vAndD[2].dxyz);
	derivatives(xF+1,yF+1,zF,vAndD[3].dx,vAndD[3].dy,vAndD[3].dz,vAndD[3].dxy,vAndD[3].dxz,vAndD[3].dyz,vAndD[3].dxyz);
	derivatives(xF,yF,zF+1,vAndD[4].dx,vAndD[4].dy,vAndD[4].dz,vAndD[4].dxy,vAndD[4].dxz,vAndD[4].dyz,vAndD[4].dxyz);
	derivatives(xF+1,yF,zF+1,vAndD[5].dx,vAndD[5].dy,vAndD[5].dz,vAndD[5].dxy,vAndD[5].dxz,vAndD[5].dyz,vAndD[5].dxyz);
	derivatives(xF,yF+1,zF+1,vAndD[6].dx,vAndD[6].dy,vAndD[6].dz,vAndD[6].dxy,vAndD[6].dxz,vAndD[6].dyz,vAndD[6].dxyz);
	derivatives(xF+1,yF+1,zF+1,vAndD[7].dx,vAndD[7].dy,vAndD[7].dz,vAndD[7].dxy,vAndD[7].dxz,vAndD[7].dyz,vAndD[7].dxyz);

	//P0 and P1
	vAndD[8].val=cubicInterpolate1DVec(mu.x,vAndD[0].val,vAndD[1].val,vAndD[0].dx,vAndD[1].dx);
	vAndD[8].dy=cubicInterpolate1DVec(mu.x,vAndD[0].dy,vAndD[1].dy,vAndD[0].dxy,vAndD[1].dxy);
	vAndD[8].dz=cubicInterpolate1DVec(mu.x,vAndD[0].dz,vAndD[1].dz,vAndD[0].dxz,vAndD[1].dxz);
	vAndD[8].dyz=cubicInterpolate1DVec(mu.x,vAndD[0].dyz,vAndD[1].dyz,vAndD[0].dxyz,vAndD[1].dxyz);

	//P2 and P3
	vAndD[9].val=cubicInterpolate1DVec(mu.x,vAndD[2].val,vAndD[3].val,vAndD[2].dx,vAndD[3].dx);
	vAndD[9].dy=cubicInterpolate1DVec(mu.x,vAndD[2].dy,vAndD[3].dy,vAndD[2].dxy,vAndD[3].dxy);
	vAndD[9].dz=cubicInterpolate1DVec(mu.x,vAndD[2].dz,vAndD[3].dz,vAndD[2].dxz,vAndD[3].dxz);
	vAndD[9].dyz=cubicInterpolate1DVec(mu.x,vAndD[2].dyz,vAndD[3].dyz,vAndD[2].dxyz,vAndD[3].dxyz);

	//P4 and P5
	vAndD[10].val=cubicInterpolate1DVec(mu.x,vAndD[4].val,vAndD[5].val,vAndD[4].dx,vAndD[5].dx);
	vAndD[10].dy=cubicInterpolate1DVec(mu.x,vAndD[4].dy,vAndD[5].dy,vAndD[4].dxy,vAndD[5].dxy);
	vAndD[10].dz=cubicInterpolate1DVec(mu.x,vAndD[4].dz,vAndD[5].dz,vAndD[4].dxz,vAndD[5].dxyz);
	vAndD[10].dyz=cubicInterpolate1DVec(mu.x,vAndD[4].dyz,vAndD[5].dyz,vAndD[4].dxyz,vAndD[5].dxyz);

	//P6 and P7
	vAndD[11].val=cubicInterpolate1DVec(mu.x,vAndD[6].val,vAndD[7].val,vAndD[6].dx,vAndD[7].dx);
	vAndD[11].dy=cubicInterpolate1DVec(mu.x,vAndD[6].dy,vAndD[7].dy,vAndD[6].dxy,vAndD[7].dxy);
	vAndD[11].dz=cubicInterpolate1DVec(mu.x,vAndD[6].dz,vAndD[7].dz,vAndD[6].dxz,vAndD[7].dxz);
	vAndD[11].dyz=cubicInterpolate1DVec(mu.x,vAndD[6].dyz,vAndD[7].dyz,vAndD[6].dxyz,vAndD[7].dxyz);

	//P8 and P9
	vAndD[12].val=cubicInterpolate1DVec(mu.y,vAndD[8].val,vAndD[9].val,vAndD[8].dy,vAndD[9].dy);
	vAndD[12].dz=cubicInterpolate1DVec(mu.y,vAndD[8].dz,vAndD[9].dz,vAndD[8].dyz,vAndD[9].dyz);

	//P10 and P11
	vAndD[13].val=cubicInterpolate1DVec(mu.y,vAndD[10].val,vAndD[11].val,vAndD[10].dy,vAndD[11].dy);
	vAndD[13].dz=cubicInterpolate1DVec(mu.y,vAndD[10].dz,vAndD[11].dz,vAndD[10].dyz,vAndD[11].dyz);

	//P12 and P13
	temp=cubicInterpolate1DVec(mu.z,vAndD[12].val,vAndD[13].val,vAndD[12].dz,vAndD[13].dz);

	if(pos.y<0)
		temp.y *= -1;
	vecOut+=temp;
}



