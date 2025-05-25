#pragma once

#include "core/application.h"

// هيكل يمثل اللعبة ويحتوي على إعدادات التطبيق ومؤشرات دوال اللعبة الأساسية وحالة داخلية
struct game {
    application_config app_config;    // إعدادات التطبيق: موقع وحجم النافذة واسمها

    /**
     * @brief دالة تهيئة اللعبة.
     * تُستدعى مرة واحدة عند بدء التطبيق لتهيئة الموارد والحالة.
     * @param game_inst مؤشر إلى هيكل اللعبة.
     * @return true إذا نجح التهيئة، false إذا فشل.
     */
    b8 (*initialize)(struct game *game_inst);

    /**
     * @brief دالة تحديث اللعبة.
     * تُستدعى كل إطار لتحديث منطق اللعبة بناءً على الوقت الفاصل بين الإطارات.
     * @param game_inst مؤشر إلى هيكل اللعبة.
     * @param delta_time الوقت الفاصل بين الإطارات (بالثواني).
     * @return true إذا تم التحديث بنجاح، false إذا حدث خطأ.
     */
    b8 (*update)(struct game *game_inst, f32 delta_time);

    /**
     * @brief دالة رسم اللعبة.
     * تُستدعى كل إطار لرسم المشاهد الحالية للعبة.
     * @param game_inst مؤشر إلى هيكل اللعبة.
     * @param delta_time الوقت الفاصل بين الإطارات (بالثواني).
     * @return true إذا تم الرسم بنجاح، false إذا حدث خطأ.
     */
    b8 (*render)(struct game *game_inst, f32 delta_time);

    /**
     * @brief دالة استدعاء عند تغيير حجم النافذة.
     * تُستخدم لإعلام اللعبة بالأبعاد الجديدة للنافذة لتهيئة العرض.
     * @param game_inst مؤشر إلى هيكل اللعبة.
     * @param width العرض الجديد للنافذة بالبكسل.
     * @param height الارتفاع الجديد للنافذة بالبكسل.
     */
    void (*one_resized)(struct game *game_inst, i32 width, i32 height);

    void *state;  // مؤشر لحالة اللعبة الداخلية الخاصة (بيانات مخصصة للعبة)
};
