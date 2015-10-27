//**********************************************************************************
//
// © 2015 Arizona Board of Regents on behalf of the University of Arizona with rights
//       granted for USDOT OSADP distribution with the Apache 2.0 open source license.
//
//**********************************************************************************



/* IDMSGcnt.h 
*  Created by Jun Ding
*  University of Arizona   
*  ATLAS Research Center 
*  College of Engineering
*
*  This code was develop under the supervision of Professor Larry Head
*  in the ATLAS Research Center.

*/

#pragma once

#include <fstream>
#include <iostream>

using namespace std;

class IDMSGcnt
{
public:
	int TempID;
	int MSGcnt;  // 0-127
public:
	IDMSGcnt()
	{
		TempID=0;
		MSGcnt=0;
	}
	IDMSGcnt(int id,int msgcnt)
	{
		TempID=id;
		MSGcnt=msgcnt;
	}
	IDMSGcnt(const IDMSGcnt& that)
	{
		TempID=that.TempID;
		MSGcnt=that.MSGcnt;
	}
public:
	~IDMSGcnt(){}
	friend ostream &operator <<(ostream &stream, IDMSGcnt e)
	{
		stream<<"TempID is: "<<e.TempID<<", MSGcnt is: "<<e.MSGcnt<<endl;
		return stream;
	}
};