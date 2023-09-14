#pragma once

#include <Windows.h>

struct ScopedDpiAware
{
	ScopedDpiAware() : ScopedDpiAware(true)
	{
	}

	explicit ScopedDpiAware(bool awareness) : ScopedDpiAware(awareness ? DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 : DPI_AWARENESS_CONTEXT_UNAWARE)
	{
	}

	explicit ScopedDpiAware(DPI_AWARENESS_CONTEXT context)
	{
		previous_ = GetThreadDpiAwarenessContext();
		SetThreadDpiAwarenessContext(context);
	}

	~ScopedDpiAware()
	{
		SetThreadDpiAwarenessContext(previous_);
	}

private:
	DPI_AWARENESS_CONTEXT previous_;
};