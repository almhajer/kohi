#include "application.h"
#include "logger.h"
#include "platform.h"
#include <game_types.h>
#include <iostream>

// هيكل يحتفظ بحالة التطبيق أثناء التشغيل، يتضمن معلومات اللعبة وحالة النظام والأبعاد ووقت التحديث
struct application_state {
    game *game_inst; // مؤشر إلى هيكل اللعبة النشطة
    b8 is_running; // علم يوضح هل التطبيق يعمل (true) أم لا (false)
    b8 is_suspended; // علم يوضح هل التطبيق موقوف مؤقتاً (true) أم لا (false)
    platform_state platform; // هيكل حالة النظام الأساسي (نافذة، إدخال، إلخ)
    i16 width; // عرض النافذة الحالي بالبكسل
    i16 height; // ارتفاع النافذة الحالي بالبكسل
    f64 last_time; // آخر وقت تحديث (بالثواني) لحساب الفرق الزمني بين الإطارات
};

// متغير ثابت لمنع تهيئة التطبيق أكثر من مرة
static b8 initialized = false;

// الحالة العامة الحالية للتطبيق
static application_state app_state;

/**
 * @brief ينشئ التطبيق باستخدام إعدادات اللعبة المقدمة.
 * - يمنع التهيئة المتكررة.
 * - يهيئ نظام التسجيل (اللوق).
 * - يهيئ النظام الأساسي (إنشاء النافذة، إعداد الإدخال، إلخ).
 * - يهيئ اللعبة نفسها (تهيئة الموارد والحالة).
 * - يخبر اللعبة بحجم النافذة.
 * @param game_inst مؤشر إلى هيكل اللعبة المحمل بالإعدادات والدوال.
 * @return true إذا تم الإنشاء بنجاح، false في حالة الفشل.
 */
b8 application_create(game *game_inst) {
    if (initialized) {
        KERROR("application_create called more than once");
        return false;
    }

    app_state.game_inst = game_inst;

    // TODO: Remove this
    KFATAL("A test message: %f", 3.14f);
    KERROR("A test message: %f",3.14f);
    KWARN("A test message: %f", 3.14f);
    KINFO("A test message: %f", 3.14f);
    KDEBUG("A test message:%f", 3.14f);
    KTRACE("A test message: %f", 3.14f);


    initialize_logger(); // تهيئة نظام السجل (لوغ)

    app_state.is_running = true; // وضع التطبيق في حالة تشغيل
    app_state.is_suspended = false; // التأكد من أن التطبيق ليس موقوف مؤقتاً

    // تهيئة النظام الأساسي: إنشاء النافذة وإعداد الإدخال
    if (!platform_startup(
        &app_state.platform,
        game_inst->app_config.name,
        game_inst->app_config.start_pos_x,
        game_inst->app_config.start_pos_y,
        game_inst->app_config.start_width,
        game_inst->app_config.start_height)) {
        KFATAL("Failed to initialize platform");
        return false;
    }

    // استدعاء دالة تهيئة اللعبة (تحميل الموارد، الإعدادات الداخلية)
    if (!app_state.game_inst->initialize(app_state.game_inst)) {
        KFATAL("Game failed to initialize");
        return false;
    }

    // تخزين أبعاد النافذة الحالية في حالة التطبيق
    app_state.width = game_inst->app_config.start_width;
    app_state.height = game_inst->app_config.start_height;

    // إعلام اللعبة بحجم النافذة الحالي لتحديث أي إعدادات متعلقة بالعرض
    app_state.game_inst->one_resized(app_state.game_inst, app_state.width, app_state.height);

    initialized = true; // تعيين علم التهيئة لمنع إعادة التهيئة

    // تهيئة متغير الوقت الأخير للحسابات القادمة لدلتا الوقت
    app_state.last_time = platform_get_absolute_time();

    return true;
}

/**
 * @brief يدير حلقة تشغيل التطبيق الرئيسية.
 * - يعالج رسائل النظام (الإدخال، أحداث النافذة، إلخ).
 * - إذا لم يكن التطبيق موقوف مؤقتاً، يقوم بتحديث اللعبة ورسم الإطارات.
 * - يحسب الفرق الزمني بين الإطارات (دلتا الوقت) لتمريرها لدوال اللعبة.
 * - يغلق النظام الأساسي وينظف الموارد عند انتهاء التطبيق.
 * @return true عند الإنهاء الطبيعي للحلقة.
 */
b8 application_run() {
    while (app_state.is_running) {
        // معالجة جميع رسائل النظام (أحداث النافذة، الإدخال، ...)
        if (!platform_pump_messages(&app_state.platform)) {
            app_state.is_running = false;
            break;
        }

        // إذا لم يكن التطبيق موقوف مؤقتًا، نفذ التحديث والرسم
        if (!app_state.is_suspended) {
            // حساب دلتا الوقت (الفارق الزمني بين الإطارات)
            f64 current_time = platform_get_absolute_time();
            f32 delta_time = (f32) (current_time - app_state.last_time);
            app_state.last_time = current_time;

            // تحديث حالة اللعبة بناءً على دلتا الوقت
            if (!app_state.game_inst->update(app_state.game_inst, delta_time)) {
                KFATAL("Game failed to update");
                app_state.is_running = false;
                break;
            }

            // رسم إطار اللعبة الحالي
            if (!app_state.game_inst->render(app_state.game_inst, delta_time)) {
                app_state.is_running = false;
                break;
            }
        }
    }

    app_state.is_running = false; // تأكيد إيقاف التشغيل
    platform_shutdown(&app_state.platform); // إغلاق النظام الأساسي وتنظيف الموارد

    return true;
}
