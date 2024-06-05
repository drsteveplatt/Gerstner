//
// Calculating servo values for each panel
//
#if defined(USING_SERVOS)
#if !defined(__SERVOCALC)
#define __SERVOCALC

// define servos and their directions
struct ServoInfo{
  uint_t m_pin;
  uint_t m_down, m_up;
  uint32_t val(uint32_t a) { return map(a, 0, 90, m_down, m_up); }
};

#define NUMSERVOS 4

#if !defined(__MAIN__)
extern
#endif
ServoInfo theServoInfo[NUMSERVOS]
#if defined(__MAIN__) 
  = { { 16, 0, 90 }, { 17, 0, 90}, { 18, 180, 90 }, { 19, 180, 90 }  }
#endif
;

#if !defined (__MAIN__)
extern
#endif
Servo theServos[NUMSERVOS];

int calcServoVal(uint_t firstLed, uint_t lastLed) ;
#endif // !defined(SERVOCALC)

#endif // defined(USING_SERVOS)
