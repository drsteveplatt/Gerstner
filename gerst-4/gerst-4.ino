//
// Gerstner v. 4.0
// Simple waves on a grid
// New version uses cLEDMatrix, cLEDMatrixWcs, and cLEDMatrixData libraries

// Pin usage on ESP32: 
//  14: LED grid LL
//  25: LED grid LR
//  26: LED grid UR
//  27: LED grid UL

// ESP32 FastLED fast sin/cos notes:
// int16_t sin16/cos16(uint16:t theta)
//   theta 0..65535, returns val -32767..32767

#define __MAIN__
#include <Streaming.h>
#include <FastLED.h>
#include "gamma.h"

#include <LEDMatrix.h>
#include <LEDMatrixWcs.h>
#include <LEDMatrixData.h>

//#include "gridlib.h"
#include "world.h"
#include "gerstner.h"

#define PERF true

// LED tile information
// LED information
#define COLOR_ORDER         GRB
#define CHIPSET             WS2812B

// Hardware connection information
#define DATA_PIN_LL           14
#define DATA_PIN_LR           25
#define DATA_PIN_UR           26
#define DATA_PIN_UL           27

#if false
GridWorld gerstWorld(GRIDPIXELCOLS, GRIDPIXELROWS);
#endif
GerstWave gerst;
GerstWave gerst2;
GerstWave gerst3;
GerstWave gerst4;

// LL/UL XY define the display space
#define WCS_LLX (0)
#define WCS_LLY (0)
#define WCS_URX ((1000*MATRIX_TILE_COLS-1))
#define WCS_URY ((1000*MATRIX_TILE_ROWS-1))

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

#define PI (3.1415926f)

int32_t angleMap(float angle) {
  // maps angle (0 2pi) to (0 65535)
  return (int32_t)((angle*65535)/(2*PI));
}

int32_t steepnessMap(float steepness) {
  // maps steepness [0.0 1.0] to [0 65535]
  return (int32_t)(steepness*65535);
}

void setup() {
  Serial.begin(115200);
  delay(1000); // allow Serial to come online
  Serial << "Gerstner v 4 starting\n";
  Serial << "PanelPixel CR: " << MATRIX_TILE_WIDTH << ' ' << MATRIX_TILE_HEIGHT
    << " GridPixel CR: " << MATRIX_WIDTH << ' ' << MATRIX_SIZE
    << " Panel size: " << MATRIX_TILE_WIDTH*MATRIX_TILE_HEIGHT
    << " Grid size: " << MATRIX_SIZE
    << endl;

  //grid.dump();

  // set up the LED strips
#if false
  FastLED.addLeds<CHIPSET, DATA_PIN_LL, COLOR_ORDER>(&(theLeds[0]), MATRIX_SIZE);                         // LL
  FastLED.addLeds<CHIPSET, DATA_PIN_LR, COLOR_ORDER>(&(theLeds[MATRIX_TILE_SIZE]), MATRIX_SIZE);         // LR
  FastLED.addLeds<CHIPSET, DATA_PIN_UR, COLOR_ORDER>(&(theLeds[MATRIX_TILE_SIZE*2]), MATRIX_SIZE);       // UR
  FastLED.addLeds<CHIPSET, DATA_PIN_UL, COLOR_ORDER>(&(theLeds[MATRIX_TILE_SIZE*3]), MATRIX_SIZE);       // UL
#endif
  FastLED.addLeds<CHIPSET, DATA_PIN_LL, COLOR_ORDER>(theLeds[0], 0,             theLeds.Size()/4).setCorrection(TypicalSMD5050);
  FastLED.addLeds<CHIPSET, DATA_PIN_LR, COLOR_ORDER>(theLeds[0], theLeds.Size()/4, theLeds.Size()/4).setCorrection(TypicalSMD5050);
  FastLED.addLeds<CHIPSET, DATA_PIN_UR, COLOR_ORDER>(theLeds[0], theLeds.Size()/2, theLeds.Size()/4).setCorrection(TypicalSMD5050);
  FastLED.addLeds<CHIPSET, DATA_PIN_UL, COLOR_ORDER>(theLeds[0], (3*theLeds.Size())/4, theLeds.Size()/4).setCorrection(TypicalSMD5050);

  // Initialize the world coord sys
//  gerstWorld.setWcs(WCS_LLX, WCS_LLY, WCS_URX, WCS_URY);
  theLeds.SetWcs(WCS_LLX, WCS_LLY, WCS_URX, WCS_URY);
  // Initialize the accumulator grid
#if false
  accum.init(GRIDPIXELCOLS, GRIDPIXELROWS);
#endif
  theData.fill(0);

  // First test is a single wave
//            duration maxAmpl wavelength speed angle
  gerst.init(/*&gerstWorld, &accum*/);
//            duration maxAmpl wavelength speed angle
//    gerst.start(5000, 900, 1500, 100, angleMap(PI/6));
//NEW     duration wavelength steepness angle
#define TEST
#if defined(TEST)
  // simple test -- a single wave along the x axis
  gerst.start(0, 5000, steepnessMap(1.0), 0);
#else
    gerst.start(5000, 1500, steepnessMap(0.5), angleMap(PI/6));
    gerst.setRangeDuration(5000, 15000);
//    gerst.setRangeAmplitude(800, 1000);
    gerst.setRangeWavelength(1000, 1500);
//  gerst.setRangeSpeed(-200, 200);
    gerst.setRangeAngle(0, angleMap(PI/4));
#endif

  gerst2.init(/*&gerstWorld, &accum*/);
//            duration maxAmpl wavelength speed angle
//    gerst2.start(3000, 200, 250, 200, angleMap(PI/2));
//NEW     duration wavelength steepness angle
    gerst2.start(3000, 1200, steepnessMap(0.5), angleMap(PI/2));
    gerst2.setRangeDuration(5000, 15000);
//    gerst2.setRangeAmplitude(200, 300);
    gerst2.setRangeWavelength(800, 1200);
//    gerst2.setRangeSpeed(-200, 200);
    gerst2.setRangeAngle(0, angleMap(PI/2));
    
  gerst3.init(/*&gerstWorld, &accum*/);
//            duration maxAmpl wavelength speed angle
//    gerst3.start(5000, 200, 150, -100, angleMap(PI/3));
//NEW     duration wavelength steepness angle
    gerst3.start(5000, 150, steepnessMap(0.5), angleMap(PI/3));
    gerst3.setRangeDuration(5000, 15000);
//    gerst3.setRangeAmplitude(200, 300);
    gerst3.setRangeWavelength(350, 600);
//  gerst3.setRangeSpeed(-200, 200);
    gerst3.setRangeAngle(0, angleMap(PI/2));

  gerst4.init(/*&gerstWorld, &accum*/);
//            duration maxAmpl wavelength speed angle
//    gerst4.start(5000, 200, 150, -100, angleMap(2*PI/3));
//NEW     duration wavelength steepness angle
    gerst4.start(5000, 450, steepnessMap(0.5), angleMap(PI/6));
    gerst4.setRangeDuration(5000, 15000);
//    gerst4.setRangeAmplitude(200, 300);
    gerst4.setRangeWavelength(350, 600);
//    gerst4.setRangeSpeed(-200, 200);
    gerst4.setRangeAngle(angleMap(PI/3), angleMap(2*PI/3));

}

#define DO_ONE_FRAME false
#if DO_ONE_FRAME
bool  frameDone = false;
#endif
void loop() {
  // performance measurement variables
  static uint32_t frameCount = 0;
  static uint32_t frameStartTime = 0;
  static const uint32_t maxFrameCount = 100;

#if DO_ONE_FRAME
  if(frameDone) return;
  else frameDone = true;
#endif

  // performance measurement display
#if PERF
  if(frameCount>maxFrameCount && millis()>frameStartTime) {
    Serial << "Perf: " << (frameCount*1000)/(millis()-frameStartTime) << " frames/sec\n";
    frameStartTime = millis();
    frameCount = 0;
  } else frameCount++;
#endif

  // calculate the new frame
#if false
  accum.clear();
#endif
  theData.fill(0);
  gerst.calc();
  //gerst2.calc();
  //gerst3.calc();
  //gerst4.calc();

  // convert to colors in the led array and display
  // find sum of all max amplitudes so we can scale colors correctly
  uint32_t sumMaxAmplitude;
  sumMaxAmplitude = gerst.getMaxAmplitude() + gerst2.getMaxAmplitude() + gerst3.getMaxAmplitude() + gerst4.getMaxAmplitude();
  // now do each pixel: map to the LED grid
  for(int r=0; r<theLeds.Height(); r++) {
    for(int c=0; c<theLeds.Width(); c++) {
      CHSV hsv;
      CRGB rgb;
//      gridwcs_t height;
      AccumType_t height;
#if false
      height = accum.get(c,r);
#endif
      height = theData(c,r);
///      if(r==0) { Serial << height << ' ';
//      if(c==GRIDPIXELCOLS-1) Serial << endl;
 //     }
      // for each of these:  was: GW_MAX_AMPLITUDE, changed to sumMaxAmplitude
      if(height<=0) {
        hsv = CHSV(H_LOW,map(height, -sumMaxAmplitude, 0, S_LOW,S_ZERO), gamma8(map(height, sumMaxAmplitude, 0, V_LOW,V_ZERO)));
      } else {
        hsv = CHSV(H_LOW,map(height, 0, sumMaxAmplitude, S_ZERO,S_HIGH), gamma8(map(height, 0, sumMaxAmplitude, V_ZERO,V_HIGH)));          
      }
      rgb = CRGB(hsv);
#if false
      grid.setPixel(c,r, rgb);
#endif
      theLeds(c,r) = rgb;
    }
  }

  FastLED.show();

}
