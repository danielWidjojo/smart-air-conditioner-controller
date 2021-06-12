#ifndef OLEDDISPLAY_H // include guard
#define OLEDDISPLAY_H
class OledDisplay{

    public:

    void starts();

    void displaySetBlank();

    void displaySetTemp(float temp, float tempRemot);

    void displayTempHum(float temp, float hum);
    };  
#endif 
