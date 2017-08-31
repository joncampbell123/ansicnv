
// either a character code, control code, or an escape sequence
class ANSICodeParser {
public:
    enum escstate {
        ESC_NONE,
        ESC_ESCAPE,
        ESC_CONTROL_PARAM,
        ESC_CONTROL
    };
public:
                                ANSICodeParser();
public:
    unsigned char               code[64];
    size_t                      code_length;
    bool                        code_ready;
    enum escstate               escape;
public:
    bool is_escape(void) const;
    bool is_control_sequence(void) const;
    void reset_code(void);
    void reset(void);
    void take_input(const unsigned char b);
    void debug(const char *fmt,...);
};

