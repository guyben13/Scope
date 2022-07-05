#pragma once

#include <iostream>

namespace scope {

/**
 * A simple struct that keeps track of indentation.
 *
 * Is much too annoying to write, because of C++ issues :(
 */
struct Indentation {
  int indent = 0;
  int per_indent = 2;
  int max_spaces = 80;

  unsigned num_spaces() const;

  Indentation& operator+=(int change) {
    indent += change;
    return *this;
  }
  Indentation& operator-=(int change) {
    indent -= change;
    return *this;
  }
  friend Indentation operator+(Indentation indent, int change) {
    indent += change;
    return indent;
  }
  friend Indentation operator+(int change, Indentation indent) {
    indent += change;
    return indent;
  }
  friend Indentation operator-(Indentation indent, int change) {
    indent -= change;
    return indent;
  }
  Indentation& operator++() {
    (*this) += 1;
    return *this;
  }
  Indentation operator++(int) {
    Indentation res = *this;
    (*this) += 1;
    return res;
  }
  Indentation& operator--() {
    (*this) -= 1;
    return *this;
  }
  Indentation operator--(int) {
    Indentation res = *this;
    (*this) -= 1;
    return res;
  }

  friend std::ostream& operator<<(std::ostream& out, const Indentation& indent);
};

}  // namespace scope
