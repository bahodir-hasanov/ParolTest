cmake_minimum_required(VERSION 3.14)
project(PasswordAnalyzer)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

add_executable(app main.cpp)

target_include_directories(app PRIVATE
    ${GTK3_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}            # yoki /include
)

target_link_libraries(app ${GTK3_LIBRARIES})