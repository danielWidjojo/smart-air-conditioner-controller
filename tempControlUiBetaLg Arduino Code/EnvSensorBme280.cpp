//bme libraries etc
#include <Arduino.h>
#include <string>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "EnvSensorBme280.h"

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

void EnvSensorBme280::starts()
{
 unsigned status;
    // default settings
    status = bme.begin(0x76);
    // You can also pass in a Wire library object like &Wire2
    // status = bme.begin(0x76, &Wire2)
    if (!status) {
      Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
      Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(), 16);
      Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
      Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
      Serial.print("        ID of 0x60 represents a BME 280.\n");
      Serial.print("        ID of 0x61 represents a BME 680.\n");
      while (1) delay(10);
    }
}
  float EnvSensorBme280::getTempEnv()
{
  
    float tempBme=bme.readTemperature();
    //Check if sensor reading is a number or not
    //If not, send previous reading
    if(!isnan(tempBme)){
      //Adjustment to make sensor more accurate
      //Sensor higher by 0.5 to 1 c compared to dht22
      //adjusting by decreasing temp slightly as seen below
      temp=tempBme-2;
    }

    return temp;
}
  float EnvSensorBme280::getHumEnv()
{
    float humBme=bme.readHumidity();
    if(!isnan(humBme)){
    hum=humBme;
    }

    return hum;
}
  float EnvSensorBme280::getPresEnv()
{
    float presBme=bme.readPressure() / 100.0F;;
    if(!isnan(presBme)){
      pres=presBme;
    }

    return pres;
}
  float EnvSensorBme280::getAltiEnv()
{
    float altiBme= bme.readAltitude(SEALEVELPRESSURE_HPA);
    if(!isnan(altiBme)){
      alti=altiBme;
    }
 
    return alti;
}
void EnvSensorBme280::printValues()
{
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");
  
    Serial.print("Pressure = ");
  
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");
  
    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");
  
    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");
  
    Serial.println();
}
