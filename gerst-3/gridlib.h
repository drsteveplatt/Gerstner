//
// 2d graphics library on a grid
// Classes to manage WCS-DCS transformation (unrotated wcs) and 2D LED arrays
//
// GridWorld -- defines WCS and has mapping functions between WCS (xy) and DCS (cr) coordinates
// Grid -- manages a single FastLED CRGB grid as an rc environment
//

#if !defined(__GRIDLIB_INCLUDED__) 
#define __GRIDLIB_INCLUDED__

#include <Streaming.h>

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

template<int paneColsPerGrid, int paneRowsPerGrid> class Grid {
  private:
    uint16_t m_pixColsPerPane, m_pixRowsPerPane;   // slze of the each pane in pixels [ [0 m_panePixCols][0 m_panePixRows] ]
    uint16_t m_paneColsPerGrid, m_paneRowsPerGrid;    // size of the grid in panes [ [0 m_gridPaneCols][0 m_gridPaneRows] ]
    CRGB* m_cells;
    // calculated values (pixel counts)
    uint16_t m_pixColsPerGrid, m_pixRowsPerGrid, m_pixPerGrid, m_pixPerPane;   // total number of cols, rows, pixels (whole grid)
  public:
    Grid(int pixColsPerPane, int pixRowsPerPane, CRGB* leds): 
        m_pixColsPerPane(pixColsPerPane), m_pixRowsPerPane(pixRowsPerPane),
        m_paneColsPerGrid(paneColsPerGrid), m_paneRowsPerGrid(paneRowsPerGrid) {  
        m_cells = leds;
        m_pixColsPerGrid = m_pixColsPerPane*m_paneColsPerGrid;
        m_pixRowsPerGrid = m_pixRowsPerPane*m_paneRowsPerGrid;
        m_pixPerGrid = m_pixColsPerPane*m_pixRowsPerPane;
        m_pixPerPane = m_pixColsPerPane*m_pixRowsPerPane;
      }
    ~Grid() {  }

    void dump() {
      Serial << "Dump: pixC/RPerPane: " << m_pixColsPerPane << ' ' << m_pixRowsPerPane 
        << " paneCRPerGrid: " << m_paneColsPerGrid << ' ' << m_paneRowsPerGrid
        << " pixCRPerGrid: " << m_pixColsPerGrid << ' ' << m_pixRowsPerGrid
        << " pixPerGrid: " << m_pixPerGrid << " pixPerPane: " << m_pixPerPane << endl;
    }

    int crToPixId(int absC, int absR) {
      uint16_t paneInGrid;    // which pane it is in (block of LEDs) (aka pig)
      uint16_t pigRow, pigCol;    // which pane row/col
      uint16_t pixInPane;     // which pixel in the pane (aka pip)
      uint16_t pipRow, pipCol;
      if(absC<0 || absC>=m_pixColsPerGrid || absR<0 || absR>=m_pixRowsPerGrid) return 0;
      // calculate pane and pix in pane
      paneInGrid = (absR*m_pixColsPerGrid + absC) / m_pixPerPane;
      pixInPane = (absR*m_pixColsPerGrid + absC) % m_pixPerPane;
      // calculate pane row/col, account for col reversal on odd rows
      ///pigRow = paneInGrid / m_paneColsPerGrid;
      ///pigCol = paneInGrid % m_paneColsPerGrid;
      pigCol = absC / m_pixColsPerPane;
 //     Serial << "  ...absC: " << absC << " m_pixColsPerPane: " << m_pixColsPerPane << " pigCol: " << pigCol << endl;
      pigRow = absR / m_pixRowsPerPane;
//      Serial << "  ...absR: " << absR << " m_pixRowsPerPane: " << m_pixRowsPerPane << " pigRow: " << pigRow<< endl;
      if((pigRow&0x01)==1) pigCol = m_paneColsPerGrid - pigCol - 1;
      // calculate pixle in pane, account for col reversal on odd rows
      pipCol = absC % m_pixColsPerPane; // these were pixInPanel
      pipRow = absR % m_pixRowsPerPane;
      if((pipRow&0x01)==1) pipCol = m_pixColsPerPane - pipCol - 1;
      // calculate absolute pix id and return
      uint16_t ret;
      ret = pigRow*m_paneColsPerGrid*m_pixPerPane + pigCol*m_pixPerPane
        + pipRow*m_pixColsPerPane + pipCol;
//      Serial << "  crToPixId cr: " << absC << ' ' << absR<< " panel cr: " << pigCol << ' ' << pigRow << " pixel cr: " << pipCol << ' ' << pipRow << " pixelId: " << ret << endl;
      return  ret;
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

    void getCR(int& c, int& r) { c=m_pixColsPerPane*m_paneColsPerGrid; r=m_pixRowsPerPane*m_paneRowsPerGrid; }
  
    CRGB* theLeds() { return m_cells; }
    CRGB& led(int i) { return m_cells[i]; }
    //CRGB& led(int c, int r) { return m_cells[r*m_nCols + c]; }
  
};

template<> class Grid<1,1> {
  private:
    int  m_nRows, m_nCols;  // slze of the grid
    //GridWorld m_world;
    CRGB* m_cells;

  public:
    Grid(int nRows, int nCols, CRGB* leds): m_nRows(nRows), m_nCols(nCols) {  m_cells = leds;   }
    ~Grid() {  }

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

    void getCR(int& c, int& r) { c=m_nCols; r=m_nRows; }
  
    CRGB* theLeds() { return m_cells; }
    CRGB& led(int i) { return m_cells[i]; }
    //CRGB& led(int c, int r) { return m_cells[r*m_nCols + c]; }
  
};

#endif
