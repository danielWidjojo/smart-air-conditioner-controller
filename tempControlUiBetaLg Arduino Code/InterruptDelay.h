// my_class.h
#ifndef INTERRUPTDELAY_H // include guard
#define INTERRUPTDELAY_H
typedef bool Boolean;
class InterruptDelay{
    
    unsigned long prevDisplayedTime;
    unsigned long updateTimePeriod=1000;
    unsigned long timeLastUpdated;
    unsigned long interuptTime;
    public:
    boolean millisDelay(unsigned long delays);
     boolean millisBlock(unsigned long delays);
    boolean millisInterrupt(unsigned long delays);
    void resetDelay();
    
  };
#endif /* MY_CLASS_H */
