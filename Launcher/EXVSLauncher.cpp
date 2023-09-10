#include "EXVSLauncher.h"

#include <Windows.h>

#pragma comment(lib, "user32.lib")

#include "MainWindow.h"
#include "RebindWindow.h"

#include <vector>

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int main()
{
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Application::Run(gcnew EXVSLauncher::MainWindow());
	return 0;
}

struct DisplayCandidate
{
	HMONITOR display;
	RECT rect;
	size_t width;
	size_t height;
	int xSplit;
	int ySplit;
};

static std::vector<DisplayCandidate> EnumerateDisplayLocations()
{
	std::vector<DisplayCandidate> result;
	EnumDisplayMonitors(
		nullptr,
		nullptr,
		[](HMONITOR monitor, HDC deviceContext, LPRECT rect, LPARAM userdata) -> BOOL
		{
			auto displays = reinterpret_cast<decltype(result)*>(userdata);
			size_t width = rect->right - rect->left;
			size_t height = rect->bottom - rect->top;

			int xSplit = 0;
			int ySplit = 0;

			if (width / 32 * 9 >= height)
			{
				// 2x1 split
				xSplit = 2;
				ySplit = 1;
			}
			else if (width >= 3840 && height >= 2160)
			{
				// 2x2 split
				xSplit = 2;
				ySplit = 2;
			}
			DisplayCandidate display;
			display.display = monitor;
			display.rect = *rect;
			display.width = width;
			display.height = height;
			display.xSplit = xSplit;
			display.ySplit = ySplit;
			displays->push_back(display);
			return true;
		},
		reinterpret_cast<LPARAM>(&result)
	);
	return result;
}

std::optional<std::vector<RECT>> GenerateWindowLocations(
	std::vector<DisplayCandidate> displays,
	size_t windowCount)
{
	std::vector<RECT> result;
	int splitsNeeded = windowCount - displays.size();
	if (splitsNeeded <= 0)
	{
		for (int i = 0; i < windowCount; ++i)
		{
			result.push_back(displays[i].rect);
		}
		return result;
	}

	// Start with horizontal-only splits.
	for (int i = windowCount - 1; i > 0; --i)
	{
		if (displays[i].xSplit > 1 && displays[i].ySplit == 1)
		{
			int splitWidth = displays[i].width / displays[i].xSplit;
			for (int i = 0; i < displays[i].xSplit; ++i)
			{

			}
		}
	}
}

void MoveWindows(const std::vector<HWND>& windows, const std::vector<RECT>& locations)
{
	if (windows.size() > )
}

void Launch(const LauncherConfiguration& config)
{
	std::vector<const ClientConfiguration*> enabled;
	for (auto& config : config.clients)
	{
		if (config.enabled)
		{
			enabled.push_back(&config);
		}
		else
		{
			printf("skipping %s\n", config.name.c_str());
		}
	}

	std::vector<HWND> handles;
	for (auto& client : enabled)
	{
		printf("showing window for %s\n", client->name.c_str());
		auto window = gcnew EXVSLauncher::RebindWindow();
		window->Show();
		handles.push_back(reinterpret_cast<HWND>(window->Handle.ToPointer()));
	}

	std::vector<DisplayCandidate> displayLocations = EnumerateDisplayLocations();

}