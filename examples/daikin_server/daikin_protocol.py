""" Interpreter of the daikin remote protocol """

MODE_BITS = {'AUTO': '000', 'DRY': '010', 'COOL': '110',
             'HEAT': '001', 'FAN': '011'}

FAN_SPEED_BITS = {'AUTO': '0101', 'NIGHT': '1101', '1': '1100',
                  '2': '0010', '3': '1010', '4': '0110', '5': '1110'}


def encode_int(value, nbits=None):
    """ Encode a integer following the DAIKIN protocol.

    Intergers are written in binary with the bits in a reverse order.
    """
    binary_value = bin(value)[2:]
    # print "Dec ", value, "Bin ", binary_value
    if nbits:
        binary_value = binary_value.zfill(nbits)
    return binary_value[::-1]


def decode_int(binary_value):
    """ Decode a integer following the DAIKIN protocol. """
    return int(binary_value[::-1], 2)


def checksum(code):
    """ Compute the checksum of a message. """
    print code
    bytes = [code[i:i+8] for i in range(0, len(code), 8)]
    sum_bytes = sum([decode_int(byte) for byte in bytes])
    return bin(sum_bytes).zfill(8)[-8:][::-1]


def verify_checksum(bin_code):
    """ Check that the checksum value is correct. """
    actual_checksum = bin_code[-8:]
    expected_checksum = checksum(bin_code[:-8])
    print actual_checksum
    print expected_checksum
    if actual_checksum != expected_checksum:
        raise Exception("CHECKSUM ERROR expected {} instead of {}".format(
                        expected_checksum, actual_checksum))


def decode_mode(mode_bits):
    """ Decode the mode used. """
    for mode, bits in MODE_BITS.iteritems():
        if bits == mode_bits:
            return mode
    return "UNKNOWN " + mode_bits


def encode_mode(mode):
    """ Return the mode bits value. """
    return MODE_BITS[mode]


def decode_fan_speed(fan_speed_bits):
    """ Decode the mode used. """
    for mode, bits in FAN_SPEED_BITS.iteritems():
        if bits == fan_speed_bits:
            return mode
    return "UNKNOWN " + fan_speed_bits


def encode_fan_speed(fan_speed):
    """ Return the fan speed bits value. """
    return FAN_SPEED_BITS[fan_speed]


def encode(state):
    """ Encode a message following DAIKIN remote protocol. """
    message = "1000100001011011111001000000000000000000"

    message += '1' if state.get("power", True) else '0'
    message += '1' if state.get("on_timer", True) else '0'
    message += '1' if state.get("off_timer", True) else '0'
    message += '1'  # Useless value
    message += encode_mode(state.get("mode", "AUTO"))
    message += '0'  # Useless value

    message += '0'  # Useless value
    message += encode_int(state.get("temperature", "25"), 6)
    message += '0'  # No use

    message += '00000000'  # No use

    message += '1111' if state.get("vertical_swing", False) else '0000'
    message += encode_fan_speed(state.get("fan_speed", 'AUTO'))
    message += '1111' if state.get("horizontal_swing", False) else '0000'
    message += '0000'

    message += '00000000'  #
    message += '01100000'  # We must add support for the timers
    message += '00000110'  #

    # We must add support for last bytes
    message += "0000000000000000100000110000100100000000"

    assert(len(message) == 18 * 8)  # The message must now be 18 bytes long
    message += checksum(message)
    return message


def decode(bin_code):
    """ Decode a DAIKIN remote message. """
    if len(bin_code) != 152:  # useful information is in this kind of message
        return 1
    verify_checksum(bin_code)
    state = {}
    # First 39 bits seems to always be :
    # 10001000 01011011 11100100 00000000 00000000

    state['power'] = bin_code[40] == '1'
    state['on_timer'] = bin_code[41] == '1'
    state['off_timer'] = bin_code[42] == '1'
    # No use for bit 43 must be 1
    state['mode'] = decode_mode(bin_code[44:47])
    # No use for bit 47 and 48 must be 0
    state['temperature'] = decode_int(bin_code[49:55])
    # No use for bytes 55 to 63 must be 0
    state['vertical_swing'] = bin_code[64:68] == '1111'
    state['fan_speed'] = decode_fan_speed(bin_code[68:72])
    state['horizontal_swing'] = bin_code[72:76] == '1111'
    # No use for bytes 76 to 80 must be 0
    return state


def hex2bin(hex_code):
    """ Convert a hex message into binary. """
    return bin(int(hex_code, 16))[2:]


def bin2hex(bin_code):
    """ Convert a hex message into binary. """
    return hex(int(bin_code, 2))[2:]

# if __name__ == "__main__":
#     import sys
#     if len(sys.argv) != 2:
#         print "Usage is {} BINARY_CODE".format(sys.argv[0])
#         sys.exit(1)
#     else:
#         hex_code = sys.argv[1]
#         num_of_bits = len(hex_code) * 4
#         bin_code = bin(int(hex_code, 16))[2:].zfill(num_of_bits)
#         decode(bin_code)
#         sys.exit(0)

# if __name__ == "__main__":
#     import serial
#     ser = serial.Serial('/dev/ttyACM5', 115200)
#     line = ser.readline()
#     hex_code = line.replace('\n', '')
#     hex_code = hex_code.replace('\r', '')
#     print "Received ", hex_code
#     num_of_bits = len(hex_code) * 4
#     bin_code = bin(int(hex_code, 16))[2:].zfill(num_of_bits)
#     state = decode(bin_code)
#     print state
#     for k, v in state.items():
#         print "{}: {}".format(k, v)
#     print " "
#     print "After re encoding : "
#     print encode(state)
