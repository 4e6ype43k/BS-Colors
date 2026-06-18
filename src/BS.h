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
//* BS (Byte Size) format is similar to that of CIMG, however, every pixel is stored in 1 byte
//* The bits in the byte represent the following: RRGGGBBA (fun fact: human eyes are the most sensitive to green, so that's the reason why the green channel appears here the most)
//? (yes i know that introduction of the alpha channel halves the amount of colors that could be represented but trust me, its wort it)

#ifndef BITE_SIZE
#define BITE_SIZE

#include <stdio.h>
#include <stdint.h>

typedef int8_t BS_color; // it is recommended to use this type instead of any other because i said so

typedef struct BS_PD {
    uint16_t width, height;
    BS_color* data; // this pointer is 8 times larger than just one pixel... glup..
} BS_PD;

#endif