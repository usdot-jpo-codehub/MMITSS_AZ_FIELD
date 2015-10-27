//**********************************************************************************
//
// © 2015 Arizona Board of Regents on behalf of the University of Arizona with rights
//       granted for USDOT OSADP distribution with the Apache 2.0 open source license.
//
//**********************************************************************************


/* Print2Log.h 
*  Created by Jun Ding
*  University of Arizona   
*  ATLAS Research Center 
*  College of Engineering
*
*  This code was develop under the supervision of Professor Larry Head
*  in the ATLAS Research Center.

*/

#pragma once

#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <istream>
#include <math.h>


#include "Signal.h"
#include "Array.h"
#include "LinkedList.h"
#include "GetInfo.h"

using namespace std;

int outputlog(char *output);
extern char temp_log[512];

void PrintPlan2Log(char *resultsfile);
void PrintFile2Log(char *resultsfile);
void PrintPhases2Log(PhaseStatus currentPhase); // Print the current phase status




void PrintPhases2Log(PhaseStatus currentPhase)
{
    char tmp_log[128]="Phase Information: ";
    char tmp_phase[16];
    for(int i=0;i<NumPhases;i++)
    {
        sprintf(tmp_phase," %d",currentPhase.phaseColor[i]);
        strcat(tmp_log,tmp_phase);
    }
    outputlog(tmp_log);
}
void PrintFile2Log(char *resultsfile)
{
    fstream fss;
    fss.open(resultsfile,fstream::in);

    if (!fss)
    {
        cout<<"***********Error opening the plan file in order to print to a log file!\n";
        sprintf(temp_log,"***********Error opening the plan file in order to print to a log file!\n");   
        outputlog(temp_log);
        exit(1);
    }
    string lineread;
    
    while(!fss.eof())
    {
        getline(fss,lineread);
        strcpy(temp_log,lineread.c_str());	
        strcat(temp_log,"\n"); outputlog(temp_log);		
    }   

    fss.close();

}

void PrintPlan2Log(char *resultsfile)
{
    fstream fss;
    fss.open(resultsfile,fstream::in);

    if (!fss)
    {
        cout<<"***********Error opening the plan file in order to print to the log file!\n";
        sprintf(temp_log,"***********Error opening the plan file in order to print to a log file!\n");   
        outputlog(temp_log);
        exit(1);
    }

    string lineread;
    
    while(!fss.eof())
    {
        getline(fss,lineread);
        strcpy(temp_log,lineread.c_str());	
        strcat(temp_log,"\n"); outputlog(temp_log);		
    }   

    fss.close();

}












