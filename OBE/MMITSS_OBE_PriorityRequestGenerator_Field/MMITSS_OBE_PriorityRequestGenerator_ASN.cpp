/*NOTICE:  Copyright 2014 Arizona Board of Regents on behalf of University of Arizona.
 * All information, intellectual, and technical concepts contained herein is and shall
 * remain the proprietary information of Arizona Board of Regents and may be covered
 * by U.S. and Foreign Patents, and patents in process.  Dissemination of this information
 * or reproduction of this material is strictly forbidden unless prior written permission
 * is obtained from Arizona Board of Regents or University of Arizona.
 */

/*  MRP_PriorityRequestServer.cpp  
 *  Created by Mehdi Zamanipour on 9/27/14.
 *  University of Arizona
 *  ATLAS Research Center
 *  College of Engineering
 *
 *  This code was develop under the supervision of Professor Larry Head
 *  in the ATLAS Research Center.
 *
 *  Revision History:
 *  1. SRM ASN library added to support j2735 srm standard.
 *  2. 
 *  3. 
 *  3. 
 *  
 */


/* This application can   1) Receive and process the SRM that is being sent from VISSIM DriveModel.dll   (Require for Laboratory Tests)
 * 						  2) Acquire GPS position from OBE GPS unit and struct the SRM  (Require for Road Tests) 										
   This applicaton should be run on Savari ASD
Prerequisits:
1. Create a log file in the /nojournal/bin folder on the device for logging
2. Modify the ConfigInfo.txt file in the folder of the VISSIM model and write the IP address of the OBE in it
3. required lib in the make file              LIBS =-L$(TOOLCHAIN)/lib -L$(TOOLCHAIN)/usr/lib -lstdc++ -luClibc++ -leloop -lnetsnmp -lj2735 -lsae -lm -lgps -lgpsapi -lsavarimath
4. Arguments that can be modified by users are :
   - The broadcasting address can be changed by the user. If Communication is through WME the address should be 127.0.0.1 otherwise the address is 192.168.1.255
   - If iLogIndicator is 2, more details information is logged
   - If outof the map detection time is 5, means after 5 * (0.5) =2.5 second if the vehicle GPS location shows the vehicle is out of the map, the request would be deleted
   - Key characters for arguments are:
    -c if c=1 the code works in Lab test, if c=2 code works in road. (by default c=2)
    -b change broadcastaddress
    -l change logging option
    -r DSRC Range 
    -o Out of The MAP detection time 
    -p the port of SRM should be sent to , default is 4444
    -s the percentage for speed change limit. SRM will be sent if the speed changes beyond this level
     -u the request serving Rectangle Range by default it is 6 seconds 
    
	
*/



#include <SRM.h>

#include <libgps.h>



#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <istream>
#include "math.h"
#include "time.h"
#include <time.h>
#include <vector>
#include <sys/time.h>

#include <iomanip>
#include <stddef.h>
#include <dirent.h>

#include <string>
using namespace std;

// Added headers
#include "NMAP.h"
#include "geoCoord.h"
#include "BasicVehicle.h"
#include "ConnectedVehicle.h"
#include "LinkedList.h"
#include "MAPEntry.h"
#include "ReqEntry.h"


#ifndef SRM_MSG_SIZE
#define SRM_MSG_SIZE  (197)
#endif


#ifndef BSM_MSG_SIZE
#define BSM_MSG_SIZE  (45)
#endif

#define ACTIVE 1
#define NOT_ACTIVE -1
#define LEAVE 0



//define log file name
char temp_log[1024];
char logfilename[256]   = "/nojournal/bin/log/MRP_PRG_";
char ConfigInfo[256]	= "/nojournal/bin/ConfigInfo.txt";
char vehicleidFile[256] = "/nojournal/bin/vehicleid.txt";
char MAP_File_Name[128] = "/nojournal/bin/Daysi_Gav_Reduced.nmap"; 
char MAPsListFile[256]  = "/nojournal/bin/Intersection_maps.txt";
char ActiveMAPFile[256] = "/nojournal/bin/ActiveMAP.txt";
char ReqListFile[128]   = "/nojournal/bin/psm.txt";
char cBusStopsFile[128] = "/nojournal/bin/busStopsRange.txt";


char BROADCAST_ADDR[64]="127.0.0.1";//"127.0.0.1";   //DSRC
int PRPORT=4444;
int LaneNode_Phase_Mapping[8][8][20];
int outputlog(char *output);
int iLogIndicator=0;
char ctimestamp[128];

// srm related variable and functions
//~ char buf[SRM_MSG_SIZE];
//~ uint8_t srmbuf[SRM_MSG_SIZE]; 
//~ int iType;
//~ J2735SRM_t srm;
//~ uint32_t oob;

//~ void printsrmcsv (J2735SRM_t *srm);
void formBSMOfSRM(BasicVehicle *veh, int tempID, int msgcnt, double lat, double lon, double heading, double speed, double elev);   // this function is used when MRP_PRG applied for road test
char BSMblob[38];
char buf[SRM_MSG_SIZE];
char SRMbuf[SRM_MSG_SIZE];
int ret;

void fillSRM(SRM_t *srm, int intersectioId, int iMsgCnt,bool bIsCanceled,int iInLane,int ioutlane, int iETA,double dMingrn,int vehState , int Rl_Ru,int vehtype, char *vehiclename, char * bsmblob);  // this function is used when MRP_PRG applied for lab test
SRM_t * srmType=0;
asn_enc_rval_t ec; /* Encoder return value */
asn_dec_rval_t rval;
// wireless connection variables
int sockfd;
int broadcast=1;
struct sockaddr_in sendaddr;
struct sockaddr_in recvaddr;
void setupConnection();
int  gps_init();
void printgpscsv ();
void read_gps ();
gps_data_t *gps_handle;
savari_gps_data_t gps;
int iDSRCRange=300; // DSRC Rannge by default is 300 meters
void populate_gps_values();

// variables for reading vehicleid
int iVehicleId;
int iVehiclePriorityLevel;   // actually is the vehicle class: NOW we have two priorities EV(1) TRANSIT(2). The information is included in the vehicleID.txt
void get_veh_id(char filename[]);
//int Priority;  
char cVehicleName[128];

// Reading the Bus Stops Range
void getBusStopsRange(char filename[]); // This function read the distance of the bus stops (far-side bus stops) to the intersections from file.
int iBusStopsApprRange[100][8]; // consider 100 intersection at most for a transit route. Each intersetcion has 8 approaches at most. 
int iBusStopsMAPID[100]; // the ID of the intersections in the bus route
int iNoIntersec=0; // total number of intersection in the transit route!


// Map DS
geoCoord geoPoint;
double ecef_x,ecef_y,ecef_z;			
//Important!!!
//Note the rectangular coordinate system: local_x is N(+) S(-) side, and local_y is E(+) W(-) side 
//This is relative distance to the ref point!!!!
//local z can be considered as 0 because no elevation change.
double local_x,local_y,local_z;
void initializeRefPoint(MAP * newmap);
void storeNodes(MAP * newmap);
double GetSeconds();
void readMAPsFromFile(char *filename,LinkedList<MAPEntry>& mapList);
int findMAPPositionInList(LinkedList<MAP> MapList,long id);
int iOutOfMAPTimer=5;  // If outof the map detection time is 5, means after 5 * (0.5) =2.5 second if the vehicle GPS location shows the vehicle is out of the map, hhe request would be deleted

// Variables and function for location vehicle in the map
void DistanceFromLine(double cx, double cy, double ax, double ay ,double bx, double by, double &distanceSegment, double &distanceLine); //Shayan Added for Geo Fencing 10.3.14  
void FindVehInMap(double Speed, double Heading,int nFrame,double N_Offset1,double E_Offset1,double N_Offset2,double E_Offset2, MAP NewMap,double &Dis_curr, double &ETA, int &iVehcleStatus,int &approach, int &lane, double &dMinGrn);
double Dis_curr=0;  //current distance to the stop bar
double ETA=0;       //estimated travel time to stop bar
int iVehicleStatus; 		// 1: appraoching; 2: leaving; 3: queue  4: Not in the map
int lane=0;
int approach=0;
int iOutLaneApproach=0;
int iOutLaneLane=0;
double dMinGrn=0.0;
BasicVehicle vehIn;
ConnectedVehicle TempVeh;   //List to store the vehicle trajectory
void storeVehicleTrajectory(int &pro_flag, int &Veh_pos_inlist);
LinkedList <ConnectedVehicle> trackedveh;
int tmp_nFrame;
double tmp_speed, tmp_heading, tmp_N_Offset1, tmp_E_Offset1,tmp_N_Offset2,tmp_E_Offset2;
int iRu_RlRange=2; // The range between upper estimated arrival time and lower estimated arrival time. ( upper and lower bound of serving rectangle )

// Filling SRM variables
//~ void fillSRM(SRM_t *srm, BasicVehicle *veh, int intersectioId, int iMsgCnt, bool bIsCanceled, int iInLane, int ioutlane, int iETA, double dMingrn, int vehState, int prioritylevel, int Ru_Rl, char* cvehiclename);   // this function is used when MRP_PRG applied for road test
//~ void fillSRM(J2735SRM_t *srm,int intersectioId, int iMsgCnt,bool bIsCanceled,int iInLane,int ioutlane, int iETA,double dMingrn,int vehState , int Rl_Ru, char *vehiclename);  // this function is used when MRP_PRG applied for lab test
int iInLane, iHour,iMin,iSec,iETA, iOutLane;
bool bIsCanceled=false;
int iMsgCnt=0;

int intID;
void obtainEndofService( int ihour,int imin,int isec, int iETA,int &iHour,int &iMin,int &iSec, int iMinGrn,int Ru_Rl);

void xTimeStamp( char * pc_TimeStamp_ );
int msleep(unsigned long milisec);
void setup_logging();

// Vairables required for getting GPS 
float laneheading; //current vehicle approaching direction
float m_vehtime;
float m_vehx;
float m_vehy;
double m_vehlat,m_vehlat_pre;
double m_vehlong,m_vehlong_pre;
float m_vehspeed;
float m_vehtraveltime;//estimated travel time to the approaching signal
int   m_vehphase;   // Approaching phase: Not approaching any intersection if <=0
double dElevation,dElevation_pre;
double dgpsTime=0.0;
double pre_speed=0.0;
double dHeading=0.0;
double pre_fix=0.0;
	
// Acknowledgement related variables and functions
int ReqListFromFile(char *filename,LinkedList<ReqEntry>& Req_List); //read the request list from request_combine txt file and retunr the number of request and also put the read request into the request data structure
int isThisVehIDinList( long VehID, LinkedList<ReqEntry> Req_List); // Will return the position of the request with VehID in the Req_List
double iSpeedChngPrc=0.2; // Critical Speed change percentage. SRM will be sent again if the speed changes beyond this level
LinkedList<ReqEntry> sentReqList;  // Req list for requests in thet is sent 
// Code Usage !!!!!! Important !!!!!
int icodeUsage=2;  //if codeusage=1 the code works in Lab test, if codeusage=2 code works in road. (by default c=2)
double dSpeedChangeLmtForSRM=0.0;
void calculateETA(int iStartMinute,int iStartSecond,int iEndMinute,int iEndSecond,int &iETA );
void obtainInLaneOutLane( int srmInLane, int srmOutLane, int & inApproach, int &outApproach, int &iInlane, int &Outlane);
void populateMAPNodes(MAP NewMap); // add MAP node to the NEW MAP structure and calculate node headings 
//void putPrioVehInPrioVehList(LinkedList<PriorityVehicle> &pVehList, int vehId, char* ,SRM_t *srm);
//LinkedList<PriorityVehicle> priorityVeh;
double  geoFencing( MAP NewMap);
int whatIsVISSIMType(int vissimVehType); 

int main ( int argc, char* argv[] )
{
	int iIsItaTransit=0;   // if the vehicle type is a transit, we have to have the location of nearside bus stops. 
	int iBusIsInRange=0;
	double dCurrenttime=0.0;
	LinkedList<MAP> MAPsParsedList;  // to store the parsed list from MAPlist 
	int iMAPPositionInList;
	long lMAPID;
	fstream fsActiveMAP;
	fsActiveMAP.open(ActiveMAPFile, ios::out | ios:: trunc);
	fsActiveMAP.close();
	int pro_flag=0;  //indicate whether to map the received data in the MAP
	int Veh_pos_inlist;
	double tmp_speed, tmp_heading, tmp_N_Offset1, tmp_E_Offset1,tmp_N_Offset2,tmp_E_Offset2;
	int temp_msgCnt=0;
	int NumberOfReq=0;    // Number of Requests in the Requests List
	int isentMsgDivisor; 
	int iacknMsgDivisor;
	long lTempVehID;
	double dTempSpeed;
	
	// ---------- Managing the input arguments-------------
	while ((ret = getopt(argc, argv, "c:b:l:r:o:u:p:s:")) != -1)    // -b change broadcastaddress -l change logging option -r DSRC Range -o Out of The MAP detection time -p the port of SRM should be sent  default is 4444 -c code usage 
	{
		switch (ret) 
		{
	    case 'c':
			icodeUsage = atoi (optarg);
			printf("Code usage is : %d\n", icodeUsage);
			break;
		case 'b':
			strcpy (BROADCAST_ADDR, optarg);
			printf ("BroadCast Address : %s\n", BROADCAST_ADDR);
			break;
		case 'l':
			iLogIndicator = atoi (optarg);
			printf ("Logging Option : %d\n", iLogIndicator);
			break;
		case 'r' :
			iDSRCRange = atoi(optarg);
			printf ("DSRC Range : %d\n", iDSRCRange);
			break;
		case 'u' :
			iRu_RlRange = atoi (optarg);
			printf ("Serving Rectangle Range : %d\n", iRu_RlRange);
			break;
		case 'o' :
			iOutOfMAPTimer = atoi (optarg);
			printf ("Out of MAP Timer : %d\n", iOutOfMAPTimer);
			break;
		case 'p' :
			PRPORT = atoi (optarg);
			printf ("Port is : %d\n", PRPORT);
			break;
		case 's' :
			iSpeedChngPrc = atoi (optarg);
			printf ("Speed Chaneg Limit Percentage for Resending the Request is : %f\n", iSpeedChngPrc);
			break;
		default:
			return 0;
		}
	}  
	
	setup_logging();
	//-----Getting the Bus stops range to intersections for the transit vehicles (if there is any)
	getBusStopsRange(cBusStopsFile);
	
	if (icodeUsage == 1) // if lab version of the PRG is used, SRM is gonaa process every 0.3 seconds
	{	
		dSpeedChangeLmtForSRM=4.8;
		printf("About to get data from Vissim...............\n"); 
	}
	if (icodeUsage == 2) // if road version of the PRG is used, SRM is gonaa process every 0.3 seconds
	{
		dSpeedChangeLmtForSRM=8.1;
		get_veh_id(vehicleidFile); // in case of updating the vehicle class
		gps_init() ;
		printf("About to query gps data..................\n");
		if (iVehiclePriorityLevel==2 )	
			iIsItaTransit=1;
	}
	
	
	//-----Begin of Network connection
	setupConnection();
	int addr_length = sizeof ( recvaddr );
	
	while (true)
	{
		dCurrenttime = GetSeconds();
		//-------- Start  getting gps position ( in road version ) or receiving SRM from Vissim (lab version)
		if (icodeUsage == 2) // means if the code is used for road tests
		{
			read_gps();
			populate_gps_values();
			formBSMOfSRM(&vehIn,iVehicleId, iMsgCnt,  m_vehlat, m_vehlong, dHeading, m_vehspeed, dElevation);
			vehIn.VehicleToBSM(BSMblob);
		}else  // for simulations
		{
			printf("\n ....  In the While loop .....\n ");
			memset(SRMbuf,0,SRM_MSG_SIZE);
			if (recvfrom(sockfd, SRMbuf, sizeof(SRMbuf), 0 , (struct sockaddr *)&sendaddr, (socklen_t *)&addr_length) <0 )
			{ 	
				printf("Receive Request failed\n");	
				continue;
			}
			srmType = (SRM_t *) calloc(1, sizeof * srmType);
			srmType->timeOfService=(struct DTime*)calloc(1,sizeof( DTime_t));
			srmType->endOfService=(struct DTime*)calloc(1,sizeof(DTime_t));
			srmType->transitStatus= (BIT_STRING_t*)calloc(1, sizeof(BIT_STRING_t));
			srmType->vehicleVIN=( VehicleIdent_t*)calloc(1, sizeof(VehicleIdent_t));
			rval = ber_decode(0, &asn_DEF_SRM,(void **)&srmType, SRMbuf, sizeof(SRMbuf));	
			//xer_fprint(stdout, &asn_DEF_SRM, srmType);
			if (rval.code==RC_OK)
			{	sprintf(temp_log,"SRM Recieved From VISSIM: Decode Success");outputlog(temp_log); cout<<temp_log<<endl; }
			else
			{	sprintf(temp_log,"SRM Received From VISSIM: Decode Failure"); outputlog(temp_log); cout<<temp_log<<endl; }
			for ( int i=0;i<38;i++) 
				BSMblob[i]=srmType->vehicleData.buf[i];
			vehIn.BSMToVehicle(BSMblob);
			iIsItaTransit=whatIsVISSIMType(srmType->request.type.buf[0]);
			//	free(srmType);
		//	putPrioVehInPrioVehList(priorityVeh,vehIn.TemporaryID,BSMblob,srmType);
		}
		//-------- End of getting gps position ( in road version ) or receiving SRM from Vissim (lab version)
		
		// Managing the received MAP
		LinkedList<MAPEntry>  MAPList;
		readMAPsFromFile(MAPsListFile,MAPList);
		MAPList.Reset();
		double tmp_distance=99999.0;
		int tmp_intersection_id=0;
		int tmp_leaving_intersection_id=0;
		int flag_active_map=0;
		int flag_leaving_map=0;
		float temp_ETA;
		double temp_dMinGrn;
		int temp_lane,temp_approach;
		int temp_vehState;
		int save_traj_flg=0;
		
		// ---- Starting the Map Process to know which map is active , and  which obe is leaving map 
		while (!MAPList.EndOfList())
		{
			MAP NewMap;
			lMAPID=MAPList.Data().ID;
			iMAPPositionInList=findMAPPositionInList(MAPsParsedList,lMAPID);
			sprintf(temp_log,"Received MAP ID %ld\n", lMAPID ); outputlog(temp_log); cout<<temp_log;
			if(iMAPPositionInList>=0) // we already have the map in our map list!!!
			{
				MAPsParsedList.Reset(iMAPPositionInList);
				NewMap=MAPsParsedList.Data();
				initializeRefPoint(&NewMap);
				//populateMAPNodes(NewMap);
				NewMap.MAP_Nodes.clear();
						
					for (unsigned int i=0;i<NewMap.intersection.Approaches.size();i++)
						for(unsigned int j=0;j<NewMap.intersection.Approaches[i].Lanes.size();j++)
							for(unsigned k=0;k<NewMap.intersection.Approaches[i].Lanes[j].Nodes.size();k++)
							{	
								LaneNodes temp_node;
								temp_node.index.Approach=NewMap.intersection.Approaches[i].Lanes[j].Nodes[k].index.Approach;
								temp_node.index.Lane=NewMap.intersection.Approaches[i].Lanes[j].Nodes[k].index.Lane;
								temp_node.index.Node=NewMap.intersection.Approaches[i].Lanes[j].Nodes[k].index.Node;
								temp_node.Latitude=NewMap.intersection.Approaches[i].Lanes[j].Nodes[k].Latitude;
								temp_node.Longitude=NewMap.intersection.Approaches[i].Lanes[j].Nodes[k].Longitude;
								geoPoint.lla2ecef(temp_node.Longitude,temp_node.Latitude,0.0 ,&ecef_x,&ecef_y,&ecef_z);
								geoPoint.ecef2local(ecef_x,ecef_y,ecef_z,&local_x,&local_y,&local_z);
								temp_node.N_Offset=local_x;
								temp_node.E_Offset=local_y;						
								NewMap.MAP_Nodes.push_back(temp_node);
							}
					
					int tmp_pos=0;
					for (unsigned int i=0;i<NewMap.intersection.Approaches.size();i++)
						for(unsigned int j=0;j<NewMap.intersection.Approaches[i].Lanes.size();j++)
							for(unsigned k=0;k<NewMap.intersection.Approaches[i].Lanes[j].Nodes.size();k++)
							{
								
								if(NewMap.MAP_Nodes[tmp_pos].index.Approach%2==1)  //odd approaches, approching lanes
								{
									if(k<NewMap.intersection.Approaches[i].Lanes[j].Nodes.size()-1)
									{
										NewMap.MAP_Nodes[tmp_pos].Heading=atan2(NewMap.MAP_Nodes[tmp_pos].N_Offset-NewMap.MAP_Nodes[tmp_pos+1].N_Offset,NewMap.MAP_Nodes[tmp_pos].E_Offset-NewMap.MAP_Nodes[tmp_pos+1].E_Offset)*180.0/PI;
										NewMap.MAP_Nodes[tmp_pos].Heading=90.0-NewMap.MAP_Nodes[tmp_pos].Heading;
									}
									else
									{
										NewMap.MAP_Nodes[tmp_pos].Heading=NewMap.MAP_Nodes[tmp_pos-1].Heading;	
										//cout<<NewMap.MAP_Nodes[tmp_pos].Heading<<" "<<NewMap.MAP_Nodes[tmp_pos-1].Heading<<endl;
									}

									//	sprintf(temp_log,"Appraoching heading %d is %f\n",tmp_pos,NewMap.MAP_Nodes[tmp_pos].Heading);
									//	cout<<temp_log;
								}
								if(NewMap.MAP_Nodes[tmp_pos].index.Approach%2==0)  //even approaches, leaving lanes
								{
									if(k<NewMap.intersection.Approaches[i].Lanes[j].Nodes.size()-1)
									{
										NewMap.MAP_Nodes[tmp_pos].Heading=atan2(NewMap.MAP_Nodes[tmp_pos+1].N_Offset-NewMap.MAP_Nodes[tmp_pos].N_Offset,NewMap.MAP_Nodes[tmp_pos+1].E_Offset-NewMap.MAP_Nodes[tmp_pos].E_Offset)*180.0/PI;
										NewMap.MAP_Nodes[tmp_pos].Heading=90.0-NewMap.MAP_Nodes[tmp_pos].Heading;
									}
									else
									{
										NewMap.MAP_Nodes[tmp_pos].Heading=NewMap.MAP_Nodes[tmp_pos-1].Heading;	
									}
									
									//	sprintf(temp_log,"Leaving heading %d is %f\n",tmp_pos,NewMap.MAP_Nodes[tmp_pos].Heading);
									//	cout<<temp_log;
								}
																
								if(NewMap.MAP_Nodes[tmp_pos].Heading<0)
								 NewMap.MAP_Nodes[tmp_pos].Heading+=360;
								
								//sprintf(temp_log,"%d %d %d %lf %lf %lf %lf %f\n",NewMap.MAP_Nodes[tmp_pos].index.Approach,NewMap.MAP_Nodes[tmp_pos].index.Lane,NewMap.MAP_Nodes[tmp_pos].index.Node,NewMap.MAP_Nodes[tmp_pos].N_Offset,NewMap.MAP_Nodes[tmp_pos].E_Offset,NewMap.MAP_Nodes[tmp_pos].Latitude,NewMap.MAP_Nodes[tmp_pos].Longitude,NewMap.MAP_Nodes[tmp_pos].Heading);
								//outputlog(temp_log); cout<<temp_log;										
								tmp_pos++;								
							}
			}
			else  // we do not have the map in our map list, we should read it for the first time!!
			{
				// Create the MAP file which would be Intersection_MAP_ID.nmap
				char predir[64]         = "/nojournal/bin/";
				char cMAPfile[128]      = "Intersection_MAP_";
				strcat(predir,cMAPfile);
				char charid[16];
				sprintf(charid,"%ld",lMAPID);
				strcat(predir,charid);
				strcat(predir,".nmap");
				NewMap.ParseIntersection(predir); // "/nojournal/bin/Intersection_MAP_XXX.nmap");
				strcpy(predir,"/nojournal/bin/");
				// Initialize the refrence points
				initializeRefPoint(&NewMap);
				// Store Map nodes information to Map_Node Structure from MAP structure
				//populateMAPNodes(NewMap);
				
				NewMap.MAP_Nodes.clear();
					for (unsigned int i=0;i<NewMap.intersection.Approaches.size();i++)
						for(unsigned int j=0;j<NewMap.intersection.Approaches[i].Lanes.size();j++)
							for(unsigned k=0;k<NewMap.intersection.Approaches[i].Lanes[j].Nodes.size();k++)
							{	
								LaneNodes temp_node;
								temp_node.index.Approach=NewMap.intersection.Approaches[i].Lanes[j].Nodes[k].index.Approach;
								temp_node.index.Lane=NewMap.intersection.Approaches[i].Lanes[j].Nodes[k].index.Lane;
								temp_node.index.Node=NewMap.intersection.Approaches[i].Lanes[j].Nodes[k].index.Node;
								temp_node.Latitude=NewMap.intersection.Approaches[i].Lanes[j].Nodes[k].Latitude;
								temp_node.Longitude=NewMap.intersection.Approaches[i].Lanes[j].Nodes[k].Longitude;
								geoPoint.lla2ecef(temp_node.Longitude,temp_node.Latitude,0.0 ,&ecef_x,&ecef_y,&ecef_z);
								geoPoint.ecef2local(ecef_x,ecef_y,ecef_z,&local_x,&local_y,&local_z);
								temp_node.N_Offset=local_x;
								temp_node.E_Offset=local_y;
								NewMap.MAP_Nodes.push_back(temp_node);
							}
							
					int tmp_pos=0;
					for (unsigned int i=0;i<NewMap.intersection.Approaches.size();i++)
						for(unsigned int j=0;j<NewMap.intersection.Approaches[i].Lanes.size();j++)
							for(unsigned k=0;k<NewMap.intersection.Approaches[i].Lanes[j].Nodes.size();k++)
							{
								if(NewMap.MAP_Nodes[tmp_pos].index.Approach%2==1)  //odd approaches, approching lanes
								{
									if(k<NewMap.intersection.Approaches[i].Lanes[j].Nodes.size()-1)
									{
										NewMap.MAP_Nodes[tmp_pos].Heading=atan2(NewMap.MAP_Nodes[tmp_pos].N_Offset-NewMap.MAP_Nodes[tmp_pos+1].N_Offset,NewMap.MAP_Nodes[tmp_pos].E_Offset-NewMap.MAP_Nodes[tmp_pos+1].E_Offset)*180.0/PI;
										NewMap.MAP_Nodes[tmp_pos].Heading=90.0-NewMap.MAP_Nodes[tmp_pos].Heading;
									}
									else
									{
										NewMap.MAP_Nodes[tmp_pos].Heading=NewMap.MAP_Nodes[tmp_pos-1].Heading;	
									}
								}
								if(NewMap.MAP_Nodes[tmp_pos].index.Approach%2==0)  //even approaches, leaving lanes
								{
									if(k<NewMap.intersection.Approaches[i].Lanes[j].Nodes.size()-1)
									{
										NewMap.MAP_Nodes[tmp_pos].Heading=atan2(NewMap.MAP_Nodes[tmp_pos+1].N_Offset-NewMap.MAP_Nodes[tmp_pos].N_Offset,NewMap.MAP_Nodes[tmp_pos+1].E_Offset-NewMap.MAP_Nodes[tmp_pos].E_Offset)*180.0/PI;
										NewMap.MAP_Nodes[tmp_pos].Heading=90.0-NewMap.MAP_Nodes[tmp_pos].Heading;
									}
									else
									{
										NewMap.MAP_Nodes[tmp_pos].Heading=NewMap.MAP_Nodes[tmp_pos-1].Heading;	
									}
								}
								if(NewMap.MAP_Nodes[tmp_pos].Heading<0)
								NewMap.MAP_Nodes[tmp_pos].Heading+=360;
								tmp_pos++;								
							}
				
				
				MAPsParsedList.InsertRear(NewMap);		
				sprintf(temp_log,"MAP id  :  %d  Added succesfully \n", MAPsParsedList.Data().intersection.ID ); outputlog(temp_log); cout<<temp_log;
			}
			if (save_traj_flg==0)
			{
				storeVehicleTrajectory(pro_flag, Veh_pos_inlist );   //Edited by YF 08/20/2014
				save_traj_flg=1;
			}
			// ------ Locate vehicle  in the map
			if(pro_flag==1)
			{
				
				trackedveh.Reset(Veh_pos_inlist);
				tmp_speed=trackedveh.Data().Speed;
				tmp_heading=trackedveh.Data().heading;
				tmp_nFrame=trackedveh.Data().nFrame;
				geoPoint.lla2ecef(trackedveh.Data().traj[trackedveh.Data().nFrame-1].longitude,trackedveh.Data().traj[trackedveh.Data().nFrame-1].latitude,trackedveh.Data().traj[trackedveh.Data().nFrame-1].elevation,&ecef_x,&ecef_y,&ecef_z);
				geoPoint.ecef2local(ecef_x,ecef_y,ecef_z,&local_x,&local_y,&local_z);					
				tmp_N_Offset1=local_x;
				tmp_E_Offset1=local_y;
				geoPoint.lla2ecef(trackedveh.Data().traj[trackedveh.Data().nFrame-2].longitude,trackedveh.Data().traj[trackedveh.Data().nFrame-2].latitude,trackedveh.Data().traj[trackedveh.Data().nFrame-2].elevation,&ecef_x,&ecef_y,&ecef_z);
				geoPoint.ecef2local(ecef_x,ecef_y,ecef_z,&local_x,&local_y,&local_z);					
				tmp_N_Offset2=local_x;
				tmp_E_Offset2=local_y;
				
			
				//------------------ Geo Fencing  Starts -------------
				double distanceToLine=0.0;
				distanceToLine=geoFencing(NewMap);
				//------------------ Geo Fencing  ends (disanceLine obtained!!!) -------------
						
				if(fabs(tmp_N_Offset1)>iDSRCRange || fabs(tmp_E_Offset1)>iDSRCRange || distanceToLine > 8) //If the vehicle is too far way from the intersection, or the vehicle is out of geoo fencing, directly consider as not on the map 
				{
					iVehicleStatus=4; 
					sprintf(temp_log,"Vehicle is far from any intersection \n ");
					outputlog(temp_log); cout<<temp_log;
				}	
				else
					FindVehInMap(tmp_speed,tmp_heading,tmp_nFrame,tmp_N_Offset1,tmp_E_Offset1,tmp_N_Offset2,tmp_E_Offset2,NewMap,Dis_curr,ETA,iVehicleStatus,approach,lane,dMinGrn);
				
				// ---- If the vehicle is a transit, we should consider the farside bus stop location to the next intersection as the range ( this range comes from busStopsRange.txt file)
				
				iBusIsInRange=0;
				if ( iIsItaTransit==1)
				{
					for (int k=0 ; k<iNoIntersec; k++)
					{
						if ( (iBusStopsMAPID[k]==NewMap.intersection.ID)&&(Dis_curr<=iBusStopsApprRange[k][approach]))
							iBusIsInRange=1;
					}
				}
				if ((iVehicleStatus==1)  || ( iVehicleStatus==3 )  &&  ( ((iIsItaTransit==1)&&(iBusIsInRange==1)) || (iIsItaTransit==0) ))  // vehicle is approaching or in the queue
				{	
					if(Dis_curr<tmp_distance)
					{
						flag_active_map=1;
						tmp_distance=Dis_curr;
						tmp_intersection_id=MAPsParsedList.Data().intersection.ID;
						temp_ETA=ETA;
						temp_approach=approach;
						temp_lane=lane;
						temp_dMinGrn=dMinGrn;
						temp_vehState=iVehicleStatus;
						temp_msgCnt++;
						cout<< "closest intersectiion ID " << tmp_intersection_id<< endl;
					}
					MAPsParsedList.Data().iOutofMap=0;    
				}
				if ( (iVehicleStatus==2)  && (MAPsParsedList.Data().intersection.iFlag>0)) // vehicle is leaving
				{
					if (MAPsParsedList.Data().intersection.iFlag==5) // Updating the Msg count 
					 temp_msgCnt=0;
					 else
					 temp_msgCnt++;
					tmp_leaving_intersection_id=MAPsParsedList.Data().intersection.ID;
					flag_leaving_map=1;
					temp_vehState=iVehicleStatus;
				}
				if  ( (iVehicleStatus==4) && (MAPsParsedList.Data().intersection.iFlag>0) )   // vehicle can not be found in the map
				{
					MAPsParsedList.Data().iOutofMap++; //increase the indicator YF:08/24/2014
					if(MAPsParsedList.Data().iOutofMap>=iOutOfMAPTimer)  //Three time continuously can't find veh in the map
					{
						temp_msgCnt=0;  // Reset the Msg count
						tmp_leaving_intersection_id=MAPsParsedList.Data().intersection.ID;
						flag_leaving_map=1;   // this flag used for both leaving time and when the vehicle can not be found in the map. in both cas the isCancel varible is Yes
						temp_vehState=iVehicleStatus;
					}
				}

			}  // End of locating the vehicle  in the map
			MAPList.Next();
			// while Maplist	
		}   
		pro_flag=0;   
		// ----  End of Map Process to know which map is active , which obe is leaving map  ----- 
		LinkedList<ReqEntry> acknReqList;  // Req list for requests that is being acknowleged by PSM
		NumberOfReq=ReqListFromFile(ReqListFile,acknReqList);
		acknReqList.Reset();
	
	//  ------- Sending the request to the intersection RSE that its MAP is Active!------- 
		MAPList.Reset();
		MAP NewMap;
		while (!MAPList.EndOfList())
		{ 				
			lMAPID=MAPList.Data().ID;
			iMAPPositionInList=findMAPPositionInList(MAPsParsedList,lMAPID);
			if(iMAPPositionInList>=0)
			{
				MAPsParsedList.Reset(iMAPPositionInList);
			}
			if (( flag_active_map==1 ) && (MAPList.Data().ID==tmp_intersection_id) ) // One map is active  and the vehicle is either approaching or in the queue 
			{
				iMAPPositionInList=findMAPPositionInList(MAPsParsedList,tmp_intersection_id);
				if(iMAPPositionInList>=0)
				{
					MAPsParsedList.Reset(iMAPPositionInList);
					NewMap=MAPsParsedList.Data();
				}
			
				// ---- seting the SRM elements ------//
				// ---- Obtaining inLane Outlane 
				iInLane=temp_approach*10+temp_lane;
				char tmp1[8];
				sprintf(tmp1,"%d.%d",temp_approach,temp_lane);
				string tmp_string(tmp1);
				for( int i=0; i<NewMap.intersection.Approaches[temp_approach-1].Lane_No;i++)
				{
					if (tmp_string.compare(NewMap.intersection.Approaches[temp_approach-1].Lanes[i].Lane_Name)==0)
					{
						iOutLaneApproach=NewMap.intersection.Approaches[temp_approach-1].Lanes[i].Connections[0].ConnectedLaneName.Approach;
						iOutLaneLane=iOutLaneApproach*10+ NewMap.intersection.Approaches[temp_approach-1].Lanes[i].Connections[0].ConnectedLaneName.Lane	;
					}
				}
				iOutLane=iOutLaneLane;
				if (iOutLane>1000) iOutLane=0;
				// ---- End of Obtaining inLane Outlane
				iETA=(int) temp_ETA;
				bIsCanceled=false;
				intID= tmp_intersection_id;
				// ---- End of seting the SRM elements ------//
				
				//------ ---------------------acknowledgment part -------------------------
				cout<<" vehIn.TemporaryID: "<<vehIn.TemporaryID<<endl;
				printf(" veh in speed: %f ",vehIn.motion.speed);
				lTempVehID=vehIn.TemporaryID;
				if (icodeUsage==2) // road version of PRG
					dTempSpeed=(vehIn.motion.speed)/3.6;  // convert from kmph to mps
				else // lab version of PRG , speed comes from VISSIM
					dTempSpeed=vehIn.motion.speed;
				
				if ( ((NumberOfReq>0) && (isThisVehIDinList(lTempVehID,acknReqList)<0))||(NumberOfReq<=0) )   // THIS VehID is not in the Request list(PSM). Therefore we should send Priority Request for THIS VehID.
				{ 
					sprintf(temp_log,"Vehicle ENTERED into the range, SENT SRM\n"); outputlog(temp_log); cout<<temp_log;
					if (isThisVehIDinList(lTempVehID,sentReqList)>=0)
					{
						sentReqList.Reset(isThisVehIDinList(lTempVehID,sentReqList));
						iMsgCnt=sentReqList.Data().iSendingMsgCnt+1;
						iMsgCnt=iMsgCnt%127;
						
					}else
						iMsgCnt=0;
					fillSRM(srmType, intID, iMsgCnt, bIsCanceled, iInLane, iOutLane, iETA, temp_dMinGrn, temp_vehState, iVehiclePriorityLevel, iRu_RlRange, cVehicleName, BSMblob);
					ec = der_encode_to_buffer(&asn_DEF_SRM, srmType,SRMbuf,SRM_MSG_SIZE);
					for (int i=0;i<SRM_MSG_SIZE;i++) // put buffer from type (char *) to srmbuf which is a (uint8_t)
						buf[i]=SRMbuf[i];
					
					if ((sendto(sockfd, buf,SRM_MSG_SIZE, 0,(struct sockaddr *)&recvaddr, addr_length)>0) )
					{	sprintf(temp_log,"..........REQUEST successfully sent to IntersectionID %d  at GPS time %.2lf.......... \n", tmp_intersection_id , dgpsTime); outputlog(temp_log); cout<< temp_log<< endl;}
					//------- Put the sent SRM data to sendReq data structure to be used in acknowledgment ----
					ReqEntry req_temp;
					req_temp.SendingReqEntry(lTempVehID, dgpsTime, dTempSpeed, iMsgCnt,iInLane,tmp_distance); 
					sentReqList.InsertRear(req_temp);
					sentReqList.Reset();
					//------- End of putting the sent SRM data to sendReqList   ----
					// ------- Updating the Active Map  txt file to show which map is active and being sent the SRM	
					fsActiveMAP.open(ActiveMAPFile, ios::out );
					fsActiveMAP<< ACTIVE <<" " << MAPsParsedList.Data().intersection.ID << " "<<iMsgCnt<<endl;
					fsActiveMAP.close();	

					MAPsParsedList.Data().intersection.iFlag=5;  
					
					//-----logging
					//~ if (icodeUsage==2)
					//~ {
						//~ sprintf(temp_log,"intersection ID %d ETA %lf Vehicle ID %llu ",MAPsParsedList.Data().intersection.ID,temp_ETA,srmType.temp_id); outputlog(temp_log); cout<<temp_log;
						//~ sprintf(temp_log," MsgCount %d MinGreenTime %8.2f InLane %d OutLane %d VehicalState %d \n",iMsgCnt,srmType.yawrate, iInLane,iOutLane,temp_vehState);  outputlog(temp_log); cout<<temp_log;
					//~ }
					//~ if (icodeUsage ==1)
					//~ {
						//~ sprintf(temp_log,"At Epoch time %ld for intersection ID: %d ETA: %lf Vehicle ID: %llu",time(NULL),MAPsParsedList.Data().intersection.ID,temp_ETA,srmType.temp_id ); outputlog(temp_log); cout<<temp_log; 
						//~ sprintf(temp_log," MsgCount %d MinGreenTime %8.2f InLane %d OutLane %d VehicalState %d \n",iMsgCnt,srmType.yawrate, iInLane,iOutLane,temp_vehState);  outputlog(temp_log); cout<<temp_log;
					//~ }
					//----end of logging
				}else if ((NumberOfReq>0) && (isThisVehIDinList(lTempVehID,acknReqList)>=0) && (isThisVehIDinList(lTempVehID,sentReqList)>=0) ) // This Request is already in the Request List (in the PSM), We will resend SRM when speed changes or when acknowledgment has not been received or when vehicle goes to lef turn bay!!
				{
					acknReqList.Reset(isThisVehIDinList(lTempVehID,acknReqList)); 
					sentReqList.Reset(isThisVehIDinList(lTempVehID,sentReqList));
					isentMsgDivisor= (int) sentReqList.Data().iSendingMsgCnt /10; 
					iacknMsgDivisor= (int) acknReqList.Data().iMsgCnt /10;
					if (icodeUsage==2) // road version of PRG
						dTempSpeed=(vehIn.motion.speed)/3.6;  // convert from kmph to mps
					else // lab version of PRG , speed comes from VISSIM
						dTempSpeed=vehIn.motion.speed;
					cout<<"Vehicle ID "<<lTempVehID<<endl;
					cout<<"Initial speed when entered the range "<< sentReqList.Data().dInitialSpeed<<endl;
					cout<<"Current speed  "<<dTempSpeed<<endl;
					cout<<"Last sent speed  "<<sentReqList.Data().dSpeed<<endl;
					cout<<"Last sent Msg Count  "<< sentReqList.Data().iSendingMsgCnt<<endl;
					cout<<"Acknow Msg Count " <<acknReqList.Data().iMsgCnt<<endl;
					cout<<"Sent iInLane "<<acknReqList.Data().iInLane<<endl;
					cout<<"Acknow iInLane " << sentReqList.Data().iInLane<<endl;
				
					if ((dTempSpeed>sentReqList.Data().dInitialSpeed/2) && ( dTempSpeed >=dSpeedChangeLmtForSRM) && (abs((sentReqList.Data().dSpeed-dTempSpeed)/sentReqList.Data().dSpeed) > iSpeedChngPrc)) // send a new SRM if speed changes happen when driver slows down upto 1/2 of inistialspeed(vehicle speed when vehicle entered the range!!)  --> decelating flag =2
					{
						sprintf(temp_log,"Vehicle Speed Changed, Vehicle SENT NEW SRM\n"); outputlog(temp_log); cout<<temp_log;
						if (isentMsgDivisor==iacknMsgDivisor) // The previous SRM already received acknowledgment
						{
							iMsgCnt=(isentMsgDivisor+1)*10;
							iMsgCnt=iMsgCnt%127;
							cout<<"iMsgCnt"<<iMsgCnt<<endl;
						}else
						{
							iMsgCnt=(sentReqList.Data().iSendingMsgCnt+1);
							iMsgCnt=iMsgCnt%127;
							cout<<"iMsgCnt1 "<<iMsgCnt<<endl;
						}
						//memset(&srmType, 0, sizeof(SRM_t));
						fillSRM(srmType,intID,iMsgCnt,bIsCanceled,iInLane,iOutLane,iETA,temp_dMinGrn,temp_vehState,iVehiclePriorityLevel,iRu_RlRange,cVehicleName, BSMblob);
						ec = der_encode_to_buffer(&asn_DEF_SRM, srmType,SRMbuf,SRM_MSG_SIZE);
						for (int i=0;i<SRM_MSG_SIZE;i++) // put buffer from type (char *) to srmbuf which is a (uint8_t)
							buf[i]=SRMbuf[i];
					
						if ((sendto(sockfd, buf,SRM_MSG_SIZE, 0,(struct sockaddr *)&recvaddr, addr_length)>0) )
						{
							sprintf(temp_log,"..........REQUEST successfully sent to IntersectionID %d  at GPS time %.2lf.......... \n", tmp_intersection_id , dgpsTime); outputlog(temp_log); cout<< temp_log<< endl;
						}
						
						//------- Find the request in the sentReqList and update it
						sentReqList.Data().iSendingTimeofReq=dgpsTime;
						sentReqList.Data().dSpeed=dTempSpeed;
						sentReqList.Data().iSendingMsgCnt=iMsgCnt;
						sentReqList.Data().iInLane=iInLane;
						sentReqList.Data().dDistanceToStpBar=tmp_distance;
				
						//------- End of finding the request in the sentReqList and updating it
						// ------- Updating the Active Map  txt file to show which map is active and being sent the SRM	
						fsActiveMAP.open(ActiveMAPFile, ios::out );
						fsActiveMAP<< ACTIVE <<" " << MAPsParsedList.Data().intersection.ID << " "<<iMsgCnt<<endl;
						fsActiveMAP.close();	

						MAPsParsedList.Data().intersection.iFlag=5;  
						//-----logging
						//~ if (icodeUsage==2)
						//~ {
							//~ sprintf(temp_log,"intersection ID %d ETA %lf Vehicle ID %llu,",MAPsParsedList.Data().intersection.ID,temp_ETA,srmType.temp_id); outputlog(temp_log); cout<<temp_log;
							//~ sprintf(temp_log,"MsgCount %d MinGreenTime %8.2f Inlane %d Outlane %d VehicleStatus %d \n",iMsgCnt,srmType.yawrate,srmType.in_lanenum,srmType.out_lanenum,srmType.vehicle_status);                                
							//~ outputlog(temp_log); cout<<temp_log;
						//~ }
						//~ if (icodeUsage ==1)
						//~ {
							//~ sprintf(temp_log,"At Epoch time %ld for intersection ID: %d ETA: %lf Vehicle ID: %llu",time(NULL),MAPsParsedList.Data().intersection.ID,temp_ETA,srmType.temp_id );
							//~ outputlog(temp_log); cout<<temp_log; 
								//~ sprintf(temp_log,"MsgCount %d MinGreenTime %8.2f Inlane %d Outlane %d VehicleStatus %d \n",iMsgCnt,srmType.yawrate,srmType.in_lanenum,srmType.out_lanenum,srmType.vehicle_status);                                
							//~ outputlog(temp_log); cout<<temp_log; 		
						//~ }
						//----end of logging
					}else if ( (dTempSpeed<dSpeedChangeLmtForSRM) && (sentReqList.Data().dSpeed>dSpeedChangeLmtForSRM) ) // send a new SRM when the vehicle join the queue 
					{
						sprintf(temp_log,"Vehicle Joined the QUEUE, vehicle SENT NEW SRM\n"); outputlog(temp_log); cout<<temp_log;
						if (isentMsgDivisor==iacknMsgDivisor) // The previous SRM already received acknowledgment
						{
							iMsgCnt=(isentMsgDivisor+1)*10;
							iMsgCnt=iMsgCnt%127;
							cout<<"iMsgCnt"<<iMsgCnt<<endl;
						}else
						{
							iMsgCnt=(sentReqList.Data().iSendingMsgCnt+1);
							iMsgCnt=iMsgCnt%127;
							cout<<"iMsgCnt1"<<iMsgCnt<<endl;
						}
						// Packing the  SRM 
						//memset(&srmType, 0, sizeof(SRM_t));
						fillSRM(srmType,intID,iMsgCnt,bIsCanceled,iInLane,iOutLane,iETA,temp_dMinGrn,temp_vehState,iVehiclePriorityLevel,iRu_RlRange,cVehicleName, BSMblob);
						ec = der_encode_to_buffer(&asn_DEF_SRM, srmType,SRMbuf,SRM_MSG_SIZE);
						for (int i=0;i<SRM_MSG_SIZE;i++) // put buffer from type (char *) to srmbuf which is a (uint8_t)
							buf[i]=SRMbuf[i];
						if ((sendto(sockfd, buf,SRM_MSG_SIZE, 0,(struct sockaddr *)&recvaddr, addr_length)>0) )
						{
							sprintf(temp_log,"..........REQUEST successfully sent to IntersectionID %d  at GPS time %.2lf.......... \n", tmp_intersection_id , dgpsTime); outputlog(temp_log); cout<< temp_log<< endl;
						}
						
						//------- Find the request in the sentReqList and update it
						sentReqList.Data().iSendingTimeofReq=dgpsTime;
						sentReqList.Data().dSpeed=dTempSpeed;
						sentReqList.Data().iSendingMsgCnt=iMsgCnt;
						sentReqList.Data().iInLane=iInLane;
						sentReqList.Data().dDistanceToStpBar=tmp_distance;
					
						//------- End of finding the request in the sentReqList and updating it
						// ------- Updating the Active Map  txt file to show which map is active and being sent the SRM	
						fsActiveMAP.open(ActiveMAPFile, ios::out );
						fsActiveMAP<< ACTIVE <<" " << MAPsParsedList.Data().intersection.ID << " "<<iMsgCnt<<endl;
						fsActiveMAP.close();	

						MAPsParsedList.Data().intersection.iFlag=5;  
						//~ //-----logging
						//~ if (icodeUsage==2)
						//~ {
							//~ sprintf(temp_log,"intersection ID %d ETA %lf Vehicle ID %llu,",MAPsParsedList.Data().intersection.ID,temp_ETA,srmType.temp_id); outputlog(temp_log); cout<<temp_log;
							//~ sprintf(temp_log,"MsgCount %d MinGreenTime %8.2f Inlane %d Outlane %d VehicleStatus %d \n",iMsgCnt,srmType.yawrate,srmType.in_lanenum,srmType.out_lanenum,srmType.vehicle_status);                                
							//~ outputlog(temp_log); cout<<temp_log;
						//~ }
						//~ if (icodeUsage ==1)
						//~ {
							//~ sprintf(temp_log,"At Epoch time %ld for intersection ID: %d ETA: %lf Vehicle ID: %llu",time(NULL),MAPsParsedList.Data().intersection.ID,temp_ETA,srmType.temp_id );
							//~ outputlog(temp_log); cout<<temp_log; 
								//~ sprintf(temp_log,"MsgCount %d MinGreenTime %8.2f Inlane %d Outlane %d VehicleStatus %d \n",iMsgCnt,srmType.yawrate,srmType.in_lanenum,srmType.out_lanenum,srmType.vehicle_status);                                
							//~ outputlog(temp_log); cout<<temp_log; 		
						//~ }
						//~ //----end of logging
					}
					else if ( ( (sentReqList.Data().dSpeed<dSpeedChangeLmtForSRM) && (sentReqList.Data().dDistanceToStpBar-tmp_distance > 10) )  || ( (sentReqList.Data().dSpeed<dSpeedChangeLmtForSRM) && ( dTempSpeed > 4.8) ) )  // send a new SRM when vehicle start to accelerate from being in queue  or when vehicle moves 10 meter while its speed is less than 5mph!
					{
						sprintf(temp_log,"Vehicle in the queue STARTS to move, vehcile SENT NEW SRM\n"); outputlog(temp_log); cout<<temp_log;
						if (isentMsgDivisor==iacknMsgDivisor) // The previous SRM already received acknowledgment
						{
							iMsgCnt=(isentMsgDivisor+1)*10;
							iMsgCnt=iMsgCnt%127;
							cout<<"iMsgCnt"<<iMsgCnt<<endl;
						}else
						{
							iMsgCnt=(sentReqList.Data().iSendingMsgCnt+1);
							iMsgCnt=iMsgCnt%127;
							cout<<"iMsgCnt1"<<iMsgCnt<<endl;
						}		
										
						//memset(&srmType, 0, sizeof(SRM_t));
						fillSRM(srmType,intID,iMsgCnt,bIsCanceled,iInLane,iOutLane,iETA,temp_dMinGrn,temp_vehState,iVehiclePriorityLevel,iRu_RlRange,cVehicleName, BSMblob);
						ec = der_encode_to_buffer(&asn_DEF_SRM, srmType,SRMbuf,SRM_MSG_SIZE);
						for (int i=0;i<SRM_MSG_SIZE;i++) // put buffer from type (char *) to srmbuf which is a (uint8_t)
							buf[i]=SRMbuf[i];
						if ((sendto(sockfd, buf,SRM_MSG_SIZE, 0,(struct sockaddr *)&recvaddr, addr_length)>0) )
						{
							sprintf(temp_log,"..........REQUEST successfully sent to IntersectionID %d  at GPS time %.2lf.......... \n", tmp_intersection_id , dgpsTime); outputlog(temp_log); cout<< temp_log<< endl;
						}
						//------- Find the request in the sentReqList and update it
						sentReqList.Data().iSendingTimeofReq=dgpsTime;
						sentReqList.Data().dSpeed=dTempSpeed;
						sentReqList.Data().iSendingMsgCnt=iMsgCnt;
						sentReqList.Data().iInLane=iInLane;
						sentReqList.Data().dDistanceToStpBar=tmp_distance;
						//------- End of finding the request in the sentReqList and updating it
						// ------- Updating the Active Map  txt file to show which map is active and being sent the SRM	
						fsActiveMAP.open(ActiveMAPFile, ios::out );
						fsActiveMAP<< ACTIVE <<" " << MAPsParsedList.Data().intersection.ID << " "<<iMsgCnt<<endl;
						fsActiveMAP.close();	

						MAPsParsedList.Data().intersection.iFlag=5;  
						//~ //-----logging
						//~ if (icodeUsage==2)
						//~ {
							//~ sprintf(temp_log,"intersection ID %d ETA %lf Vehicle ID %llu,",MAPsParsedList.Data().intersection.ID,temp_ETA,srmType.temp_id); outputlog(temp_log); cout<<temp_log;
							//~ sprintf(temp_log,"MsgCount %d MinGreenTime %8.2f Inlane %d Outlane %d VehicleStatus %d \n",iMsgCnt,srmType.yawrate,srmType.in_lanenum,srmType.out_lanenum,srmType.vehicle_status);                                
							//~ outputlog(temp_log); cout<<temp_log;
						//~ }
						//~ if (icodeUsage ==1)
						//~ {
							//~ sprintf(temp_log,"At Epoch time %ld for intersection ID: %d ETA: %lf Vehicle ID: %llu",time(NULL),MAPsParsedList.Data().intersection.ID,temp_ETA,srmType.temp_id );
							//~ outputlog(temp_log); cout<<temp_log; 
								//~ sprintf(temp_log,"MsgCount %d MinGreenTime %8.2f Inlane %d Outlane %d VehicleStatus %d \n",iMsgCnt,srmType.yawrate,srmType.in_lanenum,srmType.out_lanenum,srmType.vehicle_status);                                
							//~ outputlog(temp_log); cout<<temp_log; 		
						//~ }
						//~ //----end of logging
					}else if (sentReqList.Data().iInLane!=acknReqList.Data().iInLane) // vehicle changed ita lane, so it may enter to left turn bay, send new SRM
					{
						sprintf(temp_log,"Vehicle Changed LANE, vehcile SENT NEW SRM\n"); outputlog(temp_log); cout<<temp_log;
						if (isentMsgDivisor==iacknMsgDivisor) // The previous SRM already received acknowledgment
						{
							iMsgCnt=(isentMsgDivisor+1)*10;
							iMsgCnt=iMsgCnt%127;
							cout<<"iMsgCnt"<<iMsgCnt<<endl;
						}else
						{
							iMsgCnt=(sentReqList.Data().iSendingMsgCnt+1);
							iMsgCnt=iMsgCnt%127;
							cout<<"iMsgCnt1"<<iMsgCnt<<endl;
						}						
						//memset(&srmType, 0, sizeof(SRM_t));
						fillSRM(srmType,intID,iMsgCnt,bIsCanceled,iInLane,iOutLane,iETA,temp_dMinGrn,temp_vehState,iVehiclePriorityLevel,iRu_RlRange,cVehicleName, BSMblob);
						ec = der_encode_to_buffer(&asn_DEF_SRM, srmType,SRMbuf,SRM_MSG_SIZE);
						for (int i=0;i<SRM_MSG_SIZE;i++) // put buffer from type (char *) to srmbuf which is a (uint8_t)
							buf[i]=SRMbuf[i];
						if ((sendto(sockfd, buf,SRM_MSG_SIZE, 0,(struct sockaddr *)&recvaddr, addr_length)>0) )
						{
							sprintf(temp_log,"..........REQUEST successfully sent to IntersectionID %d  at GPS time %.2lf.......... \n", tmp_intersection_id , dgpsTime); outputlog(temp_log); cout<< temp_log<< endl;
						}
						//------- Find the request in the sentReqList and update it
						sentReqList.Data().iSendingTimeofReq=dgpsTime;
						sentReqList.Data().dSpeed=dTempSpeed;
						sentReqList.Data().iSendingMsgCnt=iMsgCnt;
						sentReqList.Data().iInLane=iInLane;
						sentReqList.Data().dDistanceToStpBar=tmp_distance;
						//------- End of finding the request in the sentReqList and updating it
						// ------- Updating the Active Map  txt file to show which map is active and being sent the SRM	
						fsActiveMAP.open(ActiveMAPFile, ios::out );
						fsActiveMAP<< ACTIVE <<" " << MAPsParsedList.Data().intersection.ID << " "<<iMsgCnt<<endl;
						fsActiveMAP.close();	

						MAPsParsedList.Data().intersection.iFlag=5;  
						//~ //-----logging
						//~ if (icodeUsage==2)
						//~ {
							//~ sprintf(temp_log,"intersection ID %d ETA %lf Vehicle ID %llu,",MAPsParsedList.Data().intersection.ID,temp_ETA,srmType.temp_id); outputlog(temp_log); cout<<temp_log;
							//~ sprintf(temp_log,"MsgCount %d MinGreenTime %8.2f Inlane %d Outlane %d VehicleStatus %d \n",iMsgCnt,srmType.yawrate,srmType.in_lanenum,srmType.out_lanenum,srmType.vehicle_status);                                
							//~ outputlog(temp_log); cout<<temp_log;
						//~ }
						//~ if (icodeUsage ==1)
						//~ {
							//~ sprintf(temp_log,"At Epoch time %ld for intersection ID: %d ETA: %lf Vehicle ID: %llu",time(NULL),MAPsParsedList.Data().intersection.ID,temp_ETA,srmType.temp_id );
							//~ outputlog(temp_log); cout<<temp_log; 
								//~ sprintf(temp_log,"MsgCount %d MinGreenTime %8.2f Inlane %d Outlane %d VehicleStatus %d \n",iMsgCnt,srmType.yawrate,srmType.in_lanenum,srmType.out_lanenum,srmType.vehicle_status);                                
							//~ outputlog(temp_log); cout<<temp_log; 		
						//~ }
						
					}
					else if  (isentMsgDivisor!=iacknMsgDivisor )      // we have not receivced back any acknowledgment 
					{
						iMsgCnt=sentReqList.Data().iSendingMsgCnt+1;  
						iMsgCnt=iMsgCnt%127;
						cout<<"iMsgCnt"<<iMsgCnt<<endl;
						sprintf(temp_log,"acknowledgment is not received, RESEND the SRM with updated message count %d\n",iMsgCnt); outputlog(temp_log); cout<<temp_log;
						// Packing the  SRM 
						//memset(&srmType, 0, sizeof(SRM_t));
						fillSRM(srmType,intID,iMsgCnt,bIsCanceled,iInLane,iOutLane,iETA,temp_dMinGrn,temp_vehState,iVehiclePriorityLevel,iRu_RlRange,cVehicleName, BSMblob);
						ec = der_encode_to_buffer(&asn_DEF_SRM, srmType,SRMbuf,SRM_MSG_SIZE);
						for (int i=0;i<SRM_MSG_SIZE;i++) // put buffer from type (char *) to srmbuf which is a (uint8_t)
							buf[i]=SRMbuf[i];
						if ((sendto(sockfd, buf,SRM_MSG_SIZE, 0,(struct sockaddr *)&recvaddr, addr_length)>0) )
						{
							sprintf(temp_log,"..........REQUEST successfully sent to IntersectionID %d  at GPS time %.2lf.......... \n", tmp_intersection_id , dgpsTime); outputlog(temp_log); cout<< temp_log<< endl;
						}
						//------- Find the request in the sentReqList and update it
						sentReqList.Data().iSendingTimeofReq=dgpsTime;
						sentReqList.Data().dSpeed=dTempSpeed;
						sentReqList.Data().iSendingMsgCnt=iMsgCnt;
						sentReqList.Data().iInLane=iInLane;
						sentReqList.Data().dDistanceToStpBar=tmp_distance;
						//------- End of finding the request in the sentReqList and updating it
						// ------- Updating the Active Map  txt file to show which map is active and being sent the SRM	
						fsActiveMAP.open(ActiveMAPFile, ios::out );
						fsActiveMAP<< ACTIVE <<" " << MAPsParsedList.Data().intersection.ID << " "<<iMsgCnt<<endl;
						fsActiveMAP.close();	
						MAPsParsedList.Data().intersection.iFlag=5;  
						//~ //-----logging
						//~ if (icodeUsage==2)
						//~ {
							//~ sprintf(temp_log,"intersection ID %d ETA %lf Vehicle ID %llu,",MAPsParsedList.Data().intersection.ID,temp_ETA,srmType.temp_id); outputlog(temp_log); cout<<temp_log;
							//~ sprintf(temp_log,"MsgCount %d MinGreenTime %8.2f Inlane %d Outlane %d VehicleStatus %d \n",iMsgCnt,srmType.yawrate,srmType.in_lanenum,srmType.out_lanenum,srmType.vehicle_status);                                
							//~ outputlog(temp_log); cout<<temp_log;
						//~ }
						//~ if (icodeUsage ==1)
						//~ {
							//~ sprintf(temp_log,"At Epoch time %ld for intersection ID: %d ETA: %lf Vehicle ID: %llu",time(NULL),MAPsParsedList.Data().intersection.ID,temp_ETA,srmType.temp_id );
							//~ outputlog(temp_log); cout<<temp_log; 
								//~ sprintf(temp_log,"MsgCount %d MinGreenTime %8.2f Inlane %d Outlane %d VehicleStatus %d \n",iMsgCnt,srmType.yawrate,srmType.in_lanenum,srmType.out_lanenum,srmType.vehicle_status);                                
							//~ outputlog(temp_log); cout<<temp_log; 		
						//~ }
						//~ //----end of logging
					}
				} // ------- end of acknowledgment logic -------
				
			}
			if (( flag_leaving_map==1 ) && (MAPList.Data().ID==tmp_leaving_intersection_id)  && ( MAPsParsedList.Data().intersection.iFlag>0 ) )  // vehicle is leaving the intersection
			{
				iMAPPositionInList=findMAPPositionInList(MAPsParsedList,tmp_leaving_intersection_id);
				if(iMAPPositionInList>=0)
				{
					MAPsParsedList.Reset(iMAPPositionInList);
					NewMap=MAPsParsedList.Data();
				}

				// ---- seting the SRM elements ------//
				trackedveh.Data().ETA=ETA; 
				iInLane=approach*10+lane;
				char tmp1[8];
				sprintf(tmp1,"%d.%d",approach,lane);
				string tmp_string(tmp1);
				for( int i=0; i<NewMap.intersection.Approaches[approach-1].Lane_No;i++)
				{
					if (tmp_string.compare(NewMap.intersection.Approaches[approach-1].Lanes[i].Lane_Name)==0)
					{
						iOutLaneApproach=NewMap.intersection.Approaches[approach-1].Lanes[i].Connections[0].ConnectedLaneName.Approach;
						iOutLaneLane=iOutLaneApproach*10+ NewMap.intersection.Approaches[approach-1].Lanes[i].Connections[0].ConnectedLaneName.Lane	;
					}
				}
				iOutLane=0;
				iETA=(int) ETA;
				bIsCanceled=true;
				intID= tmp_leaving_intersection_id;
				// ---- End of seting the SRM elements ------//
				lTempVehID=vehIn.TemporaryID;
				//~ //-------- acknowledgment part start here--------
				//~ if (icodeUsage==2) // road version of PRG
				//~ {
					//~ lTempVehID=vehIn.TemporaryID;
				//~ }else // lab version of PRG , veh ID and speed come from VISSIM
				//~ {
					//~ lTempVehID=srmType.temp_id;
				//~ }		
				
				iMsgCnt=temp_msgCnt;
				iMsgCnt=iMsgCnt%127;
				sprintf(temp_log,"Vehicle LEAVING the intersection,  SEND SRM to Cancel priority reqest, message count is %d \n", iMsgCnt ); outputlog(temp_log); cout<<temp_log<<endl;
				//memset(&srmType, 0, sizeof(SRM_t));
				fillSRM(srmType,intID,iMsgCnt,bIsCanceled,iInLane,iOutLane,iETA,temp_dMinGrn,temp_vehState,iVehiclePriorityLevel,iRu_RlRange,cVehicleName, BSMblob);
				ec = der_encode_to_buffer(&asn_DEF_SRM, srmType,SRMbuf,SRM_MSG_SIZE);
				for (int i=0;i<SRM_MSG_SIZE;i++) // put buffer from type (char *) to srmbuf which is a (uint8_t)
					buf[i]=SRMbuf[i];
				if ((sendto(sockfd, buf,SRM_MSG_SIZE, 0,(struct sockaddr *)&recvaddr, addr_length)>0) )
				{
					sprintf(temp_log,"..........REQUEST successfully sent to IntersectionID %d  at GPS time %.2lf.......... \n", tmp_intersection_id , dgpsTime); outputlog(temp_log); cout<< temp_log<< endl;
				}
						
				// ------- Updating the Active Map  txt file to show which map is not active anymore 
				fsActiveMAP.open(ActiveMAPFile, ios::out);
				fsActiveMAP<< LEAVE <<" " << MAPsParsedList.Data().intersection.ID <<" " <<iMsgCnt<< endl;
				fsActiveMAP.close();
				
				MAPsParsedList.Data().intersection.iFlag--;
				//~ //-----logging
				//~ if (icodeUsage ==1)
				//~ {
					//~ sprintf(temp_log,"At Epoch time %ld leaving intersection ID: %d, MsgCount %d, Vehicle ID: %llu \n",time(NULL),MAPsParsedList.Data().intersection.ID,iMsgCnt,srm.temp_id );
					//~ outputlog(temp_log); cout<<temp_log; 
				//~ }
				//~ //----end of logging
				//~ 
				if( isThisVehIDinList(lTempVehID,acknReqList)>=0 && acknReqList.Data().iVehState==2)
				{
					sprintf(temp_log,"Vehicle LEFT the intersection, acknowledgment to cancel the request RECEIEVED. Vehicle deleted from sent list of intersection ID %d \n", MAPsParsedList.Data().intersection.ID ); outputlog(temp_log); cout<<temp_log<<endl;
					MAPsParsedList.Data().intersection.iFlag=-1;	
					sentReqList.Reset(isThisVehIDinList(lTempVehID,sentReqList));
					sentReqList.DeleteAt();
				}else if (isThisVehIDinList(lTempVehID,acknReqList)>=0 && acknReqList.Data().iVehState!=2)
				{
					sprintf(temp_log,"acknowledgment is not received, RESEND the SRM \n"); outputlog(temp_log); cout<<temp_log<<endl;
				}else if (isThisVehIDinList(lTempVehID,acknReqList)<0 && MAPsParsedList.Data().intersection.iFlag <4)
				{
					sprintf(temp_log,"acknowledgment is not received, Stop Sending SRM after 4 times sending cancel request \n"); outputlog(temp_log); cout<<temp_log<<endl;
					MAPsParsedList.Data().intersection.iFlag=-1;	
					sentReqList.Reset(isThisVehIDinList(lTempVehID,sentReqList));
					sentReqList.DeleteAt();
				}
			}
			
			MAPList.Next();  
			
		} //  --- End of Sending the req	
	
		msleep(100);
	}
	
	
	
	//~ srmType = (SRM_t *) calloc(1, sizeof * srmType);
	//~ srmType->timeOfService=(struct DTime*)calloc(1,sizeof( DTime_t));
	//~ srmType->endOfService=(struct DTime*)calloc(1,sizeof(DTime_t));
	//~ srmType->transitStatus= (BIT_STRING_t*)calloc(1, sizeof(BIT_STRING_t));
	//~ srmType->vehicleVIN=( VehicleIdent_t*)calloc(1, sizeof(VehicleIdent_t));
//~ 
	//~ gps_init() ;
	//~ read_gps();
	//~ populate_gps_values();
	//~ 
	//~ // ----- Insert the GPS information into the BasicVehicle structure -----
	//~ formBSMOfSRM(&vehIn,iVehicleId, iMsgCnt,  m_vehlat, m_vehlong, dHeading, m_vehspeed, dElevation);
	//~ vehIn.VehicleToBSM(BSMblob);
	//~ ret = OCTET_STRING_fromBuf(&srmType->vehicleData,BSMblob,38);
	//~ 
	//~ fillSRM(srmType,301,10,1,72,42,35.6,0.0,4, 6,3,cVehicleName, BSMblob);  
	//~ 
	//~ 
	//~ 
	//~ ec = der_encode_to_buffer(&asn_DEF_SRM, srmType,SRMbuf,250);
	//~ for( int ii=0;ii<193;ii++)
	//~ {
		//~ printf("%x\t",(uint8_t) SRMbuf[ii]);
	//~ }
	//~ rval = ber_decode(0, &asn_DEF_SRM,(void **)&srmType, SRMbuf, 250);
	//~ xer_fprint(stdout, &asn_DEF_SRM, srmType);
	//~ for (int i=0;i<38;i++) 
		//~ BSMblob[i]=srmType->vehicleData.buf[i];
	//~ vehIn.BSMToVehicle(BSMblob);
		//~ 
	//~ int lintersectionID=(srmType->request.id.buf[1]<<8)+srmType->request.id.buf[0];
	//~ printf ("lintersectionID %d", lintersectionID);
//~ 
	//~ 
	//~ 
	//~ 
	//~ if (srmType->request.isCancel->buf[0]==0)
				//~ cout<< "request"<<endl;
	//~ else
		//~ cout<< "request clear"<<endl;
	//~ int iInApproach, ioutApproach, iInLane, iOutlane;
	//~ obtainInLaneOutLane( srmType->request.inLane->buf[0] , srmType->request.outLane->buf[0] , iInApproach, ioutApproach, iInLane, iOutlane); 
	//~ int iRequestedPhase=11;
	//~ int iPriorityLevel=srmType->request.type.buf[0] ; // is this the currect element of srmType to populate with vehicle  type?!
	//~ double dMinGrn=((srmType->vehicleVIN->id->buf[1]<<8)+srmType->vehicleVIN->id->buf[0])/10; 			 // there was no place in SMR to store MinGrn !!!!!
	//~ int iStartMinute=srmType->timeOfService->minute;
	//~ int iStartSecond=srmType->timeOfService->second;
	//~ int iEndMinute=srmType->endOfService->minute;
	//~ int iEndSecond=srmType->endOfService->second;
	//~ int iStartHour=srmType->timeOfService->hour;
	//~ int iEndHour=srmType->endOfService->hour;
	//~ 
	//~ calculateETA(iStartMinute,iStartSecond,iEndMinute,iEndSecond,iETA );
	//~ float fETA=(float) iETA;
	//~ int lvehicleID=vehIn.TemporaryID;
	//~ int iVehicleState=srmType->status->buf[0];
	//~ int iMsgCnt=srmType->msgCnt;
	//~ double dSpeed=vehIn.motion.speed;
	//~ //printsrmTypecsv (&srmType);
	//~ char RcvMsg[150];
	//~ sprintf(RcvMsg," %ld %d %.2f %d %.2f %ld %d %d %d %d %d %d %d %d %d %d",  lvehicleID, iPriorityLevel,fETA,iRequestedPhase,dMinGrn, time(NULL), 
	//~ srmType->request.inLane->buf[0] , srmType->request.outLane->buf[0] , iStartHour, iStartMinute, iStartSecond, iEndHour,iEndMinute, iEndSecond, iVehicleState, iMsgCnt);
	//~ sprintf(temp_log," ******** The Received SRM  match the Intersection ID  ,  at time %ld. \n",time(NULL)); 
	//~ outputlog(temp_log);
	//~ cout<< temp_log<< endl;
	//~ sprintf(temp_log,"%s\t \n",RcvMsg);
	//~ outputlog(temp_log);
	//~ cout<< temp_log<< endl;			
	//~ 
	
	
	
	
	return 0;

} // ---End of main()

int whatIsVISSIMType(int type)
{
	if (type==4)// which the category of Tram in VISSIM     
		iVehiclePriorityLevel=1;   // in priority applications, priority level of EV is 1
	else if (type==3)  // which the category of bus in VISSIM
		iVehiclePriorityLevel=2;   // in priority applications, priority level of bus is 2
	else if (type==2)  // which the category of truck in VISSIM
		iVehiclePriorityLevel=3;    // in priority applications, priority level of truck is 3
	else if (type==1)  // which the category of car in VISSIM
		iVehiclePriorityLevel=4;    // in priority applications, priority level of car is 4
	else
		iVehiclePriorityLevel=0;
	int temp=0;
	if (iVehiclePriorityLevel==2) 
		temp=1;  
	return temp;
}
double  geoFencing( MAP NewMap)
{
	//Find the Nearest Lane info from the current vehicle location
	int N_App,N_Lane,N_Node; //Nearest Approach, Lane, and Node
	int N_pos; //node position in the vector
	double Tempdis = 10000000.0;
	//The output of DistanceFromLine Function:
	double distanceSegment, distanceLine;
		//find the nearest lane node
	for(unsigned int s = 0; s < NewMap.MAP_Nodes.size(); s++)
	{
		double Dis = sqrt(pow((local_x - NewMap.MAP_Nodes[s].N_Offset),2) + pow((local_y - NewMap.MAP_Nodes[s].E_Offset),2));
		if (Dis<Tempdis)
		{
			Tempdis = Dis;
			N_App = NewMap.MAP_Nodes[s].index.Approach;
			N_Lane = NewMap.MAP_Nodes[s].index.Lane;
			N_Node = NewMap.MAP_Nodes[s].index.Node;
			N_pos = s;
		}
	}
	
	if(Tempdis<1000000.0)  // Find a valid node
	{
		if(N_Node == 1) //Case when the nearest node is the first node of the lane: The line bw MAP_Nodes[s] and MAP_Nodes[s+1]
		{
			DistanceFromLine(local_x, local_y, NewMap.MAP_Nodes[N_pos].N_Offset, NewMap.MAP_Nodes[N_pos].E_Offset , NewMap.MAP_Nodes[N_pos+1].N_Offset, NewMap.MAP_Nodes[N_pos+1].E_Offset, distanceSegment, distanceLine);
			//cout<<"Two points are: "<<MAP_Nodes[N_pos].index.Approach<<"."<<MAP_Nodes[N_pos].index.Lane<<"."<<MAP_Nodes[N_pos].index.Node<<" and "<<MAP_Nodes[N_pos+1].index.Approach<<"."<<MAP_Nodes[N_pos+1].index.Lane<<"."<<MAP_Nodes[N_pos+1].index.Node<<endl; 
		}
		else
		{
			//line bw MAP_Nodes[s] and MAP_Nodes[s-1]
			DistanceFromLine(local_x, local_y, NewMap.MAP_Nodes[N_pos].N_Offset, NewMap.MAP_Nodes[N_pos].E_Offset , NewMap.MAP_Nodes[N_pos-1].N_Offset, NewMap.MAP_Nodes[N_pos-1].E_Offset, distanceSegment, distanceLine);
			//cout<<"Two points are: "<<MAP_Nodes[N_pos].index.Approach<<"."<<MAP_Nodes[N_pos].index.Lane<<"."<<MAP_Nodes[N_pos].index.Node<<" and "<<MAP_Nodes[N_pos-1].index.Approach<<"."<<MAP_Nodes[N_pos-1].index.Lane<<"."<<MAP_Nodes[N_pos-1].index.Node<<endl; 
		}
	}
	return distanceLine;
}


//To find the Shortest Distance between the vehicle location and two nearest lane nodes ----> GeoFencing
//Shayan Added 10.3.14  
void DistanceFromLine(double cx, double cy, double ax, double ay ,double bx, double by, double &distanceSegment, double &distanceLine)
{
	// find the distance from the point (cx,cy) to the line
	// determined by the points (ax,ay) and (bx,by)
	//
	// distanceSegment = distance from the point to the line segment
	// distanceLine = distance from the point to the line (assuming
	//					infinite extent in both directions

	double r_numerator = (cx-ax)*(bx-ax) + (cy-ay)*(by-ay);
	double r_denomenator = (bx-ax)*(bx-ax) + (by-ay)*(by-ay);
	double r = r_numerator / r_denomenator;

    double px = ax + r*(bx-ax);
    double py = ay + r*(by-ay);
   
    double s =  ((ay-cy)*(bx-ax)-(ax-cx)*(by-ay) ) / r_denomenator;

	distanceLine = fabs(s)*sqrt(r_denomenator);
	double xx = px;
	double yy = py;

	if ( (r >= 0) && (r <= 1) )
	{
		distanceSegment = distanceLine;
	}
	else
	{

		double dist1 = (cx-ax)*(cx-ax) + (cy-ay)*(cy-ay);
		double dist2 = (cx-bx)*(cx-bx) + (cy-by)*(cy-by);
		if (dist1 < dist2)
		{
			xx = ax;
			yy = ay;
			distanceSegment = sqrt(dist1);
		}
		else
		{
			xx = bx;
			yy = by;
			distanceSegment = sqrt(dist2);
		}


	}

	return;
}
//~ 
//~ void putPrioVehInPrioVehList(LinkedList<PriorityVehicle> & pVehList, int vehId,char * bsmblob, SRM_t *srm)
//~ {
	//~ PriorityVehicle tempPV;
	//~ pVehList.Reset();
	//~ int isVehInList=0;
	//~ if (pVehList.ListEmpty()!=1)
	//~ {
		//~ while (!pVehList.EndOfList())
		//~ {
			//~ if (pVehList.Data().iVehID==vehId)
			//~ {
				//~ isVehInList=1;
				//~ pVehList.Data().iTimerToDel=10;
			//~ }
			//~ pVehList.Next();
		//~ }
	//~ }
	//~ if (isVehInList==0)
	//~ {
		//~ tempPV.iVehID=vehId;
		//~ tempPV.iTimerToDel=10;
		//~ for (int i=0;i<BSM_BlOB_SIZE;i++)
			//~ tempPV.bsmBlob[i]=bsmblob[i];
		//~ char Byte=srm->request.type.buf[0];
		//~ unsigned short uiTemp=Byte;
		//~ if (uiTemp==4)// which the category of Tram in VISSIM     
			//~ tempPV.iVehType=1;   // in priority applications, priority level of EV is 1
		//~ if (uiTemp==3)  // which the category of bus in VISSIM
			//~ tempPV.iVehType=2;   // in priority applications, priority level of bus is 2
		//~ if (uiTemp==2)  // which the category of truck in VISSIM
			//~ tempPV.iVehType=3;    // in priority applications, priority level of truck is 3
		//~ if (uiTemp==1)  // which the category of car in VISSIM
			//~ tempPV.iVehType=4;    // in priority applications, priority level of car is 4
		//~ pVehList.InsertRear(tempPV);	
	//~ }
//~ }
//~ 

void populateMAPNodes(MAP NewMap)
{
	NewMap.MAP_Nodes.clear();
	for (unsigned int i=0;i<NewMap.intersection.Approaches.size();i++)
		for(unsigned int j=0;j<NewMap.intersection.Approaches[i].Lanes.size();j++)
			for(unsigned k=0;k<NewMap.intersection.Approaches[i].Lanes[j].Nodes.size();k++)
			{	
				LaneNodes temp_node;
				temp_node.index.Approach=NewMap.intersection.Approaches[i].Lanes[j].Nodes[k].index.Approach;
				temp_node.index.Lane=NewMap.intersection.Approaches[i].Lanes[j].Nodes[k].index.Lane;
				temp_node.index.Node=NewMap.intersection.Approaches[i].Lanes[j].Nodes[k].index.Node;
				temp_node.Latitude=NewMap.intersection.Approaches[i].Lanes[j].Nodes[k].Latitude;
				temp_node.Longitude=NewMap.intersection.Approaches[i].Lanes[j].Nodes[k].Longitude;
				geoPoint.lla2ecef(temp_node.Longitude,temp_node.Latitude,0.0 ,&ecef_x,&ecef_y,&ecef_z);
				geoPoint.ecef2local(ecef_x,ecef_y,ecef_z,&local_x,&local_y,&local_z);
				temp_node.N_Offset=local_x;
				temp_node.E_Offset=local_y;						
				NewMap.MAP_Nodes.push_back(temp_node);
			}

	int tmp_pos=0;
	for (unsigned int i=0;i<NewMap.intersection.Approaches.size();i++)
		for(unsigned int j=0;j<NewMap.intersection.Approaches[i].Lanes.size();j++)
			for(unsigned k=0;k<NewMap.intersection.Approaches[i].Lanes[j].Nodes.size();k++)
			{
				
				if(NewMap.MAP_Nodes[tmp_pos].index.Approach%2==1)  //odd approaches, approching lanes
				{
					if(k<NewMap.intersection.Approaches[i].Lanes[j].Nodes.size()-1)
					{
						NewMap.MAP_Nodes[tmp_pos].Heading=atan2(NewMap.MAP_Nodes[tmp_pos].N_Offset-NewMap.MAP_Nodes[tmp_pos+1].N_Offset,NewMap.MAP_Nodes[tmp_pos].E_Offset-NewMap.MAP_Nodes[tmp_pos+1].E_Offset)*180.0/PI;
						NewMap.MAP_Nodes[tmp_pos].Heading=90.0-NewMap.MAP_Nodes[tmp_pos].Heading;
					}
					else
					{
						NewMap.MAP_Nodes[tmp_pos].Heading=NewMap.MAP_Nodes[tmp_pos-1].Heading;	
						//cout<<NewMap.MAP_Nodes[tmp_pos].Heading<<" "<<NewMap.MAP_Nodes[tmp_pos-1].Heading<<endl;
					}

					//	sprintf(temp_log,"Appraoching heading %d is %f\n",tmp_pos,NewMap.MAP_Nodes[tmp_pos].Heading);
					//	cout<<temp_log;
				}
				if(NewMap.MAP_Nodes[tmp_pos].index.Approach%2==0)  //even approaches, leaving lanes
				{
					if(k<NewMap.intersection.Approaches[i].Lanes[j].Nodes.size()-1)
					{
						NewMap.MAP_Nodes[tmp_pos].Heading=atan2(NewMap.MAP_Nodes[tmp_pos+1].N_Offset-NewMap.MAP_Nodes[tmp_pos].N_Offset,NewMap.MAP_Nodes[tmp_pos+1].E_Offset-NewMap.MAP_Nodes[tmp_pos].E_Offset)*180.0/PI;
						NewMap.MAP_Nodes[tmp_pos].Heading=90.0-NewMap.MAP_Nodes[tmp_pos].Heading;
					}
					else
					{
						NewMap.MAP_Nodes[tmp_pos].Heading=NewMap.MAP_Nodes[tmp_pos-1].Heading;	
					}
					
					//	sprintf(temp_log,"Leaving heading %d is %f\n",tmp_pos,NewMap.MAP_Nodes[tmp_pos].Heading);
					//	cout<<temp_log;
				}
												
				if(NewMap.MAP_Nodes[tmp_pos].Heading<0)
				 NewMap.MAP_Nodes[tmp_pos].Heading+=360;
				
				//sprintf(temp_log,"%d %d %d %lf %lf %lf %lf %f\n",NewMap.MAP_Nodes[tmp_pos].index.Approach,NewMap.MAP_Nodes[tmp_pos].index.Lane,NewMap.MAP_Nodes[tmp_pos].index.Node,NewMap.MAP_Nodes[tmp_pos].N_Offset,NewMap.MAP_Nodes[tmp_pos].E_Offset,NewMap.MAP_Nodes[tmp_pos].Latitude,NewMap.MAP_Nodes[tmp_pos].Longitude,NewMap.MAP_Nodes[tmp_pos].Heading);
				//outputlog(temp_log); cout<<temp_log;										
				tmp_pos++;								
			}

}

//**************************************FUNCTIONSSS  ********************************************//

// This function is used when lab version of the code is applied
void fillSRM(SRM_t *srm,int intID,int iMsgCnt,bool bIsCanceled,int iInLane,int iOutLane,int iETA, double dMinGr, int vehState,int Ru_Rl,int vehtype, char *cVehicleName, char * bsmBlob)
{

	time_t theTime = time(NULL);
	struct tm *aTime = localtime(&theTime);
	srm->msgID=14;
	srm->msgCnt=iMsgCnt;
	srm->request.inLane=OCTET_STRING_new_fromBuf(&asn_DEF_OCTET_STRING,(char *)(&iInLane),1);
	srm->request.outLane=OCTET_STRING_new_fromBuf(&asn_DEF_OCTET_STRING,(char *)(&iOutLane),1);
	srm->request.isCancel=OCTET_STRING_new_fromBuf(&asn_DEF_OCTET_STRING,(char *)(&bIsCanceled),1);
	int ret=0;
	ret = OCTET_STRING_fromBuf(&srm->vehicleData,bsmBlob,38);
	ret=OCTET_STRING_fromBuf(&srm->request.id,(char *)(&intID),-1);
	int reAction=209;
	srm->request.requestedAction =OCTET_STRING_new_fromBuf(&asn_DEF_OCTET_STRING,(char*)(&reAction ),-1);
	// In VISSIM different class!! of vehicles are diffrentiated by their category!. So in DrivelModel.dll , DRIVER_DATA_VEH_CATEGORY gets different values for different class of vehicles!!  car = 1, truck = 2, bus = 3, tram = 4, pedestrian = 5, bike = 6 .
	// When we pack the SRM in dll, we put these numbers in  vehicleclass_level  field of SRM. 
	// Here in PRG, we put the srm->vehicleclass_level into srm->vehicle_type
	// Since we do not have any EV in dll, we consider trams as EV!! 
	ret=OCTET_STRING_fromBuf(&srm->request.type,(char *)(&vehtype),1);
	
		
	char* icodeword="MMITSS password";
	srm->request.codeWord=OCTET_STRING_new_fromBuf(&asn_DEF_OCTET_STRING,icodeword,-1);
	
	// timeOfService
	srm->timeOfService->hour=aTime->tm_hour;  //
	srm->timeOfService->minute=aTime->tm_min;
	srm->timeOfService->second=aTime->tm_sec;
	int iHour=0;
	int iMin=0;
	int iSec=0;
	int itempMinGrn=(int) dMinGr;
	obtainEndofService( aTime->tm_hour,aTime->tm_min,aTime->tm_sec,iETA,iHour, iMin,iSec,itempMinGrn,Ru_Rl);
	if (bIsCanceled==0)
	{
		srm->endOfService->hour=iHour;
		srm->endOfService->minute=iMin;
		srm->endOfService->second=iSec;
	}else if (bIsCanceled==1)
	{ 	
		srm->timeOfService->hour=0;  //
		srm->timeOfService->minute=0;
		srm->timeOfService->second=0;
		srm->endOfService->hour=0;
		srm->endOfService->minute=0;
		srm->endOfService->second=0;
	}
	
	srm->status= OCTET_STRING_new_fromBuf(&asn_DEF_OCTET_STRING,(char*) (&vehState),-1); 
	
	long lTemp;
	lTemp = (long)(10*dMinGr);
	srm->vehicleVIN->id= OCTET_STRING_new_fromBuf(&asn_DEF_OCTET_STRING,(char*)&lTemp,3); // we have to insert min green time in the srm somehow!! I choose id to populate this value!!!
	// transit Status
	srm->transitStatus->buf=(uint8_t *)calloc(1,1);  
	assert(srm->transitStatus->buf);
	srm->transitStatus->size = 1;
	srm->transitStatus->buf[0] |=1<<0;//<<  (7 - TransitStatus_anADAuse);  just filled it up !
	srm->transitStatus->bits_unused = 2;	
	
	// Vehicle Ident
	char* ivin="91769fjut";
 	srm->vehicleVIN->name= OCTET_STRING_new_fromBuf(&asn_DEF_OCTET_STRING,cVehicleName,-1);
	srm->vehicleVIN->vin= OCTET_STRING_new_fromBuf(&asn_DEF_OCTET_STRING,ivin,-1);

}

void populate_gps_values()
{
//get gps time
	if (isnan(gps.time)==0) 
		dgpsTime=gps.time;
	else
		dgpsTime=0.0;
		
	if (isnan(gps.latitude)==0 && isnan(gps.longitude)==0 && isnan(gps.altitude)==0  )  
	{	
		//m_vehspeed   =  gpsdata->fix.speed;  //------ gps_fix_t: m/s
		m_vehlat_pre=m_vehlat;
		m_vehlong_pre=m_vehlong;
		dElevation_pre=dElevation;
		if( isnan(gps.speed)!=0)
		{
			sprintf(temp_log,"--------GPS Speed Error.--------------At(%.2lf)\n",dgpsTime);
			outputlog(temp_log);
			m_vehspeed=pre_speed;
		}
		else
		{
			//if(isnan(gpsdata->fix.speed)!=0)
			m_vehspeed    =  gps.speed;
			pre_speed     =  m_vehspeed;
		}
		if( isnan(gps.heading)!=0)
		{
			sprintf(temp_log,"--------GPS Fix Error.--------------At(%.2lf)\n",dgpsTime);
			outputlog(temp_log);
			dHeading=pre_fix;
		}
		else
		{
			//if(isnan(gpsdata->fix.track)!=0)
			dHeading      =  gps.heading;  //------ range from [0,360)
			if(m_vehspeed>2.0)
			pre_fix         = dHeading;
		}
		m_vehlat	 =  gps.latitude;
		m_vehlong	 =  gps.longitude;
		dElevation   =  gps.altitude;
		if(m_vehspeed<2.0)
		{
			m_vehlat =m_vehlat_pre;
			m_vehlong=m_vehlong_pre;
			dElevation_pre=dElevation;
			dHeading  = pre_fix;
		}
	}
	else
	{
		// if there is any error with GPS
		if(isnan(gps.latitude)!=0) {sprintf(temp_log,"--------GPS Lat Error.--------------At(%.2lf)\n",dgpsTime); outputlog(temp_log); }
		if(isnan(gps.longitude)!=0) {sprintf(temp_log,"--------GPS Long Error.--------------At(%.2lf)\n",dgpsTime);outputlog(temp_log); }
		if(isnan(gps.altitude)!=0) {sprintf(temp_log,"--------GPS Altitude Error.--------------At(%.2lf)\n",dgpsTime);outputlog(temp_log); }
		if(isnan(gps.heading)!=0) {sprintf(temp_log,"--------GPS Fix Error.--------------At(%.2lf)\n",dgpsTime);outputlog(temp_log); }
		if(isnan(gps.speed)!=0) {sprintf(temp_log,"--------GPS Speed Error.--------------At(%.2lf)\n",dgpsTime);outputlog(temp_log); }
		msleep(200);
	//	continue;
	}
	sprintf(temp_log," .................................... GPS Position Obtianed Successfully............... "); outputlog(temp_log); cout<<temp_log<<endl;
	sprintf(temp_log,"Lat %.8f Long %.8f Heading  %.3f Speed %.3f. At time %.2lf \n",m_vehlat,m_vehlong,dHeading,m_vehspeed,dgpsTime);		outputlog(temp_log);		cout<<temp_log;
}
void setup_logging()
{
	//Init log file
	//------log file name with Time stamp------------------
	xTimeStamp(ctimestamp);  // This function create a log file that has time stamp at the end 
	strcat(logfilename,ctimestamp);
	strcat(logfilename,".log");
	cout<<"logfilename is: "<<logfilename<<endl;
	
    fstream ftemp;
    ftemp.open(logfilename,fstream::out);
    if (!ftemp.good())
    {
        perror("Open logfilename failed!"); exit(1);
    }
    else
    {
        ftemp<<"Start Recording PRG at time:\t"<<time(NULL)<<endl;
        ftemp.close();
    }
	//------end log file name------------------------------
}
int outputlog(char *output)
{
	FILE * stream = fopen( logfilename, "r" );
	fseek( stream, 0L, SEEK_END );
	long endPos = ftell( stream );
	fclose( stream );

	fstream fs;
	if (endPos <50000000)
		fs.open(logfilename, ios::out | ios::app);
	else
		fs.open(logfilename, ios::out | ios::trunc);
	if (!fs || !fs.good())
	{
		cout << "could not open file!\n";
		return -1;
	}
	fs << output;

	if (fs.fail())
	{
		cout << "failed to append to file!\n";
		return -1;
	}


	return 1;
}




void get_veh_id(char filename[]) 
{
	string lineread;
	char temp[256]; 
	
	ifstream load;
	load.open(filename);
	if (load.good())
	{
		getline(load,lineread);
		sscanf(lineread.c_str(),"%s",temp);
		iVehicleId=atoi(temp);
		sscanf(lineread.c_str(),"%*s %s",temp);
		iVehiclePriorityLevel=atoi(temp);
		sscanf(lineread.c_str(),"%*s %*s %s",temp);
		strcpy(cVehicleName,temp);
		
		
		cout<<"Read vehicleid.txt successfully."<<endl;
		cout<<"Vehicle ID: " << iVehicleId<<endl;
		if (iVehiclePriorityLevel==1)
		{ 
			sprintf(temp_log,"Priority level: Emergency Vehicle");	outputlog(temp_log); cout<< temp_log <<endl;
		}
		if (iVehiclePriorityLevel==2)
		{ 
			sprintf(temp_log,"Priority level: TRANSIT");	outputlog(temp_log); cout<< temp_log <<endl;
		}
		if (iVehiclePriorityLevel==3)
		{ 
			sprintf(temp_log,"Priority level: TRUCK");	outputlog(temp_log); cout<< temp_log <<endl;
		}
		cout<<"Vehicle Name: " << cVehicleName<<endl;
	}
	else
	{
		perror("cannot open vehicleid.txt file.");
		sprintf(temp_log,"cannot open vehicleid.txt file.");
		outputlog(temp_log);
		cout<<temp_log<<endl;
	}
	load.close();
}

void setupConnection()
{
	if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) == -1)
	{
		perror("sockfd");
		exit(1);
	}
	if((setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,
					&broadcast,sizeof broadcast)) == -1)
	{
		perror("setsockopt - SO_SOCKET ");
		exit(1);
	}
	sendaddr.sin_family = AF_INET;
	sendaddr.sin_port = htons(30000);  //*** IMPORTANT: the vissim should also have this port. ***//
	sendaddr.sin_addr.s_addr =  INADDR_ANY; //inet_addr ("127.0.0.1"); //inet_addr(OBU_ADDR);// inet_addr ("150.135.152.35"); //

	memset(sendaddr.sin_zero,'\0',sizeof sendaddr.sin_zero);

	if(bind(sockfd, (struct sockaddr*) &sendaddr, sizeof sendaddr) == -1)
	{
		perror("bind");        exit(1);
	}


	recvaddr.sin_family = AF_INET;
	recvaddr.sin_port = htons(PRPORT);
	recvaddr.sin_addr.s_addr = inet_addr(BROADCAST_ADDR) ; //INADDR_BROADCAST;
	memset(recvaddr.sin_zero,'\0',sizeof recvaddr.sin_zero);
}


void initializeRefPoint(MAP * newmap)
{
	double ref_lat=newmap->intersection.Ref_Lat;
	double ref_long=newmap->intersection.Ref_Long;
	double ref_ele=newmap->intersection.Ref_Ele/10;
	geoPoint.init(ref_long, ref_lat, ref_ele);
}

void storeNodes(MAP * newmap)
{
	vector<LaneNodes> MAP_Nodes;
		for (unsigned int i=0;i<newmap->intersection.Approaches.size();i++)
		for(unsigned  int j=0;j<newmap->intersection.Approaches[i].Lanes.size();j++)
			for(unsigned int k=0;k<newmap->intersection.Approaches[i].Lanes[j].Nodes.size();k++)
			{	
				LaneNodes temp_node;
				temp_node.index.Approach=newmap->intersection.Approaches[i].Lanes[j].Nodes[k].index.Approach;
				temp_node.index.Lane=newmap->intersection.Approaches[i].Lanes[j].Nodes[k].index.Lane;
				temp_node.index.Node=newmap->intersection.Approaches[i].Lanes[j].Nodes[k].index.Node;
				temp_node.Latitude=newmap->intersection.Approaches[i].Lanes[j].Nodes[k].Latitude;
				temp_node.Longitude=newmap->intersection.Approaches[i].Lanes[j].Nodes[k].Longitude;
				geoPoint.lla2ecef(temp_node.Longitude,temp_node.Latitude,newmap->intersection.Ref_Ele/10 ,&ecef_x,&ecef_y,&ecef_z);
				geoPoint.ecef2local(ecef_x,ecef_y,ecef_z,&local_x,&local_y,&local_z);
				temp_node.N_Offset=local_x;
				temp_node.E_Offset=local_y;

				MAP_Nodes.push_back(temp_node);

		}
	newmap->MAP_Nodes=MAP_Nodes; 
}



double GetSeconds()
{
	struct timeval tv_tt;
	gettimeofday(&tv_tt, NULL);
	return (tv_tt.tv_sec+tv_tt.tv_usec/1.0e6);    
}


void storeVehicleTrajectory(int &pro_flag, int &Veh_pos_inlist)
{
	int found=0;
	double t1=GetSeconds();
	trackedveh.Reset();
	//Checking Vechicle ID to see if we have it in the vehicle list or not.
	while(!trackedveh.EndOfList())  //match vehicle according to vehicle ID
	{	
		//if ID is a match, store trajectory information
		//If ID will change, then need an algorithm to do the match!!!!
		if(trackedveh.Data().TempID==vehIn.TemporaryID)  //every 0.5s processes a trajectory 
		{
			{
				pro_flag=1;   //need to map the point						
				trackedveh.Data().receive_timer=t1;  //reset the timer
				trackedveh.Data().traj[trackedveh.Data().nFrame].latitude=vehIn.pos.latitude;
				trackedveh.Data().traj[trackedveh.Data().nFrame].longitude=vehIn.pos.longitude;
				trackedveh.Data().traj[trackedveh.Data().nFrame].elevation=vehIn.pos.elevation;
				//change GPS points to local points
				if (iLogIndicator==3)
				{
					sprintf(temp_log, " In the StoreTrajectory longitute %lf latitude %lf evelvation %lf    \n ",  vehIn.pos.longitude, vehIn.pos.latitude,vehIn.pos.elevation); outputlog(temp_log); cout<<temp_log;
				}
				
				trackedveh.Data().Speed=vehIn.motion.speed;
				trackedveh.Data().heading=vehIn.motion.heading;
				trackedveh.Data().Dsecond=vehIn.DSecond;
				trackedveh.Data().active_flag=5;  //reset active_flag every time RSE receives BSM from the vehicle
				trackedveh.Data().time[trackedveh.Data().nFrame]=t1;
				trackedveh.Data().nFrame++; 
				Veh_pos_inlist=trackedveh.CurrentPosition();  //store this vehicle's position in the tracked vehicle list, start from 0
				//if reached the end of the trajectory, start over
				if(trackedveh.Data().nFrame==4999)
					trackedveh.Data().nFrame=0;
			}
			found=1;
			break;
		}
		trackedveh.Next();
	}
	if(found==0)  //this is a new vehicle
	{
		TempVeh.TempID=vehIn.TemporaryID;
		TempVeh.traj[0].latitude=vehIn.pos.latitude;
		TempVeh.traj[0].longitude=vehIn.pos.longitude;
		TempVeh.traj[0].elevation=vehIn.pos.elevation;
		TempVeh.Speed=vehIn.motion.speed;
		TempVeh.nFrame=1;
		TempVeh.Phase_Request_Counter=0;
		TempVeh.active_flag=5;  //initilize the active_flag
		TempVeh.receive_timer=GetSeconds();
		TempVeh.time[0]=TempVeh.receive_timer;
		sprintf(temp_log,"Add Vehicle No. %d \n",TempVeh.TempID);
		outputlog(temp_log); cout<<temp_log;
		trackedveh.InsertRear(TempVeh);   //add the new vehicle to the tracked list
		Veh_pos_inlist=trackedveh.ListSize()-1;  //start from 0
	}
}


	//Find Vehicle position in the map
	//find the nearest Lane nodes from the vehicle location
void FindVehInMap(double Speed, double Heading,int nFrame,double N_Offset1,double E_Offset1,double N_Offset2,double E_Offset2, MAP NewMap, double &Dis_curr, double &est_TT, int &iVehicleStatus,int &approach, int &lane, double &dminGrn)
{
	int t_App,t_Lane,t_Node;
	int t_pos=0; //node position in the vector
	double lane_heading;
	//temp vehicle point
	//calculate the vehicle in the MAP
	//find lane, requesting phase and distance 
	double tempdis=1000000000.0;
	//find the nearest lane node
    // ----- loging 
	if (iLogIndicator==3)
	{
		sprintf(temp_log," In the Find vehicle in MAP Funciton  local x is    %lf and  local y  is %lf\n", N_Offset1,E_Offset1);
		outputlog(temp_log); cout<<temp_log;
	}
	// ---- end of logging
	
	double veh_heading=Heading;
	if (veh_heading<0)
		veh_heading+=360;
	
	int match=0;  //whether the vehicle's heading match the lanes heading  

	for(unsigned int jj=0;jj<NewMap.MAP_Nodes.size();jj++)
	{		
			double dis=sqrt((N_Offset1-NewMap.MAP_Nodes[jj].N_Offset)*(N_Offset1-NewMap.MAP_Nodes[jj].N_Offset)+(E_Offset1-NewMap.MAP_Nodes[jj].E_Offset)*(E_Offset1-NewMap.MAP_Nodes[jj].E_Offset));
			if (dis<tempdis)
			{
				tempdis=dis;
				t_App=NewMap.MAP_Nodes[jj].index.Approach;
				t_Lane=NewMap.MAP_Nodes[jj].index.Lane;
				t_Node=NewMap.MAP_Nodes[jj].index.Node;
				t_pos=jj;
			}
	}

	approach=t_App;
	lane=t_Lane;
	if(nFrame>=2) //start from second frame and we can find a node has same heading
	{
		// determine it is approaching the intersection or leaving the intersection or in queue
		// The threshold for determing in queue: 89.4 cm = 2mph
		//calculate the distance from the reference point here is 0,0;
		//int veh_state; 		// 1: appraoching; 2: leaving; 3: queue
		double N_Pos;  //current vehicle position
		double E_Pos;
		double E_Pos2; //previous vehicle position
		double N_Pos2;
		//find the first node (nearest of intersection) of the lane
		double inter_pos_N=NewMap.MAP_Nodes[t_pos-t_Node+1].N_Offset;
		double inter_pos_E=NewMap.MAP_Nodes[t_pos-t_Node+1].E_Offset;
		N_Pos=N_Offset1;//current position
		E_Pos=E_Offset1;
		N_Pos2=N_Offset2;//previous frame position
		E_Pos2=E_Offset2;
		//double veh_heading=atan2(N_Pos-N_Pos2,E_Pos-E_Pos2)*180/PI;
		lane_heading=NewMap.MAP_Nodes[t_pos].Heading;
		if(lane_heading<0)
			lane_heading+=360;
		if( abs(veh_heading - lane_heading)> 120 && abs(veh_heading - lane_heading) <240)  // if GPS drifted and we locate the vefhicle in the egress approach instead of ingreaa approach, we should recover the correct approach and lane!!
		{
			if(approach%2 == 1) //When the approach is 1, 3, 5,or 7
				approach = approach +1;
			else //When the approach number is 2, 4, 6,or 8
				approach = approach -1;
			match=1;
			double dTempDist=0.0;
			double dTempDist2=10000000.0;
			for(unsigned int jj=0;jj<NewMap.MAP_Nodes.size();jj++)
			{
				if (NewMap.MAP_Nodes[jj].index.Approach==approach)
				{
					dTempDist=sqrt((N_Offset1-NewMap.MAP_Nodes[jj].N_Offset)*(N_Offset1-NewMap.MAP_Nodes[jj].N_Offset)+(E_Offset1-NewMap.MAP_Nodes[jj].E_Offset)*(E_Offset1-NewMap.MAP_Nodes[jj].E_Offset));	
					if (dTempDist<dTempDist2)
					{
						dTempDist2=dTempDist;
						lane=NewMap.MAP_Nodes[jj].index.Lane;
					}
				}
			}
		}
		if (abs(veh_heading-lane_heading)<20)   //threshold for the difference of the heading
			match=1;
		if (veh_heading>340 && lane_heading<20)
		{
			veh_heading=veh_heading-360;
			if (abs(veh_heading-lane_heading)<20)
				match=1;
		}
		
		if (lane_heading>340 && veh_heading<20)
		{
			lane_heading=lane_heading-360;
			if (abs(lane_heading-veh_heading)<20)
				match=1;
		}
		double Dis_pre= sqrt((N_Pos2-inter_pos_N)*(N_Pos2-inter_pos_N)+(E_Pos2-inter_pos_E)*(E_Pos2-inter_pos_E));
		Dis_curr=sqrt((N_Pos-inter_pos_N)*(N_Pos-inter_pos_N)+(E_Pos-inter_pos_E)*(E_Pos-inter_pos_E));
		
		if(match==1 && approach%2==1)   //odd approach: igress approaches
		{
			if (fabs(Dis_pre-Dis_curr)<0.894/10)  //unit m/0.1s =2mph //Veh in queue is also approaching the intersection, leaving no queue
			{
				iVehicleStatus=3;  //in queue
			}
			if (fabs(Dis_pre-Dis_curr)>=0.894/10)
			{
				if (Dis_curr<Dis_pre)
					iVehicleStatus=1;  //approaching
				else
				{
					iVehicleStatus=2;  //leaving
					//request_phase=-1;  //if leaving, no requested phase
				}
			}
		}
		
		if(match==1 && approach%2==0)   //even approach: engress approaches
		{
			iVehicleStatus=2;  //leaving
		}
		
		if (match==0)
		{
			iVehicleStatus=4;   // NOT IN THE MAP
			//request_phase=-1;  
		}

		if (iVehicleStatus==1 || iVehicleStatus==3) //only vehicle approaching intersection need to do something
		{
			if (icodeUsage==2)
			{
				if(Speed<dSpeedChangeLmtForSRM) // when vehicle speed drops below 5mph=dSpeedChangeLmtForSRM   kmph on the road, we consider it as stoped vehicle!
				{
					dminGrn=1.8*(Dis_curr/7)+2;  // dminGrn assumed average length of vehicle is 7 meter and 1.8 is headway between two consecutive cars. 2 is first vehicle reaction time. 
					est_TT=dminGrn;    //if the vehicle is in queue, assume ETA is 0
				}
				else
				{
					est_TT=Dis_curr/(Speed/3.6); //if we use the lab version of the PRG (means if codeusage=1), speed is meter per second. But if road version is being used (codeusage=2),speed  should be converter to meter per second from km per hour
					dminGrn=0.0;
				}
			} else if (icodeUsage==1)
			{
				if(Speed<4.8) // when vehicle speed drops below 4.8 meterper second in the simulation , we consider it as stoped vehicle!
				{
					dminGrn=1.8*(Dis_curr/7)+2;  // dminGrn assumed average length of vehicle is 7 meter and 1.8 is headway between two consecutive cars. 2 is first vehicle reaction time. 
					est_TT=dminGrn;    //if the vehicle is in queue, assume ETA is 0
				}
				else
				{
					est_TT=Dis_curr/Speed; //if we use the lab version of the PRG (means if codeusage=1), speed is meter per second. But if road version is being used (codeusage=2),speed  should be converter to meter per second from km per hour
					dminGrn=0.0;
				}				
			}
					
			if(est_TT>9999)
				est_TT=9999;

		}
		else //Veh in State 2 or 4
		{
			est_TT=99999;
			Dis_curr=99999;
		}


	}
}


//~ void fillSRM(SRM_t *srm, BasicVehicle *veh, int intID, int iMsgCnt, bool bIsCanceled, int iInLane, int iOutLane, int iETA, double dMinGr, int vehState, int iprioritylevel, int Ru_Rl, char* cVehicleName)
//~ {
	//~ time_t theTime = time(NULL);
	//~ struct tm *aTime = localtime(&theTime);
	//~ srm->message_id=14;       
	//~ srm->msgcount=iMsgCnt;                                                      
	//~ srm->intersection_id =intID;   
	//~ srm->cancelreq_priority=bIsCanceled;
    //~ srm->cancelreq_preemption = 0;       // Extra fields that should be initialized 
	//~ srm->signalreq_priority = J2735_NAV; // Extra fields that should be initialized 
	//~ srm->signalreq_preemption = 2;       // Extra fields that should be initialized   
	//~ srm->in_lanenum=iInLane;
	//~ srm->out_lanenum=iOutLane;
//~ 
	//~ ret=OCTET_STRING_fromBuf(&srm->request.type,(char *)(&iprioritylevel),1);
	//~ 
	//~ srm->vehicleclass_level = 1;   // Extra fields that should be initialized    
	//~ strcpy(srm->code_word, "MMITSS code word");
	//~ 
	//~ srm->starttime_hour = aTime->tm_hour; 
	//~ srm->starttime_min  = aTime->tm_min; 
	//~ srm->starttime_sec  = aTime->tm_sec; 
	//~ int iHour=0;
	//~ int iMin=0;
	//~ int iSec=0;
	//~ int itempMinGrn=(int) dMinGr;
	//~ obtainEndofService( aTime->tm_hour,aTime->tm_min,aTime->tm_sec,iETA,iHour, iMin,iSec,itempMinGrn,Ru_Rl);
	//~ if (bIsCanceled==0)
	//~ {
		//~ srm->endtime_hour =  iHour;
		//~ srm->endtime_min  =  iMin;
		//~ srm->endtime_sec  =  iSec;
	//~ }else if (bIsCanceled==1)
	//~ { 	
		//~ srm->starttime_hour =0; 
		//~ srm->starttime_min = 0; 
		//~ srm->starttime_sec = 0; 
		//~ srm->endtime_hour  = 0;
		//~ srm->endtime_min   = 0;
		//~ srm->endtime_sec   = 0;
	//~ }
//~ 
//~ 
    //~ srm->transitstatus = 1;
    //~ strcpy(srm->vehicle_name,cVehicleName);
    //~ strcpy(srm->vin, "MMITSS2014");
	//~ strcpy(srm->vehicle_ownercode,"MMITSS");
	//~ srm->temp_ident_id =veh->TemporaryID;
	//~ srm->vehicleclass_type = iprioritylevel;  
	//~ srm->vehicle_class = 9985;					// Extra fields that should be initialized  
    //~ srm->vehicle_grouptype = 1;					// Extra fields that should be initialized  
//~ 
   //~ // Filling hte BSM blob part of theSRM
    //~ srm->bsm_msgcount = 0;						// Extra fields that should be initialized  
    //~ srm->temp_id=veh->TemporaryID ;
    //~ srm->dsecond	  = 0;						// Extra fields that should be initialized  
	//~ srm->latitude=veh->pos.latitude;
	//~ srm->longitude=veh->pos.longitude;
	//~ srm->elevation=veh->pos.elevation;
	//~ srm->positionalaccuracy[0]		= 0.0;
	//~ srm->positionalaccuracy[1]		= 0.0;
	//~ srm->positionalaccuracy[2]		= 0.0;
	//~ srm->speed=veh->motion.speed ;
	//~ srm->heading=veh->motion.heading;
	//~ srm->angle						= 0; 		// Extra fields that should be initialized  
	//~ srm->longaccel					= 0.0;
	//~ srm->lataccel					= 0.0;
	//~ srm->vertaccel					= 0.0;
	//~ srm->yawrate = dMinGr;						
	//~ srm->wheelbrake					= 1;	// Extra fields that should be initialized  
	//~ srm->wheelbrakeavailable		= 1;	// Extra fields that should be initialized  
	//~ srm->sparebit					= 1;	// Extra fields that should be initialized  
	//~ srm->traction					= 1;	// Extra fields that should be initialized  
	//~ srm->abs						= 0;	// Extra fields that should be initialized  
	//~ srm->stabilitycontrol			= 1;	// Extra fields that should be initialized  
	//~ srm->brakeboost					= 1;	// Extra fields that should be initialized  
	//~ srm->auxbrakes					= 1;	// Extra fields that should be initialized  
//~ 
	//~ srm->vehicle_width=2.0;
	//~ srm->vehicle_length=2.0;                                                            
	//~ srm->vehicle_status=vehState;
//~ 
//~ }



//~ // This function is used when lab version of the code is applied
//~ void fillSRM(J2735SRM_t *srm,int intID,int iMsgCnt,bool bIsCanceled,int iInLane,int iOutLane,int iETA, double dMinGr, int vehState,int Ru_Rl,char *cVehicleName)
//~ {
	//~ time_t theTime = time(NULL);
	//~ struct tm *aTime = localtime(&theTime);
	//~ srm->message_id=14;                                                             
	//~ srm->msgcount=iMsgCnt;
	//~ srm->in_lanenum=iInLane;
	//~ srm->out_lanenum=iOutLane;
	//~ srm->cancelreq_priority=bIsCanceled;
	//~ strcpy(srm->vehicle_name,cVehicleName);
//~ 
	//~ // In VISSIM different class!! of vehicles are diffrentiated by their category!. So in DrivelModel.dll , DRIVER_DATA_VEH_CATEGORY gets different values for different class of vehicles!!  car = 1, truck = 2, bus = 3, tram = 4, pedestrian = 5, bike = 6 .
	//~ // When we pack the SRM in dll, we put these numbers in  vehicleclass_level  field of SRM. 
	//~ // Here in PRG, we put the srm->vehicleclass_level into srm->vehicle_type
	//~ // Since we do not have any EV in dll, we consider trams as EV!! 
//~ /*	if (srm->vehicleclass_level==4)// which the category of Tram in VISSIM     
	    //~ srm->vehicle_type=1;  // in priority applications, priority level of EV is 1
    //~ if (srm->vehicleclass_level==3)  // which the category of bus in VISSIM
		//~ srm->vehicle_type=2;   // in priority applications, priority level of bus is 2
	//~ if (srm->vehicleclass_level==2)  // which the category of truck in VISSIM
		//~ srm->vehicle_type=3;    // in priority applications, priority level of truck is 3
	//~ if (srm->vehicleclass_level==1)  // which the category of car in VISSIM
		//~ srm->vehicle_type=4;    // in priority applications, priority level of car is 4
//~ */	
//~ 
	//~ if (srm->vehicleclass_level==4)// which the category of Tram in VISSIM     
	   //~ srm->vehicleclass_type=1;  // in priority applications, priority level of EV is 1
    //~ if (srm->vehicleclass_level==3)  // which the category of bus in VISSIM
		//~ srm->vehicleclass_type=2;   // in priority applications, priority level of bus is 2
	//~ if (srm->vehicleclass_level==2)  // which the category of truck in VISSIM
		//~ srm->vehicleclass_type=3;    // in priority applications, priority level of truck is 3
	//~ if (srm->vehicleclass_level==1)  // which the category of car in VISSIM
		//~ srm->vehicleclass_type=4;    // in priority applications, priority level of car is 4
//~ 
//~ 
	//~ srm->intersection_id =intID;   
	//~ strcpy(srm->code_word, "MMITSS code word");
	//~ srm->starttime_hour = aTime->tm_hour; 
	//~ srm->starttime_min  = aTime->tm_min; 
	//~ srm->starttime_sec  = aTime->tm_sec; 
	//~ int iHour=0;
	//~ int iMin=0;
	//~ int iSec=0;
	//~ int itempMinGrn=(int) dMinGr;
//~ 
	//~ obtainEndofService( aTime->tm_hour,aTime->tm_min,aTime->tm_sec,iETA,iHour, iMin,iSec,itempMinGrn,Ru_Rl);
//~ 
//~ 
	//~ if (bIsCanceled==0)
	//~ {
		//~ srm->endtime_hour =  iHour;
		//~ srm->endtime_min  =  iMin;
		//~ srm->endtime_sec  =  iSec;
	//~ }else if (bIsCanceled==1)
	//~ { 	
		//~ srm->starttime_hour =0; 
		//~ srm->starttime_min = 0; 
		//~ srm->starttime_sec = 0; 
		//~ srm->endtime_hour  = 0;
		//~ srm->endtime_min   = 0;
		//~ srm->endtime_sec   = 0;
	//~ }
	//~ strcpy(srm->vin, "MMITSS2014");
	//~ strcpy(srm->vehicle_ownercode,"MMITSS");
	//~ srm->transitstatus = 1;
	//~ srm->temp_ident_id = srm->temp_id;//  in the field IT should be iVehicleId ---- BE CAREFULL ----
	//~ srm->yawrate = dMinGr;
	//~ srm->vehicle_status=vehState;
	//~ printsrmcsv(srm);
	//~ 
	//~ 
//~}

void obtainEndofService( int ihour,int imin,int isec, int iETA,int &iHour,int &iMin,int &iSec, int iMinGrn,int Ru_Rl)
{
	int iRu_Rl=(int) (Ru_Rl/2);
	if (iMinGrn==0)
	{
		if ((iETA>=0) && (iETA<59))
		{
			if (imin ==59)
			{
				if ((isec + iETA +iRu_Rl)>=60)
				{
					if (ihour==23)
					{
						iHour   = 0;
						iMin = 0;
						iSec = isec + iETA+iRu_Rl - 60;
					}else
					{
						iHour   = ihour + 1;
						iMin = 0;
						iSec = isec + iETA+iRu_Rl - 60;
					}
				}else
				{
					iHour    = ihour;
					iMin  = imin;
					iSec  = isec + iRu_Rl+iETA;
				}
			}else
			{
				if ((isec + iRu_Rl + iETA)>=60)
				{
					iHour    = ihour;
					iMin  = imin+1;
					iSec  = isec + iRu_Rl +iETA- 60;
				}
				else
				{
					iHour    = ihour;
					iMin  = imin;
					iSec = isec + iRu_Rl+iETA;
				}
			}
		}

		if ((iETA>=60) && (iETA<119))
		{
			if (imin ==59) 
			{
				if ((isec + iETA +iRu_Rl)<120)
				{
					if (ihour==23)
					{
						iHour   = 0;
						iMin = 0;
						iSec = isec + iETA+iRu_Rl - 60;
					}else
					{
						iHour   = ihour + 1;
						iMin = 0;
						iSec = isec + iETA+iRu_Rl - 60;
					}
				}else
				{
					if (ihour==23)
					{
						iHour   = 0;
						iMin = 1;
						iSec = isec + iETA+iRu_Rl - 120;
					}else
					{
						iHour   = ihour + 1;
						iMin = 1;
						iSec = isec + iETA+iRu_Rl - 120;
					}
				}
			}else if (imin ==58)
			{
				if ((isec + iETA +iRu_Rl)<120)
				{
					iHour   = ihour ;
					iMin = 59;
					iSec = isec + iETA+iRu_Rl - 60;

				}else
				{
					if (ihour==23)
					{
						iHour   = 0;
						iMin = 0;
						iSec = isec + iETA+iRu_Rl - 120;
					}else
					{
						iHour   = ihour + 1;
						iMin = 0;
						iSec = isec + iETA+iRu_Rl - 120;
					}
				}
			}else if (imin <58)
			{
				if ((isec + iETA +iRu_Rl)<120)
				{
					iHour=ihour;
					iMin = imin+1;
					iSec = isec + iETA+iRu_Rl - 60;

				}else
				{
					iHour=ihour;
					iMin = imin+2;
					iSec = isec + iETA+iRu_Rl - 120;
				}
			}
		}
	}

	if (iMinGrn>0)
	{
		
		if ((iMinGrn>=0) && (iMinGrn<59))
		{
			if (imin ==59)
			{
				if ((isec + iMinGrn)>=60)
				{
					if (ihour==23)
					{
						iHour   = 0;
						iMin = 0;
						iSec = isec + iMinGrn +iRu_Rl- 60;
					}else
					{
						iHour   = ihour + 1;
						iMin = 0;
						iSec = isec + iMinGrn +iRu_Rl- 60;
					}
				}else
				{
					iHour    = ihour;
					iMin  = imin;
					iSec  = isec +iMinGrn+iRu_Rl;
				}
			}else
			{
				if ((isec + iMinGrn)>=60)
				{
					iHour    = ihour;
					iMin  = imin+1;
					iSec  = isec +iMinGrn+iRu_Rl- 60;
				}
				else
				{
					iHour    = ihour;
					iMin  = imin;
					iSec = isec +iMinGrn+iRu_Rl;
				}
			}
		}

		if ((iMinGrn>=60) && (iMinGrn<119))
		{
			if (imin ==59) 
			{
				if ((isec + iMinGrn )<120)
				{
					if (ihour==23)
					{
						iHour   = 0;
						iMin = 0;
						iSec = isec + iMinGrn+iRu_Rl- 60;
					}else
					{
						iHour   = ihour + 1;
						iMin = 0;
						iSec = isec + iMinGrn+iRu_Rl- 60;
					}
				}else
				{
					if (ihour==23)
					{
						iHour   = 0;
						iMin = 1;
						iSec = isec + iMinGrn +iRu_Rl- 120;
					}else
					{
						iHour   = ihour + 1;
						iMin = 1;
						iSec = isec + iMinGrn +iRu_Rl- 120;
					}
				}
			}else if (imin ==58)
			{
				if ((isec + iMinGrn )<120)
				{
					iHour   = ihour ;
					iMin = 59;
					iSec = isec + iMinGrn +iRu_Rl - 60;

				}else
				{
					if (ihour==23)
					{
						iHour   = 0;
						iMin = 0;
						iSec = isec + iMinGrn+iRu_Rl- 120;
					}else
					{
						iHour   = ihour + 1;
						iMin = 0;
						iSec = isec + iMinGrn+iRu_Rl - 120;
					}
				}
			}else if (imin <58)
			{
				if ((isec + iMinGrn)<120)
				{
					iHour=ihour;
					iMin = imin+1;
					iSec = isec + iMinGrn +iRu_Rl- 60;

				}else
				{
					iHour=ihour;
					iMin = imin+2;
					iSec = isec + iMinGrn +iRu_Rl- 120;

				}

			}
		}
	}
}	


void readMAPsFromFile(char *filename,LinkedList<MAPEntry>& mapList)
{
	fstream fsmap;
	fsmap.open(filename);
	string lineread;
	long mapID;
	int attime;

	mapList.ClearList();

	if(fsmap.good())
	{
		while(!fsmap.eof())
		{
			getline(fsmap,lineread);

			if(lineread.size()!=0)
			{

				if(sscanf(lineread.c_str(),"%ld %d",&mapID,&attime)!=2)
				{
					perror("Not all fields are assigned.");
					outputlog("Not all fields are assigned.");
					exit(1);
				}
				else
				{
					MAPEntry currentMap(mapID,attime);
					mapList.InsertAfter(currentMap);
					//	cout<<lineread<<endl;
				}
			}
		}
	}
	else
	{
		perror("cannot open RNDF.txt file.");
		strcpy(temp_log,"cannot open RNDF.txt file.");
		outputlog(temp_log);

	}

	if(mapList.ListSize()<=0){
		strcpy(temp_log,"Empty Intersection_map file ");
		outputlog(temp_log); cout<<"Empty Intersection_map file "<< endl;
	}

	fsmap.close();
}



void getBusStopsRange(char filename[])
{
	// the file should be in this format  
	// Route_Name xxxx 
	// Number_Of_Intersections xxx
	// Intersection MAP ID, number of considered approach
	// Approach number, bus stop distance to stop bar of this approach 
	string lineread;
	char temp[256]; 
	char cRouteName[128];
	int iApprNumber=0;
	int iNoAppr=0;
	int iMAPID=0;
	int iTempDistance=9999;
	
	memset(iBusStopsApprRange,0,sizeof(iBusStopsApprRange));
	memset(iBusStopsMAPID,0, sizeof(iBusStopsMAPID));
	ifstream load;
	load.open(filename);
	
	if(load.good())
	{
	 	getline(load,lineread);
		sscanf(lineread.c_str(),"%*s %s ",temp);
		strcpy(cRouteName,temp);
		getline(load,lineread);
		sscanf(lineread.c_str(),"%*s %s",temp);
		iNoIntersec=atoi(temp);
		for(int i=0;i<iNoIntersec;i++)
		{
			getline(load,lineread);
			sscanf(lineread.c_str(),"%s",temp);
			iMAPID=atoi(temp);
			iBusStopsMAPID[i]=iMAPID;
			sscanf(lineread.c_str(),"%*s %s",temp);
			iNoAppr=atoi(temp);
			for (int j=0;j<iNoAppr; j++)
			{
				getline(load,lineread);
				sscanf(lineread.c_str(),"%s",temp);
				iApprNumber=atoi(temp);
				sscanf(lineread.c_str(),"%*s %s",temp);
				iTempDistance=atoi(temp);
				iBusStopsApprRange[i][iApprNumber]=iTempDistance;				
			}
		}
		sprintf(temp_log," Read Transit Route %s  bus stops ranges successfully ", cRouteName); 
		outputlog(temp_log);
		cout<<temp_log<<endl;
		sprintf(temp_log," Number of Intersections %d  ", iNoIntersec); 
		outputlog(temp_log);
		cout<<temp_log<<endl;
		// ---- logging	
		for (int i=0;i<iNoIntersec;i++)
		{
			for (int j=0;j<8;j++)
			{
				if (iBusStopsApprRange[i][j]!=0)
				{
					sprintf(temp_log," MAP ID  %d in approach %d with range %d ", iBusStopsMAPID[i] , j , iBusStopsApprRange[i][j] ); 
					outputlog(temp_log);
					cout<<temp_log<<endl;
				}
			}
		}
		// ---- End of logging
	}else
	{
		perror("cannot open busStopsRange.txt file or there is no Transit Route information.");
		sprintf(temp_log,"cannot open busStopsRange.txt file or there is no Transit Route information.");
		outputlog(temp_log);
		cout<<temp_log<<endl;
	}
	load.close();
}


int findMAPPositionInList(LinkedList<MAP> mapList,long id)
{
	mapList.Reset();
	int temp=-1;
	if(mapList.ListEmpty())
	{
		return temp;
	}
	else
	{
		while(!mapList.EndOfList())
		{

			if(mapList.Data().intersection.ID==id)
			{
				return mapList.CurrentPosition();
			}
			mapList.Next();
		}
	}
	return temp;

}

//This funciton is used when road verison of the code is applied
void formBSMOfSRM(BasicVehicle *veh, int tempID, int msgcnt, double lat, double lon, double heading, double speed,double elev)
{
	veh->TemporaryID = tempID ;
	veh->MsgCount = msgcnt ;
	veh->motion.speed = speed ;
	veh->motion.heading = heading ;
	veh->pos.latitude = lat ;
	veh->pos.longitude = lon ;
	veh->pos.elevation=elev;
	veh->DSecond=0;
}


void     xTimeStamp( char * pc_TimeStamp_ )
{
	struct tm  * ps_Time;
	time_t       i_CurrentTime;
	char         ac_TmpStr[256];

	i_CurrentTime =  time(NULL);
	ps_Time = localtime( &i_CurrentTime );

	//year
	sprintf(ac_TmpStr, "%d", ps_Time->tm_year + 1900);
	strcpy(pc_TimeStamp_, ac_TmpStr);

	//month
	sprintf(ac_TmpStr, "_%d", ps_Time->tm_mon + 1 );
	strcat(pc_TimeStamp_, ac_TmpStr);

	//day
	sprintf(ac_TmpStr, "_%d", ps_Time->tm_mday );
	strcat(pc_TimeStamp_, ac_TmpStr);

	//hour
	sprintf(ac_TmpStr, "_%d", ps_Time->tm_hour  );
	strcat(pc_TimeStamp_, ac_TmpStr);

	//min
	sprintf(ac_TmpStr, "_%d", ps_Time->tm_min );
	strcat(pc_TimeStamp_, ac_TmpStr);

	//sec
	sprintf(ac_TmpStr, "_%d", ps_Time->tm_sec );
	strcat(pc_TimeStamp_, ac_TmpStr);
}

/*
int gps_init()
{
	gpsdata = gps_open("127.0.0.1", "2947");
	if (!gpsdata) {
		printf("Error: Failed to connect to gps\n");
		return -1;
	}
	gps_query(gpsdata, "o\n"); // update fix once 
	usleep(1000*1000); // give it sometime to get first fix 
	return 0;
}

*/
 
int gps_init() 
{
    int is_async = 0;
    int fd;
    gps_handle = savari_gps_open(&fd, is_async);
    if (gps_handle == 0) {
        printf("sorry no gpsd running\n");
        return -1;
    }
   return 0;
}
 
 
void printgpscsv () {
 
    static int first = 1;
   if (first) {
 
        printf ("time,latitude,longitude,elevation,speed,heading\n");
 
        first = 0;
 
    }
    printf ("%lf,", gps.time);
    printf ("%lf,", gps.latitude);
    printf ("%lf,", gps.longitude);
    printf ("%lf,", gps.altitude);
    printf ("%lf,", gps.speed);
    printf ("%lf\n", gps.heading);
}
 
 
 
 
 
void read_gps () {
 
    savari_gps_read (&gps, gps_handle);
    /* printgpscsv (); */
 
}

int msleep(unsigned long milisec)
{
	struct timespec req={0};
	time_t sec=(int)(milisec/1000);
	milisec=milisec-(sec*1000);
	req.tv_sec=sec;
	req.tv_nsec=milisec*1000000L;
	while(nanosleep(&req,&req)==-1)
		continue;
	return 1;
}





int ReqListFromFile(char *filename,LinkedList<ReqEntry>& Req_List)
{
	fstream fss;
	fss.open(filename,fstream::in);
	ReqEntry req_temp;
	int ReqNo=0;
	long OBU_ID;
	int Veh_Class=0;
	int Req_Phase=0;
	int abs_time=0;
	int ETA=0;
	float MinGrn=0.0;
	int inlane=0;
	int outlane=0;  
	int strhour=0; 
	int strmin=0; 
	int strsec=0; 
	int endhour=0; 
	int endmin=0; 
	int endsec=0;
	int vehstate=0;
	int imsgcnt=100;
	
	string lineread;
	Req_List.ClearList();
	getline(fss,lineread);
	sscanf(lineread.c_str(),"%*s %d",&ReqNo); 
	while(!fss.eof())
	{
		getline(fss,lineread);
		if(lineread.size()!=0)    
		{
			sscanf(lineread.c_str(),"%ld %d %d %d %f %d %d %d %d %d %d %d %d %d %d %d ",&OBU_ID,&Veh_Class,
				&ETA,&Req_Phase,&MinGrn,&abs_time,&inlane, &outlane, &strhour, &strmin, &strsec, &endhour, &endmin, &endsec, &vehstate, &imsgcnt);
			req_temp.ReqEntryFromFile(OBU_ID, Veh_Class, ETA, Req_Phase, MinGrn, abs_time,0, inlane, outlane, strhour, strmin, strsec, endhour, endmin, endsec, vehstate,  imsgcnt);
			Req_List.InsertAfter(req_temp);
			
			
		//	printf(" Veh State %s \n",vehstate);
		}
		
	}

	fss.close();
    return ReqNo;
}

int isThisVehIDinList( long VehID, LinkedList<ReqEntry> Req_List)
{
	Req_List.Reset();
	int iTemp=-1;
	if(Req_List.ListEmpty()==0) // when list is not empty
    {
	    while (!Req_List.EndOfList())
		{
			if (Req_List.Data().VehID==VehID)
			{
				iTemp=Req_List.CurrentPosition();
			}
			Req_List.Next();
		}
	}
	return iTemp;	
}
