
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include <string>

#include "wallclocktime.h"
#include "ansibaudrateconverter.h"
#include "ansicodeparser.h"

static void help(void) {
    fprintf(stderr,"dumpansicodes [options]\n");
    fprintf(stderr," -h --help              Show this help\n");
    fprintf(stderr," -i <file>              Take from file\n");
    fprintf(stderr," -b <rate>              Simulate baud rate\n");
}

int main(int argc,char **argv) {
    ANSIBaudRateCounter baudcounter;
    std::string input_file;
    ANSICodeParser parser;
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

    parser.reset();
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
    int col=0;

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
        parser.take_input(c);

        if (parser.code_ready) {
            if (parser.is_escape()) {
                if (col != 0) printf("\n");
                col = 0;
                printf("ANSI ESCAPE: ");
                for (size_t i=0;i < parser.code_length;i++) {
                    c = parser.code[i];

                    if (c == 27) printf("ESC ");
                    else printf("%c",c);
                }
                printf("\n");
            }
            else if (parser.is_control_sequence()) {
                if (col != 0) printf("\n");
                col = 0;
                printf("ANSI CONTROL SEQUENCE: ");
                for (size_t i=0;i < parser.code_length;i++) {
                    c = parser.code[i];

                    if (c == 27) printf("ESC ");
                    else printf("%c",c);
                }
                printf("\n");
            }
            else {
                col++;

                /* FIXME: How will we support UTF-8? */
                switch (parser.code[0]) {
                    case 0x01:  printf("SOH "); break;
                    case 0x02:  printf("STX "); break;
                    case 0x03:  printf("ETX "); break;
                    case 0x04:  printf("EOT "); break;
                    case 0x05:  printf("ENQ "); break;
                    case 0x06:  printf("ACK "); break;
                    case 0x07:  printf("BEL "); break;
                    case 0x08:  printf("BS  "); break;
                    case 0x09:  printf("HT  "); break;
                    case 0x0A:  printf("LF  "); break;
                    case 0x0B:  printf("VT  "); break;
                    case 0x0C:  printf("FF  "); break;
                    case 0x0D:  printf("CR  "); break;
                    case 0x0E:  printf("SO  "); break;
                    case 0x0F:  printf("SI  "); break;
                    case 0x10:  printf("DLE "); break;
                    case 0x11:  printf("DC1 "); break;
                    case 0x12:  printf("DC2 "); break;
                    case 0x13:  printf("DC3 "); break;
                    case 0x14:  printf("DC4 "); break;
                    case 0x15:  printf("NAK "); break;
                    case 0x16:  printf("SYN "); break;
                    case 0x17:  printf("ETB "); break;
                    case 0x18:  printf("CAN "); break;
                    case 0x19:  printf("EM  "); break;
                    case 0x1A:  printf("SUB "); break;
                    case 0x1B:  printf("ESC "); break;
                    case 0x1C:  printf("FS  "); break;
                    case 0x1D:  printf("GS  "); break;
                    case 0x1E:  printf("RS  "); break;
                    case 0x1F:  printf("US  "); break;
                    default:    printf("%c",parser.code[0]); break;
                }
            }

            fflush(stdout);
        }
    }

    close(fd);
    return 0;
}

