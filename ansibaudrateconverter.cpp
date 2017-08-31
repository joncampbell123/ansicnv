
#include "ansibaudrateconverter.h"

ANSIBaudRateCounter::ANSIBaudRateCounter() : bitcount(0), baud(9600) {
    set_frame_format(8/*bits/byte*/,1/*parity*/,1/*stop bit*/);
}

void ANSIBaudRateCounter::set_frame_format(const unsigned char new_bits_per_byte,const unsigned char parity_bits,const unsigned char stop_bits) {
    bits_per_byte = 1/*frame start*/ + new_bits_per_byte + parity_bits + stop_bits;
}

void ANSIBaudRateCounter::reset_counter(void) {
    bitcount = 0;
}

double ANSIBaudRateCounter::current_data_time(void) const { /* in seconds */
    return (double)bitcount / baud;
}

void ANSIBaudRateCounter::count_byte(void) {
    bitcount += bits_per_byte;
}

void ANSIBaudRateCounter::count_bytes(const unsigned long count) {
    bitcount += bits_per_byte * count;
}

