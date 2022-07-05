#pragma once

#include <chrono>

#include "scope_info.h"

namespace scope {

struct Scope {
  Scope(Location loc);
  ~Scope();
};

struct ScopeCallback {
  ScopeCallback(Location loc, std::function<void(ScopeInfo)> callback);
  ~ScopeCallback();

 private:
  Scope m_scope;
  std::function<void(ScopeInfo)> m_callback;
};

ScopeInfo thread_scope();

#define SCOPE_NAME_CAT2(name, line) name##line
#define SCOPE_NAME_CAT(name, line) SCOPE_NAME_CAT2(name, line)
#define SCOPE_NAME() SCOPE_NAME_CAT(xxx_scope_, __LINE__)

#define SCOPE_LOG() \
  ::scope::Scope SCOPE_NAME()({__FILE__, __PRETTY_FUNCTION__, __LINE__})

}  // namespace scope
