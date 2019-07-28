cmake_minimum_required (VERSION 3.14)
project (mc833-projeto-1)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#include_directories(${PROJECT_SOURCE_DIR})

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_SOURCE_DIR ${CMAKE_HOME_DIRECTORY}/src)

# Specify the minimum version you require.
find_package (libmongoc-1.0)

message ("--   mongoc found version \"${MONGOC_VERSION}\"")
message ("--   mongoc include path \"${MONGOC_INCLUDE_DIRS}\"")
message ("--   mongoc libraries \"${MONGOC_LIBRARIES}\"")

add_executable (servidor ${CMAKE_SOURCE_DIR}/servidor.c)
add_executable (cliente ${CMAKE_SOURCE_DIR}/cliente.c)

target_include_directories (servidor PRIVATE "${MONGOC_INCLUDE_DIRS}")
target_link_libraries (servidor PRIVATE "${MONGOC_LIBRARIES}")
target_compile_definitions (servidor PRIVATE "${MONGOC_DEFINITIONS}")
