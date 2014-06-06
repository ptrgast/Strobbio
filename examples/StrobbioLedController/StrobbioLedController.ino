#include <strobbio.h>

#define MODE_INOUT 0
#define MODE_SAWTOOTH 1
#define MODE_RANDOM 2
#define PERIOD 20000
#define PERIOD_STEP 2700

Strobbio receiver = Strobbio(); //Create a receiver

int ledA=9;
int ledB=10;
int modeA=MODE_INOUT;
int modeB=MODE_INOUT;
int periodA=PERIOD;
int periodB=PERIOD;
int valueA=0;
int valueB=0;
long t=0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  //calculate time
  t++;
  int tA=t%periodA;
  int tB=t%periodB;
  //calculate values
  calculateValue(&valueA,tA,periodA,modeA);
  calculateValue(&valueB,tB,periodB,modeB);
  //output values
  analogWrite(ledA,valueA);
  analogWrite(ledB,valueB);
  
  //Get receiver's status
  int strobbioStatus=receiver.getStatus(); 
  //Handle status
  if(strobbioStatus==STATUS_DATA) { //Data arrived
    StrobbioFrame* frame=receiver.getData();
    modeA=frame->read(0,2);
    modeB=frame->read(2,2);
    periodA=PERIOD-frame->read(4,3)*PERIOD_STEP;
    periodB=PERIOD-frame->read(7,3)*PERIOD_STEP;
    Serial.print("received: ");
    frame->print();
    delete frame;
  } else if(strobbioStatus==STATUS_LOW_SAMPLERATE) { //Low sample rate. Strobbio will not work
    Serial.println("Low sample rate!");
  }
  
}

void calculateValue(int* value, int t, int period, int mode) {  
  switch(mode) {
    case MODE_INOUT:
      if(t<=period/2) {*value=(int)(2*t/(float)period*255);}
      else {*value=(int)(255-((2*(t-period/2))/(float)period*255));}
      break;
    case MODE_SAWTOOTH:
      *value=(int)(t/(float)period*255);
      break;
    case MODE_RANDOM:
      if(t%(period/2)==0) {*value=random(0,256);}
      break;
  }
}

