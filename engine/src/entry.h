#pragma once
#include "core/application.h"
#include "core/logger.h"
#include "game_types.h"

// إعلان دالة إنشاء اللعبة، يجب تعريفها في ملف مصدر آخر
extern b8 create_game(game *out_game);

/**
 * الدالة الرئيسية (main) التي تبدأ تشغيل البرنامج.
 * - تنشئ هيكل اللعبة
 * - تستدعي create_game لتهيئة دوال اللعبة وحالتها
 * - تتحقق من أن مؤشرات الدوال المهمة (render, update, initialize, one_resized) مُعينة
 * - تنشئ التطبيق مع اللعبة
 * - تشغل حلقة التطبيق الرئيسية
 * - تعيد رموز خروج مناسبة حسب نجاح العمليات أو فشلها
 */
int main() {
    game game_inst;

    // محاولة إنشاء اللعبة، وإيقاف التنفيذ إذا فشل
    if (!create_game(&game_inst)) {
        KFATAL("could not create game!");
        return -1;
    }

    // التحقق من أن مؤشرات الدوال المهمة معرّفة
    if (!game_inst.render || !game_inst.update || !game_inst.initialize || !game_inst.one_resized) {
        KFATAL("The Game function pointer must be assigned!");
        return -2;
    }

    // إنشاء التطبيق مع اللعبة
    if (!application_create(&game_inst)) {
        KINFO("Application failed to create!");
        return 1;
    }

    // تشغيل حلقة التطبيق
    if (!application_run()) {
        KINFO("Application did not shutdown gracefully!");
        return 2;
    }

    return 0;
}
