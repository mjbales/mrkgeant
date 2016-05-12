#include "cMRKText.hh"

int skipCommentLines(ifstream& inpFileStream)
{
    string sBuffer;
    if(!inpFileStream.is_open()|| inpFileStream.fail()||inpFileStream.eof())
        return -1;
    while(inpFileStream.peek()=='#' || inpFileStream.peek()=='%')
    {
        getline(inpFileStream,sBuffer);
    }

    if(!inpFileStream.is_open()|| inpFileStream.fail()||inpFileStream.eof())
        return -1;

    return 0;
}




int getNonCommentLine(ifstream& inpFileStream, string& outString,char delim)
{
    outString="";
    if(!inpFileStream.is_open()|| inpFileStream.fail()||inpFileStream.eof())
        return -1;

    string sBuffer("#");

    while(sBuffer[0] == '#' || sBuffer[0] == '%')
    {
        getline(inpFileStream,sBuffer,delim);
        if(inpFileStream.fail())
            return -1;
    }

    outString=sBuffer;
    return 0;
}

int doubleFromList(string theList, int pos,double& result)
{
    istringstream theStringStream(theList);

    string tempString;
    result =0;

    for(int i=0;i<=pos;i++)
    {
        theStringStream >> tempString;
        if(theStringStream.fail())
            return -1;
    }
    istringstream resultStream(tempString);
    resultStream >> result;
    return 0;
}

int intFromList(string theList, int pos,int& result)
{
    double tempDouble;
    int errorCode;
    errorCode = doubleFromList(theList,pos,tempDouble);
    result = (int)tempDouble;
    return errorCode;
}

int boolFromList(string theList, int pos,bool& result)
{
    double tempDouble;
    int errorCode;
    errorCode = doubleFromList(theList,pos,tempDouble);
    result = (bool)tempDouble;
    return errorCode;
}

int stringFromList(string theList, int pos,string& result)
{
    istringstream theStringStream(theList);
    string temp;

    for(int i=0;i<=pos;i++)
    {
        theStringStream >> temp;
        if(theStringStream.fail())
            return -1;
    }
    result = temp;
    return 0;
}


bool FileExists(string strFileName) {
  struct stat stFileInfo;
  bool blnReturn;
  int intStat;
#ifndef __CINT__
  // Attempt to get the file attributes
  intStat = stat(strFileName.c_str(),&stFileInfo);
  if(intStat == 0) {
    // We were able to get the file attributes
    // so the file obviously exists.
    blnReturn = true;
  } else {
    // We were not able to get the file attributes.
    // This may mean that we don't have permission to
    // access the folder which contains this file. If you
    // need to do that level of checking, lookup the
    // return values of stat which will give you
    // more details on why stat failed.
    blnReturn = false;
  }

  return(blnReturn);
}
#else //__CINT__
    Long_t *id,*size,*flags,*mt;
    return !(gSystem->GetPathInfo(strFileName.data(),id,size,flags,mt));
}
#endif //__CINT__


string addBeforeExtension(string baseString, string addString)
{
    string outString=baseString;
    int stringEnd = baseString.length()-1;

    for (int i=stringEnd-1; stringEnd>=1 ;i--)
    {
        if(baseString.at(i)=='.')
        {
            outString.insert(i,addString);
            break;
        }
    }

    return outString;
}

string getNextEventFileName(string inpFileName)
{

    //Example: MJB-Evn-hmg-n1.B3-147-20100421.root
    string outString=fileNameFromFullPath(inpFileName);
    int fileNumber;
    stringstream theStream;
    int digits;

    for (unsigned int i =0;i < outString.length()-3 ;i++ )
    {
    	if(outString.at(i)=='.')
    	{
    	    if(outString.at(i+1)=='B')
    	    {
    	        if(outString.at(i+5)=='-') //one digit number
    	        {
    	            fileNumber=(outString.at(i+4)-'0');
                    digits = 1;
    	        }
    	        else if(outString.at(i+6)=='-') // two digit number
    	        {
                    fileNumber=(outString.at(i+5)-'0')+(outString.at(i+4)-'0')*10;
                    digits = 2;
    	        }
                else
                {
                    fileNumber=(outString.at(i+6)-'0')+(outString.at(i+5)-'0')*10+(outString.at(i+4)-'0')*100;
                    digits=3;
                }
                fileNumber++;


                    theStream << fileNumber;

                outString.replace(i+4,digits,theStream.str());

                return filePathFromFullPath(inpFileName)+outString;

    	    }
    	}
    }

    return (outString = "");
}

string getNextResultFileName(string inpFileName)
{
    string outString=fileNameFromFullPath(inpFileName);
    int fileNumber;
    int digits;
    stringstream theStream;

    for (unsigned int i =0;i < outString.length()-3 ;i++ )
    {
    	if(outString.at(i)=='S' && outString.at(i+1)=='e' && outString.at(i+2)=='t')
    	{
    	    if(outString.at(i+4)=='_' || outString.at(i+4) == '.')
            {
                fileNumber=(outString.at(i+3)-'0');
                fileNumber++;
            }
    	    else if(outString.at(i+5)=='_' || outString.at(i+5) == '.')
            {
                fileNumber=(outString.at(i+4)-'0')+(outString.at(i+3)-'0')*10;
                fileNumber++;
            }
            else
            {
                fileNumber=(outString.at(i+5)-'0')+(outString.at(i+4)-'0')*10+(outString.at(i+3)-'0')*100;
                fileNumber++;
            }

            digits =1;
            for(int fileCheck=10;;)
            {
                if(fileNumber <= fileCheck)
                    break;
                digits++;
                fileCheck*=10;
            }



            theStream << fileNumber;

            outString.replace(i+3,digits,theStream.str());

            return filePathFromFullPath(inpFileName)+outString;


    	}
    }

    return (outString = "");
}

string int2str(int i){stringstream out;out << i; return out.str();}

int getMaxNumberByDigits(int digits)
{
   int maxNum=1;
    for(int k=0;k<digits;k++)
        maxNum*=10;
    maxNum-=1;
    return maxNum;
}

string int2str(int i,int digits,bool strict)
{
    stringstream out;
    int maxNum=getMaxNumberByDigits(digits);

    if(strict && (i > maxNum || i < 0))
        return out.str();

    for(int j=digits;j>=1;j--)
    {
        if(i<getMaxNumberByDigits(j-1)+1)
            out << "0";
        else
        {
            out << i;
            break;
        }
    }
    return out.str();
}

string double2str(double d, int inpPrecision)
{
    stringstream out;
    if(inpPrecision >0)
    {
        out.precision(inpPrecision);
        out.unsetf(ios::floatfield);
    }
    out << d;
    return out.str();
}

double str2double(string s){return atof(s.data());}

int str2int(string s){return atoi(s.data());}
unsigned int str2uint(string s){return atoi(s.data());}

int convertITXtoTH1I(string itxFileName,TFile* theRootFile)
{
    theRootFile->cd();

    int numBins;
    double binStart, binEnd, binSize;
    string s,waveName;
    stringstream ss;
    int data;
    string endString="\tEND";




    ifstream itxFile(itxFileName.data());

    if(!itxFile.is_open()|| itxFile.fail()||itxFile.eof())
        return -1;

    getline(itxFile,s,'\r'); //IGOR
    getline(itxFile,s,'\r'); //WAVE <WAVENAME>
    ss << s;
    ss >> s >> waveName;
    ss.clear();
    getline(itxFile,s,'\r'); //Begin
    //First going to find the bin information
    for(numBins=0; getline(itxFile,s,'\r') && s != endString && numBins < 1000000 && !itxFile.eof();numBins++)
    {

    }
    numBins--;

    sscanf(s.data(),"%*s %*s %*s %lf,%lf",&binStart,&binSize);


    binEnd = binStart + binSize*numBins;


    cout << waveName << " in " << itxFileName << " will be converted to histogram in " << theRootFile->GetPath() << endl;
    cout << "Num Bins: " << numBins << "  Bin Start: " << binStart << "  Bin End: " << binEnd << endl;
    itxFile.close();






    TH1I theHist(waveName.data(),waveName.data(),numBins,binStart,binEnd);
    theHist.Sumw2();
    itxFile.open(itxFileName.data());

    getline(itxFile,s,'\r'); //IGOR
    getline(itxFile,s,'\r'); //WAVE <WAVENAME>
    getline(itxFile,s,'\r'); //Begin
    for(int i=0; i <numBins; i++)
    {
        getline(itxFile,s,'\r'); //Data line
        sscanf(s.data(),"%d",&data);
        for(int j=0; j < data; j++) theHist.Fill((i+.5)*binSize);
    }

    theHist.Write();

    itxFile.close();

    return 0;
}

void frstr(string& source, string find, string replace ) {
    size_t uPos = 0;
    size_t uFindLen = find.length();
    size_t uReplaceLen = replace.length();

    if( uFindLen == 0 )
    {
        return;
    }

    for( ;(uPos = source.find( find, uPos )) != std::string::npos; )
    {
        source.replace( uPos, uFindLen, replace );
        uPos += uReplaceLen;
    }
}

void makeInpFiles(int startComputer, int numComputers, int startSet,int numSets,string dateCodeString, string eventTypeString,string resultTypeString)
{
    int numProcsPerComp=4;
    ofstream inpFile;

    for(int comp=startComputer;comp <= numComputers+startComputer-1; comp++){
        for(int proc=1;proc <= numProcsPerComp; proc=proc+1){
            string inpFileName="inp"+int2str(comp)+int2str(proc)+".txt";
            inpFile.open(inpFileName.data());


            for(int set = (comp-startComputer)*numProcsPerComp+proc-1+startSet;set < numSets+startSet;set+=numProcsPerComp*numComputers){
                string set2dig;
                if(set < 10)
                    set2dig = "0" + int2str(set);
                else
                    set2dig = int2str(set);

                inpFile << "##################################" << endl;
                inpFile << "#Run Name" << endl;
                inpFile << "Results"<< dateCodeString <<"_"<<resultTypeString<<"_EP_Set"<< set2dig <<".root" << endl;
                inpFile << "#" << endl;
                inpFile << "#Comment Line to Add" << endl;
                inpFile << "MRKROOT 0.2" << endl;
                inpFile << "#" << endl;
                inpFile << "#" << endl;
                inpFile << "#Geometry File" << endl;
                inpFile << "Geometry_RDK2_Matt2.txt" << endl;
                inpFile << "#" << endl;
                inpFile << "#Electric Field File #1 (Mirror Field)" << endl;
                inpFile << "Efield_RDK2_Mirror_Matt2.txt" << endl;
                inpFile << "#" << endl;
                inpFile << "#Scale (Mirror Potential)" << endl;
                inpFile << "1400" << endl;
                inpFile << "#" << endl;
                inpFile << "#Electric Field File #2 or None (SBD Field)" << endl;
                inpFile << "Efield_RDK2_SBD_Matt3.txt" << endl;
                inpFile << "#" << endl;
                inpFile << "#Scale (SBD should be negative)" << endl;
                inpFile << "-25000" << endl;
                inpFile << "#" << endl;
                inpFile << "#Electric Field File #3 (For no file use:  None)" << endl;
                inpFile << "None" << endl;
                inpFile << "#" << endl;
                inpFile << "#Scale (Any)" << endl;
                inpFile << "0" << endl;
                inpFile << "#" << endl;
                inpFile << "#Magnetic Field File #1 (RDK2 main)" << endl;
                inpFile << "Bfield_RDK2_Matt1.txt" << endl;
                inpFile << "#" << endl;
                inpFile << "#Scale (RDK2 scaled to 1)" << endl;
                inpFile << "1" << endl;
                inpFile << "#" << endl;
                inpFile << "#Magnetic Field File #2 (Pbcoil)" << endl;
                inpFile << "None" << endl;
                inpFile << "#" << endl;
                inpFile << "#Scale (scale to 30,771 = main coil strength)" << endl;
                inpFile << "0" << endl;
                inpFile << "#" << endl;
                inpFile << "#Magnetic Field File #3" << endl;
                inpFile << "None" << endl;
                inpFile << "#" << endl;
                inpFile << "#Scale" << endl;
                inpFile << "0" << endl;
                inpFile << "#" << endl;
                inpFile << "#Events File" << endl;
                inpFile << "MJB-Evn-"<<eventTypeString<<"-n1.B3-"<< set2dig <<"-20"<<dateCodeString<<".root" << endl;
                inpFile << "#" << endl;
                inpFile << "#Create tracks(1 true, 0 false)" << endl;
                inpFile << "0" << endl;
                inpFile << "#" << endl;
                inpFile << "#Use triple cubic interpolation(1 true, 0 false)" << endl;
                inpFile << "0" << endl;
                inpFile << "#" << endl;
                inpFile << "#Starting Event/Ending Event" << endl;
                inpFile << "0 999999" << endl;
                inpFile << "#" << endl;
                inpFile << "#Position Error per step in meters:" << endl;
                inpFile << "1e-10" << endl;
                inpFile << "#" << endl;
                inpFile << "#Velocity Error per step in meters per second:" << endl;
                inpFile << "1e-5" << endl;
                inpFile << "#" << endl;
                inpFile << "#Transport Method (0 Runge Kutta-Constant Step, 1 Runge Kutta-Dynamic Step, 2 Adiabatic)" << endl;
                inpFile << "0" << endl;
                inpFile << "#" << endl;
                inpFile << "#Continue?(1 true, 0 false)" << endl;
                if(set+numProcsPerComp*numComputers <= numSets)
                    inpFile << "1" << endl;
                else
                    inpFile << "0" << endl;

            }

        inpFile.close();
        }
    }
}

void typeAnythingToContinue(string message)
{
    string tempString;
    cout << message << " Type Anything to continue: ";
    cin >> tempString;
    return;
}

string addExeDir(string inpString)
{
    string returnString = "/media/UofMPhysChupp/Matt/exe/"+inpString;
    return returnString;
}

string fileNameFromFullPath(string fullPath)  //return fullPath if not found
{
    size_t found;
    found=fullPath.find_last_of("/\\");
    if(found==string::npos)
        return fullPath;
    return fullPath.substr(found+1);
}

string filePathFromFullPath(string fullPath)
{
    size_t found;
    found=fullPath.find_last_of("/\\");
    if(found==string::npos)
        return "";
    return fullPath.substr(0,found+1);
}

string fileExtensionFromFullPath(string fullPath)
{
    size_t found;
    found=fullPath.find_last_of(".");
    if(found==string::npos)
        return "";
    return fullPath.substr(found+1);
}

int replaceAllInString(string& original,string toFind, string toReplace)
{
    size_t pos=0;
    int numReplaced=0;
    for(;;)
    {
        pos=original.find(toFind,pos);
        if(pos==string::npos)
            break;
        original.replace(pos,toFind.length(),toReplace);
        numReplaced++;
    }

    return numReplaced;
}
