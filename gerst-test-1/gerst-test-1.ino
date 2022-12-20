// Gerstner Wave Test #1
//
// Experimenting with/developing a Gerstner toolset for 16x16 or other grids
// This is expecting to run on an ESP-32.
//
//  2022-0804  Test 1.  Just paint increasing values into the grid
//  2022-1201  Test 2.  Hue varying over time, sat/val on dispaly grid
//  2022-1205  Test 3.  Play with hue values
//  2022-1206  Test 4.  Testing Spectral HSV for candidate ocean colors
//

#define __MAIN__
#include <Streaming.h>
#include <FastLED.h>
#include "gerstner.h"
#include "gridlib.h"
#include "gerst-test-1.h"
#include "gamma.h"

Grid grid(GRIDSIZE, GRIDSIZE);

#define  WCS_LLX 0
#define WCS_LLY 0
#define WCS_URX 10000
#define WCS_URY 10000

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(200); // allow Serial to come online
  Serial << "Gerstner test 4 starting\n";

  // Initialize the grid
  grid.setWCS(WCS_LLX, WCS_LLY, WCS_URX, WCS_URY);

  {
    int c,r;
    gridwcs_t x,y;
  }

  // Initialize the LEDs
  //FastLED.addLeds<WS2811, 25, GRB>(&theLeds[0][0], GRIDSIZE*GRIDSIZE);
  FastLED.addLeds<WS2811, 25, GRB>(grid.theLeds(), GRIDSIZE*GRIDSIZE);

  Serial << "HUE_BLUE is " << HUE_BLUE << endl;

  { CRGB rgb;
    CHSV hsv;
    hsv = CHSV(HUE_BLUE, 255, 255);
    rgb = hsv;
    Serial << "hsv: " << hsv.h  << ' ' << hsv.s  << ' ' << hsv.v << "  rgb: " << rgb.r << ' ' << rgb.g << ' ' << rgb.b << endl;
  }
}

// These are the high and low HSV values proposed for the wave.
// This test varies them from 0 to 15.
#define H_LOW ((161*256)/360)
#define S_LOW 255
#define V_LOW ((30*256)/100)
#define H_HIGH H_LOW
//#define S_HIGH ((55*256)/100)
#define S_HIGH 186
#define V_HIGH ((63*256)/100)
void loop() {

  int32_t r,c;
  int h,s,v;
  int x, y;
  CHSV hsv;
  CRGB rgb;
  //for(h=0; h<256; h++) {
  fill_solid(grid.theLeds(), GRIDSIZE*GRIDSIZE, CRGB(0,0,0));
    for(r=0; r<1; r++) {
      for(c=0; c<GRIDSIZE; c++) {
 /*
        hsv.h = H_LOW;
        hsv.s = map(c, 0, GRIDSIZE, S_LOW, S_HIGH);
        hsv.v = map(c, 0, GRIDSIZE, V_LOW, V_HIGH);
        Serial << r << ' ' << c << ", "<< hsv.h << ' ' << hsv.s << ' ' << hsv.v << " -> ";
        hsv2rgb_spectrum(hsv, rgb);
        Serial  << rgb.r << ' ' << rgb.g << ' ' << rgb.b << endl;
         grid.setPixelCr(c,r, rgb);
*/
        hsv = CHSV(H_LOW,map(c, 0,GRIDSIZE, S_LOW,S_HIGH), map(c, 0,GRIDSIZE, V_LOW,V_HIGH));
        //hsv.s = gamma8(hsv.s);
        hsv.v = gamma8(hsv.v);
        //hsv.s = 255-gamma8(255-hsv.s);
        hsv2rgb_spectrum(hsv, rgb);
        Serial << r << ' ' << c << ", " << hsv.h << ' ' << hsv.s << ' ' << hsv.v << " -> " << rgb.r << ' ' << rgb.g << ' ' << rgb.b << endl;
        grid.setPixelCr(c, r, rgb);
     }
    }
    for(int i=0; i<GRIDSIZE; i++) {
      CRGB rgb;
      rgb = grid.theLeds()[i];
      Serial << '[' << rgb.r << ' ' << rgb.g << ' ' << rgb.b << "]\n";
    }
    FastLED.show();
    delay(1000);
  //}

}
