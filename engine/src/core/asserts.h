#pragma once

#include <cstdio>

#include "../defines.h"

// ===============================================================
// الدالة: report_assertion_failure
// ---------------------------------------------------------------
// تُستخدم هذه الدالة للإبلاغ عن فشل تحقق معين في البرنامج.
// ===============================================================
 KAPI void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line);


// ===============================================================
// الدالة: debugBreak
// ---------------------------------------------------------------
// توقف التنفيذ فورًا لفتح المصحح (Debugger).
// ===============================================================
#if defined(_MSC_VER)
    #define debugBreak() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
    #define debugBreak() __builtin_trap()
#else
    #define debugBreak() *((volatile int*)0) = 0
#endif

// ===============================================================
// ماكرو: KASSERT - تحقق أساسي
// ===============================================================
#define KASSERT(expr)                                                  \
{                                                                      \
if(expr) {}                                                            \
    else{                                                              \
        report_assertion_failure(#expr, "", __FILE__, __LINE__);      \
        debugBreak();                                                  \
    }                                                                  \
}

// ===============================================================
// ماكرو: KASSERT_MSG - تحقق برسالة مخصصة (مفعل فقط في Debug)
// ===============================================================
#ifdef _DEBUG
    #define KASSERT_MSG(expr, message)                                 \
    {                                                                  \
        if (expr) {}                                                   \
            else{                                                      \
            report_assertion_failure(#expr, message, __FILE__, __LINE__); \
            debugBreak();                                              \
        }                                                              \
    }
#else
    #define KASSERT_MSG(expr, message)
#endif

// ===============================================================
// ماكرو: KASSERT_DEBUG - يُفعل فقط في وضع Debug
// ===============================================================
#ifdef _DEBUG
    #define KASSERT_DEBUG(expr) KASSERT(expr)
#else
    #define KASSERT_DEBUG(expr)
#endif
