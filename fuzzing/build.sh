cd $SRC/tomlplusplus
mkdir -p build
cmake -S . -B build -DBUILD_FUZZER=ON && cmake --build build --target install

# Build the corpus using the existing toml files in the source
mkdir -p corpus
find $SRC/tomlplusplus -name "*.toml" -exec cp {} corpus \;
zip -q -j $OUT/toml_fuzzer_seed_corpus.zip corpus/*
