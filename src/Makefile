CXX=g++-10
CXXFLAGS=-O3 -Wall -Werror -std=c++20 -pthread
LD=g++-10


tiny_html.o: tiny_html.cc tiny_html.h

scope_info.o: scope_info.cc scope_info.h tiny_html.h

scope_info_test.o: scope_info_test.cc scope_info.h tiny_html.h

scope_info_test: scope_info_test.o scope_info.o tiny_html.o
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@

run_tests: scope_info_test
	./scope_info_test
