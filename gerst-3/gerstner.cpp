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

#define DEBUG false

void GerstWave::calc() {
  // add this wave to the accompanying m_acc accumulator with values based on the wave parameters and the current time.
  long unsigned int tNow;
  CHSV hsv;
  CRGB rgb;
  gridwcs_t x, y, u, v, theta;
  long int u0, v0;
  int16_t cs, sn;
  int32_t height;
  int32_t curAmplitude; // scales from 0..m_maxAmplitude depending on time in wave lifespan
  int32_t du, dt, tInDuration;
  tNow = millis();
  if(m_duration==-1 || (m_duration>0 && (tNow-m_startTime > m_duration))) {
    // old wave is over, start a new one
    m_duration = random(m_rngMinDuration, m_rngMaxDuration);
//    m_maxAmplitude = random(m_rngMinAmplitude, m_rngMaxAmplitude);
    m_wavelength = random(m_rngMinWavelength, m_rngMaxWavelength);
//    m_speed = random(m_rngMinSpeed, m_rngMaxSpeed);
    m_angle = random(m_rngMinAngle, m_rngMaxAngle);
    // and rest the time
    m_startTime = tNow;
  }
  // now fill all of the p
  cs = cos16(m_angle);
  sn = sin16(m_angle);
#define CATLIKECODING_CALCS true

  curAmplitude = m_maxAmplitude;
  if(m_duration!=0) {
    tInDuration = tNow-m_startTime;     // should be [0..m_duration)
  #if DEBUG
  Serial << "calc: curAmplitude init to " << curAmplitude << " tlnDuration: " << tInDuration << " m_duration/6: " << m_duration/6 << endl;
  #endif
    if(tInDuration<m_duration/6)  curAmplitude = map(tInDuration, 0,m_duration/6, 0,m_maxAmplitude);
    else if(tInDuration>(5*m_duration)/6)  curAmplitude = map(tInDuration, (5*m_duration)/6,m_duration, m_maxAmplitude,0);
    else curAmplitude = m_maxAmplitude;
  }

    for(int r=0; r<m_nRows; r++) {
    for(int c=0; c<m_nCols; c++) {
      CRGB val;
      // map rc to XY
      m_world->crToWcs(c,r,x,y);
      u = trigMult(x, cs) + trigMult(y, sn);
      v = trigMult(x, -sn) + trigMult(y, cs);
      // shift u by du/dt
      dt = millis()-m_startTime;
      du = (m_speed*dt)/1000;
      u -= du;
      // calculate our new color
      //theta = u /*map(c, 0,GRIDCOLS, 0,65535)*2 + phaseShiftX*/;
      //while(theta>m_wavelength) theta -= m_wavelength;

      theta = (((u-m_world->m_wcsLLx)%m_wavelength)<<16) / m_wavelength;
      height = cos16(theta);    // height is in range -32767..32767
#if DEBUG
if(r==0 & c==0) {
  Serial << "calc: theta: " << theta << " curAmplitude: " << curAmplitude << " cos16(theta): " << height;
}
#endif
#define NEWFLATTENINGCODE false
#if NEWFLATTENINGCODE
//      if(r==0) {
//        if(c==0) Serial << endl;
//        Serial << "cr: " << c << ' ' << r << " height: " << height << " parts: " << (height+32767)/2 << ' '
//        << ( (height+32767)/2 * (height+32767)/2) 
//        << ' ' << (( (height+32767)/2 * (height+32767)/2) >>14)
 //       << ' ' << (( (height+32767)/2 * (height+32767)/2) >>14) - 32767;
 //     }
      // wave flattening -- implement w = 2 * ( (h+1)/2 )^k, where h in [0 1]
      height = 2 * (( (height+32767)/2 * (height+32767)/2) >> 14) - 32767;
      if(height>32767) height=32767;
      if(height<-32767) height=-32767;
#endif // NEWFLATTENINGCODE
      height = map(height, -32767,32767, -curAmplitude,curAmplitude); // map to max ampl limit
#if DEBUG
if(r==0 & c==0) {
  Serial << " final height: " << height << endl;
}
#endif
      m_acc->get(c,r) += height;
    }
  }

}

void GerstWave::dump(char* label/*=NULL*/) {
  Serial << "-->GerstWave::dump";  if(label) Serial << '(' << label << ")\n";
  Serial << "\tm_nCols: " << m_nCols << " m_nRows: " << m_nRows << endl;
  Serial << "\tm_startTime: " << m_startTime << endl;
  Serial << "\tm_duration: " << m_duration << " rng: [" << m_rngMinDuration << ' ' << m_rngMaxDuration << "]\n";
  Serial << "\tm_wavelength: " << m_wavelength << " rng: [" << m_rngMinWavelength << ' ' << m_rngMaxWavelength << "]\n";
  Serial << "\tm_angle: " << m_angle << " rng: [" << m_rngMinAngle << ' ' << m_rngMaxAngle << "]\n";
  Serial << "\tm_steepness: " << m_steepness << " rng: [" << m_rngMinSteepness << ' ' << m_rngMaxSteepness << "]\n";
  Serial << "\tm_maxAmplitude: " << m_maxAmplitude << endl;
  Serial << "\tm_speed: " << m_speed << endl;
  Serial << "\tm_c: " << m_c << endl;
  Serial << "\tm_k: " << m_k << endl;
  Serial << "<--GerstWave::dump\n";
}
