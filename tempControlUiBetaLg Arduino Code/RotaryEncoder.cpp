#include <Arduino.h>
#include "RotaryEncoder.h"

// Rotary Encoder Inputs
#define CLK 14
#define DT 13
#define SW 12

void RotaryEncoder::starts()
{
    // Set encoder pins as inputs
  pinMode(CLK,INPUT);
  pinMode(DT,INPUT);
  pinMode(SW, INPUT_PULLUP);
    // Read the initial state of CLK
  lastStateCLK = digitalRead(CLK);
}
int RotaryEncoder::rotaryCounter()
{
   // Read the current state of CLK
    currentStateCLK = digitalRead(CLK);
  
    // If last and current state of CLK are different, then pulse occurred
    // React to only 1 state change to avoid double count
    if (currentStateCLK != lastStateCLK  ){
  
      // If the DT state is different than the CLK state then
      // the encoder is rotating CCW so decrement
      if (digitalRead(DT) != currentStateCLK) {
        counter --;
        currentDir ="CCW";
      } else {
        // Encoder is rotating CW so increment
        counter ++;
        currentDir ="CW";
      }
  
      Serial.print("Direction: ");
      Serial.print(currentDir);
      Serial.print(" | Counter: ");
      Serial.println(counter);
    }
  
    // Remember last CLK state
    lastStateCLK = currentStateCLK;
  
    
  
    return counter;
}
int RotaryEncoder::buttonPressed()
{
    // Read the button state
    int btnState = digitalRead(SW);
    int btnStateInt=0;
    //If we detect LOW signal, button is pressed
    if (btnState == LOW ) {
      
     if (prevBtnState!=btnState){
      startButtonPress = millis();
       Serial.println("Change");
       prevLow=true;
     }
      //if 50ms have passed since last LOW pulse, it means that the
      //button has been pressed, released and pressed again
     
      //Serial.println("0");
        
      lastButtonPress = millis();
      
      prevBtnState=btnState;
      
      btnStateInt=0;
    }else{
       if (prevLow){
      btnStateInt=0;
        Serial.println("Change2");
         if(lastButtonPress-startButtonPress>=50 && !(lastButtonPress-startButtonPress>=250 )){
           Serial.println("Short");
           btnStateInt=1;
          
        } else if(lastButtonPress-startButtonPress>=250){
          Serial.println("Long");
          btnStateInt=2;
        }
       }
       prevLow=false;
       
       
    prevBtnState=btnState;
    
  }
  return btnStateInt;
  
}
