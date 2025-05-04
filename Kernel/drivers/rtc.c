#include <rtc.h>

extern uint8_t rtc(uint8_t mode);

static uint8_t bcd_decimal(uint8_t BCD) {
    return (BCD >> 4) * 10 + (BCD & 0x0F);
}

static uint8_t getRTC(uint8_t mode) {
    return bcd_decimal(rtc(mode));
}

uint8_t getRTCSeconds() {
    return getRTC(RTC_SECONDS);
}

uint8_t getRTCMinutes() {
    return getRTC(RTC_MINUTES);
}

uint8_t getRTCHours() {
    return getRTC(RTC_HOURS);
}

uint8_t getRTCDayOfWeek() {
    return getRTC(RTC_DAY_OF_WEEK);
}

uint8_t getRTCDayOfMonth() {
    return getRTC(RTC_DAY_OF_MONTH);
}

uint8_t getRTCMonth() {
    return getRTC(RTC_MONTH);
}

uint8_t getRTCYear() {
    return getRTC(RTC_YEAR);
}