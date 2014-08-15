#include <TimerOne.h>
#include <DaikinAirCon.h>

#define EMIT_PIN 9

const String MESSAGE_HEADER = "885be400800000000000000000000000000000c8";
const String MESSAGE2       = "885be400009c2c00050000600600008309006b";

void setup() {
	emitter_init(EMIT_PIN);
	emitter_send(MESSAGE_HEADER, MESSAGE2);
}

void loop()
{
	// we do nothing more than sending one message on boot
}
