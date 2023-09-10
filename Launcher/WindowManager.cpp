#include "WindowManager.h"

#include "log.h"

struct GameWindow {
  GameWindow(ClientConfiguration config, RECT windowLocation) : config_(config), windowLocation_(windowLocation) {}

 private:
  ClientConfiguration config_;
  RECT windowLocation_;
};

WindowManager::WindowManager() {
  HRESULT rc = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8A,
                                  reinterpret_cast<void**>(&dinput_), nullptr);

  if (rc != DI_OK) {
    fatal("DirectInput8Create failed");
  }
}

WindowManager::~WindowManager() {
  if (dinput_) dinput_->Release();
}

bool WindowManager::Initialize(const std::vector<const ClientConfiguration*>& clientConfigs,
                               std::vector<RECT>&& windowLocations) {
  windows_.clear();

  if (windowLocations.size() < clientConfigs.size()) fatal("WindowManager initialized without enough window locations");

  for (size_t i = 0; i < clientConfigs.size(); ++i) {
    windows_.emplace_back(std::make_unique<GameWindow>(*clientConfigs[i], windowLocations[i]));
  }

  UpdateDevices();
}

void WindowManager::UpdateDevices() {
  info("updating devices");
  auto cb = [](LPCDIDEVICEINSTANCE lpddi, void* arg) {
    info("product: %s", lpddi->tszProductName);
    info("instance: %s", lpddi->tszInstanceName);
    return DIENUM_CONTINUE;
  };
  dinput_->EnumDevices(DI8DEVCLASS_GAMECTRL, cb, nullptr, DIEDFL_ATTACHEDONLY);
}