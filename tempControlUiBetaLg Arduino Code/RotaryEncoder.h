// my_class.h
#ifndef ROTARYENCODER_H // include guard
#define ROTARYENCODER_H
typedef bool Boolean;
class RotaryEncoder{
    
  int counter = 0;
  int currentStateCLK;
  int lastStateCLK;
  String currentDir ="";
  unsigned long lastButtonPress = 0;
  unsigned long startButtonPress = 0;
  int counterBtn=0;
  int prevBtnState=HIGH;
   int prevLow=false;

  public:
    void starts();
    int rotaryCounter();
    int buttonPressed(); 
  
  };
#endif 