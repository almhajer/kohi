//
// Created by mac on 21/05/2025 A.
//

// الحارس التضميني لمنع تضمين الملف أكثر من مرة
#ifndef DEFINES_H
#define DEFINES_H

//--------------------------------------------
// أنواع البيانات (Data Types)
//--------------------------------------------

// تعريف أنواع البيانات غير الموقعة (unsigned integer types)
// تستخدم هذه الأنواع لتخزين القيم الموجبة فقط
typedef unsigned char      u8;     // نوع يمثل 8-بت (1 بايت)
typedef unsigned short     u16;    // نوع يمثل 16-بت (2 بايت)
typedef unsigned int       u32;    // نوع يمثل 32-بت (4 بايت)
typedef unsigned long long u64;    // نوع يمثل 64-بت (8 بايت)

// تعريف أنواع البيانات الموقعة (signed integer types)
// هذه الأنواع تدعم القيم السالبة والموجبة
typedef signed char        i8;     // نوع يمثل 8-بت (1 بايت)
typedef signed short       i16;    // نوع يمثل 16-بت (2 بايت)
typedef signed int         i32;    // نوع يمثل 32-بت (4 بايت)
typedef signed long long   i64;    // نوع يمثل 64-بت (8 بايت)

// تعريف أنواع البيانات ذات الفاصلة العائمة (floating point types)
// تستخدم للعمليات التي تتطلب دقة عشرية
typedef float              f32;    // نوع يمثل 32-بت (دقة مفردة)
typedef double             f64;    // نوع يمثل 64-بت (دقة مزدوجة)

// تعريف أنواع القيم المنطقية (Boolean types)
// تُستخدم لتخزين القيم TRUE أو FALSE
typedef int                b8;     // نوع منطقي 8-بت (عادة للتوفير في الذاكرة)
typedef int                b32;    // نوع منطقي 32-بت (للاستخدام العام)

//--------------------------------------------
// التحقق من الحجم الصحيح للأنواع (Static Assertions)
//--------------------------------------------

// نستخدم static_assert أو _Static_assert حسب دعم المترجم
#if defined(__clang__) || defined(__GNUC__) || defined(__apple_build_version__)
    #define STATIC_ASSERT _Static_assert
#else
    #define STATIC_ASSERT static_assert
#endif

// تحقق من أن الأحجام الفعلية للأنواع تطابق ما هو متوقع

// الأنواع غير الموقعة
STATIC_ASSERT(sizeof(u8)  == 1, "Expected u8 to be 1 byte");
STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 bytes");
STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 bytes");
STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 bytes");

// الأنواع الموقعة
STATIC_ASSERT(sizeof(i8)  == 1, "Expected i8 to be 1 byte");
STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 bytes");
STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 bytes");
STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 bytes");

// الأنواع العشرية
STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 bytes");
STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 bytes");

//--------------------------------------------
// القيم المنطقية الثابتة (Boolean Constants)
//--------------------------------------------

// تعريف ثابت للقيم TRUE و FALSE لاستخدامها بدلًا من القيم الرقمية المباشرة
#define TRUE  1
#define FALSE 0

//--------------------------------------------
// التعرف على النظام الأساسي (Platform Detection)
//--------------------------------------------

// تحديد النظام إذا كان Windows
#if defined(WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__)
    #define KPLATFORM_WINDOWS 1
    #ifndef _WIN64
        // دعم فقط أنظمة Windows 64-بت
        #error "Only 64-bit Windows is supported"
    #endif

// تحديد النظام إذا كان Linux
#elif defined(__linux__) || defined(__gnu_linux__)
    #define KPLATFORM_LINUX 1
    // تحديد إذا كان النظام Android
    #if defined(__ANDROID__)
        #define KPLATFORM_ANDROID 1
    #endif

// تحديد الأنظمة التي تعتمد على UNIX
#elif defined(unix)
    #define KPLATFORM_UNIX 1

// أنظمة متوافقة مع POSIX
#elif defined(__POSIX_VERSION__)
    #define KPLATFORM_POSIX 1

// أنظمة Apple (مثل macOS وiOS)
#elif defined(__APPLE__)
    #define KPLATFORM_APPLE 1
    #include <TargetConditionals.h>

    // التحقق إذا كان النظام محاكي iOS
    #if TARGET_IPHONE_SIMULATOR
        #define KPLATFORM_IOS 1
    // التحقق إذا كان النظام macOS
    #elif TARGET_OS_MAC
        #define KPLATFORM_MACOS 1
    // منصة Apple غير معروفة
    #else
        #error "Unknown Apple platform"
    #endif

// إذا لم يتم التعرف على المنصة
#else
    #error "Unknown platform"
#endif

// نهاية الحارس التضميني
#endif // DEFINES_H
//--------------------------------------------
// تصدير/استيراد الرموز (Symbol Export/Import)
//--------------------------------------------

// ملاحظة:
// هذا الجزء يُستخدم لتحديد كيفية تصدير أو استيراد الدوال أو الرموز العامة
// من/إلى المكتبات الديناميكية (Dynamic Libraries)
// يختلف حسب نظام التشغيل والمترجم.

// إذا كان التصدير (Export)
#ifdef KEXPORT

// إذا كان المترجم هو Microsoft Visual C++ (Windows)
#ifdef _MSC_VER
#define KAPI __declspec(dllexport)  // تصدير الرمز باستخدام __declspec
#else
#define KAPI __attribute__((visibility("default")))  // تصدير الرمز في GCC/Clang
#endif

// إذا كان الاستيراد (Import)
#else

// إذا كان المترجم هو Microsoft Visual C++ (Windows)
#ifdef _MSC_VER
#define KAPI __declspec(dllimport)  // استيراد الرمز باستخدام __declspec

// إذا كان Clang أو GCC (Linux / Apple)
#elif defined(__GNUC__) || defined(__clang__)
#define KAPI __attribute__((visibility("default")))  // إظهار الرمز للاستيراد

// دعم Apple: يعامل كـ Clang في الغالب (مشمول أعلاه)
#elif defined(__APPLE__)
#define KAPI __attribute__((visibility("default")))

// إذا لم يكن أي من ذلك، نترك KAPI فارغًا لتجنب الأخطاء
#else
#define KAPI
#endif
#endif // KEXPORT
