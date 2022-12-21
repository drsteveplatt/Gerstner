// implementation of Gerstner wave
// Testing full gerstner wave routines
//

#include <arduino.h>
#include <FastLED.h>
#include "gridlib.h"
#include "gerstner.h"
#include "gamma.h"

#include <Streaming.h>



static gridwcs_t trigMult(gridwcs_t val, int16_t cs) {
  // multiplies a wcs val by the result of a FastLED 16-bit cos/sin val
  // The cos/sin val is [-32767 32767].
  // The result is scaled down 16 bits
  // Designed to work well for large gridwcs_t values -- spiits up the multiplication and
  // scales components at the "right" time to avoid overflows while preserving precision
  bool resPos;
  int32_t tVal, tcs;
  int32_t valH, valL; // high, low parts
  gridwcs_t res;
  // sign-adjust; save sign of the result
  resPos = true;
  if(val>=0) { resPos = true;  tVal = val; }
  else { resPos = false;  tVal = -val;  }
  if(cs>=0) { tcs = cs; }
  else { resPos = !resPos; tcs = -cs; }
  // grab the halves of val
  valH = val>>16;
  valL = val & 0x0000ffff;
  // do the multiplication
  res = valH*tcs + ((valL*tcs)>>16);
  res <<= 1;    // we had shifted both down by one for the sign; undo the double-shift
  return resPos ? res : -res;
}

int32_t GerstWave::calcUV(int u, int v) {
  // calculates the value at uv
}
void GerstWave::calc() {
  // add this wave to the accompanying m_acc accumulator with values based on the wave parameters and the current time.
  long unsigned int tNow;
  CHSV hsv;
  CRGB rgb;
  gridwcs_t x, y, u, v, theta;
  long int u0, v0;
  int16_t cs, sn, height;
  int32_t du, dt;
  tNow = millis();
  if(m_duration>0 && (tNow-m_startTime > m_duration)) {
    // old wave is over, start a new one
    // blah blah
    // and rest the time
    m_startTime = tNow;
  }
  // now fill all of the p
  cs = cos16(m_angle);
  sn = sin16(m_angle);
  for(int r=0; r<m_nRows; r++) {
    for(int c=0; c<m_nCols; c++) {
      CRGB val;
      // map rc to XY
      m_world->crToWcs(c,r,x,y);
      u = trigMult(x, cs) + trigMult(y, sn);
      v = trigMult(x, -sn) + trigMult(y, cs);
      // shift u by du/dt
      dt = millis()-m_startTime;
      du = (m_velocity*dt)/1000;
      u -= du;
      // calculate our new color
      theta = u /*map(c, 0,GRIDCOLS, 0,65535)*2 + phaseShiftX*/;
      while(theta>65535) theta -= 65536;
      m_acc->get(c,r) += height;
      height = cos16(theta);
      #if false
      hsv = CHSV(H_LOW,map(height, -32767, 32767, S_LOW,S_HIGH), gamma8(map(height, -32767, 32767, V_LOW,V_HIGH)));
      // add it to the accumulator
      m_acc->get(c,r) += CRGB(hsv);
      #endif
     // if(r==0 & c==0) Serial << "cr: " << c << ' ' << r  << " xy: " << x << ' ' << y 
     //      << " uv: " << u << ' ' << v << " du dt th hsv: " << du << ' ' << dt << ' ' << theta << ' ' <<  hsv.h << ' ' << hsv.s << ' ' << hsv.v << endl;
    }
  }

}
