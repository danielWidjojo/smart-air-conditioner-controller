#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include "OledDisplay.h"
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


void OledDisplay::starts()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
}

void OledDisplay::displaySetBlank()
{
     display.clearDisplay();
     display.display();
}

void OledDisplay::displaySetTemp(float temp=25, float tempRemot=25)
{
      //temp remote is the remote set temperature
      float t = temp;
      float tRemote = tempRemot;
      //clear display
      display.clearDisplay();
    
      // display temperature
      display.setTextSize(1);
      //display.setFont(ArialMT_Plain_10);
      //display.setFont(&FreeSerif9pt7b);
      display.setCursor(0, 0);
      display.print("Set & Remote Temp: ");
      display.setTextSize(2);
      //display.setFont(ArialMT_Plain_16);
    
      //display.setFont(&FreeSerif9pt7b);
     display.setCursor(0, 15);
      display.print(t,1);
      display.print(" ");
      display.setTextSize(1);
      display.cp437(true);
      display.setTextSize(1);
      display.setCursor(51, 15);
      display.write(167); 
      display.setTextSize(1);
      display.setCursor(58, 20);
      display.print("C");

      // display remote temp
      display.setTextSize(1);
      display.setCursor(0, 0);

      display.setTextSize(2);
      display.setCursor(87, 15);
      display.print(tRemote,0);
      
      display.setTextSize(1);
      display.setCursor(113     , 15);
      display.write(167); 
      display.setTextSize(1);
      display.setCursor(120, 20);
      display.print("C");
      display.display();
}

void OledDisplay::displayTempHum(float temp=25, float hum=40)
{
        
      float t = temp;
      float h = hum;
      
      //clear display
      display.clearDisplay();
    
      // display temperature
      display.setTextSize(1);
      //display.setFont(ArialMT_Plain_10);
      //display.setFont(&FreeSerif9pt7b);
      display.setCursor(0, 0);
      display.print("Temp & Humidity: ");
      display.setTextSize(2);
      //display.setFont(ArialMT_Plain_16);
    
      //display.setFont(&FreeSerif9pt7b);
     display.setCursor(0, 15);
      display.print(t,1);
      display.print(" ");
      display.setTextSize(1);
      display.cp437(true);
      display.setTextSize(1);
      display.setCursor(51, 15);
      display.write(167);
      display.setTextSize(1);
      display.setCursor(58, 20);
      display.print("C");
    
      // display humidity
      display.setTextSize(1);
      display.setCursor(0, 0);
      //display.print("Humidity: ");
      display.setTextSize(2);
      display.setCursor(70, 15);
      display.print(h,1);
      display.setTextSize(1);
      display.setCursor(115, 20);
      display.print(" %");
    
      display.display();
      }
