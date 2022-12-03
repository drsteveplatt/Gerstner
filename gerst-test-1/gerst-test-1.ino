// Gerstner Wave Test #1
//
// Experimenting with/developing a Gerstner toolset for 16x16 or other grids
// This is expecting to run on an ESP-32.
//
//  2022-0804  Test 1.  Just paint increasing values into the grid
//  2022-1201  Test 2.  Hue varying over time, sat/val on dispaly grid
//

#define __MAIN__
#include <Streaming.h>
#include <FastLED.h>
#include "gerstner.h"
#include "gridlib.h"
#include "gerst-test-1.h"

Grid grid(GRIDSIZE, GRIDSIZE);

#define  WCS_LLX 0
#define WCS_LLY 0
#define WCS_URX 10000
#define WCS_URY 10000

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(200); // allow Serial to come online
  Serial << "Gerstner test 3 starting\n";

  // Initialize the grid
  grid.setWCS(WCS_LLX, WCS_LLY, WCS_URX, WCS_URY);

  {
    int c,r;
    gridwcs_t x,y;
  }

  // Initialize the LEDs
  //FastLED.addLeds<WS2811, 25, GRB>(&theLeds[0][0], GRIDSIZE*GRIDSIZE);
  FastLED.addLeds<WS2811, 25, GRB>(grid.theLeds(), GRIDSIZE*GRIDSIZE);

}

void loop() {
  int32_t r,c;
  int h,s,v;
  int x, y;
  for(h=0; h<256; h++) {
    for(r=0; r<GRIDSIZE; r++) {
      for(c=0; c<GRIDSIZE; c++) {
        s = r*16;
        v = dim8_video(c*16);
        //grid.crToWcs(c,r,x,y);
        //grid.setPixel(x,y, CHSV(h, s, v));
        grid.setPixelCr(c,r, CHSV(h, s, v));
        //Serial << r << ' ' << c << ' -> ' << h << ' ' << s << ' ' << v << endl;
      }
    }
    FastLED.show();
    delay(20);
  }
  // recompute wave
  // display it
}
