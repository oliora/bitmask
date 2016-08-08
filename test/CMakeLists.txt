add_executable(test_bitmask test.cpp)

target_include_directories(test_bitmask PRIVATE "${CMAKE_SOURCE_DIR}/include")

add_test(NAME test_bitmask COMMAND test_bitmask)
