#include "commons.h"

void us_sleep(uint32_t us) {
	uint32_t count = 0;
	const uint32_t utime = (120 * us / 7);
	do
	{
		if ( ++count > utime )
		{
			return ;
		}
	} while (1);
}