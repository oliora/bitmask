add_executable(test_bitmask test.cpp)
target_link_libraries(test_bitmask bitmask)
add_test(NAME test_bitmask COMMAND test_bitmask)
