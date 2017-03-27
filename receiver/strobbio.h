/*
Strobbio Receiver v1.0.0

Copyright (c) 2017 ptrgast

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

#ifndef STROBBIO_RECEIVER
#define STROBBIO_RECEIVER

#define STATUS_LOW_SAMPLERATE -1
#define STATUS_WAITING 0
#define STATUS_DATA 1

#define INPUT_UNDEFINED -1
#define INPUT_LOW 0
#define INPUT_HIGH 1

#include "Arduino.h"

class StrobbioSettings {
	public:
		int oneInterval;
		int zeroInterval;
		int pulseInterval;
		int frameLength;
		boolean evenParity;
		int inputPin;
		int maxSamplePeriod;
		StrobbioSettings();
};

class StrobbioFrame {
	private:
		int length;
		boolean evenParity;
		long data;
		int index;		
	public:
		StrobbioFrame();
		StrobbioFrame(StrobbioSettings settings);
		long read(int start,int length);
		void setBit(int index);
		void addBit(boolean value);
		boolean isFull();
		boolean isIntegral();
    void copyTo(StrobbioFrame*);
		void clear();
		void print();
};

class Strobbio {
	private:
		StrobbioSettings settings;
		StrobbioFrame frame;
		unsigned long previousTime;
		unsigned int previousState;
		unsigned long bitStart;
		int previousInput;
		int previousDelta;
		int* buffer;
		int bufferIndex;
		int border;
	public:
		Strobbio();
		Strobbio(StrobbioSettings settings);
		~Strobbio();
		int getStatus();
		void getData(StrobbioFrame*);
};

#endif
