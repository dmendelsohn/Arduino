#include "Communication.h"

#include "Congig.h"
#include <PacketSerial.h>

void Communication::Communication(){
  packet_serial.setPacketHandler(&_packet_handler);
  packet_serial.begin(115200);
  Timer1.initialize(US_PER_FRAME);
  Timer1.attachInterrupt(&checkUpdate);
}
void setUpdate(){
  updateSerial = true;
}
boolean Communication::checkUpdate(){
  if (updateSerial){
    packet_serial.update();
    updateSerial = false;
    return true;
  }
  return false;
}
void Communication::sendFrame(byte* frame){
  packet_serial.send(frame, BYTES_PER_FRAME);
}

