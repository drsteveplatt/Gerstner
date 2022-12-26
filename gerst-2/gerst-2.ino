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
GerstWave gerst3;
GerstWave gerst4;

// LL/UL XY define the display space
#define WCS_LLX 0
#define WCS_LLY 0
#define WCS_URX (999)
#define WCS_URY (999)

// ZMIN/ZMAX define the range for chromatic interpolation
#define WCS_ZMIN   -100
#define WCS_ZMAX   50

// HSV value  for wave heights below zero.  Interpolated [LOW ZERO] for height=[ZMIN 0]
// and [ZERO HIGH] for height=(0, ZMAX]
#define H_LOW ((161*256)/360)
#define S_LOW 255
// V_LOW was ((30*256)/100)
#define V_LOW ((20*256)/100)

#define H_ZERO H_LOW
#define S_ZERO 255
#define V_ZERO ((35*256)/100)

#define H_HIGH H_LOW
//#define S_HIGH ((55*256)/100)
#define S_HIGH 220
#define V_HIGH ((53*256)/100)

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
//            duration maxAmpl wavelength velocity angle
  gerst.init(&gerstWorld, &accum);
//            duration maxAmpl wavelength velocity angle
    gerst.start(5000, 900, 1500, 100, angleMap(PI/6));
    gerst.setRangeDuration(5000, 15000);
    gerst.setRangeAmplitude(800, 1000);
    gerst.setRangeWavelength(800, 1500);
    gerst.setRangeVelocity(-200, 200);
    gerst.setRangeAngle(0, angleMap(PI/4));

  gerst2.init(&gerstWorld, &accum);
//            duration maxAmpl wavelength velocity angle
    gerst2.start(3000, 200, 250, 200, angleMap(PI/2));
    gerst2.setRangeDuration(5000, 15000);
    gerst2.setRangeAmplitude(200, 300);
    gerst2.setRangeWavelength(250, 350);
    gerst2.setRangeVelocity(-200, 200);
    gerst2.setRangeAngle(0, angleMap(PI/2));
    
  gerst3.init(&gerstWorld, &accum);
//            duration maxAmpl wavelength velocity angle
    gerst3.start(5000, 200, 150, -100, angleMap(PI/3));
    gerst3.setRangeDuration(5000, 15000);
    gerst3.setRangeAmplitude(200, 300);
    gerst3.setRangeWavelength(150, 250);
    gerst3.setRangeVelocity(-200, 200);
    gerst3.setRangeAngle(0, angleMap(PI/2));

  gerst4.init(&gerstWorld, &accum);
//            duration maxAmpl wavelength velocity angle
    gerst4.start(5000, 200, 150, -100, angleMap(2*PI/3));
    gerst4.setRangeDuration(5000, 15000);
    gerst4.setRangeAmplitude(200, 300);
    gerst4.setRangeWavelength(150, 250);
    gerst4.setRangeVelocity(-200, 200);
    gerst4.setRangeAngle(angleMap(PI/3), angleMap(2*PI/3));
    
  FastLED.addLeds<WS2811, 25, GRB>(grid.theLeds(), GRIDROWS*GRIDCOLS);

}

void loop() {
  // testing actual gerstwave
  static bool hasRun = false;
  static uint32_t frameCount = 0;
  static uint32_t frameStartTime = 0;
  static const uint32_t maxFrameCount = 100;
  if(!hasRun) {

    if(frameCount>maxFrameCount && millis()>frameStartTime) {
      Serial << "Perf: " << (frameCount*1000)/(millis()-frameStartTime) << " frames/sec\n";
      frameStartTime = millis();
      frameCount = 0;
    } else frameCount++;
    accum.clear();
    gerst.calc();
    gerst2.calc();
    gerst3.calc();
    gerst4.calc();
    for(int r=0; r<GRIDROWS; r++) {
      for(int c=0; c<GRIDCOLS; c++) {
        CHSV hsv;
        CRGB rgb;
        gridwcs_t height;
        height = accum.get(c,r);
        if(height<=0) {
          hsv = CHSV(H_LOW,map(height, -GW_MAX_AMPLITUDE, 0, S_LOW,S_ZERO), gamma8(map(height, GW_MAX_AMPLITUDE, 0, V_LOW,V_ZERO)));
        } else {
          hsv = CHSV(H_LOW,map(height, 0, GW_MAX_AMPLITUDE, S_ZERO,S_HIGH), gamma8(map(height, 0, GW_MAX_AMPLITUDE, V_ZERO,V_HIGH)));          
        }
        rgb = CRGB(hsv);
        grid.setPixel(c,r, rgb);
      }
    }
    FastLED.show();
    
    //hasRun = true;
  }
}
