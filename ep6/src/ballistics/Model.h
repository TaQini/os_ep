
#ifndef __MODEL_H__
 
#define __MODEL_H__

#include "Result.h"

class Model {

public:

	Model();
	Model(int step);
	Result * moveToNext();
	double getTime();
	double getNextTime();

private:

double time;
double range;
int step;

Result * getNextStatus();

};


#endif
