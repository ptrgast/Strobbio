/*
Strobbio Receiver

Copyright (c) 2014 ptrgast

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "strobbio.h"
#define DEBUG
////// StrobbioSettings //////

StrobbioSettings::StrobbioSettings() {
	oneInterval=280;
	zeroInterval=100;
	pulseInterval=80;
	frameLength=12;
	evenParity=true;
	inputPin=A5;
	maxSamplePeriod=pulseInterval/4.0f;
}

////// StrobbioFrame //////

StrobbioFrame::StrobbioFrame() {
	data=0;
	index=0;
	StrobbioSettings settings=StrobbioSettings();
	length=settings.frameLength;
	evenParity=settings.evenParity;
}

StrobbioFrame::StrobbioFrame(StrobbioSettings settings) {
	data=0;
	index=0;
	length=settings.frameLength;
	evenParity=settings.evenParity;
}

long StrobbioFrame::read(int start, int length) {
	if(start<0||length<1||(start+length)>this->length) {return -1;} //invalid arguments
	int end=this->length-1-(start+length-1);
	long output=data>>end;
	long mask=(1<<length)-1;
	return output & mask;
}

void StrobbioFrame::setBit(int index) {
	if(index<0||index>=length) {return;} //invalid arguments
	int realIndex=length-1-index;
	data=data|(1<<realIndex);
}

void StrobbioFrame::addBit(boolean value) {
	if(index<length) {
		if(value) {setBit(index);}
		index++;		
	}	
}

boolean StrobbioFrame::isFull() {
	if(index==length) {return true;}
	else {return false;}
}

boolean StrobbioFrame::isIntegral() {
	if(!isFull()) {return false;}
	int counter=0;
	for(int i=0;i<length;i++) {if(read(i,1)==1) {counter++;}}
	if(!evenParity) {counter++;}
	if(counter%2==0) {return true;}
	else {return false;}
}

void StrobbioFrame::clear() {
	data=0;
	index=0;
}

void StrobbioFrame::print() {
	char output[length+1];
	for(int i=0;i<index;i++) {output[i]=read(i,1)==1?'1':'0';}
	for(int i=index;i<length;i++) {output[i]='-';}
	output[length]=0;
	Serial.println(output);
}

////// Strobbio //////

Strobbio::Strobbio() {
	settings=StrobbioSettings();
	frame=new StrobbioFrame(settings);
	previousState=INPUT_UNDEFINED;
	previousTime=millis();
	previousInput=analogRead(settings.inputPin);
	previousDelta=0;
	bitStart=0;
	buffer=new int[settings.frameLength];
	bufferIndex=0;
	border=(settings.oneInterval+settings.zeroInterval+settings.pulseInterval*2)/2;
}

Strobbio::Strobbio(StrobbioSettings settings) {
	this->settings=settings;
	frame=new StrobbioFrame(settings);
	previousState=INPUT_UNDEFINED;
	previousTime=millis();
	previousInput=analogRead(settings.inputPin);
	previousDelta=0;
	bitStart=0;
	buffer=new int[settings.frameLength];
	bufferIndex=0;
	border=(settings.oneInterval+settings.zeroInterval+settings.pulseInterval*2)/2;
}

Strobbio::~Strobbio() {
	delete buffer;
}

int Strobbio::getStatus() {
	unsigned int currentTime=millis();
	unsigned int period=currentTime-previousTime;	
	if(period>settings.maxSamplePeriod) {return STATUS_LOW_SAMPLERATE;} //check sampling period
	
	if(period>=4) {
		int input=analogRead(settings.inputPin);
		int delta=input-previousInput;
		int direction=(delta-previousDelta>0)?1:-1;
		previousDelta=delta;
		previousInput=input;	
		previousTime=currentTime;
		
		int bitDuration=currentTime-bitStart;
		
		if(delta>10&&direction==1&&bitStart==0) {
			bitStart=currentTime;
		} else if(delta>10&&direction==1&&bitStart!=0&&bitDuration>settings.pulseInterval) {
			bitStart=currentTime;			
			buffer[bufferIndex++]=bitDuration;
			if(bufferIndex>=settings.frameLength) {
				bufferIndex=0;
				for(int i=0;i<settings.frameLength;i++) {
					if(buffer[i]>border) {frame->addBit(true);}
					else if(buffer[i]>settings.zeroInterval) {frame->addBit(false);}
				}
			}
		} else if(bitDuration>2*settings.oneInterval) {
			#ifdef DEBUG
			if(bufferIndex>0) {
				Serial.print("border: ");
				Serial.println(border);
				Serial.print("intervals: ");
				for(int i=0;i<bufferIndex;i++) {Serial.print(buffer[i]);Serial.print(" ");} 
				Serial.println();
			}
			#endif			
			bitStart=0;
			bufferIndex=0;
			frame->clear();
		}
	}

	if(frame->isFull()&&frame->isIntegral()) {return STATUS_DATA;}
	#ifdef DEBUG
	else if(frame->isFull()) {Serial.print("invalid frame:");frame->print();frame->clear();}
	#endif
	else {return STATUS_WAITING;}
}

StrobbioFrame* Strobbio::getData() {
	StrobbioFrame* tmp=frame;
	frame=new StrobbioFrame(settings);
	return tmp;
}
