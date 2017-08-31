
class ANSIBaudRateCounter {
public:
                                ANSIBaudRateCounter();
public:
    unsigned long               bitcount;
    unsigned long               baud;
    unsigned int                bits_per_byte;
public:
    void set_frame_format(const unsigned char new_bits_per_byte,const unsigned char parity_bits,const unsigned char stop_bits);
    void reset_counter(void);
    double current_data_time(void) const;
    void count_byte(void);
    void count_bytes(const unsigned long count);
};

