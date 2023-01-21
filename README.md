# Gerstner
**This is a pre-release version of a Gerstner wave simulation for LED grids**

This version is meant for initial experimentation.  It is not a releasable product.
## Overview
This package implements a limited Gerstner wave simulation on 2D WS2811 LED grids.

Gerstner waves use multiple tuned sin waves to simulate ocean/water surfaces.  This package allows 
the user to define any number of individual waves.  Each wave has a defined wavelength and "steepness".
Steepness is a measure of how broad troughs will be and how steep wave peaks will be.  Values should be
in the range [0.0 1.0], where 0.0 will be flat.  We recommend values around 0.4-0.5.

The package is composed of two libraries: grid and gerstner.  Grid allows the definition of a 2D world using
an array of LED grids.  It also allows the definition of a world coordinate system (WCS), letting the user use 
arbitrary coordinates for the waves instead of pixel coordinates.  The grid is addressed using col/row indices,
and each grid location stores a single int32_t (height/z) value.  Functions are provided to map between LED col/row
addresses and WCS coordinates.

The gerstner library allows the definition of gerstner waveforms, and contains a procedure to incrementally
compute the waveform values in the defined grid space, adding the new waveform to a defined grid. 

## Version
- 2023-0120 Version 0.3.  Pre-release version.  Functional, but needs cleanup, documentation, and more testing.

## Assumed Hardware
The implementation assumes using an ESP32-class processor as a computing platform.  Is uses the FastLED library
to manage the LED grid.

Terminology:

- Grid refers to the entire LED space, set up as an array of LED panels.

- Pane refers to an individual panel (typically 16x16 or 8x32 LEDs) in the grid.
  
This implementation assumes the use of NxM panes of independently-addressable WS2811 LEDs.  All panes in
the grid must be the same size and have the same orientation.

Each pane is oriented with LED[0] in the lower left, with LED[0..rowsize] along the lower edge.  It assumes
that even-numbered rows (0, 2, 4, ...) increment left-to-right and odd-numbered rows (1, 3, 5...) increment
right-to-left.

Multiple panes can be used.  They are oriented similarly, with the first pane in the lower left and subsequent 
panes placed similarly to LEDs in a single pane (going left-to-right on even-numbered rows and right-to-left on
odd-numbered rows).

The panes can either be wired in sequence (DOUT of one pane connected to the DIN of the next pane), or they can 
have the DIN lines of each pane connected to distinct pins on the host microcontroller.

## Grid library

The Grid library (gridlib.h) defines two classes used to manage a user pixel space on an array of LED panes.

- GridWorld.  A world coordinate system for use by anything operating on the LED grid.  It includes functions
  for specifying the WCS (user) space and to map from WCS to grid column/row coordinates.
  
- template<pCols, pRows> Grid.  A physical grid of [pCols pRows] panes of LEDs.  Includes functions to map from
  total-device [col row] address to absolute LED address as well as get/set individual LED values.  Note there is an
  optimized version of this for single-pane grids.  It may be out of date in the prerelease version.

## Gerstner library

The Gerstner library (gerstner.cpp, gerstner.h) defines classes used to implement a 2D height surface and apply
Gerstner waves to the surface.

- GridHeight.  A 2D array of integer values used to store point-heights.
  
- GerstWave.  A single Gerstner wave.  It will have a duration (how long it lasts, in ms), a wavelength (distance
  between crests in WCS units), an angle (which way it faces and flows), and a sharpness (how narrow/pointy the peaks are).
  
  - The angle is in FastLED angles (0..65535), with 0 being "the peak runs up rows and it will move to the right",
  with increasing angles moving counterclockwise.
  
  - The sharpness varies from 0.0 to 1.0 (preferably), with smaller numbers being flatter
 
  - The wave will compute its own speed and amplitude based on the properties of water.
  
  - Duration is how long the wave will last (in ms).  It will increase in amplitude during the first quarter of this time
  and decrease during the last quarter.  After 'duration' ms has passed, the wave will automatically be re-created with new
  parameters.

## Test application

A single application (gerst-3.ino) is included.  It defines four waves with differing properties in setup().  The loop()
procedure calls the calc() method of each wave, then false-colors the resultant heightfield into the LED array.

## Next steps

  1. Clean up the code.
  2. Performance improvements?
  3. Document (doxygen) the class libraries.
  4. Added examples with different color maps?
  


