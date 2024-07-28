
#include "component/mainWindow.h"
#ifdef __cplusplus
extern "C" {
#endif // DEBUG

#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include <anim/lv_example_anim.h>
#include <examples/get_started/lv_example_get_started.h>
#include <examples/layouts/flex/lv_example_flex.h>
#include <examples/widgets/lv_example_widgets.h>
#include <get_started/lv_example_get_started.h>
#include <src/core/lv_obj.h>
#include <src/core/lv_obj_pos.h>
#include <src/core/lv_obj_style.h>
#include <src/display/lv_display.h>
#include <src/lv_api_map_v8.h>
#include <src/misc/lv_area.h>
#include <src/misc/lv_color.h>
#include <src/misc/lv_event.h>
#include <src/misc/lv_palette.h>
#include <src/misc/lv_style_gen.h>
#include <src/widgets/button/lv_button.h>
#include <src/widgets/label/lv_label.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

static const char * getenv_default(const char * name, const char * dflt)
{
    return getenv(name) ?: dflt;
}

#if LV_USE_LINUX_FBDEV
static void lv_linux_disp_init(void)
{
    const char * device = getenv_default("LV_LINUX_FBDEV_DEVICE", "/dev/fb0");
    lv_display_t * disp = lv_linux_fbdev_create();

    lv_linux_fbdev_set_file(disp, device);
}
#elif LV_USE_LINUX_DRM
static void lv_linux_disp_init(void)
{
    const char * device = getenv_default("LV_LINUX_DRM_CARD", "/dev/dri/card0");
    lv_display_t * disp = lv_linux_drm_create();

    lv_linux_drm_set_file(disp, device, -1);
}
#elif LV_USE_SDL
static void lv_linux_disp_init(void)
{
    const int width  = atoi(getenv("LV_SDL_VIDEO_WIDTH") ?: "800");
    const int height = atoi(getenv("LV_SDL_VIDEO_HEIGHT") ?: "480");

    lv_sdl_window_create(width, height);
}
#else
#error Unsupported configuration
#endif

static lv_display_t * hal_init(int32_t w, int32_t h)
{
    lv_group_set_default(lv_group_create());

    lv_display_t * disp = lv_sdl_window_create(w, h);

    lv_indev_t * mouse = lv_sdl_mouse_create();
    lv_indev_set_group(mouse, lv_group_get_default());
    lv_indev_set_display(mouse, disp);
    lv_display_set_default(disp);

    // add this to enable mouse icon inlay

    // LV_IMAGE_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
    // lv_obj_t * cursor_obj;
    // cursor_obj = lv_image_create(lv_screen_active()); /*Create an image object for the cursor */
    // lv_image_set_src(cursor_obj, &mouse_cursor_icon); /*Set the image source*/
    // lv_indev_set_cursor(mouse, cursor_obj);           /*Connect the image  object to the driver*/

    lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
    lv_indev_set_display(mousewheel, disp);

    lv_indev_t * keyboard = lv_sdl_keyboard_create();
    lv_indev_set_display(keyboard, disp);
    lv_indev_set_group(keyboard, lv_group_get_default());

    return disp;
}

void buttonCallbak(lv_event_t * e)
{

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn       = (lv_obj_t *)lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;

        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "Button: %d", cnt);
    }
}

void simpleButtonWithClickEvent()
{
    auto button = lv_button_create(lv_screen_active());
    lv_obj_set_pos(button, 10, 10);
    lv_obj_set_size(button, 80, 20);
    lv_obj_add_event_cb(button, buttonCallbak, LV_EVENT_ALL, NULL);
    lv_obj_center(button);

    lv_obj_t * label = lv_label_create(button);
    lv_label_set_text(label, "Button"); /*Set the labels text*/
    lv_obj_center(label);

    auto welcomeLabel = lv_label_create(lv_screen_active());
    lv_label_set_text(welcomeLabel, "SORE V2");
    lv_obj_align_to(welcomeLabel, button, LV_ALIGN_TOP_RIGHT, 0, -30);
}

int main(void)
{
    lv_init();

    /*enable sdl2 and start required event driver*/
    hal_init(480, 320);

    /*Create a Demo*/
    initSoreMainWindow();
    /*simpleButtonWithClickEvent();*/
    /*Handle LVGL tasks*/
    while(1) {
        lv_timer_handler();
        usleep(10 * 1000);
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
