#pragma once

#include <stdio.h>

enum class LogLevel {
	NONE,
	ERR,
	WARN,
	INFO,
	DEBUG,
	TRACE
};

inline LogLevel g_logLevel = LogLevel::TRACE;

#define err(...) log(LogLevel::ERR, __VA_ARGS__)
#define warn(...) log(LogLevel::WARN, __VA_ARGS__)
#define info(...) log(LogLevel::INFO, __VA_ARGS__)
#define debug(...) log(LogLevel::DEBUG, __VA_ARGS__)
#define trace(...) log(LogLevel::TRACE, __VA_ARGS__)

void log(LogLevel level, _Printf_format_string_ const char* format, ...);
void alert(_Printf_format_string_ const char* format, ...);
#define fatal(...) do { alert(__VA_ARGS__); abort();z } while(0)