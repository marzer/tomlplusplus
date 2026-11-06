add_executable(tomlplusplus_benchmarks benchmarks.cpp)
tomlplusplus_set_compile_options(tomlplusplus_benchmarks)
target_link_libraries(tomlplusplus_benchmarks PRIVATE tomlplusplus::tomlplusplus)
target_compile_definitions(
    tomlplusplus_benchmarks PRIVATE
    "TOMLPLUSPLUS_BENCHMARK_DATA=\"${PROJECT_SOURCE_DIR}/examples/benchmark_data.toml\""
)
