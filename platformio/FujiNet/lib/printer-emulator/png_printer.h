#ifndef PNG_PRINTER_H
#define PNG_PRINTER_H
#include <Arduino.h>
//#include <stdio.h>
//#include <stdint.h>

#include "printer_emulator.h"

#define DEFLATE_MAX_BLOCK_SIZE 0xFFFF

class pngPrinter : public printer_emu
{
    // complete rewrite of TinyPngOut https://www.nayuki.io/page/tiny-png-output
protected:
    const uint32_t width = 320;
    const uint32_t height = 192;

    uint32_t imgSize = 0;     // size of image including BOL filter p's for IDAT chunk
    uint32_t img_pos = 0;     // serial position within image data including BOL filter p's
    uint16_t Xpos = 0;        // current position within image line
    uint16_t Ypos = 0;        // current image line number
    uint32_t dataSize = 0;    // size of data for IDAT chunk
    uint16_t blkSize = 0;     // size of zlib block
    uint16_t blk_pos = 0;     // serial position within zlib block
    uint32_t crc_value = 0;   // running crc32 value
    uint32_t adler_value = 1; // running checksum (initilize to 1 https://en.wikipedia.org/wiki/Adler-32)

    uint8_t line_buffer[320];

    void uint32_to_array(uint32_t src, uint8_t dest[4]);
    uint32_t update_adler32(uint32_t adler, uint8_t data);
    uint32_t rc_crc32(uint32_t crc, const uint8_t *buf, size_t len);
    uint32_t rc_crc32(uint32_t crc, uint8_t c) { rc_crc32(crc, &c, 1); }

    void png_signature();
    void png_header();
    void png_palette();
    void png_data();
    void png_add_data(uint8_t *buf, uint32_t n);
    void png_end();

public:
    pngPrinter(paper_t ty = PNG) : printer_emu{ty} {};
    virtual void initPrinter(FS *filesystem);
    virtual void pageEject();
    virtual bool process(byte n);
};

#endif
