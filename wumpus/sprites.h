#ifndef _WUMPUS_SPRITES_
#define _WUMPUS_SPRITES_

unsigned char bat_1[]   = "\x1E\x38\x6F\x3E\x3E\x6F\x38\x1E";
unsigned char bat_2[]   = "\xF0\x38\x6F\x3E\x3E\x6F\x38\xF0";

unsigned char carry_1[] = "\x80\xC0\x00\x00\x00\x00\x00\x00";
unsigned char carry_2[] = "\x00\x40\x60\x00\x00\x00\x00\x00";
unsigned char carry_3[] = "\x00\x00\x20\x30\x00\x00\x00\x00";
unsigned char carry_4[] = "\x00\x00\x00\x18\x10\x00\x00\x00";
unsigned char carry_5[] = "\x00\x00\x00\x00\x14\x10\x00\x00";
unsigned char carry_6[] = "\x00\x00\x00\x00\x02\x10\x10\x00";
unsigned char carry_7[] = "\x00\x00\x00\x00\x01\x00\x20\x20";
unsigned char carry_8[] = "\x00\x00\x00\x00\x01\x00\x00\x40";
unsigned char carry_9[] = "\x00\x00\x00\x00\x01\x00\x00\x00";

unsigned char fall_1[]    = "\x02\x66\x14\xFD\xFE\x38\x40\x80";
unsigned char fall_2[]    = "\x00\x02\x1C\x7F\xBF\x28\x66\x00";
unsigned char fall_3[]    = "\x18\x5A\x3A\x3C\x38\x1E\x12\x08";
unsigned char fall_4[]    = "\x00\x22\x12\x7C\x38\x00\x00\x00";
unsigned char fall_5[]    = "\x00\x60\x10\x18\x38\x58\x10\x00";
unsigned char fall_6[]    = "\x00\x00\x00\x1C\x3E\x48\x44\x00";
unsigned char fall_7[]    = "\x00\x00\x08\x0C\x08\x04\x00\x00";
unsigned char fall_8[]    = "\x00\x00\x14\x38\x00\x00\x00\x00";
unsigned char fall_9[]    = "\x00\x00\x20\x10\x30\x10\x00\x00";
unsigned char fall_10[]   = "\x00\x00\x00\x00\x1C\x08\x00\x00";
unsigned char fall_11[]   = "\x00\x00\x08\x0C\x08\x00\x00\x00";
unsigned char fall_12[]   = "\x00\x00\x10\x38\x00\x00\x00\x00";
unsigned char fall_13[]   = "\x00\x00\x10\x30\x10\x00\x00\x00";
unsigned char fall_14[]   = "\x00\x00\x10\x08\x10\x00\x00\x00";
unsigned char fall_15[]   = "\x00\x00\x00\x10\x08\x00\x00\x00";
unsigned char fall_16[]   = "\x00\x00\x00\x10\x00\x00\x00\x00";
unsigned char fall_17[]   = "\x00\x00\x00\x00\x00\x00\x00\x00";

unsigned char grave[]     = "\x01\x03\x7F\x93\xBF\x93\x7F\x03";

unsigned char wumpus_1[]  = "\x1A\x79\x53\xF9\xF3\x59\x73\x1A";
unsigned char wumpus_2[]  = "\x1C\x7A\xD6\x7A\x76\xDA\x76\x1C";
unsigned char wumpus_3[]  = "\x0E\x3D\x2B\x7D\x7B\x2D\x3B\x0E";
unsigned char wumpus_4[]  = "\x07\x1F\x15\x3F\x3D\x17\x1D\x07";
unsigned char wumpus_5[]  = "\x03\x0F\x0B\x1F\x1F\x0B\x0F\x03";
unsigned char wumpus_6[]  = "\x07\x03\x03\x07\x07\x03\x03\x07";
unsigned char wumpus_7[]  = "\x0F\x01\x01\x03\x03\x01\x01\x0F";
unsigned char wumpus_8[]  = "\x0F\x01\x01\x01\x01\x01\x01\x0F";
unsigned char wumpus_9[]  = "\x07\x01\x01\x01\x01\x01\x01\x07";
unsigned char wumpus_10[] = "\x01\x01\x01\x01\x01\x01\x01\x01";

unsigned char trophy[]    = "\x30\xE8\x9D\x87\x87\x9D\xE8\x30";

unsigned char bow_1[]     = "\x00\x00\x00\xFF\x52\x3C\x10\x00";
unsigned char bow_2[]     = "\x00\x18\x76\x91\x52\x3C\x10\x00";
unsigned char bow_3[]     = "\x18\x34\x52\x91\x52\x3C\x00\x00";
unsigned char bow_4[]     = "\x00\x00\x00\xFF\x42\x3C\x10\x10";
unsigned char bow_5[]     = "\x00\x00\x00\xFF\x42\x3C\x00\x00";

unsigned char begin_1[]   = "\x03\x00\x00\x1F\x20\x20\x20\x1F";
unsigned char begin_2[]   = "\x00\x03\x00\x1F\x20\x20\x20\x1F";
unsigned char begin_3[]   = "\x00\x00\x03\x1F\x20\x20\x20\x1F";
unsigned char begin_4[]   = "\x00\x00\x00\x1F\x20\x20\x20\x1F";
unsigned char begin_5[]   = "\x00\x00\x00\x1F\x23\x20\x20\x1F";
unsigned char begin_6[]   = "\x00\x00\x00\x1F\x20\x23\x20\x1F";
unsigned char begin_7[]   = "\x00\x00\x00\x1F\x20\x23\x20\x1F";

unsigned char CHARSET[128][6] = {
    "\x00\x00\x00",              // space - Ascii 32
    "\xfa\x00",                  // !
    "\xc0\x00\xc0\x00",          // "
    "\x24\x7e\x24\x7e\x24\x00",  // #
    "\x24\xd4\x56\x48\x00",      // $
    "\xc6\xc8\x10\x26\xc6\x00",  // %
    "\x6c\x92\x6a\x04\x0a\x00",  // &
    "\xc0\x00",                  // '
    "\x7c\x82\x00",              // (
    "\x82\x7c\x00",              // )
    "\x10\x7c\x38\x7c\x10\x00",  // *
    "\x10\x10\x7c\x10\x10\x00",  // +
    "\x06\x07\x00",              // ,
    "\x10\x10\x10\x10\x00",      // -
    "\x06\x06\x00",              // .
    "\x04\x08\x10\x20\x40\x00",  // /
    "\x7c\x8a\x92\xa2\x7c\x00",  // 0 - Ascii 48
    "\x42\xfe\x02\x00",          // 1
    "\x46\x8a\x92\x92\x62\x00",  // 2
    "\x44\x92\x92\x92\x6c\x00",  // 3
    "\x18\x28\x48\xfe\x08\x00",  // 4
    "\xf4\x92\x92\x92\x8c\x00",  // 5
    "\x3c\x52\x92\x92\x8c\x00",  // 6
    "\x80\x8e\x90\xa0\xc0\x00",  // 7
    "\x6c\x92\x92\x92\x6c\x00",  // 8
    "\x60\x92\x92\x94\x78\x00",  // 9
    "\x36\x36\x00",              // : - Ascii 58
    "\x36\x37\x00",              //
    "\x10\x28\x44\x82\x00",      // <
    "\x24\x24\x24\x24\x24\x00",  // =
    "\x82\x44\x28\x10\x00",      // >
    "\x60\x80\x9a\x90\x60\x00",  // ?
    "\x7c\x82\xba\xaa\x78\x00",  // @
    "\x7e\x90\x90\x90\x7e\x00",  // A - Ascii 65
    "\xfe\x92\x92\x92\x6c\x00",  // B
    "\x7c\x82\x82\x82\x44\x00",  // C
    "\xfe\x82\x82\x82\x7c\x00",  // D
    "\xfe\x92\x92\x92\x82\x00",  // E
    "\xfe\x90\x90\x90\x80\x00",  // F
    "\x7c\x82\x92\x92\x5c\x00",  // G
    "\xfe\x10\x10\x10\xfe\x00",  // H
    "\x82\xfe\x82\x00",          // I
    "\x0c\x02\x02\x02\xfc\x00",  // J
    "\xfe\x10\x28\x44\x82\x00",  // K
    "\xfe\x02\x02\x02\x00",      // L
    "\xfe\x40\x20\x40\xfe\x00",  // M
    "\xfe\x40\x20\x10\xfe\x00",  // N
    "\x7c\x82\x82\x82\x7c\x00",  // O
    "\xfe\x90\x90\x90\x60\x00",  // P
    "\x7c\x82\x92\x8c\x7a\x00",  // Q
    "\xfe\x90\x90\x98\x66\x00",  // R
    "\x64\x92\x92\x92\x4c\x00",  // S
    "\x80\x80\xfe\x80\x80\x00",  // T
    "\xfc\x02\x02\x02\xfc\x00",  // U
    "\xf8\x04\x02\x04\xf8\x00",  // V
    "\xfc\x02\x3c\x02\xfc\x00",  // W
    "\xc6\x28\x10\x28\xc6\x00",  // X
    "\xe0\x10\x0e\x10\xe0\x00",  // Y
    "\x86\x8a\x92\xa2\xc2\x00",  // Z - Ascii 90
    "\xfe\x82\x82\x00",          // [
    "\x40\x20\x10\x08\x04\x00",  // forward slash
    "\x82\x82\xfe\x00",          // ]
    "\x20\x40\x80\x40\x20\x00",  // ^
    "\x02\x02\x02\x02\x02\x00",  // _
    "\xc0\xe0\x00",              // '
    "\x04\x2a\x2a\x1e\x00",      // a - Ascii 97
    "\xfe\x22\x22\x1c\x00",      // b
    "\x1c\x22\x22\x22\x00",      // c
    "\x1c\x22\x22\xfc\x00",      // d
    "\x1c\x2a\x2a\x10\x00",      // e
    "\x10\x7e\x90\x80\x00",      // f
    "\x18\x25\x25\x3e\x00",      // g
    "\xfe\x20\x20\x1e\x00",      // h
    "\xbc\x02\x00",              // i
    "\x02\x01\x21\xbe\x00",      // j
    "\xfe\x08\x14\x22\x00",      // k
    "\xfc\x02\x00",              // l
    "\x3e\x20\x18\x20\x1e\x00",  // m
    "\x3e\x20\x20 \x1e\x00",     // n
    "\x1c\x22\x22\x1c\x00",      // o
    "\x3f\x22\x22\x1c\x00",      // p
    "\x1c\x22\x22\x3f\x00",      // q
    "\x22\x1e\x20\x10\x00",      // r
    "\x12\x2a\x2a\x04\x00",      // s
    "\x20\x7c\x22\x04\x00",      // t
    "\x3c\x02\x02\x3e\x00",      // u
    "\x38\x04\x02\x04\x38\x00",  // v
    "\x3c\x06\x0c\x06\x3c\x00",  // w
    "\x22\x14\x08\x14\x22\x00",  // x
    "\x39\x05\x06\x3c\x00",      // y
    "\x26\x2a\x2a\x32\x00",      // z - Ascii 122
    "\x10\x7c\x82\x82\x00",      //
    "\xee\x00",                  // |
    "\x82\x82\x7c\x10\x00",      //
    "\x40\x80\x40\x80\x00",      // ~
    "\x60\x90\x90\x60\x00"       // Degrees sign - Ascii 127
};


#endif  // _WUMPUS_SPRITES_
