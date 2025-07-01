cmake_minimum_required(VERSION 3.15)

project(gocxx_io LANGUAGES CXX)

# Create the gocxx_io library
add_library(gocxx_io
    include/gocxx/io/io.h
    src/io.cpp
)

# Public headers
target_include_directories(gocxx_io
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# C++ standard & warnings
target_compile_features(gocxx_io PUBLIC cxx_std_17)
target_compile_options(gocxx_io PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

# Installation (optional)
install(TARGETS gocxx_io
    EXPORT gocxx_ioTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Generate pkg-config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gocxx_io.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gocxx_io.pc @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gocxx_io.pc
        DESTINATION lib/pkgconfig)


install(DIRECTORY include/ DESTINATION include)

install(EXPORT gocxx_ioTargets
    FILE gocxx_ioTargets.cmake
    NAMESPACE gocxx::
    DESTINATION lib/cmake/gocxx_io
)

# Enable unit tests
enable_testing()

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# Prevent gmock/gtest from building shared libraries
set(BUILD_GMOCK ON)
set(BUILD_GTEST ON)
set(INSTALL_GTEST OFF)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Build test binary
add_executable(gocxx_io_test tests/io_test.cpp)
target_link_libraries(gocxx_io_test PRIVATE gocxx_io gmock_main)
add_test(NAME gocxx_io_test COMMAND gocxx_io_test)
