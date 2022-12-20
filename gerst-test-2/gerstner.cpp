// implementation of Gerstner wave
//

#include <arduino.h>
#include "gridlib.h"
#include "gerstner.h"

void GerstWave::calcUV(int u, int v
void GerstWave::calc(int u, int v) {
  // fill m_pix with values based on the wave parameters and the current time.
  long unsigned int tNow;
  tNow = millis;
  if(tNow-m_startTime > m_duration) {
    // old wave is over, start a new one
    // blah blah
    // and rest the time
    m_startTime = tNow;
  }
  // now fill all of the p
  for(int r=0; r<m_nRows; r++) {
    for(c=0; c<m_nCols; c++) {
      
    }
  }

}
