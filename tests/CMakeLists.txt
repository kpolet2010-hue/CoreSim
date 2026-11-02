add_executable(coresim_tests test_main.cpp)
target_link_libraries(coresim_tests PRIVATE coresim_core)

add_test(NAME coresim_tests COMMAND coresim_tests)
