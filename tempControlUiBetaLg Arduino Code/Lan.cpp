#include <Arduino.h>
#include "Lan.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

WiFiUDP Udp;
int portServerTcp = 8888;  //Port number
WiFiServer server(portServerTcp);
WiFiClient client;
WiFiClient clientServer;
void Lan::starts(){
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password); //Connect to wifi
     
      // Wait for connection  
      Serial.println("Connecting to Wifi");
      while (WiFi.status() != WL_CONNECTED) {   
        delay(500);
        Serial.print(".");
        delay(500);
      }
    
      Serial.println("");
      Serial.print("Connected to ");
      Serial.println(ssid);
    
      Serial.print("IP address: ");
       ipAdd=WiFi.localIP().toString();
      Serial.println(ipAdd);  
     
      
      server.begin();

      Udp.begin(localUdpPort);
}
void Lan::udpServer(String *ipAdd, String *msg){
    char incomingPacket[256];
    String result[2];
    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
      //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
      String ipAddControl=Udp.remoteIP().toString().c_str();
      int len = Udp.read(incomingPacket, 255);
      if (len > 0)
      {
        incomingPacket[len] = '\0';
      }
      Serial.printf("UDP packet contents: %s\n", incomingPacket);
      *ipAdd=Udp.remoteIP().toString();
      *msg=incomingPacket;
     
    }else{
      *ipAdd="";
      *msg="";
    }
    
 }
void Lan::tcpServer(bool *powerStatusAc, float *setTemperatureAc, float humEnv,float tempEnv,int compStatus){
  clientServer = server.available();
  
  if (clientServer) {
    if(clientServer.connected())
    {
      
      Serial.println("Client Connected");
    }
    
    while(clientServer.connected()){      
      while(clientServer.available()>0){
        // read data from the connected client
        String msgClient="";
        msgClient=clientServer.readStringUntil('\n');
        Serial.println(msgClient); 
        if (msgClient=="update"){
             Serial.println("Updating task"); 
        }else{
          //jsonDeserialise(msgClient);
          StaticJsonDocument<200> doc;

          DeserializationError error = deserializeJson(doc, msgClient);
          const bool powerStatus = doc["powerStatus"];
          *powerStatusAc=powerStatus;
          //long time = doc["time"];
          double setTempAc = doc["setTempAc"];
          *setTemperatureAc=float(setTempAc);
          //double longitude = doc["data"][1];
        
          // Print values.
          Serial.println(*powerStatusAc);
          //Serial.println(time);
          Serial.println(*setTemperatureAc, 2);
        }
        //break;
        Serial.println(tempEnv);

        
        //clientServer.write('test\n');
        String jsonStatus= jsonStatusString("ToshibaAc",*powerStatusAc,tempEnv,humEnv,*setTemperatureAc,compStatus)+"\n";
        clientServer.write((jsonStatus).c_str());
        break;
      }
      
    }
    clientServer.stop();
    
    Serial.println("Client disconnected");    
  }
}
void Lan::tcpClient(String ipAdd, int port){
    Serial.println("\n[Connecting to %s ... " +ipAdd);
   const char* ipAddStr = ipAdd.c_str();
  if (client.connect(ipAdd.c_str(), port))
  {
    Serial.println("connected]");

    Serial.println("[Sending a request]");
    
    
    client.write(jsonIndetifyString("ToshibaAcRemote",WiFi.localIP().toString(),portServerTcp).c_str());
    //client.write('Test');
    Serial.println("[Response:]");
    while (client.connected() || client.available())
    {
      if (client.available())
      {
        String line = client.readStringUntil('\n');
        Serial.println(line);
        break;
      }
    }
    
    client.stop();
    Serial.println("\n[Disconnected]");
  }
  else
  {
    Serial.println("connection failed!]");
    client.stop();
  }

}
String Lan::jsonIndetifyString(String id,String ipAdd,int port){
  StaticJsonDocument<200> doc;
  doc["id"] = id;
  doc["ip"] = ipAdd;
  doc["tcpPort"] = port;
  
  String jsonString;
  serializeJson(doc,jsonString );
  
  return jsonString;
  
}
String Lan::jsonStatusString(String id,bool powerStatus,float tempEnv,float humEnv, float setTempAc, int compressorStatus){
StaticJsonDocument<200> doc;
  doc["id"] = id;
  doc["powerStatus"] = powerStatus;
  doc["tempEnv"] = serialized(String(tempEnv,3));
  doc["humEnv"] = serialized(String(humEnv,3));
  doc["setTempAc"] = serialized(String(setTempAc,3));
  doc["compressorStatus"] = compressorStatus;
  String jsonString;
  serializeJson(doc,jsonString );
  return jsonString;
}
void Lan::jsonDeserialise(String inputStringJson){
  

  }
