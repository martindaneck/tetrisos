#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "keyboardap.h"

typedef struct {
    uint32_t flags;

    uint32_t mem_lower;
    uint32_t mem_upper;

    uint32_t boot_device;
    uint32_t cmdline;

    uint32_t mods_count;
    uint32_t mods_addr;

    uint32_t syms[4];

    uint32_t mmap_length;
    uint32_t mmap_addr;

    uint32_t drives_length;
    uint32_t drives_addr;

    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;

    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;

    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;

} multiboot_info_t;

struct multiboot_color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

// common colors
struct multiboot_color black = {0, 0, 0};
struct multiboot_color red = {255, 0, 0};
struct multiboot_color green = {0, 255, 0};
struct multiboot_color blue = {0, 0, 255};


void draw_pixel_fb(uint32_t x, uint32_t y, struct multiboot_color *color, uint64_t fb_address, uint32_t pitch) {
    *(uint32_t *)(fb_address + y * pitch + x * 4) = color->red << 16 | color->green << 8 | color->blue;
}
// pretty macro for it
#define draw_pixel(x, y, color) draw_pixel_fb(x, y, color, fb_address, pitch)

void draw_rect_fb(uint32_t x, uint32_t y, uint32_t width, uint32_t height, struct multiboot_color *color, uint64_t fb_address, uint32_t pitch) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            draw_pixel(x + j, y + i, color);
        }
    }
}
#define draw_rect(x, y, width, height, color) draw_rect_fb(x, y, width, height, color, fb_address, pitch)

void kernel_main(unsigned long addr) 
{   
    // multiboot info
    multiboot_info_t *mbi;
    mbi = (multiboot_info_t *)addr;

    // framebuffer values
    uint32_t width = mbi->framebuffer_width;
    uint32_t height = mbi->framebuffer_height;
    uint32_t pitch = mbi->framebuffer_pitch;
    uint64_t fb_address = mbi->framebuffer_addr;
    
    int r = 255, g = 0, b = 0;
    struct multiboot_color pink = {255, 0, 255};
    
    kbdap_init();
    while (true) {
        /// POLL KEYBOARD
        kbdap_loop();
        char c = kbd_US[kbdap_last_scancode];
        switch (c) {
            case 'q': if (r < 255) r += 1; break;
            case 'a': if (r > 0)   r -= 1; break;
            case 'w': if (g < 255) g += 1; break;
            case 's': if (g > 0)   g -= 1; break;
            case 'e': if (b < 255) b += 1; break;
            case 'd': if (b > 0)   b -= 1; break;
        }

        /// LOGIC
        struct multiboot_color color = {r, g, b};

        /// RENDER
        // fill the screen
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                draw_pixel(x, y, &color);
            }
        }
        
        
        
    }
        
}