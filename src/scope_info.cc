#include "scope_info.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "tiny_html.h"

namespace scope {

void ScopeInfo::fix_self_time_recursive() {
  double children_time = 0.;
  for (ScopeInfo& child : children) {
    child.fix_self_time_recursive();
    children_time += child.timer.total_time;
  }
  timer.self_time = timer.total_time - children_time;
}

void ScopeInfo::sort_children_recursive() {
  for (ScopeInfo& child : children) {
    child.sort_children_recursive();
  }
  std::sort(
      children.begin(), children.end(),
      [](const ScopeInfo& a, const ScopeInfo& b) {
        return a.timer.total_time > b.timer.total_time;
      });
}

namespace {

std::string num_with_commas(size_t n) {
  std::ostringstream out;
  out.imbue(std::locale("en_US.UTF8"));
  out << n;
  return out.str();
}

std::string percent_str(double a, double b) {
  if (b == 0 || abs(1000 * a / b) > std::numeric_limits<int>::max()) {
    return "<N/A>";
  }
  int percent = 100 * a / b;
  return num_with_commas(percent) + '%';
}

std::string rate_str(double time, size_t count) {
  if (count == 0) {
    return "";
  }
  time /= count;
  return std::to_string(time) + " secs. each";
}

std::string loc_full_str(const Location& loc) {
  if (!loc.func || !loc.file) {
    return "ERROR";
  }
  return std::string(loc.file) + ":" + num_with_commas(loc.line);
}

std::string loc_simple_str(const Location& loc) {
  if (!loc.func) {
    return "ERROR";
  }
  std::string res(loc.func);
  {
    size_t first_paren = res.find('(');
    if (first_paren != std::string::npos) {
      res = res.substr(0, first_paren);
    }
  }
  {
    size_t last_colon = res.rfind(':');
    if (last_colon != std::string::npos) {
      res = res.substr(last_colon);
    }
  }
  return res + ':' + num_with_commas(loc.line);
}

}  // namespace

namespace {

std::string sec_to_str(double secs) {
  std::ostringstream out;
  if (secs < 0) {
    out << '-';
    secs = -secs;
  }
  size_t si = secs;
  secs -= si;
  size_t hours = si / 3600;
  size_t minutes = (si / 60) % 60;
  size_t seconds = si % 60;
  size_t millis = secs * 1000;
  out << std::setfill('0') << hours << ':' << std::setw(2) << minutes << ':'
      << std::setw(2) << seconds << '.' << std::setw(3) << millis;
  return out.str();
}

const std::string css = R"xxx(
.collapsible_title::before {
  content: '\25B6';
  color: blue;
  display: inline-block;
  width: 1em;
  height: 1em;
}

.collapsible_title_empty::before {
  content: '\25B6';
  color: lightgray;
  display: inline-block;
}

.collapsible_title-open::before {
  transform: rotate(90deg);
}

.collapsible_hidden {
  display: none;
}

.no_bullet {
  list-style-type: none;
}

.scope_time_percent {
  color: red;
}

.scope_location {
  font-family: monospace;
  color: black;
}

.scope_time {
  color: green;
}

.scope_count {
  color: blue;
}

.scope_group {
  font-style: italic;
}

)xxx";

const std::string javascript = R"xxx(
const collection = document.getElementsByClassName("collapsible_title");
const on_click = function() {
  const siblings = this.parentElement.children;
  for (const s of siblings) {
    if (s.classList.contains("collapsible_title")) {
      s.classList.toggle("collapsible_title-open");
    } else {
      s.classList.toggle("collapsible_hidden");
    }
  }
};

for (const e of collection) {
  const siblings = e.parentElement.children
  for (const s of siblings) {
    if (s.classList.contains("collapsible_title")) {
      s.classList.remove("collapsible_title-closed");
      s.onclick = on_click;
    } else {
      s.classList.add("collapsible_hidden");
    }
  }
}
)xxx";

HtmlNode simple_span(
    std::string content, std::string class_name, std::string title) {
  HtmlNode span = HtmlNode::create_node_with_text("span", std::move(content));
  if (!class_name.empty()) {
    span.attributes["class"] = std::move(class_name);
  }
  if (!title.empty()) {
    span.attributes["title"] = std::move(title);
  }
  return span;
}

HtmlNode title_of(const ScopeInfo& scope_info, double total_time) {
  if (scope_info.location.func == nullptr) {
    return simple_span("====", "scope_group", "");
  }
  if (scope_info.location.file == nullptr) {
    return simple_span(scope_info.location.func, "scope_group", "");
  }
  std::string location = loc_simple_str(scope_info.location);
  std::string alt_location =
      loc_full_str(scope_info.location) + '\n' + scope_info.location.func;
  std::string time_percent =
      percent_str(scope_info.timer.total_time, total_time);
  std::string time_percent_self =
      percent_str(scope_info.timer.self_time, total_time) + " self";
  std::string time = sec_to_str(scope_info.timer.total_time);
  std::string time_self = sec_to_str(scope_info.timer.self_time) + " self";
  std::string count = "for " + num_with_commas(scope_info.timer.count) + " times";
  std::string time_per_call =
      rate_str(scope_info.timer.total_time, scope_info.timer.count);
  return HtmlNode{
      "",
      {},
      simple_span(time_percent, "scope_time_percent", time_percent_self),
      simple_span(location, "scope_location", alt_location),
      simple_span(time, "scope_time", time_self),
      simple_span(count, "scope_count", time_per_call)};
}

HtmlNode collapsible_list_item(const ScopeInfo& scope_info, double total_time) {
  const char* class_name = scope_info.children.empty()
                               ? "collapsible_title_empty"
                               : "collapsible_title";
  HtmlNode li{
      "li",
      {},
      HtmlNode{
          "span",
          {{"class", class_name}},
          title_of(scope_info, total_time),
      }};
  if (scope_info.children.empty()) {
    return li;
  }
  HtmlNode content{"ul", {{"class", "no_bullet"}}};
  for (const ScopeInfo& child : scope_info.children) {
    content.add_child(collapsible_list_item(child, total_time));
  }
  li.add_child(std::move(content));
  return li;
}

}  // namespace

std::string scope_info_html(ScopeInfo scope_info) {
  scope_info.sort_children_recursive();
  double total_time = scope_info.timer.total_time;
  std::ostringstream out;
  HtmlNode html{
      "html<",
      {},
      HtmlNode{"head<", {}, HtmlNode::create_node_with_text_raw("style", css)}};
  HtmlNode body{"body<", {}};
  body.add_child(HtmlNode::create_node_with_text(
      "h1", "Run took " + sec_to_str(total_time)));
  body.add_child(HtmlNode::create_node_with_text("h2", "Profiling tree"));
  body.add_child(HtmlNode{
      "ul",
      {{"class", "no_bullet"}},
      collapsible_list_item(scope_info, total_time)});

  body.add_child(HtmlNode::create_node_with_text_raw("script", javascript));
  html.add_child(std::move(body));
  out << html;
  return out.str();
}

}  // namespace scope
