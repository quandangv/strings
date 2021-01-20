# paths to various directories
get_filename_component(GENERATED_HEADERS_DIR ${CMAKE_BINARY_DIR}/generated-headers ABSOLUTE)
get_filename_component(PUBLIC_HEADERS_DIR    ${CMAKE_CURRENT_LIST_DIR}/include ABSOLUTE)
get_filename_component(PRIVATE_HEADERS_DIR   ${CMAKE_CURRENT_LIST_DIR}/private-headers ABSOLUTE)
get_filename_component(SRC_DIR               ${CMAKE_CURRENT_LIST_DIR}/src ABSOLUTE)
get_filename_component(TEST_DIR              ${CMAKE_CURRENT_LIST_DIR}/test ABSOLUTE)
get_filename_component(LIBRARY_DIR           ${CMAKE_CURRENT_LIST_DIR}/lib ABSOLUTE)
set(INCLUDE_DIRS ${PUBLIC_HEADERS_DIR} ${PRIVATE_HEADERS_DIR} ${GENERATED_HEADERS_DIR})

# configure files {{{
  if(PLATFORM EQUAL "Linux")
    add_compile_definitions(PLATFORM_LINUX)
  endif()

  unset(DEBUG_SCOPES CACHE)
# }}}

# public headers
set(PUBLIC_HEADERS
  ${PUBLIC_HEADER_DIR}/tstring.hpp
  ${PUBLIC_HEADER_DIR}/token_iterator.hpp
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
set(EXTERNAL_TESTS tstring string_interpolate token_iterator)
