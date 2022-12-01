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
#include "gerst-test-1.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(200); // allow Serial to come online
  Serial << "Gerstner test 2 starting\n";

  // Initialize the LEDs
  FastLED.addLeds<WS2811, 25, GRB>(&theLeds[0][0], GRIDSIZE*GRIDSIZE);


    int h,s,v;
  int r,c,ct;
 /*
  for(int i=0; i<GRIDSIZE*GRIDSIZE; i++) {
    r = i/16;
    c = ((r&01)==0) ? i%16 : 16-1-(i%16);
    //c = i%16;
    Serial << r << ' ' << c << ' ' << i%256 << endl;
    //theLeds[r][c] = CRGB( i%256, 0, 0);
    theLeds[r][c] = CHSV(i%256,255,255);
    //theLeds[i/16][i%16] = CRGB( i%256, 0, 0);
  }
  FastLED.show();
  delay(1000);
  */

  /*
  int h,s,v;
  int r,c,ct;
  for(h=0; h<1; h++) {
    for(r=0; r<GRIDSIZE; r++)
      for(c=0; c<GRIDSIZE; c++) {
        ct = (r&0x01)==0 ? c : GRIDSIZE-c;
        s = r*GRIDSIZE;
        v = c*GRIDSIZE;
        Serial << r << ' ' << c << " -> " << h << ' ' << s << ' ' << v << endl;
        theLeds[c][r] = CHSV(h,s,ct);
      }
    FastLED.show();
    delay(20);
  }
  */
  // Initialize the wave space
  for(int r=0; r<LOGICALGRIDSIZE; r++) {
    for(int c=0; c<LOGICALGRIDSIZE; c++) {
      cells[r][c].setWcsFromDcs(r,c);
    }
  }
  // quick verify
//  for(int r=0; r<2; r++) {
//    Serial.printf("%2d: ",r);
//    for(int c=0; c<LOGICALGRIDSIZE; c++) Serial.printf("[%5.3f, %5.3f]", cells[r][c].px, cells[r][c].py);
//    Serial.println();
//  }
//    for(int r=LOGICALGRIDSIZE-2; r<LOGICALGRIDSIZE; r++) {
//    Serial.printf("%2d: ",r);
//    for(int c=0; c<LOGICALGRIDSIZE; c++) Serial.printf("[%5.3f, %5.3f]", cells[r][c].px, cells[r][c].py);
//    Serial.println();
//  }
}

void loop() {
  int r,c;
  int h,s,v;
  for(h=0; h<GRIDSIZE*GRIDSIZE; h++) {
    for(r=0; r<GRIDSIZE; r++) 
      for(c=0; c<GRIDSIZE; c++) {
        s = r;
        v = ((r&0x01)==0) ? c : GRIDSIZE-c-1;
        theLeds[r][c] =  CHSV(h,v*16,dim8_video(s*16));
        //Serial << r << ' ' << c << ' ' << h << ' ' << s*16 << ' ' << v*16 << endl;
      }
      FastLED.show();
      delay(20);
  }
  // recompute wave
  // display it
}
