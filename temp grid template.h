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

// Grid classes
//    Grid -- base class, defines routines and core structures
//      This class is used for general operations
//    GridPane -- a single cxr sheet of LEDs.
//      It extends Grid
//    GridMultiPane -- a cg x rg array of cp x rp sheets of LEDs
//      It extends Grid
//      All of the panes must be the same size.
//    Any operations/code should reference Grid.  Once a GridPane/GridMultiPane has been
//      defined, it can be passed to anything as a Grid.
//  Note in V3, the led array must be externally declared.

// Note in arrays of panels
//    pane[0 0] is in the lower left
//    even pane rows go left->right
//    odd pane rows go right->left
//  and each pane follows the led ordering of a single paned grid
template<int gridCols, int gridRows> class Grid {
  private:
    int m_colsPerPane, m_rowsPerPane;      // pixels in the col row for each pane
    int m_gridCols, m_gridRows;         // number of panes in each col/row of the array
    CRGB* m_leds;
  public:
    Grid(int colsPerPane, int rowsPerPane, CRGB* leds):
      m_gridCols(gridCols), m_gridRows(gridRows), 
      m_colsPerPane(paneCols, m_rowsPerPane(paneRows), m_leds(leds) { }
     int crToPixId(int col, int row) {
       if(col<0 || col>=m_paneCols*m_gridCols || row<0 || row>=m_paneRows*m_gridRows) return 0;
       // 1. Determine which pane it is in & the col/row in the pane
       uint16_t paneRow, paneCol;       // identify which pane it is in
       uint16_t rowInPane, colInPane;   // location in the pane
       paneRow = row/m_rowsPerPane; rowInPane = row%m_rowsPerPane;
       paneCol = col/m_colsPerPane; colInPane = col%m_colsPerPane;
       // adjust for reverse order of odd-numbered rows (both panes and rows in pane)
       if((paneRow&0x01)==1) { paneCol = m_gridCols - paneCol - 1; }
       if(rowInPane&0x01)==1) { colInPane = m_colsPerPane - colInPane - 1; }
       return paneRow*m_colsPerPane*m_rowsPerPane*m_gridCols +  paneCol*m_colsPerPane*m_rowsPerPane 
                + rowInPane*m_colsPerPane + colInPane;
       //return row*m_nCols + col;
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
     CRGB& led(int i) { return m_leds[i]; }
     CRGB& led(int col, int row) {
      int pixId;
      pixId = crToPixId(col, row);
      return m_leds[pixId];
     }  
};

#if false
// Note in a single pane
//    led[0 0] is in the lower left
//    even rows go left->right
//    odd rows go right->left
template<> class Grid<1,1> {
  private:
    uint16_t m_nCols, m_nRows;                // size of each pane
    CRGB* m_leds;
  public:
    Grid(int paneCols, int paneRows, CRGB* leds):
      m_nCols(paneCols), m_nRows(paneRows), m_leds(leds) { }
     int crToPixId(int col, int row) {
       if(col<0 || col>=m_nCols || row<0 || row>=m_nRows) return 0;
       if((row&0x01)==1) { col = m_nCols - c - 1; }
       return row*m_nCols + col;
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
     CRGB& led(int i) { return m_leds[i]; }
     CRGB& led(int col, int row) {
      int pixId;
      pixId = crToPixId(col, row);
      return m_leds[pixId];
     }  
};

#endif

#if defined(OLDGRID)
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
#endif  // defined(OLDGRID)

#endif  // !defined(__GRIDLIB_INCLUDED__)