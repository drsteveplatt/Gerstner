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
    uint16_t max() { return r>g ? (r>b?r:b) : (g>b?g:b); };
  public:
    CRGB16(): r(0),g(0),b(0) {};
    CRGB16(int tr, int tg, int tb):r(tr), g(tg), b(tb) {};
    ~CRGB16() {};
    CRGB16& operator+(CRGB rgb) { r += rgb.r; g += rgb.g; b += rgb.b; return *this; };
    void clip() { uint16_t mx = max();
      if(mx>255) { r=map(r,0,mx,0,255); g=map(g,0,mx,0,255); b=map(b,0,mx,0,255); };
    };
    void scale(int n) { r /= n; g /= n; b /= n; }
    void clear() { r=g=b=0; }
    uint16_t& operator[](int v) { return vals[v]; }
    CRGB16& operator+=(CRGB rgb) { r += rgb.r;  g += rgb.g; b+=rgb.b; return *this; }
};

// Grid of 16-but accumulators for RGB values
class Grid16 {
  private:
    CRGB16* m_cells;
  public:
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
    void set(int c,int r, CRGB16 val) { m_cells[r*m_nRows+c] = val; }
    CRGB16& get(int c, int r) { return m_cells[r*m_nRows+c]; }
    void clear() {
      memset(m_cells, 0, m_nRows*m_nCols*sizeof(CRGB16));
      #if false
      for(int r=0; r<m_nRows; r++) {
        for(int c=0; c<m_nCols; c++) {
          m_cells[r*m_nCols + c] = CRGB16(0,0,0);
        }
      }
      #endif
    }
    uint16_t rows() { return m_nRows; }
    uint16_t cols() { return m_nCols; }

};

class GerstWave {
  private:
    long unsigned int m_startTime;   // ms
    long unsigned int m_duration;     // ms; wave ends at startTIme+duration
    int32_t m_maxAmplitude;           // 0..32767
    int32_t m_wavelength;               // in wcs units
    int32_t  m_velocity;                 // in wcs units/sec (better resolution)  du/dt, essentially
    uint32_t m_angle;                     // 0..65535, per FastLED trig16 functions
    // These will be based on m_grid.  m_grid and m_acc are assumed to be the same size.
    uint16_t m_nRows, m_nCols;
    // Note that m_grid and m_acc are both passed in via init()
    GridWorld* m_world;                           // used to get grid mappings
    Grid16* m_acc;
  public:
    GerstWave(): m_world(NULL), m_acc(NULL),
      m_nRows(0), m_nCols(0), m_startTime(0), m_duration(0) {  };
    ~GerstWave() { delete[] m_acc; };
    void init(GridWorld* world, Grid16* acc) {
      m_world = world;
      m_acc = acc;
      m_nRows = m_acc->rows();
      m_nCols = m_acc->cols();
    }
    void start(long unsigned int dur, int32_t maxAmpl, int32_t wavelength, uint32_t velocity, uint32_t theta) {
      // create a wave with
      //    dur: duration in ms (0 -> forever)
      //    maxAmpl: maximum amplitude (0..32767) (0 -> always this ampl, else ramps up/dn over duration
      //    wavelength: wavelength in WCS units
      //    velocity: motion in +x/+y direction, WCS units/sec
      //    theta: (0..65535) angle of U wrt X axis
      m_startTime = millis();
      m_duration = dur;
      m_maxAmplitude = maxAmpl;
      m_wavelength = wavelength;
      m_velocity = velocity;
      m_angle = theta;
    }
    int32_t calcUV(int u, int v);
    void calc();

};
#endif // __GERSNER_H__
