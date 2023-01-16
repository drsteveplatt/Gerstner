//
// gerstner.h
// Things needed to compure a gerstner wave
//

#if !defined (__GERSTNER_H__)
#define __GERSTNER_H__

// Grid of 16-but accumulators for RGB values
class GridHeight {
  private:
    gridwcs_t* m_cells;
  public:
    uint16_t m_nRows, m_nCols;
  public:
    GridHeight() { m_cells=NULL; }
    ~GridHeight() { if(m_cells!=NULL) delete[] m_cells; }
    void init(uint16_t nCols, uint16_t nRows) {
      if(m_cells!=NULL) delete[] m_cells; 
      m_cells = new gridwcs_t[nCols*nRows];
      m_nRows = nRows;
      m_nCols = nCols;
    }
    void set(int c,int r, gridwcs_t val) { m_cells[r*m_nCols+c] = val; }
    gridwcs_t& get(int c, int r) { return m_cells[r*m_nCols+c]; }
    void clear() {
      memset(m_cells, 0, m_nRows*m_nCols*sizeof(gridwcs_t));
    }
    uint16_t rows() { return m_nRows; }
    uint16_t cols() { return m_nCols; }

};

// Limits for appropriate GerstWave components
//    Duration: -1: reset immediately; 0: never rest; >0: reset after <val> ms
#define GW_MIN_DURATION   -1
//    Amplitude: of a single wave; summed/averaged over all waves
//        Val will be in range [-MAX MAX].  Color is scaled depending on the final value [COLMIN COLMAX]
#define GW_MAX_AMPLITUDE    1000

class GerstWave {
  private:
    long unsigned int m_startTime;   // ms
    long unsigned int m_duration;     // ms; wave ends at startTIme+duration
      uint32_t m_rngMinDuration, m_rngMaxDuration;
    int32_t m_maxAmplitude;           // 0..32767
      int32_t m_rngMinAmplitude, m_rngMaxAmplitude;
    uint32_t m_wavelength;               // in wcs units
      uint32_t m_rngMinWavelength, m_rngMaxWavelength;
    int32_t  m_speed;                 // in wcs units/sec (better resolution)  du/dt, essentially
      int32_t m_rngMinSpeed, m_rngMaxSpeed;
    uint32_t m_angle;                     // 0..65535, per FastLED trig16 functions
      uint32_t m_rngMinAngle, m_rngMaxAngle;
    // These will be based on m_grid.  m_grid and m_acc are assumed to be the same size.
    uint16_t m_nRows, m_nCols;
    // Note that m_grid and m_acc are both passed in via init()
    GridWorld* m_world;                           // used to get grid mappings
    GridHeight* m_acc;
  public:
    GerstWave(): m_world(NULL), m_acc(NULL),
      m_nRows(0), m_nCols(0), m_startTime(0), m_duration(0),
      m_rngMinDuration(1000), m_rngMaxDuration(20000),
      m_rngMinAmplitude(50), m_rngMaxAmplitude(10000),
      m_rngMinWavelength(32), m_rngMaxWavelength(1000),
      m_rngMinSpeed(-10000), m_rngMaxSpeed(10000),
      m_rngMinAngle(0), m_rngMaxAngle(65535)
      {  };
    ~GerstWave() { delete[] m_acc; };
    void init(GridWorld* world, GridHeight* acc) {
      m_world = world;
      m_acc = acc;
      m_nRows = m_acc->rows();
      m_nCols = m_acc->cols();
    }
    void setRangeDuration(uint32_t min, uint32_t max) { m_rngMinDuration=min; m_rngMaxDuration=max; }
    void setRangeAmplitude(int32_t min, int32_t max) { m_rngMinAmplitude=min; m_rngMaxAmplitude=max; }
    void setRangeWavelength(uint32_t min, uint32_t max) { m_rngMinWavelength=min; m_rngMaxWavelength=max; }
    void setRangeSpeed(int32_t min, int32_t max) { m_rngMinSpeed=min; m_rngMaxSpeed=max; }
    void setRangeAngle(int32_t min, int32_t max) { m_rngMinAngle=min; m_rngMaxAngle=max; }
    void copyRange(const GerstWave& g) {
      m_rngMinDuration = g.m_rngMinDuration;          m_rngMaxDuration = g.m_rngMaxDuration;
      m_rngMinAmplitude = g.m_rngMinAmplitude;      m_rngMaxAmplitude = g.m_rngMaxAmplitude;
      m_rngMinWavelength = g.m_rngMinWavelength;  m_rngMaxWavelength = g.m_rngMaxWavelength;
      m_rngMinSpeed = g.m_rngMinSpeed;            m_rngMaxSpeed = g.m_rngMaxSpeed;
      m_rngMinAngle = g.m_rngMinAngle;                   m_rngMaxAngle = g.m_rngMaxAngle;
    }
    void start(long unsigned int dur, int32_t maxAmpl, int32_t wavelength, uint32_t speed, uint32_t theta) {
      // create a wave with
      //    dur: duration in ms (0 -> forever)
      //    maxAmpl: maximum amplitude (0..32767) (0 -> always this ampl, else ramps up/dn over duration
      //    wavelength: wavelength in WCS units
      //   speed: motion in +x/+y direction, WCS units/sec
      //    theta: (0..65535) angle of U wrt X axis
      m_startTime = millis();
      m_duration =  dur<GW_MIN_DURATION ? GW_MIN_DURATION : dur;
      m_maxAmplitude = maxAmpl<1 ? 1 : (maxAmpl > GW_MAX_AMPLITUDE ? GW_MAX_AMPLITUDE : maxAmpl);
      m_wavelength = wavelength<1 ? 1 : wavelength;
      m_speed = speed;
      m_angle = theta & 0x0000ffff;  // 0..65535)
    }
    
    void calc();

};
#endif // __GERSNER_H__
