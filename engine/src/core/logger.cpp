#include "logger.h"
//TODO: temporary

#include <iostream>
#include <cstring>
#include <core/asserts.h>
#include<platform.h>


b8 initialize_logger() {
    //TODO: create log file;
    return true;
}

void shutdown_logging() {
    //TODO: cleanup loging/write queued entries
}


void log_output(log_level level, const char *message, ...) {
    // سلسلة النصوص التي تُمثل البادئة حسب كل مستوى سجل
    const char *levels_string[6] = {
        "[FATAL]: ",
        "[ERROR]: ",
        "[WARN ]: ",
        "[INFO ]: ",
        "[DEBUG]: ",
        "[TRACE]: "
    };

    // نحدد إن كانت الرسالة تُعتبر خطأ (FATAL أو ERROR)

    b8 is_error = static_cast<int>(level) < static_cast<int>(log_level::LOG_LEVEL_WARN);
    const i32 msg_length = 32000;
    // مخزن مؤقت لحفظ الرسالة النصية المنسقة قبل الإخراج

    char out_message[msg_length] = {0};

    memset(out_message, 0, sizeof(out_message)); // تنظيف المخزن المؤقت

    // تجهيز قائمة المعاملات المتغيرة لقراءة المعاملات التي بعد `message`
    va_list arg_ptr;
    va_start(arg_ptr, message);

    // تنسيق الرسالة باستخدام المعاملات، وكتابتها داخل out_message
    vsnprintf(out_message, msg_length, message, arg_ptr);

    // إنهاء قراءة المعاملات المتغيرة
    va_end(arg_ptr);

    char out_message2[msg_length] = {0};

    // إعادة كتابة الرسالة نفسها لكن بإضافة بادئة المستوى في بدايتها
    snprintf(out_message2, sizeof(out_message2), "%s%s\n",
             levels_string[static_cast<int>(level)], out_message);

    // طباعة الرسالة إلى الإخراج القياسي
    // (يمكن لاحقًا التبديل بين stdout/stderr حسب is_error)
    // إخراج نصي مخصص حسب نوع الرسالة (خطأ أو عادي)
    if (is_error) {
     platform_console_write_error(out_message2, (u8) level); // طباعة رسالة خطأ
    } else {
      platform_console_write(out_message2, (u8) level); // طباعة رسالة عادية
    }
}


void report_assertion_failure(const char *expression, const char *message, const char *file, i32 line) {
    log_output(log_level::LOG_LEVEL_FATAL,
               "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n",
               expression, message, file, line);
}
