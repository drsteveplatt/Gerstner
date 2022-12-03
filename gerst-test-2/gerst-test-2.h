//  Common stuff for gerstner library test

#if !defined(__GERST_TEST_H__)
#define __GERST_TEST_H__

#if defined(__MAIN__)
#define EXTERN
#else
#define EXTERN extern
#endif

// Physical size of LED grid
#define GRIDCOLS 16
#define GRIDROWS 16

// We build a virtual space with one pseudo-space surrounding it.  
// This allows a buffer space for data outside of the simulated space.
#define LOGICALGRIDSIZE (GRIDSIZE+2)


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


#endif // __GERST_TEST_H__
