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

void draw_pixel_fb(uint32_t x, uint32_t y, struct multiboot_color *color, uint64_t fb_address, uint32_t pitch) {
    *(uint32_t *)(fb_address + y * pitch + x * 4) = color->red << 16 | color->green << 8 | color->blue;
}
// pretty macro for it
#define draw_pixel(x, y, color) draw_pixel_fb(x, y, color, fb_address, fb_pitch)

void draw_rect_fb(uint32_t x, uint32_t y, uint32_t width, uint32_t height, struct multiboot_color *color, uint64_t fb_address, uint32_t fb_pitch) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            draw_pixel(x + j, y + i, color);
        }
    }
}
#define draw_rect(x, y, width, height, color) draw_rect_fb(x, y, width, height, color, fb_address, fb_pitch)

// Global variables
// fb variables
uint32_t fb_width;
uint32_t fb_height;
uint32_t fb_pitch;
uint64_t fb_address;

// tetris variables
int width = 1024, height = 768; // inferred from boot.s, dirty magic numbers for simplicity
// definitions, magic numbers for simplicity
int center_x = 512; // width / 2
int center_y = 384; // height / 2
int tile_size = 38; // height / 20
int padding = 4; // (height % 20) / 2
int board_width = 380; // tile_size * 10
int board_height = 760; // tile_size * 20
int SPEED = 1000; // completely arbitrary value

/// TETRIS GAME SPECIFIC STUFF
// colors 1-7 + 3 grays
struct multiboot_color color_a = {94, 216, 125 }; // color 1
struct multiboot_color color_b = {72, 211, 176 }; // color 2
struct multiboot_color color_c = {91, 184, 215 }; // color 3
struct multiboot_color color_d = {49, 87, 202  }; // color 4
struct multiboot_color color_e = {147, 121, 223}; // color 5
struct multiboot_color color_f = {168, 54, 206 }; // color 6
struct multiboot_color color_g = {215, 90, 184 }; // color 7
struct multiboot_color color_light_gray = {208, 208, 208 }; // light gray
struct multiboot_color color_gray = {128, 128, 128 }; // medium gray
struct multiboot_color color_dark_gray = {32, 32, 32 }; // dark gray
struct multiboot_color color_black = {0, 0, 0 }; // black
struct multiboot_color color_white = {255, 255, 255 }; // white

struct Tile {
    int posx;
    int posy;
    struct multiboot_color *color;
};

struct Tetromino {
    struct Tile tiles[4];
};

struct multiboot_color *board[30][10] = {0};

// functions definitions
void draw_tile(struct Tile *tile);
void move_tile(struct Tile *tile, char direction);
void write_tile(struct Tile *tile);

void draw_board();

void draw_tetromino(struct Tetromino *tetromino);
int move_tetromino(struct Tetromino *tetromino, char direction);
void write_tetromino(struct Tetromino *tetromino);
struct Tetromino generate_tetromino();
void rotate_tetromino(struct Tetromino *tetromino, char direction);
bool full_row(int row);
int clear_full_rows();

void kernel_main(unsigned long addr) 
{   
    // multiboot info
    multiboot_info_t *mbi;
    mbi = (multiboot_info_t *)addr;

    // framebuffer values
    fb_width = mbi->framebuffer_width;
    fb_height = mbi->framebuffer_height;
    fb_pitch = mbi->framebuffer_pitch;
    fb_address = mbi->framebuffer_addr;

    /// TETRIS GAME
    
    

    // test stuff - temporary
    struct multiboot_color test_color = {66, 0, 88};
    struct Tile test_tile = {0, 0, &test_color};
    
    kbdap_init();
    while (true) {
        /// POLL KEYBOARD
        kbdap_loop();
        char c = get_keypress();

        /// INPUT
        switch (c) {
            case 'w': 
                move_tile(&test_tile, 'u'); break;
            case 'a': 
                move_tile(&test_tile, 'l'); break;
            case 's': 
                move_tile(&test_tile, 'd'); break;
            case 'd': 
                move_tile(&test_tile, 'r'); break;
            case 'e': 
                write_tile(&test_tile); break;
        }
        /// LOGIC
        

        /// RENDER

        // draw the board
        //draw_board();
        
        // draw test tile
        draw_tile(&test_tile);
    }
}

void draw_tile(struct Tile *tile) {
    static int margin = 1;
    int x = margin + center_x - board_width / 2  + tile->posx * tile_size; 
    int y = margin + center_y - board_height / 2 + tile->posy * tile_size; 
    int width = tile_size - margin * 2;
    int height = tile_size - margin * 2;

    struct multiboot_color *color = tile->color == NULL ? &color_black : tile->color;    
    

    draw_rect(x, y, width, height, color);
}

void move_tile(struct Tile *tile, char direction) {
    // draw background over old tile firt
    struct Tile temp_tile = {tile->posx, tile->posy, board[tile->posy][tile->posx]};
    draw_tile(&temp_tile);

    switch (direction) {
        case 'u': tile->posy--; break;
        case 'd': tile->posy++; break;
        case 'l': tile->posx--; break;
        case 'r': tile->posx++; break;
    }
}

void write_tile(struct Tile *tile) {
    board[tile->posy][tile->posx] = tile->color;
}



void draw_board() {
    for (int i = 0; i < board_height; i++) {
        for (int j = 0; j < board_width; j++) {
            draw_tile(&(struct Tile) {j, i, board[i][j]});
        }
    }
}