#include "DaikinAirCon.h"

/********************************
Receiver functions for the DAIKIN protocol
It is using the timer 1 with interrupts every 50us
*********************************/
// Internal status of the receiver
typedef struct {
	uint8_t state;
	uint16_t ticks_space;
	uint16_t ticks_mark;
	uint8_t message_buff[RAWBUF];
	uint16_t message_len;
	uint8_t recvpin;
	bool message_started = false;
} irparams_t;
volatile irparams_t ir_params;

void receiver_start(int recvpin) {
	ir_params.recvpin = recvpin;
	pinMode(recvpin, INPUT);
	receiver_resume();
}

void receiver_stop() {
	ir_params.state = STATE_STOP;
	Timer1.detachInterrupt();
}

int receiver_status() {return ir_params.state;}

void receiver_resume() {
	ir_params.message_len = 0;
	ir_params.state = STATE_IDLE;
	ir_params.ticks_space = 0;
	ir_params.ticks_mark = 0;
	ir_params.message_started = false;
	Timer1.initialize(USECPERTICK);
	Timer1.detachInterrupt();
	Timer1.attachInterrupt(_receiver_ISR);
}

String receiver_decodeMessage() {
	String message;
	for (int i = 0; i < ir_params.message_len / 8; ++i) {
		if (ir_params.message_buff[i] < 16) // We want the full hex value (2 bytes)
			message += "0";
		message += String(ir_params.message_buff[i], HEX);
	}
	ir_params.state = STATE_DECODED;
	return message;
}

void append_message(byte value) {
	byte byte_index = ir_params.message_len / 8;
		if(ir_params.message_len % 8 == 0) // first time we write to this byte, we clean first
			ir_params.message_buff[byte_index] = 0;
		else
			ir_params.message_buff[byte_index] <<= 1;
		ir_params.message_buff[byte_index] += value;
		ir_params.message_len++;
}

void _receiver_ISR() {
	int receiver_value = digitalRead(ir_params.recvpin);
	switch (ir_params.state) {
		case STATE_IDLE:
			if (receiver_value == MARK)
				ir_params.state = STATE_MARK;
			break;
		case STATE_MARK:
			if (receiver_value == MARK)
				ir_params.ticks_mark++;
			else
				ir_params.state = STATE_SPACE;
			break;
		case STATE_SPACE:
			if (receiver_value == SPACE) {
				ir_params.ticks_space++;
				if (ir_params.ticks_space > TICKS_LOW(GAP)) {
					receiver_stop();
				}
			} else {
				if (MATCH_MARK(ir_params.ticks_mark, DAIKIN_START_MARK) &&
					MATCH_SPACE(ir_params.ticks_space, DAIKIN_START_SPACE)) {
				// we received the MESSAGE_STARTED signal
				ir_params.message_started = true;
				ir_params.message_len = 0;
				} else if (ir_params.message_started &&
						   MATCH_MARK(ir_params.ticks_mark, DAIKIN_ONE_MARK) &&
						   MATCH_SPACE(ir_params.ticks_space, DAIKIN_ONE_SPACE)) {
					// we received a ONE bit
					append_message(1);
				} else if (ir_params.message_started &&
						   MATCH_MARK(ir_params.ticks_mark, DAIKIN_ZERO_MARK) &&
						   MATCH_SPACE(ir_params.ticks_space, DAIKIN_ZERO_SPACE)) {
					// we received a ZERO bit
					append_message(0);
				} else {// does not match any known signal
					// ir_params.state = STATE_ERROR; NOT SUPPORTED YET
					// receiver_stop();
					break;
				}
				ir_params.ticks_space = 0;
				ir_params.ticks_mark = 0;
				ir_params.state = STATE_MARK;
			}
			break;
	}  // switch
}


/********************************
Receiver function for the DAIKIN protocol
It is using the timer 1 pwm
please check that you output on a TimerOne library compatible pin
*********************************/

int emitter_output_pin;

void mark(int length) {
  Timer1.pwm(emitter_output_pin, 512);
  delayMicroseconds(length);
  digitalWrite(emitter_output_pin, LOW);
}

void space(int length) {
  digitalWrite(emitter_output_pin, LOW);
  delayMicroseconds(length);
}

byte char2byte(char c) {
   if(c >= '0' && c <= '9')
     return (byte)(c - '0');
   else if (c >= 'a')
     return (byte)(c-'a'+10);
   else
     return (byte)(c-'A'+10);
}

void send_one_packet(String packet) {
  mark(DAIKIN_START_MARK); space(DAIKIN_START_SPACE); // Start packet
  for (int i = 0; i < packet.length(); ++i) {
    unsigned int current_value = char2byte(packet.charAt(i));
    for (int j = 3; j > -1; --j) { // iterate over 4 last bits
      // extract the j-th bit
      int b = ((current_value & 1<<j) >> j);
      if (b == 1) {  // we send a 1 bit
      	mark(DAIKIN_ONE_MARK); space(DAIKIN_ONE_SPACE);
      } else {  // we send a 0 bit
      	mark(DAIKIN_ZERO_MARK); space(DAIKIN_ZERO_SPACE);
      }
    }
  }
  mark(DAIKIN_ONE_MARK);  // single mark must be sent at the end of a packet
}

void emitter_init(int emitpin) {
	emitter_output_pin = emitpin;
	pinMode(emitter_output_pin, OUTPUT);
	digitalWrite(emitter_output_pin, LOW);
}

void emitter_send(String packet1, String packet2) {
	Timer1.detachInterrupt();
	Timer1.initialize(PWM_PERIOD);
	send_one_packet(packet1);
	delay(33);
	send_one_packet(packet2);
	Timer1.disablePwm(emitter_output_pin);
}
