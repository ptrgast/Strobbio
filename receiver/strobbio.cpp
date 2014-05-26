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

////// StrobbioSettings //////

StrobbioSettings::StrobbioSettings() {
	this->oneInterval=180;
	this->zeroInterval=60;
	this->separatorInterval=120;
	this->frameLength=12;
	this->eventParity=true;
	this->inputPin=A5;
	this->minSampleRate=50;
}

////// StrobbioFrame //////

StrobbioFrame::StrobbioFrame() {
	data=0;
	length=StrobbioSettings().frameLength;
}

StrobbioFrame::StrobbioFrame(StrobbioSettings settings) {
	data=0;
	length=settings.frameLength;
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

void StrobbioFrame::clear() {
	data=0;
}

////// Strobbio //////

Strobbio::Strobbio() {
	this->settings=StrobbioSettings();
}

Strobbio::Strobbio(StrobbioSettings settings) {
	this->settings=settings;
}

Strobbio::Strobbio(int inputPin) {
	this->settings=StrobbioSettings();
	this->settings.inputPin=inputPin;
}

int Strobbio::getStatus() {
	//calculate samplerate
	unsigned int currentTime=millis();
	sampleRate=1000/(currentTime-previousTime);
	previousTime=currentTime;
	if(sampleRate<settings.minSampleRate) {return STATUS_LOW_SAMPLERATE;}
	
	int input=analogRead(this->inputPin);
	if(input>980) {
	
	} else {
	
	}

	return false;
}

StrobbioFrame* Strobbio::getData() {
	return NULL;
}

int Strobbio::getSampleRate() {
	return sampleRate;
}