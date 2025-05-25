/*
 * تعريف منصة macOS لنظام التشغيل باستخدام Cocoa API.
 * يحتوي هذا الملف على تنفيذ دوال التشغيل الأساسية للنظام مثل:
 * - platform_startup: تهيئة وفتح نافذة التطبيق على macOS.
 * - platform_shutdown: إغلاق النافذة وتحرير الموارد.
 * - platform_pump_messages: معالجة أحداث النظام (رسائل النافذة).
 * بالإضافة إلى دوال تخصيص الذاكرة، الطباعة، والحصول على الوقت ونوم المعالج.
 * يتم استخدام Objective-C++ لدمج كود Objective-C الخاص بـ macOS مع C++.
 * الشرط #ifdef KPLATFORM_MACOS يضمن تفعيل هذا الكود فقط على منصة macOS.
 */

#ifdef KPLATFORM_MACOS

#import <Cocoa/Cocoa.h>
#include <platform.h>
#include <stdlib.h>
#include <string.h>

// هيكل داخلي لتخزين حالة النافذة ووقت بدء التطبيق، مشابه لهياكل ويندوز لكنه مخصص لماك.
struct internal_state {
    NSWindow *window;
    CFAbsoluteTime start_time;
};

static double clock_frequency = 1.0; // macOS يستخدم الثواني كوحدة زمنية مباشرة

// دالة مساعدة لتحويل إحداثيات النافذة من نظام ويندوز (أعلى يسار الأصل) إلى نظام ماك (أسفل يسار الأصل).
static NSRect MakeCenteredRect(i32 x, i32 y, i32 width, i32 height) {
    NSScreen *mainScreen = [NSScreen mainScreen];
    NSRect screenRect = [mainScreen frame];
    // عكس محور y بسبب اختلاف أنظمة الإحداثيات بين ويندوز وماك
    i32 adjusted_y = (int)(screenRect.size.height - y - height);
    return NSMakeRect(x, adjusted_y, width, height);
}

/*
 * تهيئة النظام: إنشاء نافذة جديدة بعنوان معين وأبعاد محددة.
 * يتم حجز الذاكرة لحالة النظام الداخلية.
 */
b8 platform_startup(
    platform_state *plat_state,
    const char *application_name,
    i32 x,
    i32 y,
    i32 width,
    i32 height
) {
    @autoreleasepool {
        plat_state->internal_state = malloc(sizeof(internal_state));
        internal_state *state = (internal_state *)plat_state->internal_state;

        NSString *title = [NSString stringWithUTF8String:application_name];
        NSRect rect = MakeCenteredRect(x, y, width, height);

        state->window = [[NSWindow alloc] initWithContentRect:rect
                                                   styleMask:(NSWindowStyleMaskTitled |
                                                              NSWindowStyleMaskClosable |
                                                              NSWindowStyleMaskResizable)
                                                     backing:NSBackingStoreBuffered
                                                       defer:NO];
        [state->window setTitle:title];
        [state->window makeKeyAndOrderFront:nil];

        // تخزين وقت بدء التطبيق بدقة عالية
        state->start_time = CFAbsoluteTimeGetCurrent();

        return TRUE;
    }
}

/*
 * إغلاق النظام: إغلاق النافذة وتحرير الموارد المحجوزة.
 */
void platform_shutdown(platform_state *plat_state) {
    @autoreleasepool {
        internal_state *state = (internal_state *)plat_state->internal_state;
        if(state) {
            if(state->window) {
                [state->window close];
                state->window = nil;
            }
            free(state);
            plat_state->internal_state = NULL;
        }
    }
}

/*
 * معالجة رسائل النظام (الأحداث): استلام وإرسال كل الأحداث المتاحة للنظام.
 */
b8 platform_pump_messages(platform_state *plat_state) {
    @autoreleasepool {
        NSEvent *event;
        while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                           untilDate:[NSDate distantPast]
                                              inMode:NSDefaultRunLoopMode
                                             dequeue:YES])) {
            [NSApp sendEvent:event];
        }
        return TRUE;
    }
}

/*
 * دوال تخصيص وتحرير الذاكرة:
 * platform_allocate: حجز الذاكرة.
 * platform_free: تحرير الذاكرة.
 * platform_zero_memory: تصفير الذاكرة.
 * platform_copy_memory: نسخ الذاكرة.
 * platform_set_memory: تعيين قيمة في الذاكرة.
 */
void *platform_allocate(u64 size, b8 aligned) {
    return malloc(size);
}

void platform_free(void *block, b8 aligned) {
    free(block);
}

void platform_zero_memory(void *block, u64 size) {
    memset(block, 0, size);
}

void *platform_copy_memory(void *dest, const void *source, u64 size) {
    return memcpy(dest, source, size);
}

void *platform_set_memory(void *dest, i32 value, u64 size) {
    return memset(dest, value, size);
}

/*
 * دوال الطباعة على الكونسول:
 * platform_console_write: طباعة عادية على stdout.
 * platform_console_write_error: طباعة على stderr.
 */
void platform_console_write(const char *message, u8 colour) {
    printf("%s", message);
}

void platform_console_write_error(const char *message, u8 colour) {
    fprintf(stderr, "%s", message);
}

/*
 * دالة للحصول على الوقت المطلق منذ بداية تشغيل التطبيق بوحدة الثواني.
 */
f64 platform_get_absolute_time() {
    return CFAbsoluteTimeGetCurrent();
}

/*
 * دالة لإيقاف تنفيذ التطبيق مؤقتًا لمدة معينة بالمللي ثانية.
 */
void platform_sleep(u64 ms) {
    [NSThread sleepForTimeInterval:((double)ms / 1000.0)];
}

#endif
