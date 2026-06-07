// reads CMOS for current timestamp to seed random number generator
#include <stdint.h>

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

unsigned char second;
unsigned char minute;
unsigned char hour;
unsigned char day; 
unsigned char month;

static inline unsigned char inb (unsigned short _port);
static inline void outb (unsigned short _port, unsigned char _data);

int get_update_in_progress_flag() {
    outb(CMOS_ADDRESS, 0x0A);
    return (inb(CMOS_DATA) & 0x80);
}

unsigned char get_RTC_register(int reg) {
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

uint64_t read_rtc() {
    // may be inconsistent, but doesn't matter for seeding
    second = get_RTC_register(0x00);
    minute = get_RTC_register(0x02);
    hour = get_RTC_register(0x04);
    day = get_RTC_register(0x07);
    month = get_RTC_register(0x08);

    return second + minute * 60 + hour * 3600 + day * 86400 + month * 2592000;
}

