
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include <string>

double wallclocktime(void) {
    struct timeval tv;

    gettimeofday(&tv,NULL);

    return (double)tv.tv_sec + ((double)tv.tv_usec / 1000000);
}

class ANSIBaudRateCounter {
public:
    ANSIBaudRateCounter() : bitcount(0), baud(9600) { set_frame_format(8/*bits/byte*/,1/*parity*/,1/*stop bit*/); }
public:
    unsigned long               bitcount;
    unsigned long               baud;
    unsigned int                bits_per_byte;
public:
    void set_frame_format(const unsigned char new_bits_per_byte,const unsigned char parity_bits,const unsigned char stop_bits) {
        bits_per_byte = 1/*frame start*/ + new_bits_per_byte + parity_bits + stop_bits;
    }
    void reset_counter(void) {
        bitcount = 0;
    }
    double current_data_time(void) const { /* in seconds */
        return (double)bitcount / baud;
    }
    void count_byte(void) {
        bitcount += bits_per_byte;
    }
    void count_bytes(const unsigned long count) {
        bitcount += bits_per_byte * count;
    }
};

static void help(void) {
    fprintf(stderr,"dumpansicodes [options]\n");
    fprintf(stderr," -h --help              Show this help\n");
    fprintf(stderr," -i <file>              Take from file\n");
    fprintf(stderr," -b <rate>              Simulate baud rate\n");
}

int main(int argc,char **argv) {
    ANSIBaudRateCounter baudcounter;
    std::string input_file;
    unsigned char c;
    int fd;
    int i;

    for (i=1;i < argc;) {
        const char *a = argv[i++];

        if (*a == '-') {
            do { a++; } while (*a == '-');

            if (!strcmp(a,"h") || !strcmp(a,"help")) {
                help();
                return 1;
            }
            else if (!strcmp(a,"b")) {
                a = argv[i++];
                if (a == NULL) return 1;

                unsigned long b = strtoul(a,NULL,0);
                if (b < 1 || b > 115200) return 1;

                baudcounter.baud = (unsigned long)b;
            }
            else if (!strcmp(a,"i")) {
                a = argv[i++];
                if (a == NULL) return 1;
                input_file = a;
            }
            else {
                fprintf(stderr,"Unknown switch %s\n",a);
                return 1;
            }
        }
        else {
            fprintf(stderr,"Unexpected arg %s\n",a);
            return 1;
        }
    }

    baudcounter.reset_counter();
    if (input_file.empty()) {
        fprintf(stderr,"Need to specify file, -i switch\n");
        return 1;
    }

    if ((fd=open(input_file.c_str(),O_RDONLY|O_NOCTTY)) < 0) {
        fprintf(stderr,"Cannot open '%s', %s\n",input_file.c_str(),strerror(errno));
        return 1;
    }

    double start_t = wallclocktime();

    while (read(fd,&c,1) == 1) {
        double baud_t = baudcounter.current_data_time();
        double diff_t;

        while (1) {
            diff_t = baud_t - (wallclocktime() - start_t);
            if (diff_t <= 0) break;
            if (diff_t > 1) diff_t = 1;
            usleep((unsigned int)(diff_t * 1000000));
        }

        baudcounter.count_byte();

        // TODO
    }

    close(fd);
    return 0;
}

