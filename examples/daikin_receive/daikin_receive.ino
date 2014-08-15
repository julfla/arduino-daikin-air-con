#include <TimerOne.h>
#include <DaikinAirCon.h>

#define RECV_PIN 4
#define STATUS_LED 2

void setup()
{
  Serial.begin(9600);
  receiver_start(RECV_PIN);
  pinMode(STATUS_LED, OUTPUT);
  Serial.println("Starting Daikin Receiver");
}

void loop()
{
  // digitalWrite(STATUS_LED, !digitalRead(RECV_PIN));
  if (receiver_status() == STATE_SPACE)
    digitalWrite(STATUS_LED, HIGH);
  else
    digitalWrite(STATUS_LED, LOW);
  if (receiver_status() == STATE_STOP) {
    String message = receiver_decodeMessage();
    // if (message.length() > 0) {
      Serial.print("Message: ");
      Serial.println(message);
    // }
    receiver_resume();
  }
  if (receiver_status() == STATE_ERROR) {
    Serial.println("Error on reception restarting acquisition.");
    receiver_resume();
  }
}
