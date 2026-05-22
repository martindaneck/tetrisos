#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
__attribute__((section(".multiboot"))) struct {

} multiboot_header;

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



void kernel_main(unsigned long addr) 
{
    multiboot_info_t *mbi;
    mbi = (multiboot_info_t *)addr;

    // draw some blue
    struct multiboot_color color = { 0, 0, 255 };
    for (int i = 0; i < mbi->framebuffer_height; i++) {
        for (int j = 0; j < mbi->framebuffer_width; j++) {
            *(uint32_t *)(mbi->framebuffer_addr + i * mbi->framebuffer_pitch + j * 4) = color.red << 16 | color.green << 8 | color.blue;
        }
    }
}