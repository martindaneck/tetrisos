#include <stdint.h>

#define KBC_STATUS 0x64
#define KBC_EA 0x60

uint8_t kbdap_last_scancode = 0;
int kbdap_counter = 0;
char prev_keypress = 0;
char keypress = 0;

static inline unsigned char inportb (unsigned short _port) {
    unsigned char rv;
    asm volatile ("in %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

static inline void outportb (unsigned short _port, unsigned char _data) {
    asm volatile ("out %0, %1" : : "a" (_data), "dN" (_port));
}


void delay() { // temporary delay function until i do PIT or rdtsc
    for (int i = 0; i < 100000; i++) { // arbitrary
        asm volatile ("nop");
    }
}

void kbdap_send_command(uint8_t command) {
    while (inportb(KBC_STATUS) & 0x02); // wait for the buffer to be ready
    outportb(KBC_EA, command);
}

uint32_t kbdap_get_scancode() {
    static unsigned e0_code = 0;
    static unsigned e1_code = 0;
    static unsigned e1_prev = 0;
    uint8_t scancode = 0;
    if (inportb(KBC_STATUS) & 1) { // a scancode is available in the buffer
        scancode = inportb(KBC_EA);
        if (e0_code == 1) { // scancode is an e0 code
            e0_code = 0;
            return (0xe000 | scancode);
            } else if (e1_code == 1) { // scancode is first byte of e1 code
            e1_prev = scancode;
            e1_code = 2;
            } else  if (e1_code == 2) { // scancode is second byte of e1 code (first is in e1_prev)
            e1_code = 0;
            return (0xe10000 | e1_prev << 8 | scancode);
            } else if (scancode == 0xe0) {
            e0_code = 1;
            scancode = 0;
            } else if (scancode == 0xe1) {
            e1_code = 1;
            scancode = 0;
        }
    }
    return scancode;
}


void kbdap_init() {
    // empty keyboard buffer
    while (inportb(KBC_STATUS) & 0x01) {
        inportb(KBC_EA);
    }
    // activate keyboard
    kbdap_send_command(0xF4);
    while (inportb(KBC_STATUS) & 0x01) {
        inportb(KBC_EA);
    }
    // self test
    kbdap_send_command(0xEE);
}

void kbdap_loop() {
    uint32_t scancode = kbdap_get_scancode();
    if (scancode != 0) {
        kbdap_last_scancode = scancode;
    }
    delay();
}

char kbd_US [128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', /* <-- Tab */
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, /* <-- control key */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

char get_keypress() {
    char keypress = kbd_US[kbdap_last_scancode];
    int threshold;

    if (keypress == 's'){
        threshold = 1000; // arbitrary numbers, adjust w timer later
    } else {
        threshold = 3200;
    }

    if (prev_keypress != keypress) { // change of keypress - emit keypress
        prev_keypress = keypress;
        kbdap_counter = 0;
        return keypress;
    }  
    
    // else counter++
    kbdap_counter++;
    if (kbdap_counter > threshold * 10) { // reset counter to avoid overflow
        kbdap_counter = threshold;
    }


    if ((kbdap_counter % threshold == 0) && keypress == 's') { // s is special, holding down should accelerate, return early
        kbdap_counter = 0;
        return keypress;
    } 

    // holding -> 3 emits slowly, after that quickly
    else if ( ((kbdap_counter % threshold) == 0) || (kbdap_counter >= (threshold * 3) && (kbdap_counter % (threshold / 2)) == 0) ) {
        return keypress;
    }
    
    return 0;  
}