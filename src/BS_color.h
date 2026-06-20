/*

Copyright (c) 2026 4e6ype43k

Permission is hereby granted, free of charge,
to any person obtaining a copy of this software
and associated documentation files (the "Software"),
to deal in the Software without restriction,
including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to
whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice
shall be included in all copies or substantial portions
of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.

*/

//? instead of using a whole byte for each color channel, what if we used a whole byte for ALL the channels?
//* BS (Byte-Size) Color format is similar to that of CIMG, however, every pixel is stored in 1 byte
//* The bits in the byte represent the following: RRRGGGBB (fun fact: human eyes are the least sensitive to blue, so that's the reason why there is the least of it)

#ifndef BITE_SIZE_COLOR
#define BITE_SIZE_COLOR

#include <stdio.h>
#include <stdint.h>
#include <malloc.h>

#ifndef INVISIBLE_COLOR
#define INVISIBLE_COLOR (BS_color) {0b11100101} // this kind of acts like any color with alpha channel value 0
#endif

typedef uint8_t BS_color; // it is recommended to use this type instead of any other because i said so

typedef struct BSC_PD {
    uint16_t width, height;
    BS_color* pixels; // this pointer is 8 times larger than just one pixel... glup..
} BSC_PD; // stands for Byte-Size Color Pixel Data

void allocBytePixelMemory(BSC_PD* data) {
    data->pixels=(BS_color*) calloc(data->width*data->height,1);
} // same reason for calloc as in CIMG

void freeBytePixelMemory(BSC_PD* data) {
    free(data->pixels);
}

// for now, only converts clr to ARGB 4-byte int
int32_t BS_colorToInt(BS_color clr){
    if (clr==INVISIBLE_COLOR) return 0; // == is good enough for comparison
    uint8_t r,g,b; // alpha channel is constant, remember?
    r=clr>>5;
    g=(clr>>2)-(r<<3);
    b=clr-(r<<5)-(g<<2);

    if (r*36==252) r=255;
    else r*=36;
    if (g*36==252) g=255;
    else g*=36;

    return (r<<24)+(g<<16)+(b<<8)*85+255; // uhhh
}

// allows printing colors as normal hex....
void printBS_color(BS_color clr) {    int32_t a=BS_colorToInt(clr);
    if (a==0) { // if clr==that color, the func will return 0
        printf("0x00000000\n");
        return;
    }
    uint8_t r,g,b;
    r=a>>24;
    g=a>>16;
    b=a>>8;
    printf("0x%02x%02x%02xff",r,g,b); // alpha channel will always be FF but for one case
}

#endif