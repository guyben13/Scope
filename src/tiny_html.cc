#include "tiny_html.h"

namespace scope {

std::string escape_html(const std::string& str) {
  std::string res;
  for (char c : str) {
    switch (c) {
      case '<':
        res += "&lt;";
        break;
      case '>':
        res += "&gt;";
        break;
      case '&':
        res += "&amp;";
        break;
      case '\n':
        res += "&#10;";
        break;
      case '"':
        res += "&quot;";
        break;
      case '\'':
        res += "&#39;";
        break;
      default:
        res += c;
        break;
    }
  }
  return res;
}

void HtmlNode::print_me_as_group(std::ostream& out, Indentation indent) const {
  bool first = true;
  for (const HtmlNode& child : children) {
    if (!children_inline && !first) {
      out << '\n' << indent;
    }
    first = false;
    child.print_me(out, indent);
  }
}

void HtmlNode::print_me(std::ostream& out, Indentation indent) const {
  if (tag.empty()) {
    if (attributes.empty()) {
      print_me_as_group(out, indent);
      return;
    }
    if (attributes.size() == 1) {
      if (attributes.count("")) {
        out << escape_html(attributes.at(""));
        return;
      }
      if (attributes.count("raw")) {
        out << attributes.at("raw");
        return;
      }
    }
    // This isn't legal. Print nothing.
    return;
  }
  // The assumption is always:
  // - We enter this function in the right indentation
  // - we leave this function right at the end of the last line (before newline)
  if (tag == "html") {
    out << "<!doctype html>\n" << indent;
  }
  out << '<' << escape_html(tag);
  for (const auto& [key, value] : attributes) {
    out << ' ' << escape_html(key) << "=\"" << escape_html(value) << '"';
  }
  if (children.empty()) {
    out << "/>";
    return;
  }
  out << '>';
  Indentation next_indent = indent;
  if (!children_inline && increase_indent) {
    next_indent = next_indent + 1;
  }
  if (!children_inline) {
    out << '\n' << next_indent;
  }
  print_me_as_group(out, next_indent);
  if (!children_inline) {
    out << '\n' << indent;
  }
  out << "</" << escape_html(tag) << ">";
}

}  // namespace scope
