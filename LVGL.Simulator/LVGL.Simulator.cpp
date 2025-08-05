/*
 * PROJECT:   LVGL PC Simulator using Visual Studio
 * FILE:      LVGL.Simulator.cpp
 * PURPOSE:   Implementation for LVGL ported to Windows Desktop
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include <Windows.h>

#include "resource.h"

#if _MSC_VER >= 1200
 // Disable compilation warnings.
#pragma warning(push)
// nonstandard extension used : bit field types other than int
#pragma warning(disable:4214)
// 'conversion' conversion from 'type1' to 'type2', possible loss of data
#pragma warning(disable:4244)
#endif

#include "lvgl/lvgl.h"
#include "lvgl/examples/lv_examples.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/win32drv/win32drv.h"

#if _MSC_VER >= 1200
// Restore compilation warnings.
#pragma warning(pop)
#endif

#include <stdio.h>

bool single_display_mode_initialization()
{
    if (!lv_win32_init(
        GetModuleHandleW(NULL),
        SW_SHOW,
        800,
        480,
        LoadIconW(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_LVGL))))
    {
        return false;
    }

    lv_win32_add_all_input_devices_to_group(NULL);

    return true;
}

#include <process.h>

HANDLE g_window_mutex = NULL;
bool g_initialization_status = false;

#define LVGL_SIMULATOR_MAXIMUM_DISPLAYS 16
HWND g_display_window_handles[LVGL_SIMULATOR_MAXIMUM_DISPLAYS];

unsigned int __stdcall lv_win32_window_thread_entrypoint(
    void* raw_parameter)
{
    size_t display_id = *(size_t*)(raw_parameter);

    HINSTANCE instance_handle = GetModuleHandleW(NULL);
    int show_window_mode = SW_SHOW;
    HICON icon_handle = LoadIconW(instance_handle, MAKEINTRESOURCE(IDI_LVGL));
    lv_coord_t hor_res = 800;
    lv_coord_t ver_res = 450;

    wchar_t window_title[256];
    memset(window_title, 0, sizeof(window_title));
    _snwprintf(
        window_title,
        256,
        L"LVGL Simulator for Windows Desktop (Display %d)",
        display_id);

    if (!g_display_window_handles[display_id])
    {
        return 0;
    }

    g_initialization_status = true;

    SetEvent(g_window_mutex);

    MSG message;
    while (GetMessageW(&message, NULL, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    lv_win32_quit_signal = true;

    return 0;
}

int main()
{
    lv_init();

    if (!single_display_mode_initialization())
    {
        return -1;
    }
    lv_obj_t* obj= lv_obj_create(lv_scr_act());
    lv_obj_set_pos(obj, 400, 240);         // 设置坐标
    lv_obj_set_size(obj, 400, 240); // 设置尺寸

    // 对齐
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);  // 居中
    lv_obj_t* label = lv_label_create(obj);
    lv_label_set_text(label, "Hello wdkadk");      // 设置文本
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL);  // 长文本滚动
    lv_obj_set_style_text_color(label, lv_color_hex(0xFF0000), 0); // 红色文本
    while (!lv_win32_quit_signal)
    {
        lv_task_handler();
        Sleep(1);
    }

    return 0;
}
