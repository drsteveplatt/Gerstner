//  Common stuff for gerstner library test

#if !defined(__GERST_TEST_H__)
#define __GERST_TEST_H__

#if defined(__MAIN__)
#define EXTERN
#else
#define EXTERN extern
#endif

// Physical size of LED grid
#define GRIDSIZE 16

// We build a virtual space with one pseudo-space surrounding it.  
// This allows a buffer space for data outside of the simulated space.
#define LOGICALGRIDSIZE (GRIDSIZE+2)

// A physical location on the grid
// WCS  is [px yy] in [0 0 0]..[2pi 2pi 1]
// DCS is [0 0]..[G+2 G+2]   G==GRIDSIZE
// Display/calculation DCS space is [1 1]..[G G]
// WCS pz  is calculated as f(x,y)
// fz is the weighted average of this cell with adjacent cells depending on the relative xy value.
// fz will be capped [-1 1] then mapped [0 255] to display
class GridCell {
  public:
    float  px, py;   // physical XY coords, will never change
    float  fx, fy;   // "floating" XY coords, will move via Gersner
    float  pz;       // "floating" Z coord, computed from px py
    float fz;  // computed based on adjacent fx fy values to compensate for fake flatness
    // 
    void setWcsFromDcs(int r, int c) {
      px = (c-1.0f)/GRIDSIZE * 2*PI;
      py = (r-1.0f)/GRIDSIZE * 2*PI;
    }
};

// *** A wave operating over the grid
//  Various reset maxima used when resetting a wave
const float gerstResetMaxLamba = 20.f;          // max wavelength
const float gerstResetMaxFreqReset = 0.5f;     // max variation in frequency
const float gerstResetMaxA = 0.20f;                 // max amplitude
const float gerstResetMaxAVar = 0.5f;             // max variation in amplitude
const float gerstResetMaxMaxZero = 3.0f;           // no idea
class Wave {
  public:
    // randomization parameters for resetting
    float dirX, dirY;       // unit vector in direction of the wave
    float k;                     // magnitude of v_w (?) = 2*pi*.lambda
    float aMax;               // max amplitude of the wave
    float phi;                  // phase offset
    // a_freq allows onging variation of the max amplitude
    float aFreq;           // freq of the amplitude variation
    float aFreqPhi;        // phase offset for freq variation
    // things that determine the lifespan of the wave
    int maxZero;          // max num of zero crossings (in z) before resetting
    int curZero;          // current number of zero crossings
    
    
};

// global start time, used to compute relative T each time we recompute the wave
EXTERN unsigned long startTime;

// our dataspace
GridCell  cells[LOGICALGRIDSIZE][LOGICALGRIDSIZE];

//  our leds
CRGB theLeds[GRIDSIZE][GRIDSIZE];

#endif // __GERST_TEST_H__
