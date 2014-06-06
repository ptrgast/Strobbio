#include <strobbio.h>

Strobbio receiver = Strobbio(); //Create a receiver

void setup() {
  Serial.begin(115200);
}

void loop() {
  
  //Get receiver's status
  int strobbioStatus=receiver.getStatus(); 
  //Handle status
  if(strobbioStatus==STATUS_DATA) { //Data arrived
    StrobbioFrame* frame=receiver.getData();
    Serial.print("received frame: ");    
    frame->print();
    delete frame;
  } else if(strobbioStatus==STATUS_LOW_SAMPLERATE) { //Low sample rate. Strobbio will not work
    Serial.println("Low sample rate!");
  }
  
}
