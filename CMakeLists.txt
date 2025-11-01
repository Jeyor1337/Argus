cmake_minimum_required(VERSION 3.10)
project(argus C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(WIN32)
    # Windows-specific settings
elseif(UNIX)
    # Unix-like systems (Linux, macOS, Termux)
    add_compile_options(-pthread)
endif()

add_executable(argus src/argus.c)

# Find and link threads library
find_package(Threads REQUIRED)
target_link_libraries(argus Threads::Threads)
