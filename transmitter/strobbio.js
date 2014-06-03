/*
Strobbio Transmitter v0.9.5

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

//style
_strobbio_css="#strobbio-transmitter {position:fixed;top:0px;left:0px;width:100%;height:100%;background-color:#000;font-family:sans-serif;}"
			+"#strobbio-transmitter-content {position:relative;text-align:center;transition:opacity 700ms;height:100%}"
			+"#strobbio-transmitter-content img {position:absolute;top:50%;left:50%;margin-left:-100px;margin-top:-20px;}";
_strobbio_style=document.createElement("style");
_strobbio_style.setAttribute("type","text/css");
_strobbio_style.innerText=_strobbio_css;
_strobbio_headElement=document.getElementsByTagName("head")[0];
_strobbio_headElement.appendChild(_strobbio_style);
_strobbio_mobile_meta=document.createElement("meta"); 
_strobbio_mobile_meta.setAttribute("name","viewport");
_strobbio_mobile_meta.setAttribute("content","width=device-width,user-scalable=no");
_strobbio_headElement.appendChild(_strobbio_mobile_meta);

//internationalization
StrobbioStrings_en={
	send:"Send",
	bigFrame:"Your frame seems too long! This increases the chance of getting a corrupted frame on the other side.",
	frameFull:"You are trying to insert more data than this frame can accept.",
	nothingToSend:"There is nothing to send!"
}

//protocol settings
StrobbioProtocolSettings={
	OneInterval:280,
	ZeroInterval:100,
	pulseInterval:80,
	frameLength:12,
	evenParity:true //set to false for odd parity
}

//classes
function Strobbio() {
	this.strings=StrobbioStrings_en;
	//vars
	var _thisobj=this;
	this._frames=[];
	//gui
	this._logoElement=document.createElement("img");
	this._logoElement.src="data:image/png;base64,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";
	this._contentElement=document.createElement("div");
	this._contentElement.id="strobbio-transmitter-content";
	this._contentElement.appendChild(this._logoElement);
	this._transmitterElement=document.createElement("div");
	this._transmitterElement.id="strobbio-transmitter";
	this._transmitterElement.style.display="none";
	this._transmitterElement.appendChild(this._contentElement);
	if(!document.body) {console.error("Bitlamp instantiated before the document!");}
	else {document.body.appendChild(this._transmitterElement);}
	//functions
	this.setFrames=function(frames) {
		this._frames=frames;
	}
	this.send=function() {
		this._transmitterElement.style.display="block";
		setTimeout(function(t){_thisobj._contentElement.style.opacity=0;},400);
		setTimeout(function(t){
			var generator=new _StrobbioGenerator(
				function(){_thisobj._transmitterElement.style.backgroundColor="#fff";}, //on high
				function(){_thisobj._transmitterElement.style.backgroundColor="#000";}, //on low
				function(){ //on end
					setTimeout(function(t){_thisobj._contentElement.style.opacity=1;},400);
					setTimeout(function(){_thisobj._transmitterElement.style.display="none";},1800);
				});
			generator.generate(_thisobj._frames[0]);
		},1800);
	}	
}

function StrobbioFrame(frameLength) {
	this.strings=StrobbioStrings_en;
	this._frameLength=StrobbioProtocolSettings.frameLength;
	if(frameLength&&frameLength>0) {
		this._frameLength=frameLength;
		if(frameLength>24) {console.log(this.strings.bigFrame);}
	}
	this._payloadLength=this._frameLength-1;	
	this._payload="";
	//functions
	this.addValue=function(value,length) {
		var binary=(value*1).toString(2);
		if(binary.length<=length) {
			var extraZeros=length-binary.length;
			for(var i=0;i<extraZeros;i++) {binary="0"+binary;}
		} else {
			binary=binary.substring(binary.length-length);
		}
		if(this._payload.length+binary.length<=this._payloadLength) {
			this._payload+=binary;
		} else {
			console.error(this.strings.frameFull);
		}
	}
	this.getData=function() {
		var data=this._payload;
		//pad with zeros
		if(data.length<this._payloadLength) {
			var extraZeros=this._payloadLength-data.length;
			for(var i=0;i<extraZeros;i++) {data+="0";}
		}
		//compute parity
		var counter=0;
		for(var i=0;i<data.length;i++) {if(data[i]=="1") {counter++;}}
		if(StrobbioProtocolSettings.evenParity) {
			data=data+(counter%2); //even parity
		} else {
			data=data+((counter+1)%2); //odd parity
		}
		return data;
	}
	this.clear=function() {
		this._payload="";
	}
}

function _StrobbioGenerator(onHigh,onLow,onEnd) {
	var _thisobj=this;
	this.onHigh=onHigh;
	this.onLow=onLow;
	this.onEnd=onEnd;	
	this._timer;
	this._startTime=0;
	this._index=0;
	this._pulses=[];
	//functions
	this.generate=function(frame) {
		//prepare
		this._startTime=new Date().getTime();			
		this._index=0;
		//convert bits to pulse durations
		var data=frame.getData();
		var pulseIndex=0;
		this._pulses[pulseIndex++]=StrobbioProtocolSettings.pulseInterval; //high
		for(var i=0;i<data.length;i++) {
			this._pulses[pulseIndex++]=(data[i]=="1"?StrobbioProtocolSettings.OneInterval:StrobbioProtocolSettings.ZeroInterval); //high time
			this._pulses[pulseIndex++]=StrobbioProtocolSettings.pulseInterval;
			//this._pulses[pulseIndex++]=StrobbioProtocolSettings.separatorInterval; //low time
		}
		this._pulses[pulseIndex++]=StrobbioProtocolSettings.OneInterval; //low
		if(data.length>0&&_thisobj.onHigh) {_thisobj.onHigh();}
		//if(!this._timer) {this._timer=setInterval(_thisobj._tick,5);}
		this._draw();
	}
	this._draw=function() {
		var currentDuration=_thisobj._pulses[_thisobj._index];
		var elapsedTime=new Date().getTime()-_thisobj._startTime;
		if(elapsedTime>=currentDuration) {
			_thisobj._startTime=new Date().getTime();
			_thisobj._index++;
			if(_thisobj._index>=_thisobj._pulses.length) {			
				if(_thisobj.onEnd) {_thisobj.onEnd();}
				return;
			}
			if((_thisobj._index+1)%2==0&&_thisobj.onLow) {_thisobj.onLow();}
			else if((_thisobj._index+1)%2==1&&_thisobj.onHigh) {_thisobj.onHigh();}
		}
		requestAnimationFrame(_thisobj._draw);
	}
}
