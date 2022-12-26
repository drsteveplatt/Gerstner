//
// 2d graphics library on a grid
// Classes to manage WCS-DCS transformation (unrotated wcs) and 2D LED arrays
//
// GridWorld -- defines WCS and has mapping functions between WCS (xy) and DCS (cr) coordinates
// Grid -- manages a single FastLED CRGB grid as an rc environment
//

#if !defined(__GRIDLIB_INCLUDED__) 
#define __GRIDLIB_INCLUDED__

// A Grid has physical 0,0 in the lower left and logical (xmin ymin) in the lower left
// Note on a "standard" LED grid, the power will come in at the lower left

typedef int32_t gridwcs_t;

// GridWorld contains all of the info needed for WCS(xy) to DCS(cr) to PixID mappings.
// Note that WCS xy is considered to be aligned with the DCS cr coordinate systems
// Note that [0 0] in both systems is in the lower left. 
// Supports conversion betw WCS and DCS as well as WCS to raw pixel index.
// The latter includes row-invesion (rows 1 3 etc. are R-L instead of L-R) compensation
class GridWorld {
  private:
    int m_nRows,  m_nCols;     // physical cr limits [ [0 m_nCols-1] [0 m_nRows-1] ]
  public:
    gridwcs_t m_wcsLLx, m_wcsLLy, m_wcsURx, m_wcsURy;
    gridwcs_t m_wcsRangex, m_wcsRangey;
    gridwcs_t m_wcsStepx, m_wcsStepy;
  public:
    GridWorld(int nCols, int nRows): m_nCols(nCols), m_nRows(nRows),
      m_wcsLLx(0), m_wcsLLy(0), m_wcsURx(1000), m_wcsURy(1000) {}

    void setWcs(int wcsLLx, int wcsLLy, int wcsURx, int wcsURy) {
      m_wcsLLx = wcsLLx; m_wcsLLy = wcsLLy;
      m_wcsURx = wcsURx; m_wcsURy = wcsURy;
      m_wcsRangex = m_wcsURx - m_wcsLLx;
      m_wcsRangey = m_wcsURy - m_wcsLLy;
    }
    int wcsToPixId(gridwcs_t wcsX, gridwcs_t wcsY) {
      int r,c;
      if(m_wcsRangex==0 || m_wcsRangey==0) return 0;
      wcsToCr(wcsX, wcsY, c, r);
      if((r&0x01)==1) { c = m_nCols - c - 1; }
      return r*m_nCols + c;
    }
    void wcsToCr(gridwcs_t x, gridwcs_t y, int &c, int& r) {
      c = map(x, m_wcsLLx, m_wcsURx, 0, m_nCols);
      r = map(y, m_wcsLLy, m_wcsURy, 0, m_nRows);
    }
    
    void crToWcs(int c, int r, gridwcs_t& x, gridwcs_t& y) {
      x = map(c, 0, m_nCols, m_wcsLLx, m_wcsURx);
      y = map(r, 0, m_nRows, m_wcsLLy, m_wcsURy);
    }    
};

// Grid has a CRGB array for use by FastLED
// It also has routines to manage cr addressing of the LED grid.
class Grid {
  private:
    int  m_nRows, m_nCols;  // slze of the grid
    //GridWorld m_world;
    CRGB* m_cells;

  public:
    Grid(int nRows, int nCols): m_nRows(nRows), m_nCols(nCols) { 
      m_cells = new CRGB[nRows*nCols];
      //m_wcsLLx = m_wcsLLy = 0;
      //m_wcsURx = m_wcsURy = 255;
      //m_wcsRangex = m_wcsRangey = 255;
      //m_wcsStepx = 255/nCols;
      //m_wcsStepy = 255/nRows;
    }
    ~Grid() { delete[] m_cells; }

    int crToPixId(int c, int r) {
      if(c<0 || c>=m_nCols || r<0 || r>=m_nRows) return 0;
      if((r&0x01)==1) { c = m_nCols - c - 1; }
      return r*m_nCols + c;
    }
    void setPixel(int col, int row, int r, int g, int b) {
      int pixId;
      // convert to pix coord
      pixId =crToPixId(col, row);
      // write pixel
      m_cells[pixId] = CRGB(r, g, b);
    }
    void setPixel(int col, int row, CRGB rgb) {
      int pixId;
      pixId = crToPixId(col, row);
      m_cells[pixId] = rgb;
    }
   void setPixel(int col, int row, CHSV hsv) {
      int pixId;
      pixId =crToPixId(col, row);
      m_cells[pixId] = hsv;
    }
    //gridwcs_t pixStepX() { return m_wcsStepx; }
    //gridwcs_t pixStepY() { return m_wcsStepy; }

    void getCR(int& c, int& r) { c=m_nCols; r=m_nRows; }
  
    CRGB* theLeds() { return m_cells; }
    CRGB& led(int i) { return m_cells[i]; }
    CRGB& led(int c, int r) { return m_cells[r*m_nCols + c]; }
  
};

#endif
