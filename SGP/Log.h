#pragma once
#include "stdafx.h"
#include <iostream>
#include <string>

using namespace std;

class Log
{
private:
	Log(string& logfile);
	~Log(void);	

	ofstream* _ofs;
	static Log* _log;

	void log_internal(string& log_str);
	void log_internal(int log_int);
	void log_internal(double log_double);

public:
	static void CreateLog(string& logfile);
	static void log(string log_str);
	static void log(int log_int);
	static void log(double log_double);

	static void logln(string log_str);
	static void logln(int log_int);
	static void logln(double log_double);
	
};

