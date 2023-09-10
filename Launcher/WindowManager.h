#pragma once

#include <map>
#include <memory>
#include <unordered_map>

#undef UNICODE
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8")
#pragma comment(lib, "dxguid")

#include "Configuration.h"
#include "util.h"

enum class Binding {
  A,
  B,
  C,
  D,
  AB,
  BC,
  AC,
  ABC,
  Start,
  Coin,
  Card,
};

const char* to_string(Binding binding);

class Bindings {
  // TODO: This should really be the device instance GUID...
  int deviceIndex;

  std::map<int, Binding> bindings;
};

using DeviceId = int;

class Input {};

class GameWindow;

struct WindowManager {
  WindowManager();
  ~WindowManager();

  bool Initialize(const std::vector<const ClientConfiguration*>& clientConfigs, std::vector<RECT>&& windowLocations);

  void UpdateDevices();

 private:
  std::vector<std::unique_ptr<GameWindow>> windows_;
  IDirectInput8* dinput_;
  std::unordered_map<GUID, IDirectInputDevice8A*> devices_;
};