
#if KPLATFORM_WINDOWS
#include <platform.h>
#include<windows.h>
#include<windowsx.h>
struct internal_state {
        HINSTANCE h_instance;
        HWND hwnd;
};
// Clock
// متغير ثابت (سكوني) لتخزين تردد الساعة (عدد الدورات في الثانية)، يُستخدم لحساب الوقت بدقة عالية
static f64 clock_frequency;

// متغير ثابت (سكوني) لتخزين الوقت الابتدائي (وقت بداية التشغيل)، من نوع LARGE_INTEGER الخاص بـ Windows
static LARGE_INTEGER start_time;

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param);

b8 platform_startup(
        platform_state *plat_state,
        const char *application_name,
        i32 x,
        i32 y,
        i32 width,
        i32 height
) {
    // تخصيص مساحة في الذاكرة لحفظ الحالة الداخلية
    plat_state->internal_state = malloc(sizeof(internal_state));

    // إنشاء مؤشر من نوع internal_state للإشارة للبيانات المحجوزة
    internal_state *state = (internal_state *)plat_state->internal_state;

    // الحصول على مقبض التطبيق (instance handle)
    state->h_instance = GetModuleHandleA(0);

    // تحميل أيقونة التطبيق الافتراضية
    HICON icon = LoadIcon(state->h_instance, IDI_APPLICATION);

    // إنشاء هيكل تعريف النافذة وتصفيره
    WNDCLASSA wc;
    memset(&wc, 0, sizeof(wc));

    // ضبط إعدادات الكلاس
    wc.style = CS_DBLCLKS;                        // تمكين التقاط النقر المزدوج
    wc.lpfnWndProc = win32_process_message;       // ربط دالة معالجة الرسائل
    wc.cbClsExtra = 0;                            // لا يوجد بيانات إضافية للكلاس
    wc.cbWndExtra = 0;                            // لا يوجد بيانات إضافية للنافذة
    wc.hInstance = state->h_instance;             // ربط مقبض التطبيق
    wc.hIcon = icon;                              // تحديد الأيقونة
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);     // تحديد مؤشر الفأرة (السهم)
    wc.hbrBackground = NULL;                      // الخلفية شفافة أو يتم التحكم بها يدوياً
    wc.lpszClassName = "kohi_window_class";       // اسم الكلاس

    // تسجيل الكلاس في النظام
    if (!RegisterClassA(&wc)) {
        MessageBoxA(NULL, "Failed to register window class.", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // إنشاء النافذة - تحديد إحداثيات وأبعاد منطقة العميل (client area)
    u32 client_x = x;
    u32 client_y = y;
    u32 client_width = width;
    u32 client_height = height;

    // تعيين نفس القيم لإحداثيات وأبعاد النافذة الكلية (غالباً ستعدل لاحقاً لإضافة الإطارات أو العنوان)
    u32 window_x = client_x;
    u32 window_y = client_y;
    u32 window_width = client_width;
    u32 window_height = client_height;

    // تعيين أنماط النافذة الأساسية
    u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    u32 window_ex_style = WS_EX_APPWINDOW;

    // إضافة بعض الخصائص لأنماط النافذة
    window_style |= WS_MAXIMIZEBOX;    // تمكين زر التكبير (maximize)
    window_style |= WS_MINIMIZEBOX;    // تمكين زر التصغير (minimize)
    window_style |= WS_THICKFRAME;     // تمكين إطار قابل للسحب (يمكن تغيير حجم النافذة بواسطة المستخدم)

    // الحصول على حجم إطار النافذة
    RECT border_rect = {0, 0, 0, 0};  // تهيئة مستطيل بقيم صفرية
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    window_x +=border_rect.left;
    window_y +=border_rect.top;
    // إنشاء النافذة باستخدام الكلاس المسجل
    HWND handle = CreateWindowExA(
        window_ex_style,           // نمط النافذة الإضافي (extended style)
        "kohi_window_class",       // اسم الكلاس المسجل للنافذة (يجب أن يكون نفس الاسم عند تسجيل الكلاس)
        application_name,          // اسم التطبيق أو عنوان النافذة (يظهر في الشريط العلوي)
        window_style,              // نمط النافذة (style) مثل: WS_OVERLAPPEDWINDOW
        window_x,                  // موضع النافذة على المحور X
        window_y,                  // موضع النافذة على المحور Y
        window_width,              // عرض النافذة
        window_height,             // ارتفاع النافذة
        0,                         // نافذة الأب (parent window)، 0 تعني لا يوجد أب (نافذة مستقلة)
        0,                         // قائمة (menu)، 0 تعني لا توجد قائمة
        state->h_instance,         // مقبض التطبيق (instance handle)
        0                          // معطيات إضافية تُمرر لدالة الرسائل (عادةً لا تحتاجها هنا)
    );


    if (handle==0) {
        MessageBoxA(NULL, "Failed to create window.", "Error", MB_OK | MB_ICONERROR);
        KFATAL("window create failed!");
        return false;
    }else {
        state->hwnd=handle;
    }
    // إظهار النافذة

    b32 should_activate = 1;  // إذا كانت النافذة يجب أن تستقبل المدخلات عند الظهور (افتراضي: نعم)
    i32 show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
    // SW_SHOW: إظهار وتفعيل النافذة (تستقبل مدخلات)
    // SW_SHOWNOACTIVATE: إظهار بدون تفعيل (لا تستقبل مدخلات فوراً)

    // إذا كنت تريد إظهار النافذة مصغرة أو مكبرة افتراضياً:
    // SW_MINIMIZE أو SW_SHOWMINNOACTIVE أو SW_SHOWMAXIMIZED أو SW_MAXIMIZE

    ShowWindow(state->hwnd, show_window_command_flags);
    // إعداد المؤقت عالي الدقة (Clock Setup)

    // متغير لتخزين تردد المؤقت (عدد التكات في الثانية)
    LARGE_INTEGER frequency;

    // الحصول على تردد المؤقت عالي الدقة من نظام ويندوز
    QueryPerformanceFrequency(&frequency);

    // حساب عدد الثواني لكل تكة (tick) وتخزينه في clock_frequency
    clock_frequency = 1.0 / (f64)frequency.QuadPart;

    // الحصول على قيمة المؤقت الحالية (وقت البدء) وتخزينها في start_time
    QueryPerformanceCounter(&start_time);


    return true;
}
void platform_shutdown(platform_state *plat_state) {
    // تحويل النوع للهيكلية الداخلية المعروفة
    internal_state *state = (internal_state *)plat_state->internal_state;

    // إذا كانت النافذة موجودة، قم بتدميرها
    if (state->hwnd) {
        DestroyWindow(state->hwnd);
        state->hwnd = 0; // إعادة تعيين المقبض بعد التدمير
    }
}

// معالجة رسائل النظام (ويندوز) للنافذة، مثل الإغلاق أو التفاعل مع المستخدم
b8 platform_pump_messages(platform_state* plat_state) {
    MSG message; // متغير لتخزين الرسائل
    // حلقة تستخرج جميع الرسائل من طابور الرسائل وتعالجها
    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);      // ترجمة بعض الرسائل (مثل اختصارات الكيبورد)
        DispatchMessageA(&message);      // إرسال الرسالة لدالة النافذة لمعالجتها
    }
    return true; // تعيد true للدلالة أن الرسائل عولجت بنجاح
}

// تخصيص (حجز) مساحة في الذاكرة بالحجم المطلوب
void *platform_allocate(u64 size, b8 aligned) {
    return malloc(size); // استخدام malloc للحجز
}

// تحرير (إلغاء) مساحة الذاكرة المحجوزة مسبقًا
void platform_free(void *block, b8 aligned) {
    free(block); // استخدام free للتحرير
}

// تصفير منطقة من الذاكرة (جعل جميع القيم 0)
void *platform_zero_memory(void *block, u64 size) {
    return memset(block, 0, size); // memset للقيم صفرية
}

// نسخ بيانات من مصدر إلى وجهة (بالحجم المطلوب)
void *platform_copy_memory(void *dest, const void *source, u64 size) {
    return memcpy(dest, source, size); // memcpy للنسخ
}

// تعيين قيمة محددة لكل بايت في الذاكرة (مثل ملء مصفوفة بقيمة محددة)
void *platform_set_memory(void *dest, i32 value, u64 size) {
    return memset(dest, value, size); // memset لقيمة محددة
}

// طباعة رسالة على كونسول ويندوز مع لون محدد حسب مستوى الرسالة
void platform_console_write(const char *message,u8 colour){
    // الحصول على مقبض نافذة الكونسول
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    // تعريف مستويات الألوان للرسائل المختلفة
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    static u8 levels[6] = {64, 4, 6, 2, 1, 8};

    // تعيين لون النص في الكونسول
    SetConsoleTextAttribute(console_handle, levels[colour]);

    // إرسال الرسالة إلى نافذة الديبق (اختياري)
    OutputDebugStringA(message);

    // حساب طول الرسالة (عدد الأحرف)
    u64 length = strlen(message);

    // متغير لحفظ عدد الأحرف المكتوبة فعليًا
    DWORD number_written = 0;

    // طباعة الرسالة في الكونسول النصي
    WriteConsoleA(console_handle, message, (DWORD)length, &number_written, 0);

    // استعادة لون الكونسول الافتراضي (اختياري)
    // SetConsoleTextAttribute(console_handle, 7);
}
void platform_console_write_error(const char* message, u8 colour) {
    // الحصول على مقبض كونسول الخطأ القياسي
    HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);

    // تعريف ألوان المستويات (FATAL, ERROR, WARN, INFO, DEBUG, TRACE)
    static u8 levels[6] = {64, 4, 6, 2, 1, 8};

    // تعيين لون النص في كونسول الخطأ حسب المستوى
    SetConsoleTextAttribute(console_handle, levels[colour]);

    // إرسال الرسالة إلى نافذة الديبق (اختياري)
    OutputDebugStringA(message);

    // حساب طول الرسالة
    u64 length = strlen(message);

    // متغير لحفظ عدد الأحرف المكتوبة فعليًا (يجب أن يكون DWORD وليس مؤشر)
    DWORD number_written = 0;

    // طباعة الرسالة في كونسول الخطأ النصي
    WriteConsoleA(console_handle, message, (DWORD)length, &number_written, 0);

    // استعادة لون النص الافتراضي (اختياري)
    // SetConsoleTextAttribute(console_handle, 7);
}

// دالة للحصول على الوقت المطلق منذ بداية تشغيل التطبيق بوحدة الثانية (دقة عالية)
f64 platform_get_absolute_time() {
    LARGE_INTEGER now_time; // متغير لتخزين الوقت الحالي

    // الحصول على قيمة المؤقت عالية الدقة في اللحظة الحالية
    QueryPerformanceCounter(&now_time);

    // حساب الزمن (بالثواني) منذ بدء المؤقت باستخدام التردد المحسوب مسبقًا
    return (f64)now_time.QuadPart * clock_frequency;
}
// دالة لإيقاف تنفيذ البرنامج مؤقتًا لمدة معينة بالمللي ثانية
void platform_sleep(u64 ms) {
    Sleep(ms); // دالة من WinAPI لإيقاف التنفيذ لمدة ms مللي ثانية
}
LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param) {
    switch (msg) {
        case WM_ERASEBKGND:
            // إبلاغ نظام التشغيل أن التطبيق سيتولى مسح الخلفية لمنع الوميض
            return 1;

        case WM_CLOSE:
            // عند محاولة إغلاق النافذة: يمكنك هنا إطلاق حدث خاص للخروج الآمن من التطبيق
            // TODO: Fire an event for the application to quit.
            return 0;

        case WM_DESTROY:
            // عند تدمير النافذة، أرسل رسالة إنهاء البرنامج إلى حلقة الرسائل
            PostQuitMessage(0);
            return 0;

        case WM_SIZE: {
            // عند تغيير حجم النافذة (تكبير/تصغير)
            // الحصول على الحجم الجديد لمنطقة العميل (يمكنك استخدام هذا الكود عند الحاجة)
            // RECT r;
            // GetClientRect(hwnd, &r);
            // u32 width = r.right - r.left;
            // u32 height = r.bottom - r.top;
            // TODO: Fire an event for window resize.
        } break;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            // عند الضغط أو رفع زر في الكيبورد
            b8 pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            // TODO: Keyboard input processing.
        } break;

        case WM_MOUSEMOVE: {
            // حدث تحريك مؤشر الفأرة داخل النافذة
            // i32 x_position = GET_X_LPARAM(l_param); // استخراج الإحداثي X للفأرة
            // i32 y_position = GET_Y_LPARAM(l_param); // استخراج الإحداثي Y للفأرة
            // TODO: Mouse move processing.
        } break;

        case WM_MOUSEWHEEL: {
            // معالجة دوران عجلة الفأرة (scroll)
            // i32 z_delta = GET_WHEEL_DELTA_WPARAM(w_param);   // استخراج مقدار الدوران
            // if (z_delta != 0) {
            //     // تسوية قيمة الدوران إلى -1 أو 1 (استقلالاً عن نظام التشغيل)
            //     z_delta = (z_delta < 0) ? -1 : 1;
            //     // TODO: Mouse wheel processing.
            // }
        } break;

        case WM_LBUTTONDOWN:    // الضغط على زر الفأرة الأيسر
        case WM_MBUTTONDOWN:    // الضغط على زر الفأرة الأوسط
        case WM_RBUTTONDOWN:    // الضغط على زر الفأرة الأيمن
        case WM_LBUTTONUP:      // إفلات زر الفأرة الأيسر
        case WM_MBUTTONUP:      // إفلات زر الفأرة الأوسط
        case WM_RBUTTONUP: {    // إفلات زر الفأرة الأيمن
            // معالجة أحداث أزرار الفأرة
            b8 pressed = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
            // TODO: Mouse button input processing.
        } break;
    }
    // في حال لم تتم معالجة الرسالة، استخدم المعالج الافتراضي للرسائل
    return DefWindowProc(hwnd, msg, w_param, l_param);
}



#endif
