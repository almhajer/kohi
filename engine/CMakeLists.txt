cmake_minimum_required(VERSION 3.10)
project(engine_library)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp
)

message(STATUS "عدد ملفات engine المصدرية: ${SOURCE_FILES}")

if (WIN32)
    message(STATUS "Building engine library for Windows")
    add_compile_definitions(KPLATFORM_WINDOWS=1)
    add_library(engine SHARED
            ${SOURCE_FILES}
            platform/platform_win32.cpp
            src/core/logger.cpp
    )
elseif (APPLE)

    message(STATUS "Building engine library for Apple/macOS")
    add_compile_definitions(KPLATFORM_MACOS=1)
    add_library(engine SHARED
            ${SOURCE_FILES}
            platform/platform_apple.mm
            src/core/logger.cpp
    )

    # الربط مع مكتبات النظام يجب أن يكون بعد add_library
    target_link_libraries(engine PRIVATE
            "-framework Cocoa"
            "-framework CoreFoundation"
    )
else ()
    message(STATUS "Building engine library for other platforms")
    add_library(engine SHARED
            ${SOURCE_FILES}
            src/core/application.cpp

            # أضف ملفات منصة أخرى هنا
    )
endif ()

target_include_directories(engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/platform
)
