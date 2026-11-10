add_executable(coresim_tests
    test_main.cpp
    test_alu.cpp
    test_memory.cpp
    test_cpu.cpp
    test_register.cpp
    test_cpu_state.cpp
    test_decoder.cpp
    test_isa.cpp
    test_loader.cpp
    test_examples.cpp
    test_assembler.cpp
)
target_link_libraries(coresim_tests PRIVATE coresim_core)
target_compile_definitions(coresim_tests PRIVATE CORESIM_EXAMPLES_DIR="${PROJECT_SOURCE_DIR}/examples")

add_test(NAME coresim_tests COMMAND coresim_tests)
