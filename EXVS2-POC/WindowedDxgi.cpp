// ReSharper disable CppClangTidyClangDiagnosticMicrosoftCast
#define CINTERFACE
#define D3D11_NO_HELPERS
#define INITGUID
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#pragma optimize("", off)

#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include <dxgi1_5.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include <chrono>

#include <intrin.h>

#include "injector.hpp"
#include "log.h"
#include "MinHook.h"
#include "Configs.h"

using namespace std::chrono_literals;

static IDXGISwapChain* g_swapChain;

// Hook trampolines
static HRESULT(STDMETHODCALLTYPE* g_origSetFullscreenState)(IDXGISwapChain* This, BOOL Fullscreen, IDXGIOutput* pTarget);
static HRESULT(STDMETHODCALLTYPE* g_origPresent)(IDXGISwapChain* This, UINT SyncInterval, UINT Flags);
static HRESULT(STDMETHODCALLTYPE* g_origCreateSwapChain)(IDXGIFactory2* This, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain);
static HRESULT(WINAPI* g_origCreateDXGIFactory2)(UINT Flags, REFIID riid, void** ppFactory);
static BOOL(WINAPI* g_origSetWindowPos)(HWND hWnd, HWND hWndInsertAfter, int  X, int  Y, int  cx, int  cy, UINT uFlags);
static HWND(WINAPI* g_origCreateWindowExW)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
static BOOL(WINAPI* g_origMoveWindow)(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint);

// Functions
template<typename T>
inline T HookVtableFunction(T* functionPtr, T target)
{
	if (*functionPtr == target)
	{
		return nullptr;
	}

	auto old = *functionPtr;
	injector::WriteMemory(functionPtr, target, true);

	return old;
}

static HRESULT STDMETHODCALLTYPE SetFullscreenStateHook(IDXGISwapChain* This, BOOL Fullscreen, IDXGIOutput* pTarget)
{
	if (!globalConfig.Windowed)
	{
		return g_origSetFullscreenState(This, Fullscreen, pTarget);
	}
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE PresentHook(IDXGISwapChain* This, UINT SyncInterval, UINT Flags)
{
	static std::chrono::high_resolution_clock::time_point last;

	HRESULT rc = g_origPresent(This, SyncInterval, Flags);

	auto desired = last + 16'666'666ns;

	// TODO: Instead of spinning, sleep for X ms and then spin.
	while (std::chrono::high_resolution_clock::now() < desired);

	last = std::chrono::high_resolution_clock::now();
	return rc;
}

static HRESULT STDMETHODCALLTYPE CreateSwapChainHook(IDXGIFactory2* This, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
{
	info("CreateSwapChain(%dx%d@%d, SampleCount=%d, SampleQuality=%d, BufferCount=%d)", pDesc->BufferDesc.Width, pDesc->BufferDesc.Height, pDesc->BufferDesc.RefreshRate, pDesc->SampleDesc.Count, pDesc->SampleDesc.Quality, pDesc->BufferCount);

	auto hr = g_origCreateSwapChain(This, pDevice, pDesc, ppSwapChain);
	if (*ppSwapChain)
	{
		auto oldSetFullScreenState = HookVtableFunction(&(*ppSwapChain)->lpVtbl->SetFullscreenState, SetFullscreenStateHook);
		g_origSetFullscreenState = (oldSetFullScreenState) ? oldSetFullScreenState : g_origSetFullscreenState;

		auto oldPresent = HookVtableFunction(&(*ppSwapChain)->lpVtbl->Present, PresentHook);
		g_origPresent = (oldPresent) ? oldPresent : g_origPresent;
	}
	g_swapChain = *ppSwapChain;
	return hr;
}

static HRESULT WINAPI CreateDXGIFactory2Hook(UINT Flags, REFIID riid, void** ppFactory)
{
	HRESULT hr = g_origCreateDXGIFactory2(Flags, riid, ppFactory);

	if (SUCCEEDED(hr))
	{
		IDXGIFactory2* factory = (IDXGIFactory2*)*ppFactory;

		auto old = HookVtableFunction(&factory->lpVtbl->CreateSwapChain, CreateSwapChainHook);
		g_origCreateSwapChain = (old) ? old : g_origCreateSwapChain;
	}

	return hr;
}

static HWND WINAPI CreateWindowExWHook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	if (nWidth > 0 && nHeight > 0)
	{
		dwStyle = WS_VISIBLE | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		X = (GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2;
		Y = (GetSystemMetrics(SM_CYSCREEN) - nHeight) / 2;

		// lpWindowName = L"POC";
	}

	return g_origCreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

static BOOL WINAPI MoveWindowHook(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint)
{
	if (nWidth > 0 && nHeight > 0)
	{
		X = (GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2;
		Y = (GetSystemMetrics(SM_CYSCREEN) - nHeight) / 2;
		nWidth = 1920;
		nHeight = 1080;
	}

	return g_origMoveWindow(hWnd, X, Y, nWidth, nHeight, bRepaint);
}

static BOOL WINAPI SetWindowPosHook(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	// Don't put ourselves on top if we're in windowed mode.
	if (hWndInsertAfter == HWND_TOPMOST && g_swapChain)
	{
		DXGI_SWAP_CHAIN_DESC desc;
		HRESULT rc = g_swapChain->lpVtbl->GetDesc(g_swapChain, &desc);
		if (rc != S_OK)
		{
			printf("DXGISwapChain::GetDesc failed: %x", rc);
		}
		else if (desc.Windowed)
		{
			hWndInsertAfter = HWND_TOP;
		}
	}

	return g_origSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

void InitDXGIWindowHook()
{
	MH_CreateHookApi(L"user32.dll", "CreateWindowExW", CreateWindowExWHook, reinterpret_cast<void**>(&g_origCreateWindowExW));
	MH_CreateHookApi(L"user32.dll", "MoveWindow", MoveWindowHook, reinterpret_cast<void**>(&g_origMoveWindow));
	MH_CreateHookApi(L"user32.dll", "SetWindowPos", SetWindowPosHook, reinterpret_cast<void**>(&g_origSetWindowPos));
	MH_CreateHookApi(L"dxgi.dll", "CreateDXGIFactory2", CreateDXGIFactory2Hook, (void**)&g_origCreateDXGIFactory2);
}