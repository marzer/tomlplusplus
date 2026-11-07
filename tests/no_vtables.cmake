execute_process(
    COMMAND "${NM}" "${BINARY}"
    OUTPUT_VARIABLE symbols
    RESULT_VARIABLE result
)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "${NM} failed on ${BINARY}")
endif()

string(REGEX MATCHALL "_ZTVN4toml[A-Za-z0-9_]*" vtables "${symbols}")
if(vtables)
  message(FATAL_ERROR "toml++ vtables emitted by TUs that build no nodes: ${vtables}")
endif()
