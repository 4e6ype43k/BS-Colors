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

#ifdef BSC_EXTRA_MACROS
#define MAX_BYTE_COLORS 256 // max amount of colors that could fit in a byte
#define MAX_DIMENSION_SIZE 65535 // max size of one dimension in a PD
#define MAX_PIXEL_COUNT 4294836225 // max amount of pixels 1 PD can store
#define RED_MAX_VALUE 7 // max value each color channel can have
#define BLUE_MAX_VALUE 7
#define GREEN_MAX_VALUE 3
#endif

typedef uint8_t BS_color; // it is recommended to use this type instead of any other because i said so

typedef struct PD {
    uint16_t width, height;
    BS_color* pixels; // this pointer is 8 times larger than just one pixel... glup..
} PD; // stands for Byte-Size Color Pixel Data

void allocPixelMemory(PD* data) {
    data->pixels=(BS_color*) calloc(data->width*data->height,1);
} // same reason for calloc as in CIMG

void freePixelMemory(PD* data) {
    free(data->pixels);
}

#ifndef BSC_LUT // if defined, uses a LUT for the values (making the program run faster ig)
// for now, only converts clr to ARGB 4-byte int
int32_t BS_colorToInt(BS_color clr){
    if (clr==INVISIBLE_COLOR) return 0; // == is good enough for comparison
    uint8_t r,g,b; // alpha channel is constant, remember?
    r=clr>>5; // last 5 bits are for g and b
    g=(clr>>2)&7; // 7 is 0b111, anding the shifted color with it will only give bits where 7 bits are high, ignoring all other bits after
    b=clr&3; // 3 is 0b11

    uint8_t four; // i have a long explanation to this part but it is too long to be written
		  // TODO actually explain all this horrid code
		  // but for now, enjoy the magic
	four=r<<2;
	r<<=5;
	r|=four;

	four=g<<2;
	g<<=5;
	g|=four;

	if (r==252) r=255; // translation error checking (turns out 255%7!=0 (this gives a very vague description of what just happened))
	if (g==252) g=255;

	// no more crazy bitwise logic
    return ((r<<24)|(g<<16)|((b<<8)*85))|255; // oring the numbers rather than adding as the byte the channels are writing to are GUARANTEED to be 0
}
#else

uint8_t rgLUT={0,36,72,108,144,180,216,255}; // the amount of bits r and g channels occupy are the same, so creating two different LUTs for both would be wasteful (im bad at explaining stuff, ok?)
uint8_t bLUT={0,85,170,255};

int32_t BS_colorToInt(BS_color clr) {
	if (clr==INVISIBLE_COLOR) return 0;
	uint8_t r,g,b;
	r=clr>>5; // same magic as before
	g=(clr>>2)&7;
	b=clr&3;
	return ((rgLUT[r]<<24)|(rgLUT[g]<<16)|(bLUT[b]<<8))|255; // same thing with or but now with LUTs
}

// adds two colors (literally just adds the channel values and if they overflow, just sets them to the max values)
BS_color addBS_colors(BS_color clr0, BS_color clr1) {
	if (clr0==INVISIBLE_COLOR) return clr1; // idk
	if (clr1==INVISIBLE_COLOR) return clr0; // idk 2: electric boogaloo

	uint8_t r0,g0,b0; // for clr0
	r0=clr0>>5;
	g0=(clr0>>2)&7;
	b0=clr0&3;

	uint8_t r1,g1,b1; // clr1
	r1=clr1>>5;
	g1=(clr1>>2)&7;
	b1=clr1&3;

	uint8_t r,g,b; // for the output color
	r=r0+r1;
	g=g0+g1;
	b=b0+b1;

	if (r>7) r=7; // i dont have any bitwise tricks for this... so sad.. .
	if (g>7) g=7;
	if (b>3) b=3;

	return (r<<5)|(g<<2)|b; // but i have some for this!
}

#endif

// allows printing colors as 4 byte (or 8 nybble (or nibble)) hex....
void printBS_color(BS_color clr) {
    int32_t a=BS_colorToInt(clr);
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

// loads a file to a new PD
PD loadFile(int8_t* path) {
	FILE* pFile=fopen(path,"rb");
	uint8_t sizeData[4];
	fread(sizeData,1,4,pFile);
	
	uint16_t width=(sizeData[0]<<8)|sizeData[1];
	uint16_t height=(sizeData[2]<<8)|sizeData[3];
	BSC_PD out={width,height}; // no need to specify NULL for pixels as does it for me (i love c)
	allocBytePixelMemory(&out);
	
	int16_t chr=1; // a
	for (uint32_t x=0;;x++) { // for is apparently faster than while
		chr=fgetc(pFile);
		if (chr==EOF) break;
		out.pixels[x]=chr;
	}

	fclose(pFile);
	return out;
}

// loads file into an existant PD and returns bytes required to store all the pixels
int32_t loadFileToPD(PD* data,int8_t* path){
	FILE* pFile=fopen(path,"rb");
	uint8_t sizeData[4];
	fread(sizeData,1,4,pFile);
	
	uint16_t width=(sizeData[0]<<8)|sizeData[1]; // wait a second, i can do this here too!
	uint16_t height=(sizeData[2]<<8)|sizeData[3];
	data->width=width;
	data->height=height;

	int16_t chr=40; // this is, in fact, a random number
	for (uint32_t x=0;;x++) {
		chr=fgetc(pFile); // fread offsets the char* in FILE
		if (chr==EOF) break;
		data->pixels[x]=chr;
	}

	fclose(pFile);
	return width*height;
}

// stores data in path
void encodePD(PD* data,int8_t* path){
	FILE* pFile=fopen(path,"wb");
	int8_t low=data->width; // assigning a 2-byte value to a 1-byte var makes it store only the low byte (probably because they just access the value through the 8-bit registers)
	int8_t high=(data->width)>>8; // moving it left 8 times is the same as dividing by 256 (2**8=256)
	fputc(high,pFile);
	fputc(low,pFile);
	
	low=data->height;
	high=(data->height)>>8;
	fputc(high,pFile);
	fputc(low,pFile);

	for (uint32_t index=0;index<data->width*data->height;index++) {
		fputc(data->pixels[index],pFile);
	}

	fclose(pFile);
}

#ifdef BSC_SIZE_MANIPULATION // alows manipulation of PD size



#endif

#endif
