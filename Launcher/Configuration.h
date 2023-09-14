#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

enum class ClientMode
{
	Client = 0,
	LiveMonitor = 1,
};

struct IpAddress
{
	std::string value;
};

struct InterfaceName
{
	std::string value;
};

struct ClientConfiguration
{
	std::string name;
	std::string storagePath;
	ClientMode mode = ClientMode::Client;
	bool hidden = false;
	std::variant<std::monostate, IpAddress, InterfaceName> networkInterface;

	bool enabled = true;
};

struct LauncherConfiguration
{
	std::vector<ClientConfiguration> clients;
	std::string gamePath;
	std::string serverPath;
	bool rebindOnLaunch = false;
};

struct LauncherState
{
	LauncherConfiguration config;
	std::optional<size_t> selected;
};