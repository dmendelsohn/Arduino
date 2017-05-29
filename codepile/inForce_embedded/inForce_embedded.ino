/* inForce_embedded
 * Part of the inForce project by Daniel Fitzgerald, Luke Vink, Tangible Media Group, MIT Media Lab
 * This is the embedded code for the 10 arduino Teeneys on inForce.
 */

#include "Config.h"
#include "Communication.h"
#include "Test.h"

Communication communication;

frame_t outFrame;
frame_t inFrame;

void setup() {
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if(communication.checkUpdate()){
    computeTestFrame();
    communication.sendFrame();
  }
}
