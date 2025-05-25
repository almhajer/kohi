#pragma once
#include "../defines.h"

// تفعيل المستويات حسب الحاجة (الإعدادات الافتراضية)
#define LOG_WARN_ENABLED   1
#define LOG_INFO_ENABLED   1
#define LOG_DEBUG_ENABLED  1
#define LOG_TRACE_ENABLED  1

// إذا تم تعريف KRELEASE على أنها 1، نعطل المستويات التفصيلية
#if defined(KRELEASE) && KRELEASE == 1
    #undef  LOG_DEBUG_ENABLED
    #undef  LOG_TRACE_ENABLED
    #define LOG_DEBUG_ENABLED 0
    #define LOG_TRACE_ENABLED 0
#endif


/**
 * @brief يحدد مستويات الخطورة لرسائل السجل (Log).
 *
 * يُستخدم هذا التعداد لتحديد درجة خطورة الرسائل التي يتم تسجيلها في نظام السجلات،
 * بدءًا من الأخطاء القاتلة (FATAL) التي تؤدي إلى إنهاء البرنامج، وصولًا إلى رسائل التتبع (TRACE)
 * المفصّلة التي تُستخدم عادة أثناء التطوير لتشخيص المشاكل.
 *
 * مثال على الاستخدام:
 *     log_level level = log_level::LOG_LEVEL_ERROR;
 *     if (level == log_level::LOG_LEVEL_ERROR) {
 *         // التعامل مع خطأ مسجّل
 *     }
 *
 * يُفضل استخدام log_level للتحكم في مستوى التفاصيل الظاهرة في السجل،
 * أو لتصفية الرسائل بحسب أهميتها.
 */
enum class log_level {
    LOG_LEVEL_FATAL = 0, ///< خطأ قاتل يؤدي عادة إلى إيقاف البرنامج.
    LOG_LEVEL_ERROR = 1, ///< خطأ مهم يمكن الاستمرار بعده، لكنه يستوجب التسجيل.
    LOG_LEVEL_WARN = 2, ///< تحذير من احتمال حدوث مشكلة، لا يؤثر على الاستمرار.
    LOG_LEVEL_INFO = 3, ///< معلومات عامة حول سير عمل البرنامج.
    LOG_LEVEL_DEBUG = 4, ///< معلومات تفصيلية تُستخدم أثناء التصحيح (debug).
    LOG_LEVEL_TRACE = 5 ///< تتبع دقيق جدًا لمسار التنفيذ، غالبًا لأغراض التشخيص العميق.
};
// ============================================================
// واجهة تهيئة وتحرير نظام السجلات
// ============================================================

/**
 * @brief يقوم بتهيئة نظام السجلات.
 * يجب استدعاؤه مرة واحدة قبل أي عملية تسجيل.
 * @return true إذا تم التهيئة بنجاح، false في حال الفشل.
 */
b8 initialize_logger();

/**
 * @brief يُستخدم لإغلاق نظام السجلات وتحرير الموارد المرتبطة به.
 * يُستحسن استدعاؤه عند إنهاء التطبيق.
 */
void shutdown_logging();

/**
 * @brief الدالة الأساسية لتسجيل الرسائل.
 *
 * تُستخدم من قبل الماكروز لتسجيل الرسائل بمستويات مختلفة،
 * وتدعم تنسيقات متعددة كما في printf.
 *
 * @param level مستوى السجل (log_level)
 * @param message الرسالة أو التنسيق النصي
 * @param ... متغيرات إضافية لملء التنسيق
 */
KAPI void log_output(log_level level, const char* message, ...);



// ============================================================
// ماكروز السجلات لكل مستوى - للتحكم بسهولة في إخراج الرسائل
// ============================================================

/**
 * @brief تسجيل رسالة بمستوى FATAL (خطأ قاتل).
 * تُستخدم للحالات الحرجة التي تؤدي لإيقاف البرنامج.
 */
#define KFATAL(message, ...) \
    log_output(log_level::LOG_LEVEL_FATAL, message, ##__VA_ARGS__)

/**
 * @brief تسجيل رسالة بمستوى ERROR (خطأ مهم).
 * تُستخدم للإبلاغ عن مشاكل تمنع استكمال العمليات بشكل صحيح.
 */
#ifndef KERROR
#define KERROR(message, ...) \
    log_output(log_level::LOG_LEVEL_ERROR, message, ##__VA_ARGS__)
#endif

/**
 * @brief تسجيل رسالة بمستوى WARN (تحذير).
 * تُستخدم للإشارة إلى أمور غير حرجة لكنها غير معتادة.
 */
#if LOG_WARN_ENABLED == 1
    #define KWARN(message, ...) \
        log_output(log_level::LOG_LEVEL_WARN, message, ##__VA_ARGS__)
#else
    #define KWARN(message, ...)
#endif

/**
 * @brief تسجيل رسالة بمستوى INFO (معلومات عامة).
 * تُستخدم لتوفير معلومات مفيدة عن سير تنفيذ البرنامج.
 */
#if LOG_INFO_ENABLED == 1
    #define KINFO(message, ...) \
        log_output(log_level::LOG_LEVEL_INFO, message, ##__VA_ARGS__)
#else
    #define KINFO(message, ...)
#endif

/**
 * @brief تسجيل رسالة بمستوى DEBUG (تصحيح).
 * تُستخدم أثناء التطوير لتوضيح القيم والتصرفات.
 */
#if LOG_DEBUG_ENABLED == 1
    #define KDEBUG(message, ...) \
        log_output(log_level::LOG_LEVEL_DEBUG, message, ##__VA_ARGS__)
#else
    #define KDEBUG(message, ...)
#endif

/**
 * @brief تسجيل رسالة بمستوى TRACE (تتبع دقيق).
 * تُستخدم لتتبع كل خطوة في التنفيذ، غالبًا للتشخيص العميق.
 */
#if LOG_TRACE_ENABLED == 1
    #define KTRACE(message, ...) \
        log_output(log_level::LOG_LEVEL_TRACE, message, ##__VA_ARGS__)
#else
    #define KTRACE(message, ...)
#endif
