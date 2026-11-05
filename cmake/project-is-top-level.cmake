# These variables are set by project() in CMake 3.21+
string(
    COMPARE EQUAL
    "${CMAKE_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}"
    PROJECT_IS_TOP_LEVEL
)
set("${PROJECT_NAME}_IS_TOP_LEVEL" "${PROJECT_IS_TOP_LEVEL}")
