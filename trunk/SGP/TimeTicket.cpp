#include "StdAfx.h"
#include "TimeTicket.h"

TimeTicket TimeTicket::_timer;

TimeTicket::TimeTicket(void)
{
	reset_internal();
}


TimeTicket::~TimeTicket(void)
{
}

double TimeTicket::check_internal(void)
{
	check_end = clock();
	double duration = (check_end-check_start)/CLOCKS_PER_SEC;
	check_start = check_end;
	return duration;
}

double TimeTicket::check(void)
{
	return _timer.check_internal();
}


double TimeTicket::total_elapse(void)
{
	return _timer.total_elapse_internal();
}


double TimeTicket::total_elapse_internal(void)
{
	end = clock();
	double duration = (end-start)/CLOCKS_PER_SEC;
	return duration;
}


void TimeTicket::reset(void)
{
	_timer.reset_internal();
}


void TimeTicket::reset_internal(void)
{
	start = end = check_start = check_end = clock();
}
