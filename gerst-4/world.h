//
// world.h
//  All of the stuff needed to define the world
//
// 1. Geometry of the matrix and tiles
// 2. Globals for cLEDMatrixWcs and cLEDMatrixData
//

#if !defined(__WORLD__)
#define __WORLD__ true

#if defined(__MAIN__)
#define EXTERN
#else
#define EXTERN extern
#endif

// size of each tile in pixels
#define MATRIX_TILE_WIDTH 16
#define MATRIX_TILE_HEIGHT 16
#define MATRIX_TILE_SIZE  (MATRIX_TILE_WIDTH*MATRIX_TILE_HEIGHT)
#define MATRIX_TYPE                 HORIZONTAL_ZIGZAG_MATRIX

// grid dimensions in panels
#define MATRIX_TILE_COLS 2
#define MATRIX_TILE_ROWS 2
#define MATRIX_TILE_TYPE         HORIZONTAL_ZIGZAG_BLOCKS

// grid dimension in pixels
#define MATRIX_WIDTH (MATRIX_TILE_WIDTH*MATRIX_TILE_COLS)
#define MATRIX_HEIGHT (MATRIX_TILE_HEIGHT*MATRIX_TILE_ROWS)

// other useful things
#define MATRIX_SIZE (MATRIX_WIDTH*MATRIX_HEIGHT)

//EXTERN CRGB theLeds[MATRIX_SIZE];

//Grid<GRIDPANELCOLS, GRIDPANELROWS> grid(PANELPIXELCOLS, PANELPIXELROWS, theLeds);
//GridData<int32_t> accum;
// WCS definition
#define PI 3.1415926f
#define LL_X    (-PI/2)
#define LL_Y    (-PI/2)
#define UR_X   (PI/2)
#define UR_Y    (PI/2)

// How we're storing the per-pixel data
typedef int32_t AccumType_t;

// use -tile_height if the tiles are mounted first-row-down
// use -tile_rows if the first tile is on the bottom.
EXTERN cLEDMatrixWcs<MATRIX_TILE_WIDTH, -MATRIX_TILE_HEIGHT, MATRIX_TYPE, 
  MATRIX_TILE_COLS, -MATRIX_TILE_ROWS, MATRIX_TILE_TYPE> theLeds;

// and a forward decl for the color calculator
EXTERN CRGB color(AccumType_t f, float dmin, float dmax);

template<int tWidth, int tHeight, typename tData> class AppData: public cLEDMatrixData<tWidth, tHeight, tData> {
  public:
     CRGB toRgb(int x, int y) override {
      // we are not using toRgb
      CRGB newCol(255,255,255);
      //newCol = color(this->operator()(x,y), this->rangeMin(), this->rangeMax());
      newCol.r = gamma8(newCol.r);
      newCol.g = gamma8(newCol.g);
      newCol.b = gamma8(newCol.b);
      return newCol;
    };
};
EXTERN AppData<MATRIX_WIDTH, MATRIX_HEIGHT, AccumType_t> theData;


#endif // !defined(__WORLD__)