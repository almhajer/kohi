cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
# اسم المشروع الرئيسي
project(kohi_project)

# تعيين المجلدات لمخرجات الملفات التنفيذية والمكتبات
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
# إضافة المجلد الخاص بمكتبة engine
add_subdirectory(engine)

# إضافة المجلد الخاص بـ testbed
add_subdirectory(testbed)