#include "scope_info.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "tiny_html.h"

namespace scope {

#define CHECK(x)                                                           \
  if (!(x)) {                                                              \
    std::cerr << __FILE__ << ":" << __LINE__ << ": ERROR! " << #x << "\n"; \
    exit(0);                                                               \
  }

void ScopeInfo::fix_self_time_recursive() {
  double children_time = 0.;
  for (ScopeInfo& child : children) {
    child.fix_self_time_recursive();
    children_time += child.timer.total_time;
  }
  timer.self_time = timer.total_time - children_time;
}

void ScopeInfo::merge_with(const ScopeInfo& other) {
  CHECK(location == other.location);
  timer += other.timer;
  std::map<Location, size_t> location_to_idx;
  for (size_t i = 0; i < children.size(); ++i) {
    location_to_idx[children[i].location] = i;
  }
  for (const ScopeInfo& other_child : other.children) {
    size_t idx = location_to_idx.emplace(other_child.location, children.size())
                     .first->second;
    if (idx == children.size()) {
      children.push_back(other_child);
    } else {
      children[idx].merge_with(other_child);
    }
  }
}

void ScopeInfo::merge_child(const ScopeInfo& other) {
  for (ScopeInfo& child : children) {
    if (other.location == child.location) {
      child.merge_with(other);
      return;
    }
  }
  children.push_back(other);
}

void ScopeInfo::diff_from(const ScopeInfo& other) {
  CHECK(location == other.location);
  timer -= other.timer;
  std::map<Location, size_t> location_to_idx;
  for (size_t i = 0; i < children.size(); ++i) {
    location_to_idx[children[i].location] = i;
  }
  for (const ScopeInfo& other_child : other.children) {
    size_t idx = location_to_idx.emplace(other_child.location, children.size())
                     .first->second;
    CHECK(idx < children.size());
    children[idx].diff_from(other_child);
  }
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
  return std::string(loc.file) + ":" + std::to_string(loc.line);
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
  return res + ':' + std::to_string(loc.line);
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

void merge_scopes_recursive(const ScopeInfo& src, ScopeInfo& target) {
  target.merge_child(src);
  for (const ScopeInfo& child : src.children) {
    merge_scopes_recursive(child, target);
  }
}

struct DocumentBuilderBase {
  enum class StyleType {
    STYLE_GROUP,
    STYLE_TIME_PERCENT,
    STYLE_LOCATION,
    STYLE_TIME,
    STYLE_COUNT,
    STYLE_NONE
  };
  HtmlNode description_of(const ScopeInfo& scope_info, double total_time) const;
  HtmlNode collapsible_scope_info(
      const ScopeInfo& scope_info, double total_time) const;
  std::string build(ScopeInfo scope_info) const;

 private:
  virtual HtmlNode simple_span(
      std::string content, StyleType, std::string title) const = 0;
  virtual HtmlNode collapsible_item(
      HtmlNode description, std::optional<HtmlNode> content) const = 0;
  virtual HtmlNode collapsible_container() const = 0;
  virtual HtmlNode heading(size_t depth, std::string text) const = 0;
  virtual std::string finalize(HtmlNode content) const = 0;
};

HtmlNode DocumentBuilderBase::description_of(
    const ScopeInfo& scope_info, double total_time) const {
  if (scope_info.location.func == nullptr) {
    return simple_span("====", StyleType::STYLE_GROUP, "");
  }
  if (scope_info.location.file == nullptr) {
    return simple_span(scope_info.location.func, StyleType::STYLE_GROUP, "");
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
  std::string count =
      "for " + num_with_commas(scope_info.timer.count) + " times";
  std::string time_per_call =
      rate_str(scope_info.timer.total_time, scope_info.timer.count);
  return HtmlNode{
      "",
      {},
      simple_span(
          time_percent, StyleType::STYLE_TIME_PERCENT, time_percent_self),
      simple_span(location, StyleType::STYLE_LOCATION, alt_location),
      simple_span(time, StyleType::STYLE_TIME, time_self),
      simple_span(count, StyleType::STYLE_COUNT, time_per_call)};
}

HtmlNode DocumentBuilderBase::collapsible_scope_info(
    const ScopeInfo& scope_info, double total_time) const {
  HtmlNode description = description_of(scope_info, total_time);
  if (scope_info.children.empty()) {
    return collapsible_item(std::move(description), std::nullopt);
  }

  HtmlNode content = collapsible_container();
  for (const ScopeInfo& child : scope_info.children) {
    content.add_child(collapsible_scope_info(child, total_time));
  }
  return collapsible_item(std::move(description), std::move(content));
}

std::string DocumentBuilderBase::build(ScopeInfo scope_info) const {
  scope_info.sort_children_recursive();
  double total_time = scope_info.timer.total_time;
  HtmlNode content{"", {}};
  content.add_child(heading(1, "Run Took " + sec_to_str(total_time)));
  content.add_child(heading(2, "Profiling Tree"));
  {
    HtmlNode tmp = collapsible_container();
    tmp.add_child(collapsible_scope_info(scope_info, total_time));
    content.add_child(std::move(tmp));
  }
  content.add_child(heading(2, "Per Function Summary"));
  {
    HtmlNode tmp = collapsible_container();

    ScopeInfo summary;
    merge_scopes_recursive(scope_info, summary);

    summary.sort_children_recursive();
    summary.location.func = "Summary - ordered by total time";
    tmp.add_child(collapsible_scope_info(summary, total_time));

    summary.sort_children_recursive([](const ScopeInfo& a, const ScopeInfo& b) {
      return a.timer.self_time > b.timer.self_time;
    });
    summary.location.func = "Summary - ordered by self time";
    tmp.add_child(collapsible_scope_info(summary, total_time));

    content.add_child(std::move(tmp));
  }
  return finalize(std::move(content));
}

struct HtmlBuilder : DocumentBuilderBase {
  static constexpr const char* COLLAPSIBLE_TITLE_EMPTY =
      "collapsible_title_empty";
  static constexpr const char* COLLAPSIBLE_TITLE = "collapsible_title";
  static const char* style_to_class_name(StyleType style) {
    switch (style) {
      case StyleType::STYLE_GROUP:
        return "scope_group";
      case StyleType::STYLE_TIME_PERCENT:
        return "scope_time_percent";
      case StyleType::STYLE_LOCATION:
        return "scope_location";
      case StyleType::STYLE_TIME:
        return "scope_time";
      case StyleType::STYLE_COUNT:
        return "scope_count";
      case StyleType::STYLE_NONE:
        return "";
      default:
        CHECK(false)
    }
  }
  virtual HtmlNode simple_span(
      std::string content, StyleType style, std::string title) const override {
    HtmlNode span = HtmlNode::create_node_with_text("span", std::move(content));
    std::string class_name = style_to_class_name(style);
    if (!class_name.empty()) {
      span.attributes["class"] = std::move(class_name);
    }
    if (!title.empty()) {
      span.attributes["title"] = std::move(title);
    }
    return span;
  }

  virtual HtmlNode collapsible_item(
      HtmlNode description, std::optional<HtmlNode> content) const override {
    const char* class_name =
        content.has_value() ? COLLAPSIBLE_TITLE : COLLAPSIBLE_TITLE_EMPTY;
    HtmlNode li{
        "li",
        {},
        HtmlNode{
            "span",
            {{"class", class_name}},
            std::move(description),
        }};
    if (content.has_value()) {
      li.add_child(std::move(content.value()));
    }
    return li;
  }
  virtual HtmlNode collapsible_container() const override {
    return HtmlNode{"ul", {{"class", "no_bullet"}}};
  }
  virtual HtmlNode heading(size_t depth, std::string text) const override {
    CHECK(depth >= 1);
    return HtmlNode::create_node_with_text(
        "h" + std::to_string(depth), std::move(text));
  }

  static const std::string css;
  static const std::string javascript;
  virtual std::string finalize(HtmlNode content) const override {
    HtmlNode html{
        "html<",
        {},
        HtmlNode{
            "head<", {}, HtmlNode::create_node_with_text_raw("style", css)},
        HtmlNode{
            "body<",
            {},
            std::move(content),
            HtmlNode::create_node_with_text_raw("script", javascript),
        },
    };
    std::ostringstream out;
    out << "<!doctype html>\n" << html << '\n';
    return out.str();
  }
};

struct MdBuilder : DocumentBuilderBase {
  static const char* style_to_style(StyleType style) {
    switch (style) {
      case StyleType::STYLE_GROUP:
        return "font-style: italic;";
      case StyleType::STYLE_TIME_PERCENT:
        return "color: red;";
      case StyleType::STYLE_LOCATION:
        return "font-family: monospace; color: black;";
      case StyleType::STYLE_TIME:
        return "color: green;";
      case StyleType::STYLE_COUNT:
        return "color: orange;";
      case StyleType::STYLE_NONE:
        return "";
      default:
        CHECK(false)
    }
  }

  virtual HtmlNode simple_span(
      std::string content, StyleType style, std::string title) const override {
    std::string style_str = style_to_style(style);
    HtmlNode span =
        HtmlNode::create_node_with_text_raw("span", std::move(content));
    if (!style_str.empty()) {
      span.attributes["style"] = std::move(style_str);
    }
    if (!title.empty()) {
      span.attributes["title"] = std::move(title);
    }
    return span;
  }
  virtual HtmlNode collapsible_item(
      HtmlNode description, std::optional<HtmlNode> content) const override {
    if (!content.has_value()) {
      return HtmlNode{"p|", {}, std::move(description)};
    }
    return HtmlNode{
        "details|",
        {},
        HtmlNode{"summary", {}, std::move(description)},
        HtmlNode{"blockquote", {}, std::move(content.value())},
    };
  }
  virtual HtmlNode collapsible_container() const override {
    return HtmlNode{"", {}};
  }
  virtual HtmlNode heading(size_t depth, std::string text) const override {
    CHECK(depth >= 1);
    return HtmlNode::create_text_raw(
        '\n' + std::string(depth + 1, '#') + " " + std::move(text) + '\n');
  }
  virtual std::string finalize(HtmlNode content) const override {
    std::ostringstream out;
    out << content << '\n';
    return out.str();
  }
};

const std::string HtmlBuilder::css = R"xxx(
.collapsible_title::before {
  content: '\25B6';
  color: blue;
  display: inline-block;
  width: 1em;
  height: 1em;
  transition: transform 0.25s;
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
  color: orange;
}

.scope_group {
  font-style: italic;
}

)xxx";

const std::string HtmlBuilder::javascript = R"xxx(
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

}  // namespace

std::string scope_info_html(ScopeInfo scope_info) {
  HtmlBuilder builder;
  return builder.build(std::move(scope_info));
}

std::string scope_info_md(ScopeInfo scope_info) {
  MdBuilder builder;
  return builder.build(std::move(scope_info));
}

}  // namespace scope
