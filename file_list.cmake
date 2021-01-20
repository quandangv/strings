# paths to various directories
get_filename_component(GENERATED_HEADERS_DIR ${CMAKE_BINARY_DIR}/generated-headers ABSOLUTE)
get_filename_component(INCLUDE_DIR         ${CMAKE_SOURCE_DIR}/include ABSOLUTE)
get_filename_component(PRIVATE_HEADERS_DIR ${CMAKE_SOURCE_DIR}/private-headers ABSOLUTE)
get_filename_component(SRC_DIR             ${CMAKE_SOURCE_DIR}/src ABSOLUTE)
get_filename_component(TEST_DIR            ${CMAKE_SOURCE_DIR}/test ABSOLUTE)
get_filename_component(LIBRARY_DIR         ${CMAKE_SOURCE_DIR}/lib ABSOLUTE)
set(HEADER_DIRS ${INCLUDE_DIR} ${PRIVATE_HEADERS_DIR} ${GENERATED_HEADERS_DIR})

# configure files {{{
  if(PLATFORM EQUAL "Linux")
    add_compile_definitions(PLATFORM_LINUX)
  endif()

  unset(DEBUG_SCOPES CACHE)
# }}}

# public headers
set(HEADERS
  ${INCLUDE_DIR}/tstring.hpp
  ${INCLUDE_DIR}/token_iterator.hpp
)

# source files
set(TSTRING_SOURCES
  ${SRC_DIR}/tstring.cpp
  ${SRC_DIR}/tstring_utils.cpp
)
set(TOKEN_ITERATOR_SOURCES
  ${SRC_DIR}/token_iterator.cpp
)
set(SOURCES ${TSTRING_SOURCES} ${TOKEN_ITERATOR_SOURCES})

# tests
set(EXTERNAL_TESTS tstring)
