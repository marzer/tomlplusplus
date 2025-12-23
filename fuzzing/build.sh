cd $SRC/tomlplusplus
mkdir -p build
cmake -S . -B build -DBUILD_FUZZER=ON && cmake --build build --target install

# Build the corpus using the existing toml files in the source
mkdir -p corpus
find $SRC/tomlplusplus -name "*.toml" -exec cp {} corpus \;
zip -q -j $OUT/toml_fuzzer_seed_corpus.zip corpus/*

# Build unit test
clang++ -std=c++17 -O2 -DUSE_VENDORED_LIBS=1 \
    -Iinclude -Itests \
    tests/at_path.cpp \
    tests/conformance_burntsushi_invalid.cpp \
    tests/conformance_burntsushi_valid.cpp \
    tests/conformance_iarna_invalid.cpp \
    tests/conformance_iarna_valid.cpp \
    tests/formatters.cpp \
    tests/for_each.cpp \
    tests/impl_toml.cpp \
    tests/main.cpp \
    tests/manipulating_arrays.cpp \
    tests/manipulating_parse_result.cpp \
    tests/manipulating_tables.cpp \
    tests/manipulating_values.cpp \
    tests/parsing_arrays.cpp \
    tests/parsing_booleans.cpp \
    tests/parsing_comments.cpp \
    tests/parsing_dates_and_times.cpp \
    tests/parsing_floats.cpp \
    tests/parsing_integers.cpp \
    tests/parsing_key_value_pairs.cpp \
    tests/parsing_spec_example.cpp \
    tests/parsing_strings.cpp \
    tests/parsing_tables.cpp \
    tests/path.cpp \
    tests/tests.cpp \
    tests/user_feedback.cpp \
    tests/using_iterators.cpp \
    tests/visit.cpp \
    tests/windows_compat.cpp \
    -o unit_tests \
    -pthread
