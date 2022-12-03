//
// 2d graphics library on a grid
//

#if !defined(__GRIDLIB_INCLUDED__) 
#define __GRIDLIB_INCLUDED__

// A Grid has physical 0,0 in the lower left and logical (xmin ymin) in the lower left
// Note on a "standard" LED grid, the power will come in at the lower left

typedef int32_t gridwcs_t;

class Grid {
  private:
    int  m_nRows, m_nCols;  // slze of the grid
    CRGB* m_cells;
    gridwcs_t m_wcsLLx, m_wcsLLy, m_wcsURx, m_wcsURy;
    gridwcs_t m_wcsRangex, m_wcsRangey;
    gridwcs_t m_wcsStepx, m_wcsStepy;

    int wcsToPixId(gridwcs_t wcsX, gridwcs_t wcsY) {
      int r,c;
      if(m_wcsRangex==0 || m_wcsRangey==0) return 0;
      wcsToCr(wcsX, wcsY, c, r);
      if((r&0x01)==1) { c = m_nCols - c - 1; }
      return r*m_nCols + c;
    }

    int crToPixId(int c, int r) {
      if(c<0 || c>=m_nCols || r<0 || r>=m_nRows) return 0;
      if((r&0x01)==1) { c = m_nCols - c - 1; }
      return r*m_nCols + c;
    }
    
  public:
    Grid(int nRows, int nCols): m_nRows(nRows), m_nCols(nCols) { 
      m_cells = new CRGB[nRows*nCols];
      m_wcsLLx = m_wcsLLy = 0;
      m_wcsURx = m_wcsURy = 255;
      m_wcsRangex = m_wcsRangey = 255;
      m_wcsStepx = 255/nCols;
      m_wcsStepy = 255/nRows;
    }
    ~Grid() { delete[] m_cells; }

    void setWCS(gridwcs_t wcsLLx, gridwcs_t wcsLLy, gridwcs_t wcsURx, gridwcs_t wcsURy) {
      m_wcsLLx = wcsLLx; m_wcsLLy = wcsLLy; 
      m_wcsURx = wcsURx; m_wcsURy = wcsURy; 
      m_wcsRangex = wcsURx - wcsLLx;
      m_wcsRangey = wcsURy - wcsLLy;
      m_wcsStepx = m_wcsRangex/m_nCols;
      m_wcsStepy = m_wcsRangey/m_nRows;
    }

    void setPixel(gridwcs_t wcsX, gridwcs_t wcsY, int r, int g, int b) {
      int pixId;
      // convert to pix coord
      pixId = wcsToPixId(wcsX, wcsY);
      // write pixel
      m_cells[pixId] = CRGB(r, g, b);
    }
    void setPixel(gridwcs_t wcsX, gridwcs_t wcsY, CRGB rgb) {
      int pixId;
      pixId = wcsToPixId(wcsX,wcsY);
      m_cells[pixId] = rgb;
    }
   void setPixel(gridwcs_t wcsX, gridwcs_t wcsY, CHSV hsv) {
      int pixId;
      pixId = wcsToPixId(wcsX,wcsY);
      m_cells[pixId] = hsv;
    }
    void setPixelCr(int col, int row, int r, int g, int b) {
      int pixId;
      // convert to pix coord
      pixId =crToPixId(col, row);
      // write pixel
      m_cells[pixId] = CRGB(r, g, b);
    }
    void setPixelCr(int col, int row, CRGB rgb) {
      int pixId;
      pixId = crToPixId(col, row);
      m_cells[pixId] = rgb;
    }
   void setPixelCr(int col, int row, CHSV hsv) {
      int pixId;
      pixId =crToPixId(col, row);
      m_cells[pixId] = hsv;
    }
    gridwcs_t pixStepX() { return m_wcsStepx; }
    gridwcs_t pixStepY() { return m_wcsStepy; }

    void wcsToCr(gridwcs_t x, gridwcs_t y, int &c, int& r) {
      c = map(x, m_wcsLLx, m_wcsURx, 0, m_nCols);
      r = map(y, m_wcsLLy, m_wcsURy, 0, m_nRows);
    }
  void crToWcs(int c, int r, gridwcs_t& x, gridwcs_t& y) {
    x = map(c, 0, m_nCols, m_wcsLLx, m_wcsURx);
    y = map(r, 0, m_nRows, m_wcsLLy, m_wcsURy);
  }

  CRGB* theLeds() { return m_cells; }
};

#endif
