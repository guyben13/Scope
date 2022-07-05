#include "scope.h"

#include <cmath>
#include <fstream>
#include <iostream>

#define OUT(x) #x " = " << (x) << '\n'

double nilakantha_pi(double requested_prec) {
  SCOPE_LOG();
  double res = 3;
  size_t n = 2;
  int sign = 1;
  while (true) {
    double term = 4. / (double(n) * (n + 1) * (n + 2));
    res += sign * term;
    n += 2;
    sign = -sign;
    if (term < requested_prec) {
      break;
    }
  }
  return res;
}

double leibniz_pi(double requested_prec) {
  SCOPE_LOG();
  double res = 4;
  size_t n = 3;
  int sign = -1;
  while (true) {
    double term = 4. / double(n);
    res += sign * term;
    n += 2;
    sign = -sign;
    if (term < requested_prec) {
      break;
    }
  }
  return res;
}

double area_pi(double requested_prec) {
  SCOPE_LOG();
  // We divide the 1x1 "first quarter" square into n^2 cells, and count how many
  // are inside vs. outside of the (quarter) circle. The result is
  // count=pi/4*n^2 squares inside, so we get pi = count * 4 / n^2
  //
  // There are 2n squares that are "on the edge" and hence randomally go one way
  // or the other. The error in the count is hence sqrt(2n) - meaning the error
  // is pi is prec=4*sqrt(2n)/n^2. This gives us n^1.5=2^2.5/prec
  size_t n = std::pow(std::pow(2, 2.5) / requested_prec, 1 / 1.5);
  size_t n2 = n * n;
  size_t count = 0;
  for (size_t i = 0; i < n; ++i) {
    size_t x = 2 * i + 1;
    size_t x2 = x * x;
    for (size_t j = 0; j < n; ++j) {
      size_t y = 2 * j + 1;
      size_t r = x2 + y * y;
      count += r < 4 * n2;
    }
  }
  return 4. * count / n2;
}

double edge_pi(double requested_prec) {
  SCOPE_LOG();
  // We divide the 1x1 "first quarter" square into n^2 cells, and count how many
  // are inside vs. outside of the (quarter) circle. The result is
  // count=pi/4*n^2 squares inside, so we get pi = count * 4 / n^2
  //
  // There are 2n squares that are "on the edge" and hence randomally go one way
  // or the other. The error in the count is hence sqrt(2n) - meaning the error
  // is pi is prec=4*sqrt(2n)/n^2. This gives us n^1.5=2^2.5/prec
  size_t n = std::pow(std::pow(2, 2.5) / requested_prec, 1 / 1.5);
  size_t n2 = n * n;
  size_t count = 0;
  size_t j = n;
  for (size_t i = 0; i < n; ++i) {
    size_t x = 2 * i + 1;
    size_t x2 = x * x;
    while (true) {
      size_t y = 2 * j + 1;
      size_t r = x2 + y * y;
      if (r < 4 * n2 || j == 0) {
        break;
      }
      --j;
    }
    count += j + 1;
  }
  return 4. * count / n2;
}

int main() {
  double prec = 1e-6;
  std::cerr << OUT(nilakantha_pi(prec) - M_PI)  //
            << OUT(leibniz_pi(prec) - M_PI)     //
            << OUT(area_pi(prec) - M_PI) //
            << OUT(edge_pi(prec) - M_PI)  //
            << OUT(prec);
  scope::ScopeInfo scope_info = scope::thread_scope();
  std::cerr << scope_info_str(scope_info);
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
