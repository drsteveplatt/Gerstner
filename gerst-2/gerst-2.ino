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
Grid16 accum;

GridWorld gerstWorld(GRIDCOLS, GRIDROWS);
GerstWave gerst;

#define WCS_LLX 0
#define WCS_LLY 0
#define WCS_URX (65535*2)
#define WCS_URY (65535*2)

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
    gerst.calc();
    for(int r=0; r<GRIDROWS; r++) {
      for(int c=0; c<GRIDCOLS; c++) {
        CRGB16 val(accum.get(c,r));
        grid.setPixel(c,r,val.r, val.g, val.b);
      }
    }
    FastLED.show();
    delay(100);
    
    //hasRun = true;
  }
}
