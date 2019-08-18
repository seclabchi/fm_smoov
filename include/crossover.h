/*
 * Crossover.h
 *
 *  Created on: Aug 18, 2019
 *      Author: zaremba
 */

#ifndef INCLUDE_CROSSOVER_H_
#define INCLUDE_CROSSOVER_H_

class Crossover {
public:
	Crossover();
	virtual ~Crossover();
	void process(float* inbuf, int nsamp, float** out);
};

#endif /* INCLUDE_CROSSOVER_H_ */
