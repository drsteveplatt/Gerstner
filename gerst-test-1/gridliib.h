//
// 2d graphics library on a grid
//

#if !defined(__GRIDLIB_INCLUDED__) 
#define __GRIDLIB_INCLUDED__

// A Grid has physical 0,0 in the lower left and logical (xmin ymin) in the lower left
// Note on a "standard" LED grid, the power will come in at the lower left

class Grid {
  private:
    int  m_nRows, m_nCols;  // slze of the grid
    CRGB* m_cells;
    float m_wcsLLx, m_wcsLLy, m_wcsURx, m_wcsURy;
    float m_wcsRangex, m_wcsRangey;

    int wcsToPix(float wcsX, float wcsY) {
      int r,c;
      if(m_wcsRangex==0.0f || m_wcsRangey==0.0f) return 0;
      r = (int)( ((wcsX-m_wcsLLx)/m_wcsRangex) * m_nRows);
      c = (int)( (wcsY-m_wcsLLy)/m_wcsRangey) * m_nCols);
      if((r&0x01)==1) { c = m_nCols - c - 1; }
      return r*GRIDSIZE + c;
    }
    
  public:
    Grid(int nRows, int nCols): m_nRows(nRows), m_nCols(nCols) { 
      m_cells = new CRGB[nRows*nCols];
      m_wcsLLx = m_wcsLLy = 0.0f;
      m_wcsURx = m_wcsURy = 1.0f;
      m_wcsRangex = m_wcsRangey = 1.0f;
    }
    ~Grid() { delete[] m_cells; }

    setWCS(float wcsLLx, float wcsLLy, float wcsURx, float wcsURy) {
      m_wcsLLx = wcsLLx; m_wcsLLy = wcsLLy; 
      m_wcsURx = wcsURx; m_wcsURy = wcsURy; 
      m_wcsRangex = wcsURx - wcsLLx;
      m_wcsRangey = wcsURy - wcsLLy;
    }

    setPixel(float wcsX, float wcsY, int r, int g, int b) {
      int pixId;
      // convert to pix coord
      pixId = wcsToPix(wcsX, wcsY);
      // write pixel
      m_cells[pixId] = CRGB(r,g,b);
    }
    setPixel(float wcsX, float wcsY, CRGB rgb) {
      int pixId;
      pixId = wcsToPix(wcsX,wcsY);
      m_cells[pixId] = rgb;
    }
  
};

#endif
