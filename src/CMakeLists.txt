add_library(indent indent.cc)
add_library(tiny_html tiny_html.cc)
target_link_libraries(tiny_html indent)
add_library(scope_info scope_info.cc)
target_link_libraries(scope_info tiny_html)
add_library(scope scope.cc)
target_link_libraries(scope scope_info)

add_executable(scope_info_test scope_info_test.cc)
target_link_libraries(scope_info_test scope_info)
add_executable(scope_test scope_test.cc)
target_link_libraries(scope_test scope)
