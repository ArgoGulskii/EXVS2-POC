#pragma once

#include <Windows.h>

#include <string_view>

struct ScopedDpiAware {
  ScopedDpiAware() : ScopedDpiAware(true) {}

  explicit ScopedDpiAware(bool awareness)
      : ScopedDpiAware(awareness ? DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 : DPI_AWARENESS_CONTEXT_UNAWARE) {}

  explicit ScopedDpiAware(DPI_AWARENESS_CONTEXT context) {
    previous_ = GetThreadDpiAwarenessContext();
    SetThreadDpiAwarenessContext(context);
  }

  ~ScopedDpiAware() { SetThreadDpiAwarenessContext(previous_); }

 private:
  DPI_AWARENESS_CONTEXT previous_;
};

namespace std {
template <>
struct hash<GUID> {
  size_t operator()(const GUID& guid) const {
    const char* p = reinterpret_cast<const char*>(&guid);
    std::string_view sv(p, p + sizeof(guid));
    return hash<std::string_view>()(sv);
  }
};
}  // namespace std

std::string to_string(const GUID& guid);
GUID from_string(const std::string str);