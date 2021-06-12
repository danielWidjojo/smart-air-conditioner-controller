#ifndef ENVSENSORBME280_H // include guard
#define ENVSENSORBME280_H
class EnvSensorBme280{
  float temp=23;
  float hum=50;
  float pres=0;
  float alti=0;
  public:
  void starts();
  float getTempEnv();
  float getHumEnv();
  float getPresEnv();
  float getAltiEnv();
  void printValues();
};
#endif /* MY_CLASS_H */
