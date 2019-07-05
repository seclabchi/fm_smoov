#ifndef __PCM_INPUT_H__
#define __PCM_INPUT_H__

class PcmInput()
{
public:
	PcmInput();
	void open(string deviceName);
	void close();

};
