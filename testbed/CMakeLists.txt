# CMakeLists.txt داخل مجلد testbed
cmake_minimum_required(VERSION 3.10)

# إضافة البرنامج التنفيذي الرئيسي
add_executable(testbed src/entry.cpp
        src/game.cpp
)


# ربط البرنامج التنفيذي بالمكتبة الديناميكية المحملة مسبقًا
target_link_libraries(testbed PRIVATE engine)
# ✅ إضافة تعريفات D_DEBUG و KIMPORT
target_compile_definitions(testbed PRIVATE _DEBUG KIMPORT)
