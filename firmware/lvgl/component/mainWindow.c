

#include "mainWindow.h"
#include <src/core/lv_obj_pos.h>
#include <src/layouts/flex/lv_flex.h>
#include <src/layouts/lv_layout.h>
#include <src/misc/lv_area.h>
#include <src/widgets/label/lv_label.h>

void initSoreMainWindow()
{
    static lv_style_t sidePanelStyle;
    lv_style_init(&sidePanelStyle);
    lv_style_set_shadow_color(&sidePanelStyle, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_color(&sidePanelStyle, lv_palette_main(LV_PALETTE_RED));

    lv_obj_t * sidePanel = lv_obj_create(lv_screen_active());
    lv_obj_add_style(sidePanel, &sidePanelStyle, 0);
    lv_obj_align(sidePanel, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_size(sidePanel, 100, 320);
    lv_obj_set_layout(sidePanel, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(sidePanel, LV_FLEX_FLOW_COLUMN);

    lv_obj_t * button;
    lv_obj_t * buttonLabel;

    for(int i = 0; i < 20; i++) {
        button      = lv_button_create(sidePanel);
        buttonLabel = lv_label_create(button);
        lv_obj_set_size(button, 80, 20);
        lv_obj_align_to(button, sidePanel, LV_ALIGN_CENTER, 0, 0);
        lv_label_set_text(buttonLabel, "btn");
    }

    lv_obj_t * titlePanel = lv_obj_create(lv_screen_active());
    lv_obj_align_to(titlePanel, sidePanel, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);
}
