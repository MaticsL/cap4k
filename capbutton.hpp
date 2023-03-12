#include<stdint.h>
#include "Adafruit_FreeTouch.h"

#define BufferSize 5

class CapButton{
    public:
    CapButton(uint8_t pin, uint16_t tiggerThreshold, uint16_t releaseThreshold, bool isBoostTrigger, uint16_t deathThresholdRelease, uint16_t deathThresholdDown);
    ~CapButton();
    bool isPushing();
    bool isReleasing();
    uint16_t measureOnce();
    bool isKeyDown;
    private:
    uint16_t pushBuffer();
    uint16_t cBuffer[BufferSize]={0};
    uint16_t getHistoryCap(uint8_t pos);
    uint16_t deathThresholdRelease, deathThresholdDown;
    uint16_t releaseThreshold, tiggerThreshold;
    Adafruit_FreeTouch qt;
    bool isBoostTrigger;
    uint8_t bufferPosition;
};

CapButton::CapButton(uint8_t pin, uint16_t TT, uint16_t RT, bool iBT, uint16_t DTR, uint16_t DTD): releaseThreshold(TT), tiggerThreshold(RT), isBoostTrigger(iBT), deathThresholdRelease(DTR), deathThresholdDown(DTD), isKeyDown(false), bufferPosition(0) {
  qt = Adafruit_FreeTouch(pin, OVERSAMPLE_2, RESISTOR_50K, FREQ_MODE_NONE);
  qt.begin();
  for(uint8_t i=0;i<BufferSize;i++)pushBuffer();
}

CapButton::~CapButton() {
}

uint16_t CapButton::pushBuffer() {
    uint16_t data =qt.measure();
    if(data>=1000)data=999;
    cBuffer[bufferPosition++] = data;
    if(bufferPosition>=BufferSize)bufferPosition=0;
    return data;
}

uint16_t CapButton::measureOnce() {
    uint16_t now=pushBuffer();
    uint16_t last=getHistoryCap(1);
    if(isBoostTrigger){
      if(isBoostTrigger&&isPushing()&&isKeyDown==false)isKeyDown=true;
      else if(isBoostTrigger&&isReleasing()&&isKeyDown==true)isKeyDown=false;
      if(now<deathThresholdRelease&&isKeyDown==true)isKeyDown=false;
      if(now>deathThresholdDown&&isKeyDown==false)isKeyDown=true;
    }
    else {
      if(now>tiggerThreshold&&last<=tiggerThreshold&&isKeyDown==false)isKeyDown=true;
      else if(now<=releaseThreshold&&last>releaseThreshold&&isKeyDown==true)isKeyDown=false;
    }
    return now;
}



uint16_t CapButton::getHistoryCap(uint8_t pos) {
    pos = pos % BufferSize + 1;
    return cBuffer[(bufferPosition+BufferSize-pos)%BufferSize];
}

bool CapButton::isPushing() {
    uint16_t raiseCount=0, fallCount=0;
    uint16_t nowCap=getHistoryCap(1);
    for(uint8_t i=2; i<BufferSize; i++){
        uint16_t lastCap=getHistoryCap(i);
        if(nowCap>lastCap)raiseCount++;
        else if(nowCap<lastCap)fallCount++;
        nowCap=lastCap;
    }
    return (raiseCount>BufferSize/2) && (fallCount < 2);
}

bool CapButton::isReleasing() {
    uint16_t raiseCount=0, fallCount=0;
    uint16_t nowCap=getHistoryCap(1);
    for(uint8_t i=2; i<BufferSize; i++){
        uint16_t lastCap=getHistoryCap(i);
        if(nowCap>lastCap)raiseCount++;
        else if(nowCap<lastCap)fallCount++;
        nowCap=lastCap;
    }
    return (fallCount>BufferSize/2) && (raiseCount < 2);
}