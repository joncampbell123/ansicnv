
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

#include "ansicodeparser.h"

ANSICodeParser::ANSICodeParser() {
    reset();
}

bool ANSICodeParser::is_escape(void) const {
    return code_length >= 1 && code[0] == 033;
}

bool ANSICodeParser::is_control_sequence(void) const {
    return code_length >= 2 && code[0] == 033 && code[1] == 0133;
}

void ANSICodeParser::reset_code(void) {
    code_ready = false;
    code_length = 0;
}

void ANSICodeParser::reset(void) {
    reset_code();
    escape = ESC_NONE;
}

void ANSICodeParser::take_input(const unsigned char b) {
    code_ready = false;

    if (b == 0/*NUL*/) return/*ignored*/;

    if (b == 033/*ESC*/) {
        if (escape != ESC_NONE)
            debug("ESC while processing ESC. Interrupted escape ignored.");

        escape = ESC_ESCAPE;
        code_length = 1;
        code[0] = b;
    }
    else if (escape == ESC_ESCAPE) { /* ESC ... */
        if (b == 0133/*[*/ && code[0] == 033/*ESC*/ && code_length == 1/*must follow 033 ESC*/) {
            assert(code_length < (sizeof(code)-1));
            code[code_length++] = b;
            escape = ESC_CONTROL_PARAM;
        }
        else if (b >= 040 && b <= 0176) {/* 040-057 intermediate, 060-176 final */
            if (code_length < (sizeof(code)-1)) {
                code[code_length++] = b;

                if (b >= 060) { /* final code */
                    escape = ESC_NONE;
                    code_ready = true;
                }
            }
            else {
                debug("ESC code too long, returning truncated code to host program");
                code_ready = true;
                escape = ESC_NONE;
            }
        }
        else {
            debug("ESC followed by neither intermediate nor final");
            code_ready = true;
            escape = ESC_NONE;
        }
    }
    else if (escape == ESC_CONTROL || escape == ESC_CONTROL_PARAM) { /* ESC [ ... */
        if (b >= 060 && b <= 077) {
            if (escape == ESC_CONTROL_PARAM) {
                if (code_length < (sizeof(code)-1)) {
                    code[code_length++] = b;
                }
                else {
                    debug("ESC [ code too long, returning truncated code to host program");
                    code_ready = true;
                    escape = ESC_NONE;
                }
            }
            else {
                debug("ESC [ parameter following intermediate or final");
                code_ready = true;
                escape = ESC_NONE;
            }
        }
        else if (b >= 040 && b <= 057) {/*intermediate*/
            escape = ESC_CONTROL;
            if (code_length < (sizeof(code)-1)) {
                code[code_length++] = b;
            }
            else {
                debug("ESC [ code too long, returning truncated code to host program");
                code_ready = true;
                escape = ESC_NONE;
            }
        }
        else if (b >= 0100 && b <= 0176) {/*final*/
            if (code_length < (sizeof(code)-1))
                code[code_length++] = b;
            else
                debug("ESC code too long, returning truncated code to host program");

            code_ready = true;
            escape = ESC_NONE;
        }
        else {
            debug("ESC [ followed by neither parameter, intermediate nor final");
            code_ready = true;
            escape = ESC_NONE;
        }
    }
    else {
        code_ready = true;
        code_length = 1;
        code[0] = b;
    }
}

void ANSICodeParser::debug(const char *fmt,...) {
    va_list va;

    va_start(va,fmt);
    fprintf(stderr,"ANSI debug: ");
    vfprintf(stderr,fmt,va);
    va_end(va);
}

