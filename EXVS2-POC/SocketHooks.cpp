#include "SocketHooks.h"

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")


#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <codecvt>
#include <string>
#include <vector>

#include "MinHook.h"

#include "Configs.h"

static SOCKET(WSAAPI* orig_WSASocketW)(int af, int type, int protocol, LPWSAPROTOCOL_INFOW lpProtocolInfo, GROUP g, DWORD dwFlags);
static int (WSAAPI* orig_bind)(SOCKET s, const struct sockaddr* name, int namelen);
static std::vector<char> selectedSockaddr;

static const char* SocketTypeToString(int type)
{
	switch (type)
	{
	case SOCK_DGRAM:
		return "SOCK_DGRAM";
	case SOCK_STREAM:
		return "SOCK_STREAM";
	default:
		return "<unknown>";
	}
}

static SOCKET WSAAPI WSASocketWHook(int af, int type, int protocol, LPWSAPROTOCOL_INFOW lpProtocolInfo, GROUP g, DWORD dwFlags)
{
	SOCKET result = orig_WSASocketW(af, type, protocol, lpProtocolInfo, g, dwFlags);
	printf("WSASocketW(%d, %s, %d, ?, ?, %d) = %p\n", af, SocketTypeToString(type), protocol, dwFlags, reinterpret_cast<void*>(result));
	return result;
}

static int bindHook(SOCKET s, const struct sockaddr* name, int namelen)
{
	printf("bind(%p, %p, %d)\n", reinterpret_cast<void*>(s), name, namelen);
	return orig_bind(s, reinterpret_cast<sockaddr*>(selectedSockaddr.data()), selectedSockaddr.size());
}

static void fatal [[noreturn]] (_Printf_format_string_ const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	puts("\n");
	abort();
}

static std::optional<std::string> SockaddrToIPv4String(LPSOCKADDR addr)
{
	if (addr->sa_family != AF_INET)
		return {};

	// TODO: Do we need to handle IPv4-mapped IPv6 addresses?
	std::string result;
	result.resize(sizeof("999.999.999.999"), '\0');
	inet_ntop(AF_INET, &reinterpret_cast<LPSOCKADDR_IN>(addr)->sin_addr, result.data(), result.size());
	result.resize(strlen(result.data()));
	return result;
}

template<typename F>
static bool IterateInterfaces(F&& callback) {
	DWORD rc = 0;

	// Strictly speaking, this isn't guaranteed to be sufficiently aligned, but in practice, this is going to allocate with new and we're on x86 anyway.
	std::vector<char> buffer;
	PIP_ADAPTER_ADDRESSES pAddresses = nullptr;
	ULONG outBufLen = 16384;

	for (int i = 0; i < 32; ++i)
	{
		buffer.resize(outBufLen);
		pAddresses = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());

		rc = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, pAddresses, &outBufLen);
		if (rc != ERROR_BUFFER_OVERFLOW)
		{
			break;
		}
	}

	if (rc != NO_ERROR)
	{
		fatal("Failed to enumerate network adapters");
	}

	for (PIP_ADAPTER_ADDRESSES cur = pAddresses; cur->Next; cur = cur->Next)
	{
		std::u16string currentInterfaceNameU16 = reinterpret_cast<char16_t*>(cur->FriendlyName);
		std::string currentInterfaceName = std::wstring_convert<
			std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(currentInterfaceNameU16);

		if (!callback(std::move(currentInterfaceName), cur)) {
			return true;
		}
	}
	return false;
}

static void SelectInterface(PIP_ADAPTER_ADDRESSES adapter, std::string&& interfaceName, std::string&& ipAddress, LPSOCKADDR sockaddr, int sockaddrLen)
{
	printf("Interface selected: %s (%s)\n", interfaceName.c_str(), ipAddress.c_str());
	// TODO: Set config variables.
	const char* p = reinterpret_cast<char*>(sockaddr);
	selectedSockaddr.assign(p, p + sockaddrLen);
}

static bool FindInterfaceByName(const std::string& interfaceName)
{
	printf("Selecting interface by name: %s\n", interfaceName.c_str());
	return IterateInterfaces([&](std::string&& currentInterfaceName, PIP_ADAPTER_ADDRESSES adapter) {
		printf("Current interface: %s\n", currentInterfaceName.c_str());
		if (interfaceName == currentInterfaceName) {
			if (!adapter->FirstUnicastAddress)
				fatal("No IP address for interface %s", currentInterfaceName.c_str());

			// Select the first IPv4 address.
			for (PIP_ADAPTER_UNICAST_ADDRESS unicast = adapter->FirstUnicastAddress; unicast; unicast = unicast->Next) {
				LPSOCKADDR addr = unicast->Address.lpSockaddr;
				std::optional<std::string> v4AddrString = SockaddrToIPv4String(addr);
				if (!v4AddrString)
					continue;
				SelectInterface(adapter, std::move(currentInterfaceName), std::move(*v4AddrString), addr, unicast->Address.iSockaddrLength);
				return false;
			}
			fatal("Failed to find IPv4 address for interface %s", currentInterfaceName.c_str());
		}

		return true;
		});
}

static bool FindInterfaceByAddress(const std::string& ipAddress)
{
	printf("Selecting interface by address: %s\n", ipAddress.c_str());
	return IterateInterfaces([&](std::string&& currentInterfaceName, PIP_ADAPTER_ADDRESSES adapter) {
		for (PIP_ADAPTER_UNICAST_ADDRESS unicast = adapter->FirstUnicastAddress; unicast; unicast = unicast->Next) {
			LPSOCKADDR addr = unicast->Address.lpSockaddr;
			std::optional<std::string> v4AddrString = SockaddrToIPv4String(addr);
			if (!v4AddrString)
				continue;
			if (*v4AddrString == ipAddress)
			{
				SelectInterface(adapter, std::move(currentInterfaceName), std::move(*v4AddrString), addr, unicast->Address.iSockaddrLength);
				return false;
			}
		}
		return true;
		});
}

static void FindInterface()
{
	if (globalConfig.InterfaceName && globalConfig.IpAddress)
		fatal("Only one of InterfaceName and IpAddress can be specified in config.ini");

	bool result = false;
	if (globalConfig.InterfaceName)
		result = FindInterfaceByName(*globalConfig.InterfaceName);
	else if (globalConfig.IpAddress)
		result = FindInterfaceByAddress(*globalConfig.IpAddress);
	else
		fatal("At least one of InterfaceName or IpAddress should be specified in config.ini");

	if (!result)
		fatal("Failed to find interface");
}

void InitializeSocketHooks()
{
	FindInterface();
	MH_Initialize();
	MH_CreateHookApi(L"WS2_32.dll", "WSASocketW", WSASocketWHook, reinterpret_cast<void**>(&orig_WSASocketW));
	MH_CreateHookApi(L"WS2_32.dll", "bind", bindHook, reinterpret_cast<void**>(&orig_bind));
	// A hook for socket is unnecessary, at least on Windows 10: it wraps WSASocketW.
	MH_EnableHook(MH_ALL_HOOKS);
}
