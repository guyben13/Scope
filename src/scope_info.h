#pragma once

#include <algorithm>
#include <string>
#include <tuple>
#include <vector>
#include <iostream>

namespace scope {

#define CHECK(x)                                                           \
  if (!(x)) {                                                              \
    std::cerr << __FILE__ << ":" << __LINE__ << ": ERROR! " << #x << "\n"; \
    ::exit(0);                                                               \
  }


struct Location {
  const char* file = nullptr;
  const char* func = "N/A";
  int line = 0;

  bool operator==(const Location& other) const {
    return file == other.file && line == other.line && func == other.func;
  }
  bool operator!=(const Location& other) const {
    return !(*this == other);
  }
  bool operator<(const Location& other) const {
    return std::make_tuple(file, line, func) <
           std::make_tuple(other.file, other.line, other.func);
  }
};

struct Timer {
  size_t count = 0;
  double total_time = 0.;
  double self_time = total_time;
  void add(double duration) {
    ++count;
    total_time += duration;
  }
  Timer& operator+=(const Timer& other) {
    count += other.count;
    total_time += other.total_time;
    self_time += other.self_time;
    return *this;
  }
  Timer& operator-=(const Timer& other) {
    count -= other.count;
    total_time -= other.total_time;
    self_time -= other.self_time;
    return *this;
  }
};

struct ScopeInfo {
  Location location = {};
  Timer timer = {};
  std::vector<ScopeInfo> children = {};

  void fix_self_time_recursive();
  template <typename CMP>
  void sort_children_recursive(CMP&& cmp) {
    for (ScopeInfo& child : children) {
      child.sort_children_recursive();
    }
    std::sort(children.begin(), children.end(), std::forward<CMP>(cmp));
  }
  void sort_children_recursive() {
    sort_children_recursive([](const ScopeInfo& a, const ScopeInfo& b) {
      return a.timer.total_time > b.timer.total_time;
    });
  }

  void merge_with(const ScopeInfo& other);
  void merge_child(const ScopeInfo& other);
  void diff_from(const ScopeInfo& other);
};

std::string scope_info_html(ScopeInfo scope_info);
std::string scope_info_md(ScopeInfo scope_info);
std::string scope_info_str(ScopeInfo scope_info);

}  // namespace scope
