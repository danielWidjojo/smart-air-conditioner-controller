
#include <Arduino.h>
//import classes required
// for interupts and delay based on millis that is non blocking
#include "InterruptDelay.h"
// regarding aircond ir commands to control the air cond set to coolix for toshiba

#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRutils.h>
#include "Lan.h"
#include "AirCondControl.h"
const uint16_t kIrLed = 15;  // The ESP GPIO pin to use that controls the IR LED.
IRac ac(kIrLed);  // Create a A/C object using GPIO to sending messages with.

//bme libraries etc
// regarding bme sensor that detects pressure, temperature and humidity
#include "EnvSensorBme280.h"
//regarding rotary encoder to detect long and short clicks androtation direction as well
#include "RotaryEncoder.h"
// regarding displaying image to oled display
#include "OledDisplay.h"



//---- Global variable declaration section
  
//variable to check if rotary encoder is turned clockwise or anticlockwise
//clockwise increase the number
int currentCounterLoop=0;
int previousCounterLoop=0;
// temp set of the room
float tempSet=23;
//different cooling state with differing values for different fan speed and compressorstate
int coolingState=0;
//Interrupt time for displaying temp and humidity
int interTimeSetTemp=1000;
int interTimeTempHum=1000;
//set aircond status to be off in the beginning
boolean acPrevPowerStatus=false;
boolean acCurrentPowerStatus=false;
//Blynk previous set temp and power status to ascertain blynk settings has changed or not
//The current setting variable is declared in the header file containing a set of blynk functions
float tcpSetTemp;
float tcpTempEnv;
float tcpHumEnv;
int tcpCompresState;
bool tcpPowerStatus;

//----- Object declaration section

//Rotary encoder to determine the state of the rotary encoder
RotaryEncoder rotar1;
//Oled onject to display settings and current temperature on the oled screen
OledDisplay oled1;
//interrupt for interrupting
//interrupting allows certain things to remain on display
//for displaying set temp
InterruptDelay tmr1;
//interrupt to function as a non-blocking delay
InterruptDelay tmr2;
//interrupt for interrupting
//interrupting allows certain things to remain on display
//for displaying temp and hum
InterruptDelay tmr3;
//Interrupt for blynk connectivity
InterruptDelay tmr4;
//create object to get temp and hum from bme sensor
EnvSensorBme280 sens1;
//create object to control ac
AirCondControl control1(&ac);
//remote set temp
float tempRemote = 25.0;
Lan lan;
void setup() {
  // start serial comm for diagnostic
  Serial.begin(115200);

  //start the necessary port assignment of the classes
  sens1.starts();
  rotar1.starts();
  oled1.starts();
  lan.starts();
  //start blynk iot communication
  //blynkStarts();
  //sync blynk setting regarding ac Power and set temperature
  //blynkSetPowerStatus(acCurrentPowerStatus);
  //blynkSetTempSlider(tempSet);

}

void loop() {
  //check if power is supposed to be on or off
  
  if (acCurrentPowerStatus){
    //if power of the aircond is on proceed to check if the rotary encoder has been rotated
    //getting the current counter value of rotary encoder
    //clockwise resulted in an increase in counter value
    //anticlockwire resulted in a decrease in counter value
    currentCounterLoop=rotar1.rotaryCounter();
    
    //check if rotary encoder is rotated clockwise or counterclockwise
    if (currentCounterLoop>previousCounterLoop){
      //rotary encoder is rotated clockwise
      Serial.println("Increasing");
      //increment value of set temperature
      tempSet-=0.1;
      //reset timer to start displaying set temp (by interrupt)
      tmr1.resetDelay();
      //reset timer to start displaying environment temp and hum (by interrupt)
      tmr3.resetDelay();
    }else if(currentCounterLoop<previousCounterLoop){
      //rotary encoder is rotated anti clockwise
      Serial.println("Decreasing");
      //decrement the value of set temperature
      tempSet+=0.1;
      //reset timer to start displaying set temp (by interrupt)
      tmr1.resetDelay();
      //reset timer to start displaying environment temp and hum (by interrupt)
      tmr3.resetDelay();
    }
    //Set the previous number of counter of rotary to be equal to the current number of rotary
    previousCounterLoop=currentCounterLoop;
    //check if interrupt is set
    //If so start displaying set temp for the specified time
    //Serial.println(millis());
    if (tmr1.millisInterrupt(interTimeSetTemp)){
      //set delay using millis to reduce cpu usage for displaying set temp
      if (tmr2.millisDelay(100)){
        //display the set temmperature
        oled1.displaySetTemp(tempSet, tempRemote);
        
       }
      
    }else{
      //set delay using milling to reduce freq of checking the temperatue sensor
      
      if (tmr2.millisDelay(1500)){
        //variable for env temperature and humidity
        //get temperature from sensor
        float currentTempEnv=sens1.getTempEnv();
        tcpTempEnv=currentTempEnv;
        Serial.println(tcpTempEnv);
        //get humidity reading from sensor
        float currentHumEnv=sens1.getHumEnv();
        tcpHumEnv=currentHumEnv;
        //Check if temp set is different from the set temp in blynk
        /*
        if(blynkSetTemp!=tempSet){
          //If the blynk set temp is different
          //Update the set temp in blynk
          Serial.println("Updating slider");
          blynkSetTempSlider(tempSet);
          //change the blynksettemp variable to the set temp
          blynkSetTemp=tempSet;
         }
         */
        
        //set an interrupt to display temp and hum for the specified time
        if (tmr3.millisInterrupt(interTimeTempHum)){
          //display temp and hum of the environment
          oled1.displayTempHum(currentTempEnv,currentHumEnv);
          
        }else{
          //if the time to display has been exceeded
          //set the display to be black
          oled1.displaySetBlank();
        }
        //check the set temperature with the current env temp
        //ir command will be sent to maintain temp
        //further adjustment in the AirCondControl class
        //get the cooling state for checking status
        coolingState =control1.determineCompressorStateAc(tempSet,currentTempEnv,&tempRemote);
        tcpCompresState=coolingState;
        
        //Use a delay to reduce freq of updating to the internet
        /*
        
        if (tmr4.millisDelay(3000)){
          //Update the current temperature and hummidity
          blynkSetTempHum(currentTempEnv,currentHumEnv);
        }
        */
        //if control status changes then update the setting using ir
        if(control1.statusChange(coolingState)){
          control1.sendIrSignal();
        }
       }
      
    }
    
    

  }else{
    //Power of airCond is supposed to be off
    //Set a delay using millis to reduce cpu usage
    if (tmr2.millisDelay(1000)){
        //set an interrupt to start displaying env temp and hum for the specified length of time
        //trigger is set when button is clicked
        if (tmr3.millisInterrupt(interTimeTempHum)){
          //get temp and hum data from sensor
          float currentTempEnv=sens1.getTempEnv();
          float currentHumEnv=sens1.getHumEnv();
          tcpTempEnv=currentTempEnv;
          
          tcpHumEnv=currentHumEnv;
          //display the env temp and hum
          oled1.displayTempHum(currentTempEnv,currentHumEnv);
        }else{
          //if time to display the temp and hum has been exceeded
          //Set the screen to be black
          oled1.displaySetBlank();
        }
        
       }

  }

  //Code to check for button click or rotary encoder
  //Check rotary class if button is clicked
  //A value 0 will be returned when the button is not clicked
  //A value 1 will be returned when the button is clicked 
  //A value 2 will be returned when the button is clicked for a long period
 
  
  
  switch (rotar1.buttonPressed()){

    //case 0 when button is not clicked 
    case 0:
    
    
  
    if (tmr4.millisDelay(300)){
      String ipAddUdp="";
      String msgUdp="";
      
      
 
      lan.udpServer(&ipAddUdp,&msgUdp);
      lan.tcpServer(& acCurrentPowerStatus,&tempSet,tcpHumEnv,tcpTempEnv,tcpCompresState);
      if(ipAddUdp!="")
      {
        Serial.print(ipAddUdp);
        Serial.print(msgUdp);
        lan.tcpClient(ipAddUdp,9999);
    //lan1.jsonDeserialise();
      }
        //display the set temmperature
        
         //if(blynkPrevSetTemp!=blynkSetTemp){
          //tempSet=tcpSetTemp;
         //}
        // blynkPrevSetTemp=blynkSetTemp;
        
         //if(blynkPrevPowerStatus!=blynkPowerStatus){
          //acCurrentPowerStatus=blynkPowerStatus;
         //}
        // blynkPrevPowerStatus=blynkPowerStatus;
         
       
       
    //checking if aircond status has changed from previously
    //prevent unnecessary command from being sent multiple times
    if(acPrevPowerStatus !=acCurrentPowerStatus){
      //if ac power status changes the current setting will need to be sent
      //true for on and false for off
      if(acCurrentPowerStatus){
        //ac is supposed to be on
        //send a status that ac is supposed to be on using ir
        control1.turnOnAc();
                //variable for env temperature and humidity
        //get temperature from sensor
        float currentTempEnv=sens1.getTempEnv();

        //check the set temperature with the current env temp
        //ir command will be sent to maintain temp
        //further adjustment in the AirCondControl class
        
        coolingState=control1.determineCompressorStateAc(tempSet,currentTempEnv, &tempRemote);
        control1.statusChange(coolingState);
        
        control1.sendIrSignal();
        
        
      }else{
          //ac is supposed to be off
          //send signal to turn of ac
          interTimeTempHum=0;
          control1.turnOffAc();
          control1.sendIrSignal();
          //possible that display is still displaying temp after ac is off
          //maybe confusing
          //maybe a method to set all timer to stop displaying image (future improment)
          
      }
      //set PrevPowerStatus to be the same as current power status
      acPrevPowerStatus =acCurrentPowerStatus;
    }
    }
    
     

    break;

    case 1:
    //button is clicked for a short time
    Serial.println("Display env temp and hum and temp setting");
    //reset timer of set temperature interrupt to start displaying set temperature
    //for specified period
    //only start displaying if the ac is supposed to be on
    if(acCurrentPowerStatus==false){
      interTimeTempHum=2500;
    }
    tmr1.resetDelay();
    //reset timer of set temperature interrupt to start displaying env temp and hum
    //for specified period
    //different period is set depending whether the ac is on or off
    interTimeTempHum=100000;
    
    tmr3.resetDelay();
    break;

    case 2:
    //button is clicked for a long period of time
    //the ac status will be switched from on to off or vice versa
    Serial.println("Ac will be switched On or off");
    //flipping the current status from on to off or vice versa
    acCurrentPowerStatus=!acCurrentPowerStatus;
    //checking if aircond status has changed from previously
    //prevent unnecessary command from being sent multiple times
    if(acPrevPowerStatus !=acCurrentPowerStatus){
      //blynkSetPowerStatus(acCurrentPowerStatus);
      //if ac power status changes the current setting will need to be sent
      //true for on and false for off
      if(acCurrentPowerStatus){
        //ac is supposed to be on
        //send a status that ac is supposed to be on using ir
        control1.turnOnAc();
                //variable for env temperature and humidity
        //get temperature from sensor
        float currentTempEnv=sens1.getTempEnv();

        //check the set temperature with the current env temp
        //ir command will be sent to maintain temp
        //further adjustment in the AirCondControl class
        
        coolingState=control1.determineCompressorStateAc(tempSet,currentTempEnv,&tempRemote);
        control1.statusChange(coolingState);
        
        control1.sendIrSignal();
        
        
        //reset timer1 to display set temperature
        interTimeSetTemp=2500;
        tmr1.resetDelay();
        //reset timer3 to display env temp and hum
        interTimeTempHum=100000;
        tmr3.resetDelay();
      }else{
          //ac is supposed to be off
          //send signal to turn of ac
          interTimeTempHum=0;
          control1.turnOffAc();
          control1.sendIrSignal();
          //possible that display is still displaying temp after ac is off
          //maybe confusing
          //maybe a method to set all timer to stop displaying image (future improment)
          
      }
      //set PrevPowerStatus to be the same as current power status
      acPrevPowerStatus =acCurrentPowerStatus;
    }
      break;
    }


  

}
