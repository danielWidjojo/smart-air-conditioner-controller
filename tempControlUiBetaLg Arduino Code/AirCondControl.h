// my_class.h
#ifndef AIRCONDCONTROL_H // include guard
#define AIRCONDCONTROL_H

class AirCondControl
{
  private:
  IRac *ac2; 
  
  //vars required for operation
  bool airCondStatus ;
  int prevStatus;
  //bool airCondControl = true;
  float m_currentEnvTemp;
  float m_setAcTemp=30 ;
  int statusCompressor=1;

  // assuming only cooling function only
  float turnOnTemp=0.4 ;
  float turnOffTemp =-0.4;
  
  public:

  AirCondControl(IRac *ac3);
  void sendIrSignal();
  boolean statusChange(int statusCurrent);
  void setFanSpeedAc(int speeds);
  void turnOnAc();
  void turnOffAc();
  void setTempAc(float temp);
  void compressorOffAc(float temp);
  void compressorOnAc(float temp);
  int determineCompressorStateAc(float setTempAc, float currentTempEnv, float *tempRemot);
  

};

#endif /* MY_CLASS_H */
