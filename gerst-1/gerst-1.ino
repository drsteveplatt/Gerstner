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
  Serial << "Gerstner full test 1 starting\n";
  Serial << "long int size: " << sizeof(long int) << endl;

  // Initialize the display grid
  grid.setWCS(WCS_LLX, WCS_LLY, WCS_URX, WCS_URY);
  // Initialize the accumulator grid
  accum.init(GRIDCOLS, GRIDROWS);

  // First test is a single wave
  gerst.init(&grid, &accum);
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
  #if DO_TEST_1
  // test 1: paint a wave on x, and repeat the same wave along y
  Serial << "Subtest 1.  Paint the screen.\n";
  for(int r=0; r<GRIDROWS; r++) {
    for(int c=0; c<GRIDCOLS; c++) {
      // map rc to uv
      gridwcs_t x, y;
      gridwcs_t u, v;
      grid.crToWcs(c,r, x, y);
      u = x;
      v = y;
     
      int red, green, blue;
      red = map(c,0,GRIDCOLS,0,255);
      green = map(r,0,GRIDROWS, 0, 255);
      blue = 0;
      grid.setPixel(x, y, red, green, blue);
    }
  }
  FastLED.show();
  delay(1000);
  #endif // DO_TEST_1

  // test 2.  Paint a wave on x, repeat along y
  #define WATER_HUE HUE_BLUE
  // sat varies from MIN (at peak) to MAX (at midpoint).  It is MAX below the midpoint
  #define WATER_MIN_SAT gamma8(224)
  #define WATER_MAX_SAT gamma8(255)
  // val varies from MIN (at trough) to MAX (at midpoint).  It is MAX above the midpoint
  #define WATER_MIN_VAL gamma8(128)
  #define WATER_MAX_VAL gamma8(192)
  #define COS_MIN -32767
  #define COS_MAX 32767
  #define COS_PEAK ((COS_MAX)/4)

  #if DO_TEST_2
  Serial << "Subtest 2.  Paint a simple wave.\n";
  { 
    CHSV tmph = CHSV(HUE_BLUE, 255, 255);
    CRGB tmpr = tmph;
    Serial << "Blue hsv: " << tmph.h << ' ' << tmph.s << ' ' << tmph.v;
    Serial << ' ' << tmpr.r << ' ' << tmpr.g << ' ' << tmpr.b << endl;
  }
  for(int r=0; r<GRIDROWS; r++) {
    for(int c=0; c<GRIDCOLS; c++) {
      gridwcs_t x, y;
      CHSV col;
      grid.crToWcs(c, r, x, y);
      gridwcs_t tx;
      for(tx=x; tx>65535; tx-=65535) continue;
      int16_t cs = cos16(x&0x0000ffff);
      if(cs>COS_MAX-COS_PEAK) {
        // top 25*, fade from pure blue to blue-white (fade S down from SAT_MAX to SAT_MIN) (V at VAL_MAX)
        Serial << "Max ";
        col = CHSV(HUE_BLUE, map(cs, 0, 32767, WATER_MAX_SAT,WATER_MIN_SAT), WATER_MAX_VAL);
      } else if(cs<COS_MIN+COS_PEAK) {
        Serial << "Min ";
        // bottom 25%, fade from pure blue to blue-black (fade V down from VAL_MAX to VAL_MIN) (S at SAT_MAX)
        col = CHSV(HUE_BLUE, WATER_MAX_SAT, map(cs, -32767,0, WATER_MIN_VAL, WATER_MAX_VAL));
      } else {
        // mid half, use SAT_MAX VAL_MAX
        Serial << "Mid ";
        col = CHSV(255*2/3, WATER_MAX_SAT, WATER_MAX_VAL);
      }

      Serial <<  "cr: " << c << ' ' << r << " x,tx: " << x << ' ' << (x&0x0000ffff) << " cos: " << cs << " hsv: " << col.h << ' ' << col.s << ' ' << col.v << endl;
      grid.setPixelCr(c,r,col);
      Serial << "  Pixel rgb: " << grid.led(c).r << ' ' << grid.led(c).g << ' ' << grid.led(c).b << endl;
   }
  }
  FastLED.show();
  delay(1000);
  #endif DO_TEST_2

  #if DO_TEST_3
  // paint a simple wave using trig, ability to scale theta
#define H_LOW ((161*256)/360)
#define S_LOW 255
#define V_LOW ((20*256)/100)
#define H_HIGH H_LOW
//#define S_HIGH ((55*256)/100)
#define S_HIGH 186
#define V_HIGH ((63*256)/100)

  CRGB rgb;
  CHSV hsv;
  int theta, costheta;
  // updated color scheme
  for(int r=0; r<GRIDROWS; r++) {
    for(int c=0; c<GRIDCOLS; c++) {
        theta = (map(c, 0,GRIDCOLS, 0,65535)*4)/3;
        while(theta>65535) theta-=65535;
        costheta = cos16(theta&0x0000ffff);
//        hsv = CHSV(H_LOW,map(theta, 0,GRIDCOLS, S_LOW,S_HIGH), map(theta, 0,GRIDCOLS, V_LOW,V_HIGH));
        hsv = CHSV(H_LOW,map(costheta, -32767, 32767, S_LOW,S_HIGH), map(costheta, -32767, 32767, V_LOW,V_HIGH));
        //hsv.s = gamma8(hsv.s);
        hsv.v = gamma8(hsv.v);
        //hsv.s = 255-gamma8(255-hsv.s);
        hsv2rgb_spectrum(hsv, rgb);
        Serial << r << ' ' << c << ", " << theta << ' ' << costheta <<", " << hsv.h << ' ' << hsv.s << ' ' << hsv.v << " -> " << rgb.r << ' ' << rgb.g << ' ' << rgb.b << endl;
        grid.setPixelCr(c, r, rgb);      
    }
  }
  FastLED.show();
  delay(1000);
  #endif DO_TEST_3

  #if DO_TEST_4
  // Moving wave in x (phase shift in x with each cycle)
  // A second moving wave in y (phase shift in y with each cycle)
  static int phaseShiftX = 0;
  static int phaseShiftDeltaX= 1000;  // per iteration
  static int phaseShiftY = 0;
  static int phaseShiftDeltaY = 2500;
#define H_LOW ((161*256)/360)
#define S_LOW 255
//#define V_LOW ((30*256)/100)
#define V_LOW 0
#define H_HIGH H_LOW
//#define S_HIGH ((55*256)/100)
#define S_HIGH 186
#define V_HIGH ((63*256)/100)

  CRGB rgb;
  CHSV hsv1, hsv2, hsv;
  int theta, costheta;
  // updated color scheme
  for(int r=0; r<GRIDROWS; r++) {
    for(int c=0; c<GRIDCOLS; c++) {
        // x wave
        theta = map(c, 0,GRIDCOLS, 0,65535)*2 + phaseShiftX;
        while(theta>65535) theta-=65535;
        costheta = cos16(theta);
        hsv1 = CHSV(H_LOW,map(costheta, -32767, 32767, S_LOW,S_HIGH), map(costheta, -32767, 32767, V_LOW,V_HIGH));
        // y wave
        theta = map(r, 0, GRIDROWS, 0, 65535) * 4 + phaseShiftY;
        while(theta>65535) theta -= 65535;
        costheta = cos16(theta);
        hsv2 = CHSV(H_LOW,map(costheta, -32767, 32767, S_LOW,S_HIGH), map(costheta, -32767, 32767, V_LOW,V_HIGH));
        // merge them
        hsv.h = (hsv1.h+hsv2.h)/2;
        hsv.s = (hsv1.s+hsv2.s)/2;
        hsv.v = (hsv1.v+hsv2.v)/2;
        hsv.v = gamma8(hsv.v);
        hsv2rgb_spectrum(hsv, rgb);
        //if(r==0&&c==0) Serial << r << ' ' << c << ", " << theta << ' ' << costheta << ' ' << phaseShift << ", " << hsv.h << ' ' << hsv.s << ' ' << hsv.v << " -> " << rgb.r << ' ' << rgb.g << ' ' << rgb.b << endl;
        grid.setPixelCr(c, r, rgb);      
    }
  }
  FastLED.show();
  phaseShiftX += phaseShiftDeltaX;
  phaseShiftY += phaseShiftDeltaY;
  delay(20);
  #endif // DO_TEST_4

  #if DO_TEST_5

  // testing actual gerstwave
  static bool hasRun = false;
  if(!hasRun) {
    
    accum.clear();
    gerst.calc();
    for(int r=0; r<GRIDROWS; r++) {
      for(int c=0; c<GRIDCOLS; c++) {
        CRGB16 val(accum.get(c,r));
        grid.setPixelCr(c,r,val.r, val.g, val.b);
      }
    }
    FastLED.show();
    delay(100);
    
    //hasRun = true;
  }
  #endif // DO_TEST_5
}
