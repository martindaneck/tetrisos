#include <stdint.h>

#define DATA_PORT 0x40
#define CONTROL_PORT 0x43

static inline unsigned char inb (unsigned short _port);
static inline void outb (unsigned short _port, unsigned char _data);


void set_timer() {
    outb(0x43, 0b00110100);

    uint16_t divisor = 1193182 / 60; // 60 Hz

    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor & 0xFF00) >> 8);
}

// both of these functions could probably be optimized but i got this black magic working and im not touching it

static inline uint16_t read_pit_counter() {
    outb(0x43, 0b00000000);
    uint8_t lo = inb(0x40);
    uint8_t hi = inb(0x40);

    return (hi << 8) | lo;
}


int read_timer() {
    static uint16_t last = 0xFFFF;

    uint16_t current = read_pit_counter();
    if (current > last) {
        last = current;
        return 1;
    }

    last = current;
    return 0;
}

