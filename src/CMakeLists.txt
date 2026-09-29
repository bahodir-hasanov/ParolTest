cmake_minimum_required(VERSION 3.14)
project(ParolTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    add_executable(app WIN32 ui_win32.cpp)
    target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(app PRIVATE user32 gdi32 comctl32)
    if(MSVC)
        target_compile_options(app PRIVATE /utf-8 /W3)
        target_compile_definitions(app PRIVATE UNICODE _UNICODE _WIN32_WINNT=0x0601 WINVER=0x0601)
    else()
        target_compile_definitions(app PRIVATE UNICODE _UNICODE _WIN32_WINNT=0x0601 WINVER=0x0601)
        target_link_options(app PRIVATE -mwindows)
    endif()
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    add_executable(app main.cpp)
    target_include_directories(app PRIVATE
        ${GTK3_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    target_link_directories(app PRIVATE ${GTK3_LIBRARY_DIRS})
    target_link_libraries(app PRIVATE ${GTK3_LIBRARIES})
    target_compile_options(app PRIVATE ${GTK3_CFLAGS_OTHER})
endif()

set_target_properties(app PROPERTIES
    OUTPUT_NAME paroltest
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}
)
