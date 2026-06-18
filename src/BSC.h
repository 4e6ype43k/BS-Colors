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
//* BSC (Byte-Size Color) format is similar to that of CIMG, however, every pixel is stored in 1 byte
//* The bits in the byte represent the following: RRGGGBBA (fun fact: human eyes are the most sensitive to green, so that's the reason why the green channel appears here the most)
//? (yes i know that introduction of the alpha channel halves the amount of colors that could be represented but trust me, its wort it)

#ifndef BITE_SIZE_COLOR
#define BITE_SIZE_COLOR

#include <stdio.h>
#include <stdint.h>
#include <malloc.h>

typedef uint8_t BSC_color; // it is recommended to use this type instead of any other because i said so

typedef struct BSC_PD {
    uint16_t width, height;
    BSC_color* pixels; // this pointer is 8 times larger than just one pixel... glup..
} BSC_PD; // stands for Byte-Size Color Pixel Data

void allocBytePixelMemory(BSC_PD* data) {
    data->pixels=(BSC_color*) calloc(data->width*data->height,1);
} // same reason for calloc as in CIMG

void freeBytePixelMemory(BSC_PD* data) {
    free(data->pixels);
}

// allows printing colors as normal hex....
void printColor(BSC_color color) {
    int8_t r,g,b; // a has only 2 states
    r=color>>6;
    g=(color>>3)-(r<<3);
    b=(color>>1)-(r<<5)-(g<<2);
    printf("0x%02x%02x%02x%02x",r*85,g*36,b*85,(((uint8_t) (color<<7))>>7)*255);
}

#endif