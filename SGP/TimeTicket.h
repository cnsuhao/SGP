#pragma once
#include <ctime>

class TimeTicket
{
private:
	clock_t start, end, check_start, check_end;
	static TimeTicket _timer;

	TimeTicket(void);
	~TimeTicket(void);

	double check_internal();
	void reset_internal(void);
	double total_elapse_internal(void);
public:
	//return the elapse time from the last of check
	static double check(void);
	static double total_elapse(void);
	static void reset(void);
};

