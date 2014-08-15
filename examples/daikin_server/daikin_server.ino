/*
 * DaikinAirCon: daikin_python - sending and receiving commands in python
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * An IR Transmitter must be connected to the output EMIT_PIN
 *
 * The followings commands can be sent (they msut end with '\n'):
 * SEND PACKET1 PACKET2   =>   Emits the two packets
 * STOP                   =>   Stop the receiver
 * LISTEN N               =>   Send over Serial the N next packets received
 *                             if N < 0 LISTEN UNTIL stopped
 */

#include <TimerOne.h>
#include <DaikinAirCon.h>

#define RECV_PIN          4        // The pin connected to the IR receiver
#define EMIT_PIN          9        // The pin commanding the IR transmitter
#define COMMAND_BUFF_LEN  100      // The number of bytes reserved

String command_buffer = "";        // a string to hold incoming commands
boolean command_received = false;  // whether the string is complete
int next_packets = -1;             // Number of packets that we want to send

void setup() {
  Serial.begin(9600);
  emitter_init(EMIT_PIN);
  receiver_start(RECV_PIN);
  command_buffer.reserve(COMMAND_BUFF_LEN);
  Serial.println("Starting Daikin Receiver");
}

// Return the n-th argument, separated by a space
String getArg(const String command, int index, int start_pos = 0) {
  int next_space_index = command.indexOf(' ', start_pos);
  if (next_space_index == -1) {
    return command.substring(start_pos);
  } else if (index > 0) {
    return getArg(command, index - 1, next_space_index+1);
  } else {
    return command.substring(start_pos, next_space_index);
  }
}

void loop() {

  // Parse the command that has been received
  if (command_received) {
    String command = getArg(command_buffer, 0);
    if (command == "SEND") {  // We received the send command
      emitter_send(getArg(command_buffer, 1), getArg(command_buffer, 2));
      receiver_resume();  // Sending a packet disable the receiver.
    } else if (command == "STOP") {
      next_packets = 0;
    } else if (command == "LISTEN") {
      next_packets = getArg(command_buffer, 1).toInt();
      receiver_resume();
    } else {
      command_buffer = "ERROR UNKNOWN COMMAND";
    }
    Serial.println(command_buffer);
    // clear the string:
    command_buffer = "";
    command_received = false;
  }

  // If we have received a daikin code, we send it over Serial
  if (next_packets != 0 && receiver_status() == STATE_STOP) {
    String message = receiver_decodeMessage();
    if (message.length() > 0) {
      Serial.print("Message: ");
      Serial.println(message);
      if (next_packets > 0)
        next_packets--;
    }
    if (next_packets != 0)
      receiver_resume();
  }
}


// Process the incomming bytes in Serial
// Not inside the loop in order to keep a fast loop function
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') { // End of command char
      command_received = true;
      return;
    } else {
      command_buffer += inChar;
    }
  }
}
