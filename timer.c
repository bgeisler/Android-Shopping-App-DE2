#include "timer.h"

void initTimer(){
	float i;
	//do{
	alt_timestamp_start();
	//}while (i < 0);
}

float getTime(){
	float gettime = (float) alt_timestamp() / (float) alt_timestamp_freq();
	return gettime;
}

	//printf("Start Time: %f\n",start_time);

