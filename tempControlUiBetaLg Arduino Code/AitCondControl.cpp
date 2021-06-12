

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRutils.h>
#include "AirCondControl.h"

AirCondControl::AirCondControl(IRac *ac3){
    ac2=ac3;
   
      
     ac2->next.protocol = decode_type_t::LG2 ;  // Set a protocol to use.
     ac2->next.model = 1;  // Some A/Cs have different models. Try just the first.
     ac2->next.mode = stdAc::opmode_t::kCool;  // Run in cool mode initially.
     ac2->next.celsius = true;  // Use Celsius for temp units. False = Fahrenheit
     ac2->next.degrees = 30;  // 30 degrees.
     ac2->next.fanspeed = stdAc::fanspeed_t::kMin;  // Start the fan at low.
     ac2->next.swingv = stdAc::swingv_t::kOff;  // Don't swing the fan up or down.
     ac2->next.swingh = stdAc::swingh_t::kOff;  // Don't swing the fan left or right.
     ac2->next.light = false;  // Turn off any LED/Lights/Display that we can.
     ac2->next.beep = false;  // Turn off any beep from the A/C if we can.
     ac2->next.econo = false;  // Turn off any economy modes if we can.
     ac2->next.filter = false;  // Turn off any Ion/Mold/Health filters if we can.
     ac2->next.turbo = false;  // Don't use any turbo/powerful/etc modes.
     ac2->next.quiet = false;  // Don't use any quiet/silent/etc modes.
     ac2->next.sleep = -1;  // Don't set any sleep time or modes.
     ac2->next.clean = false;  // Turn off any Cleaning options if we can.
     ac2->next.clock = -1;  // Don't set any current time if we can avoid it.
     ac2->next.power = false;  // Initially start with the unit off.
     airCondStatus = true;

}
  void AirCondControl::sendIrSignal()
  { 
    ac2->sendAc();  // Have the IRac class create and send a message.
    Serial.println("Sending IR signal");
  }
  void AirCondControl::setFanSpeedAc(int speeds=1)
  {
    switch(speeds){
      case 0:
        ac2->next.fanspeed = stdAc::fanspeed_t::kMin;  
        Serial.println("Sending a message to set fan speed Min");
        break;
      case 1:
        ac2->next.fanspeed = stdAc::fanspeed_t::kMedium;  
        Serial.println("Sending a message to set fan speed Low");
        break;
      case 2:
        ac2->next.fanspeed = stdAc::fanspeed_t::kHigh; 
        Serial.println("Sending a message to set fan speed Medium");
        break;     
      case 3:
        ac2->next.fanspeed = stdAc::fanspeed_t::kMax;  
        Serial.println("Sending a message to set fan speed High");
        break;
    }

    

  }
  void AirCondControl::turnOnAc()
  {
    ac2->next.power = true;  
    Serial.println("Sending a message to turn ON the A/C unit.");
    
    
  }
  boolean AirCondControl::statusChange(int statusCurrent)
  {
    bool status1;
    
    if(statusCurrent==prevStatus){
      status1=false;
    }else{
      status1=true;
    }
    //Serial.println("Status changes "+(String) status1);
    prevStatus=statusCurrent;
    return status1;
  }
  void AirCondControl::turnOffAc()
  {
    ac2->next.power = false;  // Initially start with the unit off.
    Serial.println("Sending a message to turn OFF the A/C unit.");
    
  }
  void AirCondControl::setTempAc(float temp=25)
  {
    m_setAcTemp=temp;
    ac2->next.degrees = m_setAcTemp;  // 25 degrees.
    String setTempString=(String)m_setAcTemp;
    
    Serial.println("Sending a message to set Temp to "+(String)m_setAcTemp);
  }
  void AirCondControl::compressorOffAc(float temp=25)
  {
    //setTempAc(30.0);
    setTempAc(temp);
    
    Serial.println("Sending a message to set Temp to turn off compressor");

  }
  void AirCondControl::compressorOnAc(float temp=25)
  {
    setTempAc(temp );
    
    Serial.println("Sending a message to set Temp to turn on compressor");
  }
  int AirCondControl::determineCompressorStateAc(float setTempAc, float currentTempEnv, float *tempRemot)
  {
    float m_setTempAc = setTempAc;
    float m_currentTempEnv = currentTempEnv;
    float tempDiff = m_currentTempEnv-m_setTempAc;
    Serial.println("Current Temp "+(String)m_currentTempEnv+" Settemp "+(String)setTempAc);
    

    
    if (tempDiff>=turnOnTemp){

      
      
          
        if(tempDiff<=0.6 && statusCompressor!=2){
          setFanSpeedAc(0);
          statusCompressor =2;
        }else if(tempDiff<=0.8 && statusCompressor==2){
          setFanSpeedAc(0);
          statusCompressor =2;
        }else if(tempDiff<=1.2 && statusCompressor!=1){
          setFanSpeedAc(1);
          m_setTempAc -= 1;
          statusCompressor =1;
        }else if(tempDiff<=1.6 && statusCompressor==1){
          setFanSpeedAc(1);
          m_setTempAc -= 1;
          statusCompressor =1;
        }else if(tempDiff<=2.5 && statusCompressor!=3){
          setFanSpeedAc(3);
          m_setTempAc -= 2;
          statusCompressor =3;
        }else if(tempDiff<=3 && statusCompressor==3){
          setFanSpeedAc(3);
          m_setTempAc -= 2;
          statusCompressor =3;
        }else {
          
          setFanSpeedAc(3);
          m_setTempAc -= 3;
          statusCompressor =4;
        }
     m_setTempAc = round(m_setTempAc);
     compressorOnAc(m_setTempAc);
        
     Serial.println("Cooling setting compressor state "+(String)statusCompressor  );
      }else if(tempDiff<=turnOffTemp){
       // trying to increase temp
       setFanSpeedAc(0);
       
        //compressorOffAc(m_setTempAc);
        if(tempDiff>=-0.8 && statusCompressor!=0){
          m_setTempAc += 1;
          statusCompressor =0;
        }else if(tempDiff>=-1.1 && statusCompressor==0){
          m_setTempAc += 1;

          statusCompressor =0;
        }else if(tempDiff>=-1.4 && statusCompressor!=-1){
          //compressorOffAc(m_setTempAc+1);
          m_setTempAc += 2;
          statusCompressor =-1;
         }else if(tempDiff>=-1.7 && statusCompressor==-1){
          m_setTempAc += 2;
          
          statusCompressor =-1;
          }else{
          m_setTempAc += 3;
          statusCompressor =-2;
        }
      m_setTempAc = round(m_setTempAc);
      compressorOffAc(m_setTempAc);
        
      Serial.println("Stop cooling setting compressor state "+(String)statusCompressor  );
    }else{
     setFanSpeedAc(0);
     if(statusCompressor!=2 && statusCompressor!=0){
      if(statusCompressor>0){
        statusCompressor=2;
      }else{
        statusCompressor=0;
      }
     }
      Serial.println("Maintaining current setting compressor state "+(String)statusCompressor  );
    }
    // set the remot temp into the variable to be retrieved
    *tempRemot = m_setTempAc;
    return statusCompressor;
    
  }
    
  
