#ifndef DaikinHelper_h
#define DaikinHelper_h

// Duration of the signal as measured with our oscilloscope
#define DAIKIN_ONE_SPACE    1280
#define DAIKIN_ONE_MARK     428
#define DAIKIN_ZERO_MARK    428
#define DAIKIN_ZERO_SPACE   428
#define DAIKIN_START_MARK   DAIKIN_ZERO_MARK*8
#define DAIKIN_START_SPACE  DAIKIN_ZERO_MARK*4
#define GAP                 5000  // Minimum gap between transmissions

// Helpers used when decoding the message
#define USECPERTICK 50  // microseconds per clock interrupt tick
#define RAWBUF 20 // Length of bytes buffer for DAIKIN protocol (max 19 used)
#define MARK_EXCESS 100
#define TOLERANCE 25  // percent tolerance in measurements
#define LTOL (1.0 - TOLERANCE/100.)
#define UTOL (1.0 + TOLERANCE/100.)
#define TICKS_LOW(us) (int) (((us)*LTOL/USECPERTICK))
#define TICKS_HIGH(us) (int) (((us)*UTOL/USECPERTICK + 1))
#define MATCH(measured, desired) (measured >= TICKS_LOW(desired) && measured <= TICKS_HIGH(desired))
#define MATCH_MARK(measured_ticks, desired_us) MATCH(measured_ticks, (desired_us + MARK_EXCESS))
#define MATCH_SPACE(measured_ticks, desired_us) MATCH(measured_ticks, (desired_us - MARK_EXCESS))

// Helpers used for sending messages
#define PWM_PERIOD 27  // 37kHz

// IR detector output is active low
#define MARK  LOW
#define SPACE HIGH

// receiver states
#define STATE_ERROR       0
#define STATE_DECODED     1
#define STATE_IDLE        2
#define STATE_MARK        3
#define STATE_SPACE       4
#define STATE_STOP        5

#endif  // DaikinHelper_h
