add_executable(coresim_tests
    test_main.cpp
    test_alu.cpp
    test_memory.cpp
    test_cpu.cpp
    test_register.cpp
    test_cpu_state.cpp
    test_decoder.cpp
    test_isa.cpp
)
target_link_libraries(coresim_tests PRIVATE coresim_core)

add_test(NAME coresim_tests COMMAND coresim_tests)
