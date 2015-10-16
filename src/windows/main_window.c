#include "main_window.h"

static Window *s_window;
static TextLayer *s_time_layer;
static Layer *s_canvas;

static void layer_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  const int16_t top_offset = (bounds.size.h / 2) - 28;
  GRect text_frame = grect_inset(bounds, GEdgeInsets(top_offset, 0, 0, 0));

  // Create time TextLayer
  s_time_layer = text_layer_create(text_frame);
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(layer, text_layer_get_layer(s_time_layer));

  // Yellow circle
  GRect frame = grect_inset(bounds, GEdgeInsets(30));
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 30, 
                       DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_canvas = layer_create(bounds);
  layer_set_update_proc(s_canvas, layer_update_proc);
  layer_add_child(window_layer, s_canvas);
}

static void window_unload(Window *window) {
  layer_destroy(s_canvas);
  window_destroy(s_window);
}

void main_window_push() {
  s_window = window_create();
  window_set_background_color(s_window, BG_COLOR);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
}

void main_window_update() {
  layer_mark_dirty(s_canvas);

  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}