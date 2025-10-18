#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

size_t terminal_row = 0;
size_t terminal_col = 0;
uint8_t terminal_color = 0x0F; // белый на черном
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

static inline uint16_t vga_entry(unsigned char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

int strcmp(const char* a, const char* b) {
    while(*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(unsigned char*)a - *(unsigned char*)b;
}

void terminal_initialize(void) {
    for(size_t y=0; y<VGA_HEIGHT; y++)
        for(size_t x=0; x<VGA_WIDTH; x++)
            terminal_buffer[y*VGA_WIDTH+x] = vga_entry(' ', terminal_color);
}

void terminal_putchar(char c) {
    if(c == '\n') { terminal_col = 0; if(++terminal_row == VGA_HEIGHT) terminal_row=0; return; }
    terminal_buffer[terminal_row*VGA_WIDTH + terminal_col] = vga_entry(c, terminal_color);
    if(++terminal_col == VGA_WIDTH) { terminal_col=0; if(++terminal_row==VGA_HEIGHT) terminal_row=0; }
}

void terminal_writestring(const char* str) {
    for(size_t i=0; str[i]; i++) terminal_putchar(str[i]);
}

// заглушки для команд
void do_reboot(void) { terminal_writestring("\nRebooting...\n"); while(1){__asm__("hlt");} }
void do_poweroff(void) { terminal_writestring("\nPowering off...\n"); while(1){__asm__("hlt");} }

// простое ядро с командной строкой
void kernel_main(void) {
    terminal_initialize();
    terminal_writestring("Welcome to FreedomUI!\n> ");

    char buffer[128];
    size_t pos = 0;

    while(1) {
        char c = 0;
        __asm__ __volatile__("inb $0x60, %0":"=a"(c)); // читаем клавишу с клавиатуры
        if(c == '\n') {
            buffer[pos] = 0;
            if(pos > 0) {
                if(!strcmp(buffer,"help"))
                    terminal_writestring("Commands: help info reboot poweroff\n");
                else if(!strcmp(buffer,"info"))
                    terminal_writestring("FreedomUI kernel v0.1\n");
                else if(!strcmp(buffer,"reboot"))
                    do_reboot();
                else if(!strcmp(buffer,"poweroff"))
                    do_poweroff();
                else
                    terminal_writestring("Unknown command\n");
            }
            pos=0;
            terminal_writestring("> ");
        } else if(c >= 32 && c <= 126) { // печатные символы
            buffer[pos++] = c;
            terminal_putchar(c);
        }
    }
}

