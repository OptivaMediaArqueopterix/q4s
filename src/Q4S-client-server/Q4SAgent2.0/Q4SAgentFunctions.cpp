#include "Q4SAgentFunctions.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "rapidjson/istreamwrapper.h"
#include <fstream>
#include "rapidjson/ostreamwrapper.h"



#include <cstdio>

#include <stdio.h>	
#include <stdlib.h>     
#include <iostream>
#include <string>
#include <sstream>      
#include <fstream>
#include <vector>

using namespace rapidjson;
using namespace std;

int	   Actuator::CountSameStates=0;
string Actuator::CurrentState ="S0";
string Actuator::PreviousState="XXXXXXX";
float  Actuator::inf = std::numeric_limits<float>::infinity();

/*Actuator::Actuator ()
{
    clear();
}

Actuator::~Actuator ()
{
    done();
}*/

void Actuator::ReadConfigFile()
{
	ifstream config ("Rules.csv");
	std::string line;
	std::string key;	
	std::string value;
	    if ( config.is_open() )
	    {
	      	while( std::getline(config, line) )
	     	{
		        std::istringstream is_line(line);
		        if( std::getline(is_line, key, ';') ){
		        	State.push_back(key);
		        }
				if( std::getline(is_line, value,';') ) {
	           	 	TypeAlert.push_back(value);
          		}
	          	if( std::getline(is_line, value,';') ) {
					if(value.compare("*")!=0)
	           	 		LatencyMin.push_back(stof(value));
					else
						LatencyMin.push_back(0);
	          	}
	          	if( std::getline(is_line, value,';') ) {
					if(value.compare("*")!=0)
	           	 		LatencyMax.push_back(stof(value));
					else
						LatencyMax.push_back(inf);
	          	}   
	          	if( std::getline(is_line, value,';') ) {
	           	 	if(value.compare("*")!=0)
	           	 		JitterMin.push_back(stof(value));
					else
						JitterMin.push_back(0);
	          	}        
	          	if( std::getline(is_line, value,';') ) {
					if(value.compare("*")!=0)
	           	 		JitterMax.push_back(stof(value));
					else
						JitterMax.push_back(inf);
          		}  
          		if( std::getline(is_line, value,';') ) {
	           	 	if(value.compare("*")!=0)
	           	 		PacketlossMin.push_back(stof(value));
					else
						PacketlossMin.push_back(0);
          		} 
          		if( std::getline(is_line, value,';') ) {
					if(value.compare("*")!=0)
	           	 		PacketlossMax.push_back(stof(value));
					else
						PacketlossMax.push_back(inf);
          		} 
          		if( std::getline(is_line, value,';') ) {
	           	 	Action.push_back(value);
          		}
          		if( std::getline(is_line, value,' ') ) {
	           	 	Nextstate.push_back(value);
          		}
	      	}
      		config.close();
	    }
	else  cout << "Unable to open file";  
}

void Actuator::CleanVectors(){

	State.clear();		        
	TypeAlert.clear();
	LatencyMin.clear();
	LatencyMax.clear();
	JitterMin.clear();
	JitterMax.clear();
	PacketlossMin.clear();
	PacketlossMax.clear();
	Action.clear();
	Nextstate.clear();
          		
}

void Actuator::PolicyServerComunication()
{

	/**TODO: REALIZAR CONTROL DE ERRORES**/
	int Authentication, StartSesion;
	printf ("Policy Server Enabled");
	Authentication=system ("REQ1_startLogin.bat");
	/*if (Authentication!=0){
		printf("Authentication Error \n");
	
	}*/
	StartSesion=system ("REQ2_startSession.bat");
	
	
	ifstream ifst("response.file");
	IStreamWrapper iswr(ifst);
	Document dd;
	dd.ParseStream(iswr);
	
	ifstream ifs("alert.json");
	IStreamWrapper isw(ifs);
	Document d;
	d.ParseStream(isw);

	Value& sesionIDResponse= dd["sessionID"];
	Value& sesionIDAlert = d["session"]["sessionID"];
	sesionIDAlert=sesionIDResponse;

	ofstream ofs("alerttest.json");
	OStreamWrapper osw(ofs);
	Writer<OStreamWrapper> writer(osw);
	d.Accept(writer);

}

void Actuator::Print (){

	for (int i=0; i<State.size();i++)
	{
		cout << "states"<< i << "  =" << State[i] << endl;
		cout << "LatencyMin"<< i << "  =" << LatencyMin[i] << endl;
		cout << "LatencyMax"<< i << "  =" << LatencyMax[i] << endl;
		cout << "JitterMin"<< i << "  =" << JitterMin[i] << endl;
		cout << "JitterMax"<< i << "  =" << JitterMax[i] << endl;
		cout << "PacketlossMin"<< i << "  =" << PacketlossMin[i] << endl;
		cout << "PacketlossMax"<< i << "  =" << PacketlossMax[i] << endl;
		cout << "Action"<< i << "  =" << Action[i] << endl;
		cout << "Nextstate"<< i << "  =" << Nextstate[i] << endl;
	}
}

void Actuator::SearchState(std::string TypeAlertIn){

	bool FirstTime=true;
	CountSameStates=0;
	for (int i=0; i<State.size();i++){
#if DEBUG
		cout << "State i " << State[i] << "Current" << CurrentState << "type alert i" << TypeAlert[i] <<"rype alert in " <<TypeAlertIn<< "State.size()"<< State.size()<< endl;
#endif
		if ( State[i] == CurrentState && TypeAlert[i]==TypeAlertIn){
			if (FirstTime){
				CurrentValues=i;
				FirstTime=false;
			}
			//break;
			CountSameStates++;
		}
	}
}

void Actuator::UpdateState(){
	
//	if (PreviousState!=Nextstate[CurrentValues])
//	{
		PreviousState=CurrentState;
		CurrentState=Nextstate[CurrentValues];
//	}
#if DEBUG
	cout << "Current State: "<< CurrentState << " - Previous State: " << PreviousState << endl;

#endif
	cout << "Current State: "<< CurrentState << endl;
}

void Actuator::JsonFile(float Jitter, float Latency, unsigned int Packetloss){

	 
	ifstream ifs("alert.json");
	IStreamWrapper isw(ifs);
	Document d;
	d.ParseStream(isw);

	Value& sLantency = d["latency"];
	sLantency=Latency;
	Value& sJitter = d["jitter"];
	sJitter=Jitter;
	Value& sPacketloss = d["packetloss"];
	sPacketloss=Packetloss;

	ofstream ofs("alertupdate.json");
	OStreamWrapper osw(ofs);
	Writer<OStreamWrapper> writer(osw);
	d.Accept(writer);
	
}

void Actuator::TestRules (float Jitter, float Latency, unsigned int Packetloss){

	for (int i=0;i<CountSameStates;i++){
		if (Jitter > JitterMin[CurrentValues+i] && Jitter<JitterMax[CurrentValues+i]){
			UpdateJitter=false;
		}
		else{
			UpdateJitter=true;
			CurrentValues=CurrentValues+i;
			break;
		}
		
		if (Latency > LatencyMin[CurrentValues+i] && Latency<LatencyMax[CurrentValues+i]){
#if DEBUG
			printf("if Latency=%f, Latencymin %f, LatencyMax %f\n",Latency, LatencyMin[CurrentValues+i], LatencyMax[CurrentValues+i]);
#endif		
			UpdateLatency=false;
		}
		else{
#if DEBUG
			printf("else Latency=%f, Latencymin %f, LatencyMax %f\n",Latency, LatencyMin[CurrentValues+i], LatencyMax[CurrentValues+i]);
#endif
			UpdateLatency=true;
			CurrentValues=CurrentValues+i;
			break;
		}
		if (Packetloss >= PacketlossMin[CurrentValues+i] && Packetloss<PacketlossMax[CurrentValues+i]){
			UpdatePacketloss=false;
		}
		else{
			UpdatePacketloss=true;
			CurrentValues=CurrentValues+i;
			break;
		}
	}
}



bool Actuator::PathAlert (float Jitter, float Latency, unsigned int Packetloss, std::string &action, std::string TypeAlerIn )
{
	bool ok=true;
	SearchState(TypeAlerIn);
	TestRules (Jitter, Latency, Packetloss);
#if DEBUG
	printf("Lmin=%f Lmax=%f  State.size() %d \n",  LatencyMin, LatencyMax, State.size());
	printf("J=%f L=%f P=%d, UJ=%d, UL=%d, UPL=%d \n", Jitter, Latency, Packetloss, UpdateJitter, UpdateLatency, UpdatePacketloss);
#endif
	if (UpdateJitter==true || UpdateLatency==true || UpdatePacketloss==true)
	{
		UpdateState();
		action=Action[CurrentValues];
	}
	else
	{
		action="echo No action need it";
	}
	return ok;
}

bool Actuator::PathRecovery (std::string &action, std::string TypeAlerIn )
{
	bool ok=true;
	SearchState(TypeAlerIn);
	UpdateState();
	action=Action[CurrentValues];
	return ok;
}