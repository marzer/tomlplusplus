set ( src_example_parsing
      example_parsing.cpp
)

add_executable( example_parsing ${src_example_parsing} )

target_include_directories( example_parsing PRIVATE ../include )