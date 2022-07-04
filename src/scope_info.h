#pragma once

#include <vector>
#include <string>

namespace scope {

struct Location {
  const char* file = nullptr;
  const char* func = "N/A";
  int line = 0;
};

struct Timer {
  size_t count = 0;
  double total_time = 0.;
  double self_time = total_time;
};

struct ScopeInfo {
  Location location;
  Timer timer;
  std::vector<ScopeInfo> children;

  void fix_self_time_recursive();
  void sort_children_recursive();
};

std::string scope_info_html(ScopeInfo scope_info);

}  // namespace scope
