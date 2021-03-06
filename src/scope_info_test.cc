#include "scope_info.h"

#include <fstream>
#include <iostream>

int main() {
  scope::ScopeInfo scope_info{
      {"f1.cc", "func1", 4},
      {14, 14},
      {
          {{"f1.cc", "func2", 8}, {12, 4}, {}},
          {{nullptr, "group"},
           {},
           {
               {{"f1.cc", "func4", 12345}, {3, 2}, {}},
               {{"f1.cc", "func2", 8}, {1'000'000, 2}, {}},
           }},
      }};
  scope_info.fix_self_time_recursive();
  {
    std::string fname = "/tmp/blah.md";
    std::ofstream(fname) << scope_info_md(scope_info) << '\n';
    std::cerr << "write to " << fname << '\n';
  }
  {
    std::string fname = "/tmp/blah.html";
    std::ofstream(fname) << scope_info_html(scope_info);
    std::cerr << "write to " << fname << '\n';
  }
  return 0;
}
