#include <stdint.h>
#include <stdbool.h>

struct multiboot_color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

// drawing functions

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

/// FONT
// each letter is an array of 15-25 booleans - 5 rows of 3-5 pixels

typedef struct {
    const uint8_t data[5][5];
    uint8_t w;
} Glyph;

/* A */
static const Glyph font_A = {
{
    {1,1,1,0,0},
    {1,0,1,0,0},
    {1,1,1,0,0},
    {1,0,1,0,0},
    {1,0,1,0,0},
}, 3 };

/* B */
static const Glyph font_B = {
{
    {1,1,1,0,0},
    {1,0,1,0,0},
    {1,1,0,0,0},
    {1,0,1,0,0},
    {1,1,1,0,0},
}, 3 };

/* C */
static const Glyph font_C = {
{
    {1,1,1,0,0},
    {1,0,0,0,0},
    {1,0,0,0,0},
    {1,0,0,0,0},
    {1,1,1,0,0},
}, 3 };

/* D */
static const Glyph font_D = {
{
    {1,1,1,1,0},
    {1,0,0,1,0},
    {1,0,0,1,0},
    {1,0,0,1,0},
    {1,1,1,1,0},
}, 4 };

/* E */
static const Glyph font_E = {
{
    {1,1,1,0,0},
    {1,0,0,0,0},
    {1,1,1,0,0},
    {1,0,0,0,0},
    {1,1,1,0,0},
}, 3 };

/* F */
static const Glyph font_F = {
{
    {1,1,1,0,0},
    {1,0,0,0,0},
    {1,1,1,0,0},
    {1,0,0,0,0},
    {1,0,0,0,0},
}, 3 };

/* G */
static const Glyph font_G = {
{
    {0,1,1,1,0},
    {1,0,0,0,0},
    {1,0,1,1,0},
    {1,0,0,1,0},
    {0,1,1,1,0},
}, 4 };

/* H */
static const Glyph font_H = {
{
    {1,0,1,0,0},
    {1,0,1,0,0},
    {1,1,1,0,0},
    {1,0,1,0,0},
    {1,0,1,0,0},
}, 3 };

/* I */
static const Glyph font_I = {
{
    {1,1,1,0,0},
    {0,1,0,0,0},
    {0,1,0,0,0},
    {0,1,0,0,0},
    {1,1,1,0,0},
}, 3 };

/* J */
static const Glyph font_J = {
{
    {1,1,1,0,0},
    {0,0,1,0,0},
    {0,0,1,0,0},
    {1,0,1,0,0},
    {1,1,1,0,0},
}, 3 };

/* K */
static const Glyph font_K = {
{
    {1,0,0,1,0},
    {1,0,1,0,0},
    {1,1,0,0,0},
    {1,0,1,0,0},
    {1,0,0,1,0},
}, 4 };

/* L */
static const Glyph font_L = {
{
    {1,0,0,0,0},
    {1,0,0,0,0},
    {1,0,0,0,0},
    {1,0,0,0,0},
    {1,1,1,0,0},
}, 3 };

/* M */
static const Glyph font_M = {
{
    {1,0,0,0,1},
    {1,1,0,1,1},
    {1,0,1,0,1},
    {1,0,0,0,1},
    {1,0,0,0,1},
}, 5 };

/* N */
static const Glyph font_N = {
{
    {1,0,0,1,0},
    {1,1,0,1,0},
    {1,0,1,1,0},
    {1,0,0,1,0},
    {1,0,0,1,0},
}, 4 };

/* O */
static const Glyph font_O = {
{
    {1,1,1,0,0},
    {1,0,1,0,0},
    {1,0,1,0,0},
    {1,0,1,0,0},
    {1,1,1,0,0},
}, 3 };

/* P */
static const Glyph font_P = {
{
    {1,1,1,0,0},
    {1,0,1,0,0},
    {1,1,1,0,0},
    {1,0,0,0,0},
    {1,0,0,0,0},
}, 3 };

/* Q */
static const Glyph font_Q = {
{
    {1,1,1,1,0},
    {1,0,0,1,0},
    {1,0,1,1,0},
    {1,0,0,1,0},
    {1,1,1,1,0},
}, 4 };

/* R */
static const Glyph font_R = {
{
    {1,1,1,1,0},
    {1,0,0,1,0},
    {1,1,1,0,0},
    {1,0,1,0,0},
    {1,0,0,1,0},
}, 4 };

/* S */
static const Glyph font_S = {
{
    {1,1,1,0,0},
    {1,0,0,0,0},
    {1,1,1,0,0},
    {0,0,1,0,0},
    {1,1,1,0,0},
}, 3 };

/* T */
static const Glyph font_T = {
{
    {1,1,1,0,0},
    {0,1,0,0,0},
    {0,1,0,0,0},
    {0,1,0,0,0},
    {0,1,0,0,0},
}, 3 };

/* U */
static const Glyph font_U = {
{
    {1,0,1,0,0},
    {1,0,1,0,0},
    {1,0,1,0,0},
    {1,0,1,0,0},
    {1,1,1,0,0},
}, 3 };

/* V */
static const Glyph font_V = {
{
    {1,0,1,0,0},
    {1,0,1,0,0},
    {1,0,1,0,0},
    {1,0,1,0,0},
    {0,1,0,0,0},
}, 3 };

/* W */
static const Glyph font_W = {
{
    {1,0,0,0,1},
    {1,0,0,0,1},
    {1,0,1,0,1},
    {1,1,0,1,1},
    {1,0,0,0,1},
}, 5 };

/* X */
static const Glyph font_X = {
{
    {1,0,1,0,0},
    {1,0,1,0,0},
    {0,1,0,0,0},
    {1,0,1,0,0},
    {1,0,1,0,0},
}, 3 };

/* Y */
static const Glyph font_Y = {
{
    {1,0,1,0,0},
    {1,0,1,0,0},
    {0,1,0,0,0},
    {0,1,0,0,0},
    {0,1,0,0,0},
}, 3 };

/* Z */
static const Glyph font_Z = {
{
    {1,1,1,1,0},
    {0,0,0,1,0},
    {0,0,1,0,0},
    {0,1,0,0,0},
    {1,1,1,1,0},
}, 5 };

/* 0 */
static const Glyph font_0 = {
{
    {1,1,1,0,0},
    {1,0,1,0,0},
    {1,0,1,0,0},
    {1,0,1,0,0},
    {1,1,1,0,0},
}, 3 };

/* 1 */
static const Glyph font_1 = {
{
    {0,1,0,0,0},
    {1,1,0,0,0},
    {0,1,0,0,0},
    {0,1,0,0,0},
    {1,1,1,0,0},
}, 3 };

/* 2 */
static const Glyph font_2 = {
{
    {1,1,1,0,0},
    {0,0,1,0,0},
    {1,1,1,0,0},
    {1,0,0,0,0},
    {1,1,1,0,0},
}, 3 };

/* 3 */
static const Glyph font_3 = {
{
    {1,1,1,0,0},
    {0,0,1,0,0},
    {1,1,1,0,0},
    {0,0,1,0,0},
    {1,1,1,0,0},
}, 3 };

/* 4 */
static const Glyph font_4 = {
{
    {1,0,1,0,0},
    {1,0,1,0,0},
    {1,1,1,0,0},
    {0,0,1,0,0},
    {0,0,1,0,0},
}, 3 };

/* 5 */
static const Glyph font_5 = {
{
    {1,1,1,0,0},
    {1,0,0,0,0},
    {1,1,1,0,0},
    {0,0,1,0,0},
    {1,1,1,0,0},
}, 3 };

/* 6 */
static const Glyph font_6 = {
{
    {1,1,1,0,0},
    {1,0,0,0,0},
    {1,1,1,0,0},
    {1,0,1,0,0},
    {1,1,1,0,0},
}, 3 };

/* 7 */
static const Glyph font_7 = {
{
    {1,1,1,0,0},
    {0,0,1,0,0},
    {0,1,0,0,0},
    {0,1,0,0,0},
    {0,1,0,0,0},
}, 3 };

/* 8 */
static const Glyph font_8 = {
{
    {1,1,1,0,0},
    {1,0,1,0,0},
    {1,1,1,0,0},
    {1,0,1,0,0},
    {1,1,1,0,0},
}, 3 };

/* 9 */
static const Glyph font_9 = {
{
    {1,1,1,0,0},
    {1,0,1,0,0},
    {1,1,1,0,0},
    {0,0,1,0,0},
    {1,1,1,0,0},
}, 3 };

/* : */
static const Glyph font_colon = {
{
    {0,0,0,0,0},
    {1,0,0,0,0},
    {0,0,0,0,0},
    {1,0,0,0,0},
    {0,0,0,0,0},
}, 1 };

static const Glyph* font_map[128] = {0};
void font_init(void)
{
    font_map['A'] = &font_A;
    font_map['B'] = &font_B;
    font_map['C'] = &font_C;
    font_map['D'] = &font_D;

    font_map['E'] = &font_E;
    font_map['F'] = &font_F;
    font_map['G'] = &font_G;
    font_map['H'] = &font_H;
    font_map['I'] = &font_I;
    font_map['J'] = &font_J;
    font_map['K'] = &font_K;
    font_map['L'] = &font_L;
    font_map['M'] = &font_M;
    font_map['N'] = &font_N;
    font_map['O'] = &font_O;
    font_map['P'] = &font_P;
    font_map['Q'] = &font_Q;
    font_map['R'] = &font_R;
    font_map['S'] = &font_S;
    font_map['T'] = &font_T;
    font_map['U'] = &font_U;
    font_map['V'] = &font_V;
    font_map['W'] = &font_W;
    font_map['X'] = &font_X;
    font_map['Y'] = &font_Y;
    font_map['Z'] = &font_Z;

    font_map['0'] = &font_0;
    font_map['1'] = &font_1;
    font_map['2'] = &font_2;
    font_map['3'] = &font_3;
    font_map['4'] = &font_4;
    font_map['5'] = &font_5;
    font_map['6'] = &font_6;
    font_map['7'] = &font_7;
    font_map['8'] = &font_8;
    font_map['9'] = &font_9;

    font_map[':'] = &font_colon;
}

void draw_letter_fb(char letter, uint32_t x, uint32_t y, int size, struct multiboot_color *color, uint64_t fb_address, uint32_t fb_pitch) {
    const Glyph *glyph = font_map[letter];
    
    for (int i = 0; i < glyph->w; i++) {
        for (int j = 0; j < 5; j++) {
            if (glyph->data[j][i]) {
                draw_rect(x + i * size, y + j * size, size, size, color);
            }
        }
    }
}
#define draw_letter(letter, x, y, size, color) draw_letter_fb(letter, x, y, size, color, fb_address, fb_pitch)

void draw_text(char *text, uint32_t x, uint32_t y, int size, struct multiboot_color *color, uint64_t fb_address, uint32_t fb_pitch) {
    int offset = 0;

    for (int i = 0; text[i]; i++) {
        draw_letter(text[i], x + offset, y, size, color);

        // calculate offset for next letter
        offset += font_map[text[i]]->w * size + size;
    }
}
#define draw_text(text, x, y, size, color) draw_text(text, x, y, size, color, fb_address, fb_pitch)