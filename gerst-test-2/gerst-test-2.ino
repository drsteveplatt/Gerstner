//
// Gerstner test 2
// Simple waves on a grid

// ESP32 fast sin/cos notes:
// int16_t sin16/cos16(uint16:t theta)
//   theta 0..65535, returns val -32767..32767

#define __MAIN__
#include <Streaming.h>
#include <FastLED.h>
#include "gerstner.h"
#include "gridlib.h"
#include "gerst-test-2.h"

Grid grid(GRIDCOLS, GRIDROWS);

#define  WCS_LLX 0
#define WCS_LLY 0
#define WCS_URX (65535*2)
#define WCS_URY (65535*2)

void setup() {
  Serial.begin(115200);
  delay(200); // allow Serial to come online
  Serial << "Gerstner test 2.1 starting\n";

  // Initialize the grid
  grid.setWCS(WCS_LLX, WCS_LLY, WCS_URX, WCS_URY);
  FastLED.addLeds<WS2811, 25, GRB>(grid.theLeds(), GRIDROWS*GRIDCOLS);

  CHSV tmp;
  tmp = CHSV(255,253, 251);
  Serial << tmp[0] << ' ' << tmp[1] << ' ' << tmp[2] << endl;

}

void loop() {
  #if false
  // test 1: paint a wave on x, and repeat the same wave along y
  Serial << "Subtest 1.  Paint the screen.\n";
  for(int r=0; r<GRIDROWS; r++) {
    for(int c=0; c<GRIDCOLS; c++) {
      gridwcs_t x, y;
      int red, green, blue;
      grid.crToWcs(c,r, x, y);
      red = map(c,0,GRIDCOLS,0,255);
      green = map(r,0,GRIDROWS, 0, 255);
      blue = 0;
      grid.setPixel(x, y, red, green, blue);
    }
  }
  FastLED.show();
  delay(1000);
  #endif

  // test 2.  Paint a wave on x, repeat along y
  Serial << "Subtest 2.  Paint a simple wave.\n";
  for(int r=0; r<GRIDROWS; r++) {
    for(int c=0; c<GRIDCOLS; c++) {
      gridwcs_t x, y;
      CHSV col;
      grid.crToWcs(c, r, x, y);
      gridwcs_t tx;
      for(tx=x; tx>65535; tx-=65535) continue;
      int16_t cs = cos16(x&0x0000ffff);
      if(cs<0) {
        // scale blue from black to pure blue
        col = CHSV(255*2/3, 192, map(cs, -32767,0, 32, 192));
      } else {
        // scale blue from blue to pure white
        col = CHSV(255*2/3, map(cs, 0, 32767, 255,192), 192);
      }
      Serial <<  "cr: " << c << ' ' << r << " x,tx: " << x << ' ' << (x&0x0000ffff) << " cos: " << cs << " hsv: " << col.h << ' ' << col.s << ' ' << col.v << endl;
      grid.setPixelCr(c,r,col);
    }
  }
  FastLED.show();
  delay(1000);

}
