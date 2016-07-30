#include "stdafx.h"
#include "commondef.h"

int rand(int range_min, int range_max)
{
	return (double)rand() / (RAND_MAX + 1) * (range_max - range_min) + range_min;
}

float randf(float range_min, float range_max)
{
	return (float)rand() / (RAND_MAX + 1) * (range_max - range_min) + range_min;
}
