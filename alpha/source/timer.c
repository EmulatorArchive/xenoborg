#include "win32.h"
#include "timer.h"


/* Processor details */
LARGE_INTEGER li_freq;
float		  f_freq = 0;
__int64		  timer_start = 0;



int timer_init()
{
	/* Get processor frequency */
	if( !QueryPerformanceFrequency( &li_freq ) )
		return 0;

	f_freq = (float) li_freq.QuadPart / ( 1000.0f * 1000.0f );

	/* Print the processor's clock speed */
	printf( "timer_init(): CPU Frequency: %f Mhz\n", f_freq );

	return 1;
}

void timer_reset()
{
	LARGE_INTEGER li;

	/* Set the start timing */
	QueryPerformanceCounter( &li );
	timer_start = li.QuadPart;
}

float timer_get_freq()
{
	return f_freq;
}

__int64 timer_get_start_time()
{
	return timer_start;
}

__int64 timer_get_current_time()
{
	LARGE_INTEGER li;

	/* Set the start timing */
	QueryPerformanceCounter( &li );

	return li.QuadPart - timer_start;
}