//
// Gerstner full test 1
// Simple waves on a grid

// ESP32 fast sin/cos notes:
// int16_t sin16/cos16(uint16:t theta)
//   theta 0..65535, returns val -32767..32767

#define __MAIN__
#include <Streaming.h>
#include <FastLED.h>
#include "gridlib.h"
#include "gerstner.h"
//#include "gerst-test-2.h"
#include "gamma.h"

#define GRIDCOLS 16
#define GRIDROWS 16

Grid grid(GRIDCOLS, GRIDROWS);
GridHeight accum;

GridWorld gerstWorld(GRIDCOLS, GRIDROWS);
GerstWave gerst;
GerstWave gerst2;

#define WCS_LLX 0
#define WCS_LLY 0
#define WCS_URX (65535*2)
#define WCS_URY (65535*2)

#define H_LOW ((161*256)/360)
#define S_LOW 255
// V_LOW was ((30*256)/100)
#define V_LOW ((40*256)/100)
#define H_HIGH H_LOW
//#define S_HIGH ((55*256)/100)
#define S_HIGH 186
#define V_HIGH ((63*256)/100)

#define PI 3.1415926
int32_t angleMap(float angle) {
  // maps angle (0 2pi) to (0 65535)
  return (int32_t)((angle*65535)/(2*PI));
}

void setup() {
  Serial.begin(115200);
  delay(200); // allow Serial to come online
  Serial << "Gerstner full test 2 starting\n";

  // Initialize the display grid
  gerstWorld.setWcs(WCS_LLX, WCS_LLY, WCS_URX, WCS_URY);
  // Initialize the accumulator grid
  accum.init(GRIDCOLS, GRIDROWS);

  // First test is a single wave
  gerst.init(&gerstWorld, &accum);
  gerst.start(0, 32767, 65536, 10000, angleMap(PI/6));

  gerst2.init(&gerstWorld, &accum);
  gerst2.start(0, 16384, 16384, 20000, angleMap(PI/2));
//            duration maxAmpl wavelength velocity angle
  
  FastLED.addLeds<WS2811, 25, GRB>(grid.theLeds(), GRIDROWS*GRIDCOLS);

}

#define DO_TEST_1 false
#define DO_TEST_2 false
#define DO_TEST_3 false
#define DO_TEST_4 false
#define DO_TEST_5 true
void loop() {
  // testing actual gerstwave
  static bool hasRun = false;
  if(!hasRun) {
    
    accum.clear();
    //gerst.calc();
    gerst2.calc();
    for(int r=0; r<GRIDROWS; r++) {
      for(int c=0; c<GRIDCOLS; c++) {
        CHSV hsv;
        gridwcs_t height;
        //CRGB16 val(accum.get(c,r));
        height = accum.get(r,c);
        if(height>32767) height=32767;
        if(height<-32767) height=-32767;
        hsv = CHSV(H_LOW,map(accum.get(c,r), -32767, 32767, S_LOW,S_HIGH), gamma8(map(accum.get(c,r), -32767, 32767, V_LOW,V_HIGH)));
        grid.setPixel(c,r, CRGB(hsv));
        //grid.setPixel(c,r,val.r, val.g, val.b);
      }
    }
    FastLED.show();
    delay(100);
    
    //hasRun = true;
  }
}
