add_executable(fuzz fuzz.cpp)

target_compile_options(fuzz PRIVATE -fsanitize=fuzzer)
target_link_options(fuzz PRIVATE -fsanitize=fuzzer)
target_link_libraries(fuzz dd_trace_cpp-static)
