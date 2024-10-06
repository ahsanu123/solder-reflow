#ifndef INPUT_ENCODER_FROM_FORUM
#define INPUT_ENCODER_FROM_FORUM
//*********ROTARY SETUP BELOW HERE**********

#include <MD_REncoder.h>             // This library for rotary
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay    = 50; // the debounce time; increase if the output flickers
int32_t       RotaryCount      = 0;  // used to track rotary position
#define RotaryPinA 32                // SET DT PIN
#define RotaryPinB 25                // SET CLK PIN
MD_REncoder R = MD_REncoder(RotaryPinA, RotaryPinB);

#include <MD_UISwitch.h>                         //This library for button click (on the rotary)
const uint8_t       DIGITAL_SWITCH_PIN    = 33;  // SET SW PIN
const uint8_t       DIGITAL_SWITCH_ACTIVE = LOW; // digital signal when switch is pressed 'on'
int                 ButtonPressed         = 0;   // 0 is not pressed, 1 is pressed
MD_UISwitch_Digital S(DIGITAL_SWITCH_PIN, DIGITAL_SWITCH_ACTIVE);

//*********ROTARY SETUP ABOVE HERE**********

#include <TFT_eSPI.h>
#include <Ticker.h>
#include <lvgl.h>

static lv_group_t *g;             // An Object Group
static lv_indev_t *encoder_indev; // The input device

#define LVGL_TICK_PERIOD 20

Ticker               tick;             /* timer for interrupt handler */
TFT_eSPI             tft = TFT_eSPI(); /* TFT instance */
static lv_disp_buf_t disp_buf;
static lv_color_t    buf[LV_HOR_RES_MAX * 10];

#if USE_LV_LOG != 0
/* Serial debugging */
void my_print(lv_log_level_t level, const char *file, uint32_t line, const char *dsc) {

  Serial.printf("%s@%d->%s\r\n", file, line, dsc);
  delay(100);
}
#endif

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint16_t c;

  tft.startWrite(); /* Start new TFT transaction */
  tft.setAddrWindow(
    area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1)
  ); /* set the working window */
  for (int y = area->y1; y <= area->y2; y++) {
    for (int x = area->x1; x <= area->x2; x++) {
      c = color_p->full;
      tft.writeColor(c, 1);
      color_p++;
    }
  }
  tft.endWrite();            /* terminate TFT transaction */
  lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
}

/* Interrupt driven periodic handler */
static void lv_tick_handler(void) { lv_tick_inc(LVGL_TICK_PERIOD); }

// Reading input device

bool read_encoder(lv_indev_drv_t *indev, lv_indev_data_t *data) {
  static int     lastBtn;
  static int32_t last_diff = 0;

  int32_t        diff      = RotaryCount;
  int            btn_state = 0;

  if (ButtonPressed == 1) {
    data->state   = LV_INDEV_STATE_PR;
    ButtonPressed = 0;
    Serial.println("Button Pressed and Variable RESET to 0");
  } else {
    data->state = LV_INDEV_STATE_REL;
  }

  Serial.print("diff: ");
  Serial.println(diff);

  Serial.print("diff - last_diff: ");
  Serial.println(diff - last_diff);

  data->enc_diff = diff - last_diff;
  ;

  last_diff = diff;

  if (lastBtn != btn_state) {
    lastBtn = btn_state;
  }
  Serial.println();
  return false;
}

void setup() {

  Serial.begin(57600);

  //******Rotary and button read below******
  R.begin();
  S.begin();
  S.enableDoublePress(false);
  // S.enableLongPress(false);
  // S.enableRepeat(false);
  //   S.enableRepeatResult(true);
  //******Rotary and button read above******

  lv_init();

#if USE_LV_LOG != 0
  lv_log_register_print(my_print); /* register print function for debugging */
#endif

  tft.begin();        /* TFT init */
  tft.setRotation(1); /* Landscape orientation */

  lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

  /*Initialize the display*/
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res  = 320;
  disp_drv.ver_res  = 240;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer   = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the ENCODER*/
  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type    = LV_INDEV_TYPE_ENCODER;
  indev_drv.read_cb = read_encoder;
  encoder_indev     = lv_indev_drv_register(&indev_drv);

  // Create Group for encoder
  g = lv_group_create();
  lv_indev_set_group(encoder_indev, g);

  /*Initialize the graphics library's tick*/
  tick.attach_ms(LVGL_TICK_PERIOD, lv_tick_handler);

  /* Create simple keyboard */
  static lv_style_t rel_style, pr_style;

  lv_style_copy(&rel_style, &lv_style_btn_rel);
  rel_style.body.radius       = 0;
  rel_style.body.border.width = 1;

  lv_style_copy(&pr_style, &lv_style_btn_pr);
  pr_style.body.radius       = 0;
  pr_style.body.border.width = 1;

  /*Create a keyboard and apply the styles*/
  lv_obj_t *kb = lv_kb_create(lv_scr_act(), NULL);
  lv_kb_set_cursor_manage(kb, true);
  lv_kb_set_style(kb, LV_KB_STYLE_BG, &lv_style_transp_tight);
  lv_kb_set_style(kb, LV_KB_STYLE_BTN_REL, &rel_style);
  lv_kb_set_style(kb, LV_KB_STYLE_BTN_PR, &pr_style);

  /*Create a text area. The keyboard will write here*/
  lv_obj_t *ta = lv_ta_create(lv_scr_act(), NULL);
  lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);
  lv_ta_set_text(ta, "");

  /*Assign the text area to the keyboard*/
  lv_kb_set_ta(kb, ta);

  // ADD KEYBOARD TO GROUP G
  lv_group_add_obj(g, kb);
  lv_group_set_wrap(g, true);
}

void loop() {

  lv_task_handler(); /* let the GUI do its work */
  delay(5);

  //**********************ClickButton Start*************

  MD_UISwitch::keyResult_t k = S.read();

  switch (k) {
    case MD_UISwitch::KEY_NULL: /* Serial.print("KEY_NULL"); */
      break;
    case MD_UISwitch::KEY_UP: /*Serial.print("\nKEY_UP "); */
      break;
    case MD_UISwitch::KEY_DOWN: /* Serial.print("\n\nKEY_DOWN ");*/
      break;
    case MD_UISwitch::KEY_PRESS:
      Serial.println("********************* A KEY_PRESS ");
      ButtonPressed = 1;
      break;
    case MD_UISwitch::KEY_DPRESS:
      Serial.println("KEY_DOUBLE ");
      break;
    case MD_UISwitch::KEY_LONGPRESS:
      Serial.println("KEY_LONG   ");
      break;
    case MD_UISwitch::KEY_RPTPRESS: /*Serial.print("\nKEY_REPEAT ");*/
      break;
    default: /*Serial.print("\nKEY_UNKNWN ");*/
      break;
  }

  //**********************ClickButton End***************

  //**********************Rotary Start***************
  uint8_t x = R.read();
  if (x) {
    if (x == DIR_CW) {
      Serial.print("NEXT ");
      ++RotaryCount;
      Serial.println(RotaryCount);
    } else {
      Serial.print("PREV ");
      --RotaryCount;
      Serial.println(RotaryCount);
    }
  }
  //**********************Rotary End***************
}
#endif
