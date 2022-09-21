/*
 * processor_live_data.h
 *
 *  Created on: Sep 21, 2022
 *      Author: zaremba
 */

#ifndef PROCESSOR_LIVE_DATA_H_
#define PROCESSOR_LIVE_DATA_H_

class ProcessorLiveData {
public:
	ProcessorLiveData();
	virtual ~ProcessorLiveData();
	float inL, inR, outL, outR;
};

#endif /* PROCESSOR_LIVE_DATA_H_ */
