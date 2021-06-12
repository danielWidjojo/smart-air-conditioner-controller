/*
 * Copyright (c) 2018, circuits4you.com
 * All rights reserved.
 * Create a TCP Server on ESP8266 NodeMCU. 
 * TCP Socket Server Send Receive Demo
*/
#ifndef LAN_H // include guard
#define LAN_H
typedef bool Boolean;

class Lan{
String ipAdd="";
const char *ssid = "ikan";  //Enter your wifi SSID
const char *password = "f1f403a03a";  //Enter your wifi Password
unsigned int localUdpPort = 10000;
public:
void starts();
void udpServer(String *ipAdd, String *msg);
void tcpServer(bool *powerStatusAc, float *setTemperatureAc, float humEnv=40,float tempEnv=23,int compStatus=1);
void tcpClient(String ipAdd, int port);
String jsonIndetifyString(String id,String ipAdd,int port);
String jsonStatusString(String id,bool powerStatus,float tempEnv,float humEnv, float setTempAc, int compressorStatus);
void jsonDeserialise(String inputStringJson);

};

#endif 
