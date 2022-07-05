#include "indent.h"

namespace scope {

namespace {

int sign(int i) { return (i > 0) - (i < 0); }

}  // namespace

unsigned Indentation::num_spaces() const {
  if (max_spaces <= 0 || sign(per_indent) * sign(indent) <= 0) {
    return 0;
  }
  if (abs(indent) <= max_spaces / abs(per_indent)) {
    return indent * per_indent;
  }
  return max_spaces;
}

std::ostream& operator<<(std::ostream& out, const Indentation& indent) {
  unsigned num_spaces = indent.num_spaces();
  for (unsigned i = 0; i < num_spaces; ++i) {
    out << ' ';
  }
  return out;
}
}  // namespace scope
