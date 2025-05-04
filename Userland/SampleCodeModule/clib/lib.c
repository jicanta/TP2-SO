/* Libreria general */

#include <lib.h>

uint64_t lib_set_scale(uint8_t scale) {
  return _sys_set_scale(scale);
}

uint8_t lib_get_scale() {
  uint8_t scale;
  _sys_get_scale(&scale);
  return scale;
}

void lib_get_time(time_struct *time) {
  _sys_get_time(time);
}

//int _sys_read(int fd, char * buffer, int count);
uint64_t lib_read_char(char *buffer) {
  _sys_read(STDIN, buffer, 1);
  return buffer[0] == 0 ? 0 : 1;
}

void lib_sleep(uint64_t millis) {
  _sys_sleep(millis);
}

void lib_draw_rectangle(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint32_t color) {
  _sys_draw_rectangle(x, y, width, height, color); 
}

void lib_draw_string(char *string, uint64_t x, uint64_t y, uint32_t color, uint32_t bg_color) {
  _sys_draw_string(string, x, y, color, bg_color);
}

void lib_play_sound(uint64_t f, uint64_t ms) {
  _sys_play_sound(f, ms);
}

// TODO: usar lib_get_time en commands.c. time_struct esta duplicado en commands.h.