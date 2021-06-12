
#include <Arduino.h>
#include "InterruptDelay.h"
#include <Wire.h>


boolean InterruptDelay::millisDelay(unsigned long delays=1000)
{
   boolean returnVal;
        if ((unsigned long)(millis()-timeLastUpdated)>delays){
          returnVal=true;
          timeLastUpdated=millis();
        }else{
          returnVal=false;
        }
       return returnVal;
}
boolean InterruptDelay::millisInterrupt(unsigned long delays=1000)
{
   boolean returnVal;
        if ((unsigned long)(millis()-timeLastUpdated)<delays){
          returnVal=true;
         
        }else{
          returnVal=false;
        }
       return returnVal;
}
boolean InterruptDelay::millisBlock(unsigned long delays=1000)
{
   boolean returnVal;
        if ((unsigned long)(millis()-timeLastUpdated)<delays){
          returnVal=true;
         
        }else{
          returnVal=false;
        }
       return returnVal;
}
void  InterruptDelay::resetDelay()
{ 
    timeLastUpdated=millis();
    Serial.println("Reset");
}
