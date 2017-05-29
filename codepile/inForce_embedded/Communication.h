#ifndef COMMUNICAITON_H
#define COMMUNICATION_H

// Using this library for Consistent Overhead Byte Stuffing (COBS) over serial.
// https://github.com/bakercp/PacketSerial

#define BYTES_PER_POSITION sizeof(POSITION_TYPE)
#define BYTES_PER_FORCE 2 sizeof(FORCE_TYPE);
#define BYTES_PER_TOUCH sizeof(TOUCH_TYPE);

#define BYTES_PER_FRAME (PINS_PER_TEENSEY * (BYTES_PER_POSITION + BYTES_PER_FORCE + BYTES_PER_TOUCH));

#define FRAMES_PER_SECOND 60
#define US_PER_FRAME (1000/FRAMES_PER_SECOND)

class Communication{
  public:
    Communication();
    sendFrame();
  private:
    PacketSerial packet_serial;
    void packetHandler();
    void setUpdate();
    boolean checkUpdate();
    volatile boolean updateSerial= false;
}

#endif
