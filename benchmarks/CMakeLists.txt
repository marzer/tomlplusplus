add_executable(tomlplusplus_benchmarks benchmarks.cpp)
tomlplusplus_set_compile_options(tomlplusplus_benchmarks)
# where a hot function lands relative to a 64-byte boundary can move a timing by 10%
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND NOT MSVC)
    target_compile_options(tomlplusplus_benchmarks PRIVATE -falign-functions=64)
endif()
target_link_libraries(tomlplusplus_benchmarks PRIVATE tomlplusplus::tomlplusplus)
target_compile_definitions(
    tomlplusplus_benchmarks PRIVATE
    "TOMLPLUSPLUS_BENCHMARK_DATA=\"${PROJECT_SOURCE_DIR}/examples/benchmark_data.toml\""
)
