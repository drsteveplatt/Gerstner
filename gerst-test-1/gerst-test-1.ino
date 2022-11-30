// Gerstner Wave Test #1
//
// Experimenting with/developing a Gerstner toolset for 16x16 or other grids
// This is expecting to run on an ESP-32.
//
//  2022-0804  Test 1.  Just paint increasing values into the grid
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
  Serial << "Gerstner test 1 starting\n";

  // Initialize the LEDs
  FastLED.addLeds<WS2811, 25, GRB>(&theLeds[0][0], GRIDSIZE*GRIDSIZE);
  
  for(int i=0; i<GRIDSIZE*GRIDSIZE; i++) {
    theLeds[i/16][i%16] = CRGB( i%256, 0, 0);
  }
  FastLED.show();
  delay(1000);
  
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
  // recompute wave
  // display it

}
