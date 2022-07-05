#include "scope.h"

#include <algorithm>
#include <chrono>
#include <optional>

namespace scope {

namespace {

using TimePoint = std::chrono::high_resolution_clock::time_point;
inline TimePoint time_point_now() {
  return std::chrono::high_resolution_clock::now();
}
inline double time_point_duration(TimePoint a, TimePoint b) {
  return std::chrono::duration<double>(b - a).count();
}

struct ThreadData;

struct StackNode {
  Location loc;
  ThreadData* thread_data;
  Timer timer = {};
  std::vector<StackNode> children = {};
  std::optional<TimePoint> curr_start = std::nullopt;

  StackNode& child(Location loc) {
    auto iter = std::partition_point(
        children.begin(), children.end(), [loc](const StackNode& node) {
          return node.loc < loc;
        });
    if (iter == children.end() || iter->loc != loc) {
      [[unlikely]] iter = children.insert(iter, StackNode{loc, thread_data});
    }
    return *iter;
  }

  ScopeInfo to_scope_info(TimePoint now) const {
    ScopeInfo root{loc, timer};
    if (curr_start.has_value()) {
      root.timer.add(time_point_duration(curr_start.value(), now));
    }
    root.children.reserve(children.size());
    for (const StackNode& child : children) {
      root.children.push_back(child.to_scope_info(now));
    }
    return root;
  }
};

struct ThreadData {
  ThreadData() {
    path.reserve(50);
    path.push_back(&root);
    path.back()->curr_start = time_point_now();
  }

  void enter(Location loc) {
    path.push_back(&path.back()->child(loc));
    path.back()->curr_start = time_point_now();
  }
  void exit() {
    CHECK(path.size() >= 2);
    StackNode& node = *path.back();
    path.pop_back();
    CHECK(node.curr_start.has_value());
    node.timer.add(
        time_point_duration(node.curr_start.value(), time_point_now()));
    node.curr_start=std::nullopt;
  }

  StackNode root{{nullptr, "<Root>", 0}, this};
  std::vector<StackNode*> path;
};

ThreadData& thread_local_data() {
  static thread_local ThreadData* thread_data = new ThreadData();
  return *thread_data;
}

}  // namespace

Scope::Scope(Location loc) { thread_local_data().enter(loc); }

Scope::~Scope() { thread_local_data().exit(); }

ScopeInfo thread_scope() {
  ScopeInfo res = thread_local_data().root.to_scope_info(time_point_now());
  res.fix_self_time_recursive();
  return res;
}

}  // namespace scope
