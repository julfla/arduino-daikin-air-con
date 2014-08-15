#ifndef DaikinAirCon_h
#define DaikinAirCon_h

#include <WString.h>
#include <TimerOne.h>
#include "helper.h"

/********************************
Receiver functions for the DAIKIN protocol
It is using the TimerOne with interrupts every 50us
*********************************/
void receiver_start(int recvpin);
void receiver_resume();
void receiver_stop();
int receiver_status();
String receiver_decodeMessage();
void _receiver_ISR();

/********************************
Emitter functions for the DAIKIN protocol
It is using the TimerOne PWM
You must use an output pin compatible with the TimerOne library
For instance 8,9 one the arduino uno
*********************************/
void emitter_init(int emitpin);
void emitter_send(String packet1, String packet2);

#endif  // DaikinAirCon_h
