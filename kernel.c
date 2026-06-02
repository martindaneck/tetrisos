#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "keyboardap.h"
#include "timer.h"
#include "text.h"

typedef struct { // multiboot 
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



static inline unsigned char inb (unsigned short _port) {
    unsigned char rv;
    asm volatile ("in %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

static inline void outb (unsigned short _port, unsigned char _data) {
    asm volatile ("out %0, %1" : : "a" (_data), "dN" (_port));
}

int itoa(int value, char *str) {
    int base = 10;
    int i = 0;
    int len = 0;
    char temp[16];

    do {
        temp[i++] = (value % base) + '0';
    } while (value /= base);

    len = i;

    // reverse the string
    
    for (int j = 0; j < len; j++) {
        str[j] = temp[len - j - 1];
    }

    str[len] = '\0';

    return len;
}

// macro definitions
#define WIDTH 1024
#define HEIGHT 768 // inferred from boot.s, dirty magic numbers for simplicity
#define CENTER_X (WIDTH / 2)
#define CENTER_Y (HEIGHT / 2)
#define TILE_SIZE ((HEIGHT) / 20) 
#define PADDING (((HEIGHT) % 20) / 2)
#define MARGIN 1
#define BOARD_WIDTH (TILE_SIZE * 10) 
#define BOARD_HEIGHT (TILE_SIZE * 20) 


// Global variables
char c = 0; // input character

uint64_t time;
uint64_t last_time;
// fb variables
uint32_t fb_width;
uint32_t fb_height;
uint32_t fb_pitch;
uint64_t fb_address;

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
struct multiboot_color color_dark_gray = {30, 30, 30 }; // dark gray
struct multiboot_color color_black = {0, 0, 0 }; // black
struct multiboot_color color_white = {255, 255, 255 }; // white
struct multiboot_color color_red = {255, 0, 0 }; // red

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

void draw_board(struct Tetromino *active_tetromino); 

void draw_tetromino(struct Tetromino *tetromino);
bool move_tetromino(struct Tetromino *tetromino, char direction);
void write_tetromino(struct Tetromino *tetromino);
struct Tetromino generate_tetromino();
void rotate_tetromino(struct Tetromino *tetromino, char direction);
bool row_full(int row);
int clear_full_rows();
void animate_cleared_rows(int cleared_rows[4]);

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

    // init timer
    set_timer();

    // init font
    font_init();

    /// TETRIS GAME
    struct Tetromino active_tetromino = generate_tetromino();
    struct Tetromino next_tetromino = generate_tetromino();
    bool tetromino_down = false;
    bool paused = false;

    uint8_t G_VALUES[30] = {48, 43, 38, 33, 28, 23, 18, 13, 8, 6, 
                             5, 5, 5, 4, 4, 4, 3, 3, 3, 
                             2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                             1};
    uint16_t G = 48; //gravity, adjusted with levels
    uint16_t level = 0;
    uint16_t old_cleared_rows = 0;
    uint16_t cleared_rows = 0;
    uint64_t score = 0;
    
    
    kbdap_init();
    while (true) {
        /// POLLING
        // timer
        time += read_timer(); // incremented at 60 Hz
        // read keyboard
        kbdap_loop();
        if (last_time != time) {
            c = get_keypress();

            // toggle pause
            if (c == 'p') {
                paused = !paused;
            }

        } else {
            continue; // ensures game logic runs at 60 Hz
        }
        // update last time
        last_time = time;

        if (paused) {
            continue;
        }


        /// INPUT
        switch (c) {
            case 'a': 
                move_tetromino(&active_tetromino, 'l'); break;
            case 's': 
                tetromino_down = move_tetromino(&active_tetromino, 'd'); break;
            case 'd': 
                move_tetromino(&active_tetromino, 'r'); break;
            case 'q':
                rotate_tetromino(&active_tetromino, 'a'); break;
            case 'e':
                rotate_tetromino(&active_tetromino, 'c'); break;
        }
        /// LOGIC
        // fall
        if (time % G == 0) {
            tetromino_down |= move_tetromino(&active_tetromino, 'd');
        }

        if (tetromino_down) {
            write_tetromino(&active_tetromino);
            
            // generate next tetromino
            active_tetromino = next_tetromino;
            next_tetromino = generate_tetromino();

            // draw black over old next tetromino, here for performance
            draw_rect(760, 80, 300, 300, &color_black);
        }

        tetromino_down = false;

        // draw next tetromino, here because of visual bug
        for (int i = 0; i < 4; i++) {
            draw_tile(&(struct Tile){next_tetromino.tiles[i].posx + 9, next_tetromino.tiles[i].posy - 6, next_tetromino.tiles[i].color});
        }        

        
        // next
        draw_text("NEXT:", 780, 80, 10, &color_white);

        // clear full rows
        cleared_rows += clear_full_rows();

        if (old_cleared_rows != cleared_rows || cleared_rows == 0) {
            
            // score
            draw_text("SCORE:", 40, 80, 10, &color_white);
            draw_rect(40, 160, 280, 100, &color_black);
            int lines_cleared = cleared_rows - old_cleared_rows;
            int multiplier;
            switch (lines_cleared) {
                case 1: multiplier = 40; break;
                case 2: multiplier = 100; break;
                case 3: multiplier = 300; break;
                case 4: multiplier = 1200; break;
            }
            score += multiplier * (level + 1);


            char score_text[16];
            itoa(score, score_text);
            draw_text(score_text, 40, 160, 8, &color_white);

            // lines cleared
            draw_text("LINES:", 760, 450, 9, &color_white);
            draw_rect(760, 530, 280, 100, &color_black);
            char cleared_rows_text[16];
            itoa(cleared_rows, cleared_rows_text);
            draw_text(cleared_rows_text, 760, 530, 8, &color_white);

            // level
            level = cleared_rows / 10;
            draw_text("LEVEL:", 40, 450, 9, &color_white);
            draw_rect(40, 530, 280, 100, &color_black);
            char level_text[16];
            itoa(level, level_text);
            draw_text(level_text, 40, 530, 8, &color_white);

            // update G
            G = level < 30 ? G_VALUES[level] : G_VALUES[29];
        }

        old_cleared_rows = cleared_rows;

        // draw tetromino
        draw_tetromino(&active_tetromino);   
        
        // draw the board
        draw_board(&active_tetromino);


        
    }
}

void draw_tile(struct Tile *tile) {
    if (tile->posy > 19) { // dont draw outside board
        return;
    }

    int x = MARGIN + CENTER_X - BOARD_WIDTH / 2  + tile->posx * TILE_SIZE; 
    int y = MARGIN + CENTER_Y - BOARD_HEIGHT / 2 + (19 - tile->posy) * TILE_SIZE; 
    int tile_width = TILE_SIZE - MARGIN * 2;
    int tile_height = TILE_SIZE - MARGIN * 2;

    struct multiboot_color *color = tile->color == NULL ? &color_black : tile->color;    
    
    draw_rect(x, y, tile_width, tile_height, color);
}

void move_tile(struct Tile *tile, char direction) {
    // draw background over old tile first
    struct Tile temp_tile = {tile->posx, tile->posy, board[tile->posy][tile->posx]};
    draw_tile(&temp_tile);

    switch (direction) {
        case 'u': tile->posy++; break;
        case 'd': tile->posy--; break;
        case 'l': tile->posx--; break;
        case 'r': tile->posx++; break;
    }

    
}

void write_tile(struct Tile *tile) {
    board[tile->posy][tile->posx] = tile->color;
}



void draw_board(struct Tetromino *active_tetromino) { // don't draw board over the active tetromino
    bool flag = false;
    for (int i = 0; i < 20; i++) { // rows
        for (int j = 0; j < 10; j++) { // columns
            for (int k = 0; k < 4; k++) { // tiles
                if (i == active_tetromino->tiles[k].posy && j == active_tetromino->tiles[k].posx) {
                    flag = true;
                    break;
                }
            }
            if (flag) {
                flag = false;
                continue;
            }

            struct multiboot_color *color = board[i][j] == NULL ? &color_dark_gray : board[i][j];

            draw_tile(&(struct Tile) {j, i, color});
        }
    }
}

struct Tetromino generate_tetromino(){
    struct Tetromino tetromino;
    struct Tile start_tile = {5, 20, NULL}; // This is the initial position of the tetromino

    static int r = 0; // temp, implement random tetromino

    switch (r) {
        case 0: // T
            start_tile.color = &color_a;
            tetromino.tiles[0] = start_tile;
            tetromino.tiles[1] = (struct Tile){start_tile.posx-1, start_tile.posy, &color_a};
            tetromino.tiles[2] = (struct Tile){start_tile.posx+1, start_tile.posy, &color_a};
            tetromino.tiles[3] = (struct Tile){start_tile.posx, start_tile.posy-1, &color_a};
            break;
        case 1: // J
            start_tile.color = &color_b;
            tetromino.tiles[0] = start_tile;
            tetromino.tiles[1] = (struct Tile){start_tile.posx-1, start_tile.posy,   &color_b};
            tetromino.tiles[2] = (struct Tile){start_tile.posx+1, start_tile.posy-1, &color_b};
            tetromino.tiles[3] = (struct Tile){start_tile.posx+1, start_tile.posy,   &color_b};
            break;
        case 2: // L
            start_tile.color = &color_c;
            tetromino.tiles[0] = start_tile;
            tetromino.tiles[1] = (struct Tile){start_tile.posx-1, start_tile.posy-1, &color_c};
            tetromino.tiles[2] = (struct Tile){start_tile.posx+1, start_tile.posy, &color_c};
            tetromino.tiles[3] = (struct Tile){start_tile.posx-1, start_tile.posy, &color_c};
            break;
        case 3: // I
            start_tile.color = &color_d;
            tetromino.tiles[0] = start_tile;
            tetromino.tiles[1] = (struct Tile){start_tile.posx-1, start_tile.posy, &color_d};
            tetromino.tiles[2] = (struct Tile){start_tile.posx+1, start_tile.posy, &color_d};
            tetromino.tiles[3] = (struct Tile){start_tile.posx+2, start_tile.posy, &color_d};
            break;
        case 4: // O
            start_tile.color = &color_e;
            tetromino.tiles[0] = start_tile;
            tetromino.tiles[1] = (struct Tile){start_tile.posx+1, start_tile.posy, &color_e};
            tetromino.tiles[2] = (struct Tile){start_tile.posx, start_tile.posy-1, &color_e};
            tetromino.tiles[3] = (struct Tile){start_tile.posx+1, start_tile.posy-1, &color_e};
            break;
        case 5: // Z
            start_tile.color = &color_f;
            tetromino.tiles[0] = start_tile;
            tetromino.tiles[1] = (struct Tile){start_tile.posx+1, start_tile.posy-1, &color_f};
            tetromino.tiles[2] = (struct Tile){start_tile.posx, start_tile.posy-1, &color_f};
            tetromino.tiles[3] = (struct Tile){start_tile.posx-1, start_tile.posy, &color_f};
            break;
        case 6: // S
            start_tile.color = &color_g;
            tetromino.tiles[0] = start_tile;
            tetromino.tiles[1] = (struct Tile){start_tile.posx+1, start_tile.posy, &color_g};
            tetromino.tiles[2] = (struct Tile){start_tile.posx, start_tile.posy-1, &color_g};
            tetromino.tiles[3] = (struct Tile){start_tile.posx-1, start_tile.posy-1, &color_g};
            break;
    }

    // temp, for testing
    r++;
    if (r > 6) {
        r = 0;
    }

    return tetromino;
}

void draw_tetromino(struct Tetromino *tetromino) {
    for (int i = 0; i < 4; i++) {
        draw_tile(&(tetromino->tiles[i]));
    }
}

bool move_tetromino(struct Tetromino *tetromino, char direction) {
    for (int i = 0; i < 4; i++) {
        struct Tile *tile = &(tetromino->tiles[i]);
        struct Tile new_tile = {tile->posx, tile->posy, tile->color};

        move_tile(&new_tile, direction);

        // bounds checking
        if (new_tile.posx < 0 || new_tile.posx > 9 || new_tile.posy < 0) {
            return direction == 'd';
        }

        // other pieces checking
        if (board[new_tile.posy][new_tile.posx] != NULL) {
            return direction == 'd';
        }
    }

    for (int i = 0; i < 4; i++) {
        move_tile(&(tetromino->tiles[i]), direction);
    }

    return false;
}

void write_tetromino(struct Tetromino *tetromino) {
    for (int i = 0; i < 4; i++) {
        write_tile(&(tetromino->tiles[i]));
    }
}

void rotate_tetromino(struct Tetromino *tetromino, char direction) {
    int newx[4];    // new positions
    int newy[4];

    // tile to pivot on
    int px = tetromino->tiles[0].posx;
    int py = tetromino->tiles[0].posy;

    for (int i = 0; i < 4; i++) {
        struct Tile *tile = &(tetromino->tiles[i]);

        // relative position
        int relx = tile->posx - px;
        int rely = tile->posy - py;

        // new position 
        if (direction == 'c') { // clockwise
            newx[i] = px + rely;
            newy[i] = py - relx;
        } else { // counter clockwise
            newx[i] = px - rely;
            newy[i] = py + relx;
        }

        // bounds checking
        if (newx[i] < 0 || newx[i] > 9 || newy[i] < 0) {
            return;
        }

        // other pieces checking
        if (board[newy[i]][newx[i]] != NULL) {
            return;
        }
    }

    // apply rotation
    for (int i = 1; i < 4; i++) { // first tile doesn't move
        tetromino->tiles[i].posx = newx[i];
        tetromino->tiles[i].posy = newy[i];
    }
}

bool row_full(int row) {
    bool full = true;
    for (int i = 0; i < 10; i++) {
        if (board[row][i] == NULL) {
            full = false;
            break;
        }
    }
    return full;
}

int clear_full_rows() {
    int cleared = 0;
    int cleared_rows[4] = {-1, -1, -1, -1}; // array of full rows' indices

    for (int i = 0; i < 20; i++) {
        if (row_full(i)) {
            cleared++;
        }
    }

    for (int l = 0; l < 4; l++) {
        for (int i = 0; i < 20; i++) {

            if (row_full(i) && cleared_rows[0] != i && cleared_rows[1] != i && cleared_rows[2] != i && cleared_rows[3] != i) { // if row is full and not already in array
                cleared_rows[l] = i;
            }
        }
    }

    if (cleared) {
        animate_cleared_rows(cleared_rows);
    }
    
    for (int i = 0; i < 4; i++) {
        if (cleared_rows[i] == -1) {
            continue;
        }
        for (int j = cleared_rows[i]; j < 21; j++) {
            for (int k = 0; k < 10; k++) {
                board[j][k] = board[j+1][k];
            }
        }
    } 

    return cleared;
}

void animate_cleared_rows(int cleared_rows[4]) {
    int frames = 0;
    int counter = 0;
    int last_time = -1;
    int time = 0; 
    time += read_timer();

    draw_board(NULL); // i have ZERO idea why the FUCK does this have to be there, otherwise black is written over last active tetromino
    
    while(counter < 7) { // 7 is shenanigance for pretty animation, otherwise 4
        time += read_timer();
        if (last_time != time) { // same 60 fps logic as in main loop
            last_time = time;
            frames++;

            if (frames % 4 != 0) { 
                continue;
            }

            for (int i = 0; i < 4; i++) {
                if (cleared_rows[i] == -1) {
                    continue;
                }

                if (counter > 4) { // shenanigance for pretty animation
                    continue;   
                }

                draw_tile(&(struct Tile){4 - counter, cleared_rows[i], &color_dark_gray}); // draw empty spaces from middle to edges
                draw_tile(&(struct Tile){5 + counter, cleared_rows[i], &color_dark_gray});
            }

            counter++;
        }
    } 
}