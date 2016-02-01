#include "StdAfx.h"
#include "Log.h"
#include <fstream>

Log* Log::_log = NULL;

Log::Log(string& logfile)
{
	_ofs = new ofstream(logfile);
}


Log::~Log(void)
{
	if(_ofs!=NULL)
	{
		_ofs->close();
		delete _ofs;
	}
}

void Log::CreateLog(string& logfile)
{
	if(_log!=NULL)
		delete _log;

	_log = new Log(logfile);
}

void Log::log(string log_str)
{
	if(_log!=NULL)
	{
		_log->log_internal(log_str);
	}
}

void Log::log(int log_int)
{
	if(_log!=NULL)
	{
		_log->log_internal(log_int);
	}
}

void Log::log(double log_double)
{
	if(_log!=NULL)
	{
		_log->log_internal(log_double);
	}
}

void Log::logln(string log_str)
{
	log(log_str);
	log("\n");
}

void Log::logln(int log_int)
{
	log(log_int);
	log("\n");
}

void Log::logln(double log_double)
{
	log(log_double);
	log("\n");
}

void Log::log_internal(string& log_str)
{
	if(_ofs!=NULL)
	{
		(*_ofs)<<log_str;
		_ofs->flush();
		cout<<log_str;
	}
}

void Log::log_internal(int log_int)
{
	if(_ofs!=NULL)
	{
		(*_ofs)<<log_int;
		_ofs->flush();
		cout<<log_int;
	}
}

void Log::log_internal(double log_double)
{
	if(_ofs!=NULL)
	{
		(*_ofs)<<log_double;
		_ofs->flush();
		cout<<log_double;
	}
}

