/* Libreria general */

#include <lib.h>

uint64_t lib_set_scale(uint8_t scale) {
  return dispatcherSetScale(scale);
}

uint8_t lib_get_scale() {
  uint8_t scale;
  dispatcherGetScale(&scale);
  return scale;
}

void lib_get_time(time_struct *time) {
  dispatcherGetTime(time);
}

//int dispatcherread(int fd, char * buffer, int count);
uint64_t lib_read_char(char *buffer) {
  dispatcherRead(STDIN, buffer, 1);
  return buffer[0] == 0 ? 0 : 1;
}

void lib_sleep(uint64_t millis) {
  dispatcherSleep(millis);
}

void lib_draw_rectangle(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint32_t color) {
  dispatcherDrawRectangle(x, y, width, height, color); 
}

void lib_draw_string(char *string, uint64_t x, uint64_t y, uint32_t color, uint32_t bg_color) {
  dispatcherDrawString(string, x, y, color, bg_color);
}

// TODO: usar lib_get_time en commands.c. time_struct esta duplicado en commands.h.