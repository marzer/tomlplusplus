#!/usr/bin/env bash
cd "$SRC/tomlplusplus" || exit 1
mkdir -p build
cmake -S . -B build -DBUILD_FUZZER=ON && cmake --build build --target install

# Build the corpus using the existing toml files in the source
mkdir -p corpus
find "$SRC/tomlplusplus" -name "*.toml" -exec cp {} corpus \;
zip -q -j "$OUT/toml_fuzzer_seed_corpus.zip" corpus/*

# Build unit test
mapfile -t test_sources < <(sed 's#^#tests/#' tests/sources.txt)
clang++ -std=c++17 -O2 -DUSE_VENDORED_LIBS=1 \
    -Iinclude -Itests \
    "${test_sources[@]}" \
    -o unit_tests \
    -pthread
