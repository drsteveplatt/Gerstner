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
    
  public:
    Grid(int nRows, int nCols): m_nRows(nRows), m_nCols(nCols) { m_cells = new CRGB[nRows*nCols]; }
    ~Grid() { delete[] m_cells; }

    setWCS(float wcsLLx, float wcsLLy, float wcsURx, float wcsURy) {
      m_wcsLLx = wcsLLx; m_wcsLLy = wcsLLy; m_wcsURx = wcsURx; m_wcsURy = wcsURy; 
    }

    setPixel(float wcsX, float wcsY, int r, int g, int b) {
      int px, py;
      // convert to pix coord
      px = (wcsX-m_wcsLLx)/(m_wcsURx-m_wcsURx) * m_nCols;
      py = (wcxY-m_wcsLLy)/(m_wcsURy-m_wcsURy) * m_nRows;
      // write pixel
      m_cells(
    }
  
};

#endif
