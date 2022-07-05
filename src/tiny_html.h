#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "indent.h"

namespace scope {

template <typename T, typename... Args>
void inplace_move_vector_impl(std::vector<T>&) {}

template <typename T, typename... Args>
void inplace_move_vector_impl(std::vector<T>& res, T&& a, Args&&... args) {
  res.push_back(std::move(a));
  inplace_move_vector_impl(res, std::move(args)...);
}

template <typename T, typename... Args>
std::vector<T> inplace_move_vector(Args&&... args) {
  std::vector<T> res;
  res.reserve(sizeof...(Args));
  inplace_move_vector_impl(res, std::move(args)...);
  return res;
}

std::string escape_html(const std::string& str);

struct HtmlNode {
  HtmlNode() = default;
  HtmlNode(const HtmlNode&) = delete;
  HtmlNode(HtmlNode&&) = default;
  HtmlNode& operator=(const HtmlNode&) = delete;
  HtmlNode& operator=(HtmlNode&&) = default;

  template <typename... Nodes>
  HtmlNode(
      std::string _tag, std::map<std::string, std::string> _attributes,
      Nodes&&... _children)
      : tag(std::move(_tag)),
        attributes(std::move(_attributes)),
        children(inplace_move_vector<HtmlNode>(std::move(_children)...)) {
    if (!tag.empty() && tag.back() == '|') {
      children_inline = true;
      tag.pop_back();
    } else if (!tag.empty() && tag.back() == '<') {
      increase_indent = false;
      tag.pop_back();
    }
    if (tag.empty()) {
      increase_indent = false;
    }
  }

  static HtmlNode create_text(std::string text) {
    HtmlNode res{"", {{"", std::move(text)}}};
    res.children_inline = true;
    return res;
  }

  static HtmlNode create_text_raw(std::string text) {
    HtmlNode res{"", {{"raw", std::move(text)}}};
    res.children_inline = true;
    return res;
  }

  static HtmlNode create_node_with_text(std::string tag, std::string text) {
    HtmlNode res{std::move(tag), {}, create_text(std::move(text))};
    res.children_inline = true;
    return res;
  }

  static HtmlNode create_node_with_text_raw(std::string tag, std::string text) {
    HtmlNode res{std::move(tag), {}, create_text_raw(std::move(text))};
    res.children_inline = true;
    return res;
  }

  void add_child(HtmlNode&& child) { children.push_back(std::move(child)); }
  void add_class(const std::string& class_name) {
    std::string& curr = attributes["class"];
    if (!curr.empty()) {
      curr += ' ';
    }
    curr += class_name;
  }

  std::string tag;
  std::map<std::string, std::string> attributes;
  std::vector<HtmlNode> children;

  bool children_inline = false;
  bool increase_indent = true;

  void print_me(std::ostream& out, Indentation indent) const;
  friend std::ostream& operator<<(std::ostream& out, const HtmlNode& node) {
    node.print_me(out, {});
    return out;
  }

 private:
  void print_me_as_group(std::ostream& out, Indentation indent) const;
};

}  // namespace scope
