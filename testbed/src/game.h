#pragma once
#include <defines.h>      // تعريف أنواع البيانات والمنصات
#include <game_types.h>   // تعريف هيكل game والأساسيات

// هيكل لحفظ حالة اللعبة، يحتوي على دلتا الوقت بين الإطارات
struct game_state {
    f32 delta_time;       // الفرق الزمني بين التحديثات
};

// دالة تهيئة اللعبة، تستقبل مؤشر إلى هيكل اللعبة
// تعيد قيمة منطقية (true/false) للدلالة على نجاح التهيئة
b8 game_initialize(game *game_inst);

// دالة تحديث اللعبة مع دلتا الوقت بين الإطارات
// تُستخدم لتحريك المنطق، الفيزياء، الأحداث، إلخ.
b8 game_update(game *game_inst, f32 delta_time);

// دالة رسم الإطار الحالي للعبة
// تستقبل دلتا الوقت لتزامن الحركة أو الرسوم المتحركة
b8 game_render(game *game_inst, f32 delta_time);

// دالة استدعائية عند تغيير حجم النافذة
// ترسل الأبعاد الجديدة للعبة للتعامل مع التغيير
void game_one_resized(game *game_inst, i32 width, i32 height);
