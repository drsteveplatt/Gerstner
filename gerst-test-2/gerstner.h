//
// gerstner.h
// Things needed to compure a gerstner wave
//

#if !defined (__GERSTNER_H__)
#define __GERSTNER_H__

// a "large" CRGB, an accumulator for CRGB values so we can get vals>255
class CRGB16 {
  public:
    union {
      uint16_t vals[3];
      struct { uint16_t r,g,b; };
      struct { uint16_t red, green, blue; };
    };
  private:
    max() { return r>g ? (r>b?r:b) : (g>b?g:b); };
  public:
    CRGB16(): r(0),g(0),b(0) {};
    ~CRGB16() {};
    CRGB16& operator+(CRGB rgb) { r += rgb.r; g += rgb.g; b += rgb.b; return *this; };
    void clip() { uint16_t mx = max(*this);
      if(mx>255) { r=map(r,0,mx,0,255); g=map(g,0,mx,0,255); b=map(b,0,mx,0,255); };
    };
    void clear() { r=g=b=0; }
    uint16_t& operator[](int v) { return vals[v]; }
};

// Grid of 16-but accumulators for RGB values
class Grid16 {
  private:
    CRGB16* m_cells;
    uint16_t m_nRows, m_nCols;
  public:
    Grid16() { m_cells=NULL; }
    ~Grid16() { if(m_cells!=NULL) delete[] m_cells; }
    void init(uint16_t nCols, uint16_t nRows) {
      if(m_cells!=NULL) delete[] m_cells; 
      m_cells = new CRGB16[nCols*nRows];
      m_nRows = nRows;
      m_nCols = nCols;
    }
    void set(int c, r, CRGB16 val) { m_cells[r*m_nRows+c] = val; }
    CRGB16& get(int c, r) { return m_cells[r*m_nRows+c]; }
    void clear() {
      memset(m_cells, 0, m_nRows*m_nCols*sizeof(uint16_t));
    }

};

class GerstWave {
  private:
    long unsigned int m_startTime;   // ms
    long unsigned int m_duration;     // ms; wave ends at startTIme+duration
    int32_t m_maxAmplitude;           // 0..32767
    int32_t m_wavelength;               // in wcs units
    uint32_t  m_velocity;                 // in wcs units/sec (better resolution)
    uint32_t m_angle;                     // 0..65535, per FastLED trig16 functions
    // Note that m_grid and m_acc are both passed in via init()
    Grid* m_grid;                           // used to get grid mappings
    Grid16* m_acc;
  public:
    GerstWave(Grid* grid, int ncol, int nrow): m_grid(grid), m_startTime(0), m_duration(0) { m_pix = new CRGB16[ncol*nrow]; };
    ~GerstWave( delete[] m_pix; );
    void init(Grid* grid, Grid16* acc) {
      m_grid = grid;
      m_acc = acc;
    }
    void start(long unsigned int dur, int32_t maxAmpl, int32_t wavelength, uint32_t velocity, uint32_t theta) {
      m_startTime = millis();
      m_duration = dur;
      m_maxAmplitude = maxAmpl;
      m_wavelength = wavelength;
      m_velocity = velocity;
      m_angle = theta;
    }
    void calcUV(int u, int v);
    void calc(int u, int v);
      
    void calc(int u, int v) {
      // fill m_pix with values based on the wave parameters and the current time.
      long unsigned int tNow;
      CHSV hsv;
      CRGB rgb;
      tNow = millis;
      if(tNow-m_startTime > m_duration) {
        // old wave is over, start a new one
        // blah blah
        // and rest the time
        m_startTime = tNow;
      }
      // now fill all of the p
      for(int r=0; r<m_nRows; r++) {
        for(int c=0; c<m_nCols; c++) {
          CRGB val;
          
        }
      }
 
    }
};
#endif // __GERSNER_H__
