//////////////////////////////////////////////////
//cMRKText.h - by Matthew Bales
//misc string/text related functions
//
///////////////////////////////////////////////////
#ifndef CMRKTEXT_H_INCLUDED
#define CMRKTEXT_H_INCLUDED

//Standard Libraries
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <algorithm>
#include <sys/stat.h>
#include <cmath>

//Matt Libraries
#include "constants.hh"

//Root Libraries
#include "TSystem.h"
#include "TH1.h"
#include "TFile.h"

using namespace std;
using std::string;
using std::stringstream;


int getNonCommentLine(ifstream& inpFileStream, string& outString, char delim='\n');  //given and input file stream, extracts lines until a non comment line is reached.  A comment line is designed by starting with a #
int doubleFromList(string theList, int pos,double& result);         //Given a sting based list of doubles seperated by spaces/tabs, it will extact the pos'th one
int intFromList(string theList, int pos,int& result);               //Same only for ints
int boolFromList(string theList, int pos,bool& result);             //Same only for bools
int stringFromList(string theList,int pos,string& result);            //Same only for strings
int skipCommentLines(ifstream&);                                    //Given and input stream it skips lines until the next line will be a noncomment line
bool FileExists(string strFilename);                                //Checks to see whether file exists.  Has IFDEF for working with CINT
string addBeforeExtension(string baseString, string addString);     //Adds addstring before the extension in basestring (for filenames)
string getNextEventFileName(string inpFileName);                       //Based on Changbo's event naming pattern, increments the set number
string getNextResultFileName(string inpFileName);                      //Based on Matt's result file naming pattern, increments the set number

void typeAnythingToContinue(string message);                        //Simple cin input to type anything and press enter before continuing...useful for debugging

string int2str(int i);              //Converts an int to a str
string int2str(int i,int digits,bool strict=true);   //Converts an int to a str ensuring it has the appropriate digits (for simple alphabatetical order)
string double2str(double d,int inpPrecision=-1);        //Converts a double to a string

double str2double(string s);        //Converts a string to a double
int str2int(string s);              //Converts a string to an int
unsigned int str2uint(string s);              //Converts a string to an int

int convertITXtoTH1I(string itxFileName,TFile* theRootFile);  //Not very robust, must be customized conversion of an IGOR ITX file to a ROOT histogram

void frstr( string &source, string find, string replace );  //finds the find string in the source and replaces it with the replace string

void makeInpFiles(int startComputer, int numComputers, int startSet,int numSets,string dateCodeString, string eventTypeString, string resultTypeString);   //makes an array of input files for MRK

string addExeDir(string inpString);
string fileNameFromFullPath(string fullPath);
string filePathFromFullPath(string fullPath);
string fileExtensionFromFullPath(string fullPath);
int replaceAllInString(string& original,string toFind, string toReplace);

//string strFromList(string list, int position, char delimitor);

#endif // CMRKTEXT_H_INCLUDED
