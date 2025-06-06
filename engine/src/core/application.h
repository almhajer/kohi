#pragma once

#include "defines.h"

struct games; // إعلان مسبق لهيكل games (غير معرف هنا)

// هيكل يحتوي إعدادات تهيئة التطبيق مثل:
// - start_pos_x, start_pos_y: إحداثيات بداية النافذة (الموقع على الشاشة)
// - start_width, start_height: أبعاد النافذة (العرض والارتفاع)
// - name: اسم التطبيق (سلسلة نصية ثابتة)
struct application_config {
    i16 start_pos_x;   // موقع النافذة الابتدائي على المحور الأفقي (X)
    i16 start_pos_y;   // موقع النافذة الابتدائي على المحور الرأسي (Y)
    i16 start_width;   // عرض النافذة الابتدائي
    i16 start_height;  // ارتفاع النافذة الابتدائي
    const char *name;  // اسم التطبيق (سلسلة نصية ثابتة)
};

// دالة لإنشاء التطبيق مع تمرير مؤشر إلى هيكل اللعبة (game)
// تقوم بتهيئة التطبيق بناءً على إعدادات اللعبة
// تعيد قيمة منطقية (b8) للدلالة على نجاح الإنشاء (true) أو فشله (false)
KAPI b8 application_create(struct game *game_inst);

// دالة لتشغيل حلقة التطبيق الرئيسية
// تقوم بإدارة رسائل النظام وتنفيذ تحديث وعرض اللعبة بشكل متكرر
// لا تأخذ معاملات، تعيد قيمة منطقية (b8) تعبر عن نجاح بدء التشغيل والاستمرار
KAPI b8 application_run();
