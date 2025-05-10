#include <interrupts/kernel_syscalls.h>
#include <drivers/videoDriver.h>
#include <drivers/time.h>
#include <keyboard.h>
#include <font.h>
#include <rtc.h>
#include <memory/memoryManager.h>

static int specialCase(char c);

static int posx=START_X;
static int posy=START_Y;

extern uint64_t regs_snapshot[SNAPSHOT_REGS_LENGTH];
extern uint64_t snapshot;

void syscall_handler(Registers *regs) {
  switch (regs->rax) {
    case 0x00:
      sys_read(regs->rdi, (char *)regs->rsi, regs->rdx);
      break;
    case 0x01:
      sys_write(regs->rdi, (char *)regs->rsi, regs->rdx, regs->rcx);
      break;
    case 0x02:
      sys_draw_rectangle(regs->rdi, regs->rsi, regs->rdx, regs->rcx, regs->r8);
      break;
    case 0x03:
      sys_clear_screen();
      break;
    case 0x04:
      sys_reset_position();
      break;
    case 0x05:
      sys_jump_line();
      break;   
    case 0x06:
      sys_get_screen_info((screen_info *)regs->rdi); 
      break;
    case 0x07:
      sys_get_registers();
      break;
    case 0x08: 
      sys_get_time((time_struct * ) regs->rdi);
      break;
    case 0x09:
      sys_draw_string((char *)regs->rdi, regs->rsi, regs->rdx, regs->rcx, regs->r8);
      break;
    case 0x0A:
      sys_sleep(regs->rdi);
      break;
    case 0x0B:
      sys_inc_scale();
      break;
    case 0x0C:
      sys_dec_scale();
      break;
    case 0x0E:
      sys_get_scale((uint8_t *) regs->rdi);
      break;
    case 0x0F:
      sys_set_scale((uint8_t) regs->rdi);
      break;
    case 0x11:
      sys_free((void *)regs->rdi);
      break;
    case 0x12:
      sys_mstatus((uint32_t *) regs->rdi);
      break;
    case 0x13:
      sys_malloc(regs->rdi);
      break;
  }
}

void sys_write(int fd, char *buffer, int count, int textColor) {
  if (fd == STDOUT) {
    for (int i = 0; i < count && buffer[i] != 0; i++) {
      if (posx > getScreenWidth() - getFontWidth() - START_X - MARGIN){
        sys_jump_line();
      }
      if(specialCase(buffer[i]) == 0){
        putChar(buffer[i], textColor, 0x00000000, posx += (getFontWidth() + SPACE_BETWEEN)*getScale(), posy);
      }
    }
  }
}

int sys_read(int fd, char * buffer, int count) {
    if(fd != STDIN) {
        return 0;
    }
    for(int i = 0; i < count; i++) {
        char c = kb_getchar();
        buffer[i] = c;
    }
    return count;
}

uint64_t sys_draw_rectangle(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint32_t color) {
  return putRectangle(color, x, y, width, height);
}

uint64_t sys_draw_string(char * string, uint64_t x, uint64_t y, uint32_t color, uint32_t bg_color){
  posx = x;
  posy = y;
  while(*string){
    putChar(*string, color, bg_color, posx += (getFontWidth() + SPACE_BETWEEN)*getScale(), posy);
    string++;
  }
  return 0;
}

uint64_t sys_clear_screen() {
  posx = START_X;
  posy = START_Y;
  return clearScreen();
  }

void sys_reset_position(){
    posx = START_X;
    posy = START_Y;
}

void sys_jump_line(){  void * p = allocMemory(16);
  freeMemory(p);
    posx=START_X;
    posy+=(getFontHeight() + LINE_SPACE)*getScale();
}

void sys_get_screen_info(screen_info *info){
  info->width = getScreenWidth();
  info->height = getScreenHeight();
}

uint64_t sys_sleep(uint64_t millis) {
    unsigned long long initial_time = ms_elapsed();
    unsigned long long currentTime = initial_time;
    // Activate interrupts
    _sti();
    while ((currentTime - initial_time) <= millis) {
        currentTime = ms_elapsed();
    }
    // Deactivate interrupts
    _cli();
    return 1;
}

static size_t strlen(const char *str) {
    size_t len = 0;
    while(str[len] != 0) {
        len++;
    }
    return len;
}

static char * reverse(char *str) {
    size_t len = strlen(str);
    for (size_t i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
    return str;
}

void writeNumber(uint64_t num, char * str) {
    uint64_t i = 0;

    if (num == 0) {
        str[i++] = '0';
    }
    else {
        while (num != 0) {
            str[i++] = num % 10 + '0';
            num /= 10;
        }
    }
    str[i] = 0;

    str = reverse(str);
    
    sys_write(STDOUT, str, strlen(str), 0xFF00AA00);
}
   
void sys_get_registers() {
  if (!snapshot) {
    sys_write(STDOUT, "No se encontro ninguna snapshot, presione 'ESC' para obtener una snapshot de los registros.\n", 92, 0xFFFFFFFF);
    return;
  }
  
	const char * regs_names[] = {"rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", "rip"};
  for (int i = 0; i < SNAPSHOT_REGS_LENGTH; i++) {
    sys_write(STDOUT, regs_names[i], 10 , 0x00FFFFFF);
    sys_write(STDOUT, ": 0x", 4 , 0x00FFFFFF);
    sys_write(STDOUT, (char *) num_to_string(regs_snapshot[i], 16), 10 , 0x00FFFFFF);
    sys_jump_line();
  }
  return;
}

static int specialCase(char c) {
    if(c == '\n'){
        sys_jump_line();
        return 1;
    }
    else if(c=='\b'){
        if(posx > START_X){
            putChar(' ', 0x00000000, 0x00000000, posx, posy);
            posx -= (getFontWidth() + SPACE_BETWEEN)*getScale();
        }
        return 1;
    }
    return 0;
}

uint64_t sys_get_time(time_struct * time){
    time->seconds = getRTCSeconds();
    time->minutes =  getRTCMinutes();
    time->hour =  getRTCHours();
    time->day = getRTCDayOfMonth();
    time->month = getRTCMonth();
    time->year = getRTCYear();
    return 0;
}

void sys_inc_scale(){
  incScale();
  return;
}

void sys_dec_scale(){
  decScale();
  return;
}

void sys_get_scale(uint8_t *scale){
  *scale = getScale();
  return;
}

uint64_t sys_set_scale(uint8_t scale){
  return setScale(scale);
}

void * sys_malloc(uint32_t size){
  return allocMemory(size);
}

void sys_free(void *memorySegment){
  sys_write(STDOUT, "KERNEL: Freeing memory at address: ", 34, 0x00FFFFFF);
  writeNumber((uint64_t)memorySegment, (char *) num_to_string((uint64_t)memorySegment, 16));
  sys_write(STDOUT, "\n", 1, 0x00FFFFFF);
  freeMemory(memorySegment);
  return;
}

void sys_mstatus(MemoryStatus *status){
  getMemoryStatus(status);
  return;
}