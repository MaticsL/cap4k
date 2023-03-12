#include "capbutton.hpp"
#include "Adafruit_NeoPixel.h"
#include "Keyboard.h"

Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, 3, NEO_GRB + NEO_KHZ800);
char keymap[] = {'d', 'f', 'j', 'k'};
bool lastKeyStatus[4] = {false, false, false, false};
CapButton key[4] = {
  CapButton(A0, 700, 710, true, 630, 710),
  CapButton(A6, 500, 510, true, 358, 510),
  CapButton(A7, 500, 510, true, 377, 510),
  CapButton(A9, 500, 510, true, 380, 510)};

void setup() {
  //Serial.begin(115200);
  //while (!Serial);
  strip.begin();
  Keyboard.begin();
}

void loop() {
  for(uint8_t i=0;i<4;i++){
    key[i].measureOnce();
    if(key[i].isKeyDown){
      strip.setPixelColor(i, strip.Color(220, 220, 0));
      if(!lastKeyStatus[i])Keyboard.press(keymap[i]);  
    }
    else{
      strip.setPixelColor(i, strip.Color(0, 0, 0));
      if(lastKeyStatus[i])Keyboard.release(keymap[i]);  
    }
    lastKeyStatus[i] = key[i].isKeyDown;
  }
  strip.show();
}