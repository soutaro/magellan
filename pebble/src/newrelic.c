#include <pebble.h>

#define KEY_THROUGHPUT 0
#define KEY_APPDEX 1
#define KEY_TIME 2
#define KEY_STATUS 3
#define KEY_ERROR 4

static Window *window;
static TextLayer *background_layer;
static TextLayer *time_layer;
static TextLayer *status_layer;
static TextLayer *throughput_title_layer;
static TextLayer *throughput_value_layer;
static TextLayer *appdex_title_layer;
static TextLayer *appdex_value_layer;
static TextLayer *time_title_layer;
static TextLayer *time_value_layer;
static TextLayer *footer_line_layer;
static time_t next_update_at;

static void update_time_layer() {
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  static char buffer[] = "00:00";

  if(clock_is_24h_style() == true) {
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  text_layer_set_text(time_layer, buffer);
}

static void update_values(int32_t throughput, int32_t appdex100, int32_t time, char *status) {
  static char throughput_buf[10];
  static char appdex_buf[10];
  static char time_buf[10];
  static char status_buf[10];

  GColor bg_color;
  GColor fg_color;

  if (throughput > 1000) {
    snprintf(throughput_buf, 30, "%d,%03d", (int)throughput/1000, (int)throughput%1000);
  } else {
    snprintf(throughput_buf, 30, "%d", (int)throughput);
  }
  text_layer_set_text(throughput_value_layer, throughput_buf);

  snprintf(appdex_buf, 10, "%d.%02d", (int)appdex100/100, (int)appdex100%100);
  text_layer_set_text(appdex_value_layer, appdex_buf);

  if (time < 1000) {
    snprintf(time_buf, 10, "%dms", (int)time);
  } else {
    snprintf(time_buf, 10, "%d.%03ds", (int)time/1000, (int)time%1000);
  }
  text_layer_set_text(time_value_layer, time_buf);

  strcpy(status_buf, status);
  status_buf[0] += 'A'-'a';
  text_layer_set_text(status_layer, status_buf);

  if (strcmp(status, "green")) {
    bg_color = GColorBlack;
    fg_color = GColorWhite;
  } else {
    bg_color = GColorWhite;
    fg_color = GColorBlack;
  }

  text_layer_set_text_color(time_layer, fg_color);
  text_layer_set_text_color(status_layer, fg_color);
  text_layer_set_text_color(throughput_title_layer, fg_color);
  text_layer_set_text_color(throughput_value_layer, fg_color);
  text_layer_set_text_color(appdex_title_layer, fg_color);
  text_layer_set_text_color(appdex_value_layer, fg_color);
  text_layer_set_text_color(time_title_layer, fg_color);
  text_layer_set_text_color(time_value_layer, fg_color);
  text_layer_set_background_color(background_layer, bg_color);
}

void kick_update() {
  if (time(NULL) < next_update_at) {
    APP_LOG(APP_LOG_LEVEL_INFO, "[kick_update] update will be in %d seconds", (int)(next_update_at - time(NULL)));
    return;
  }

  APP_LOG(APP_LOG_LEVEL_INFO, "[kick_update] updating...");

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  app_message_outbox_send();
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  int16_t width = bounds.size.w - 10;

  background_layer = text_layer_create(bounds);
  layer_add_child(window_layer, text_layer_get_layer(background_layer));

  time_layer = text_layer_create(GRect(5, 15, width, 30));
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_background_color(time_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  status_layer = text_layer_create(GRect(0, 50, width+10, 50));
  text_layer_set_font(status_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text_alignment(status_layer, GTextAlignmentCenter);
  text_layer_set_background_color(status_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(status_layer));

  throughput_title_layer = text_layer_create(GRect(5, 100, 70, 30));
  text_layer_set_font(throughput_title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(throughput_title_layer, GTextAlignmentLeft);
  text_layer_set_background_color(throughput_title_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(throughput_title_layer));

  throughput_value_layer = text_layer_create(GRect(5+70, 95, width-70, 30));
  text_layer_set_font(throughput_value_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(throughput_value_layer, GTextAlignmentRight);
  text_layer_set_background_color(throughput_value_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(throughput_value_layer));

  appdex_title_layer = text_layer_create(GRect(5, 120, 70, 30));
  text_layer_set_font(appdex_title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(appdex_title_layer, GTextAlignmentLeft);
  text_layer_set_background_color(appdex_title_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(appdex_title_layer));

  appdex_value_layer = text_layer_create(GRect(5+70, 115, width-70, 30));
  text_layer_set_font(appdex_value_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(appdex_value_layer, GTextAlignmentRight);
  text_layer_set_background_color(appdex_value_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(appdex_value_layer));

  time_title_layer = text_layer_create(GRect(5, 140, 70, 20));
  text_layer_set_font(time_title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(time_title_layer, GTextAlignmentLeft);
  text_layer_set_background_color(time_title_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(time_title_layer));

  time_value_layer = text_layer_create(GRect(5+70, 135, width-70, 30));
  text_layer_set_font(time_value_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(time_value_layer, GTextAlignmentRight);
  text_layer_set_background_color(time_value_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(time_value_layer));

  footer_line_layer = text_layer_create(GRect(5, 95, width, 1));
  text_layer_set_background_color(footer_line_layer, GColorBlack);
  layer_add_child(window_layer, text_layer_get_layer(footer_line_layer));

  text_layer_set_text(time_layer, "00:00");
  text_layer_set_text(status_layer, "......");
  text_layer_set_text(throughput_title_layer, "Throughput");
  text_layer_set_text(throughput_value_layer, "0");
  text_layer_set_text(appdex_title_layer, "Appdex");
  text_layer_set_text(appdex_value_layer, "0");
  text_layer_set_text(time_title_layer, "Response");
  text_layer_set_text(time_value_layer, "0ms");
}

static void window_unload(Window *window) {
  text_layer_destroy(background_layer);
  text_layer_destroy(time_layer);
  text_layer_destroy(status_layer);
  text_layer_destroy(throughput_title_layer);
  text_layer_destroy(throughput_value_layer);
  text_layer_destroy(appdex_title_layer);
  text_layer_destroy(appdex_value_layer);
  text_layer_destroy(time_title_layer);
  text_layer_destroy(time_value_layer);
  text_layer_destroy(footer_line_layer);
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
  kick_update();
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  int32_t throughput = 0;
  int32_t appdex = 0;
  int32_t response_time = 0;
  int32_t delay = 0;
  char* status = NULL;

  Tuple *t = dict_read_first(iterator);

  while(t) {
    switch(t->key) {
    case KEY_THROUGHPUT:
      throughput = t->value->int32;
      break;
    case KEY_APPDEX:
      appdex = t->value->int32;
      break;
    case KEY_TIME:
      response_time = t->value->int32;
      break;
    case KEY_STATUS:
      status = t->value->cstring;
      if (strcmp(status, "green") == 0) {
        delay = 300;
      }
      break;
    case KEY_ERROR:
      status = "error";
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    t = dict_read_next(iterator);
  }

  APP_LOG(APP_LOG_LEVEL_INFO, "[invox_received_callback] will update in %d seconds", (int)delay);
  next_update_at = time(NULL) + delay;
  update_values(throughput, appdex, response_time, status);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
}

static void tick_handler(struct tm *tick_time, TimeUnits unit_changed) {
  update_time_layer();
  kick_update();
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, false);

  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  kick_update();
  app_event_loop();
  deinit();
}
