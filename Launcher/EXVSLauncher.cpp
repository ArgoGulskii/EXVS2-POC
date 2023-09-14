#include "EXVSLauncher.h"

#include <Windows.h>
#include <Psapi.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Psapi.lib")

#include <filesystem>
#include <vector>

#include "log.h"
#include "util.h"
#include "MainWindow.h"
#include "RebindWindow.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int main()
{
	if (g_logLevel != LogLevel::NONE)
	{
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
	}

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

static std::vector<DisplayCandidate> EnumerateDisplays()
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
			debug("Found display: %dx%d (%dx%d)", width, height, xSplit, ySplit);
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

static void CalculateWindowLocationsPerDisplay(std::vector<RECT>* output, const std::vector<DisplayCandidate>& displays)
{
	for (auto& display : displays)
	{
		// TODO: Maintain aspect ratio.
		output->push_back(display.rect);
	}
}

static void CalculateWindowLocationSplit(
	std::vector<RECT>* output,
	std::vector<DisplayCandidate>& displays,
	int xSplit,
	int ySplit)
{
	for (auto it = displays.begin(); it != displays.end();)
	{
		if (it->xSplit != xSplit || it->ySplit != ySplit)
		{
			++it;
			continue;
		}

		int splitWidth = it->width / it->xSplit;
		int splitHeight = it->height/ it->ySplit;
		for (int i = 0; i < it->ySplit; ++i)
		{
			for (int j = 0; j < it->xSplit; ++j)
			{
				// TODO: Maintain aspect ratio.
				RECT rect = {
					.left = it->rect.left + j * splitWidth,
					.top = it->rect.top + i * splitHeight,
					.right = it->rect.left + (j + 1) * splitWidth,
					.bottom = it->rect.top + (i + 1) * splitHeight,
				};
				output->push_back(rect);
			}
		}

		it = displays.erase(it);
	}
}

static void CalculateWindowLocations2x1(std::vector<RECT>* output, std::vector<DisplayCandidate> displays)
{
	CalculateWindowLocationSplit(output, displays, 2, 1);
	CalculateWindowLocationsPerDisplay(output, displays);
}

static void CalculateWindowLocations2x2(std::vector<RECT>* output, std::vector<DisplayCandidate> displays)
{
	CalculateWindowLocationSplit(output, displays, 2, 2);
	CalculateWindowLocations2x1(output, std::move(displays));
}

bool CalculateWindowLocations(std::vector<RECT>* output, std::vector<DisplayCandidate> displays, size_t windowCount)
{
	output->clear();
	CalculateWindowLocationsPerDisplay(output, displays);

	if (output->size() >= windowCount)
		return true;

	output->clear();
	CalculateWindowLocations2x1(output, displays);

	if (output->size() >= windowCount)
		return true;

	output->clear();
	CalculateWindowLocations2x2(output, displays);
	
	if (output->size() >= windowCount)
		return true;

	output->clear();
	return false;
}

void MoveWindows(const std::vector<HWND>& windows, const std::vector<RECT>& locations)
{
	if (windows.size() > locations.size()) abort();
	for (size_t i = 0; i < windows.size(); ++i)
	{
		auto& window = windows[i];
		auto& location = locations[i];
		info("Moving window to (%d, %d), size = (%d, %d)", location.left, location.top, location.right - location.left, location.bottom - location.top);
		LONG style = GetWindowLong(window, GWL_STYLE);
		style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
		SetWindowLong(window, GWL_STYLE, style);
		SetWindowPos(window, HWND_TOPMOST, location.left, location.top, location.right - location.left, location.bottom - location.top, SWP_SHOWWINDOW);
	}
}

static bool IsServerRunning(const std::filesystem::path& serverPath)
{
	std::vector<DWORD> pids;
	DWORD bytesWritten;
	pids.resize(65536);

	if (!EnumProcesses(pids.data(), pids.size() * sizeof(DWORD), &bytesWritten))
	{
		alert("Failed to enumerate processes");
		return false;
	}

	pids.resize(bytesWritten / sizeof(DWORD));
	for (auto pid : pids)
	{
		HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, false, pid);
		if (!process)
		{
			warn("OpenProcess failed on pid %d", pid);
			continue;
		}

		std::string path;
		path.resize(65536, '\0');
		DWORD pathLength = path.size();
		if (!QueryFullProcessImageNameA(process, 0, path.data(), &pathLength))
		{
			warn("QueryFullProcessImageName failed on pid %d", pid);
			CloseHandle(process);
			continue;
		}
		path.resize(pathLength);

		std::error_code ec;
		if (std::filesystem::equivalent(serverPath, path, ec))
		{
			info("Server (%s) already running: pid = %d", path.c_str(), pid);
			CloseHandle(process);
			return true;
		}

		printf("%d = %s\n", pid, path.data());
		CloseHandle(process);
	}

	return false;
}

static bool LaunchServer(std::string* error, const std::filesystem::path& serverPath)
{
	if (IsServerRunning(serverPath))
	{
		info("Server is already running, skipping");
		return true;
	}

	STARTUPINFOW startupInfo;
	PROCESS_INFORMATION procInfo;

	memset(&startupInfo, 0, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);

	bool result = CreateProcessW(
		serverPath.c_str(),
		nullptr,
		nullptr,
		nullptr,
		false,
		NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP,
		nullptr,
		serverPath.parent_path().c_str(),
		&startupInfo,
		&procInfo
	);

	if (!result)
	{
		warn("Failed to start %ls", serverPath.c_str());
		char buf[256];
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
			buf, sizeof(buf), NULL);
		*error = buf;
	}

	CloseHandle(procInfo.hProcess);
	CloseHandle(procInfo.hThread);
	return result;
}

void Launch(const LauncherConfiguration& config)
{
	ScopedDpiAware _;

	std::vector<const ClientConfiguration*> enabled;
	std::vector<const ClientConfiguration*> visible;
	for (auto& config : config.clients)
	{
		if (config.enabled)
		{
			enabled.push_back(&config);
			if (!config.hidden)
			{
				visible.push_back(&config);
			}
		}
		else
		{
			info("skipping disabled instance %s", config.name.c_str());
		}
	}

	std::vector<DisplayCandidate> displays = EnumerateDisplays();
	std::vector<RECT> windowLocations;

	if (!CalculateWindowLocations(&windowLocations, displays, visible.size())) {
		alert("Unable to find %zd places to put windows", visible.size());
		return;
	}

	debug("Placed %zd windows: ", visible.size());
	for (auto& window : windowLocations)
	{
		debug("\t%d,%d: %dx%d", window.left, window.top, window.right - window.left, window.bottom - window.top);
	}
	debug("");

	std::string error;
	if (!LaunchServer(&error, config.serverPath))
	{
		alert("Failed to start server: %s", error.c_str());
		return;
	}

	std::vector<HWND> handles;
	for (auto& client : visible)
	{
		debug("Showing window for %s", client->name.c_str());
		auto window = gcnew EXVSLauncher::RebindWindow();
		window->Show();
		handles.push_back(reinterpret_cast<HWND>(window->Handle.ToPointer()));
	}

	MoveWindows(handles, windowLocations);
}