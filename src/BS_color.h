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

	for (uint16_t x=0; x<data->width; x++) {
		for (uint16_t y=0; y<data->height; y++) {
			data->pixels[y*data->width+x]=INVISIBLE_COLOR; // invisible
		}
	}
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

    uint8_t four; // right, here is the explanation:
		  // the highest value r and g can take are both 7, which means that a value of 1 should actually be 255/7 (which is 36ish)
		  // 36 is 0b00100100, so multiplying by 36 is the same as multiplying by 32 and 4 and adding the two numbers together (mindblowing, init lads?)
		  // what is interesting is that multiplication by 32 and 4 can both be done by just bitshifting by 5 (2**5=32) and 2 (2**2=4), respectively
		  // so what i am doing here is storing the value for 4r and 4g to then add (actually, or them) to the shifted r and g, to turn them into actual int values
	four=r<<2;
	r<<=5;
	r|=four;

	four=g<<2;
	g<<=5;
	g|=four;

	uint8_t one,sixteen; // same here but 85 (255/3) is 0b01010101, so more vars are used
	one=b;
	four=b<<2;
	sixteen=b<<4;
	b<<=6;
	b|=one|four|sixteen;

	// the reason why i | instead of + the vars is based on a very curious observation:
	// if foo is an integer under 9
	// and if bar==(int) 255/((2**foo)-1)
	// then in foo*bar, NO bits will overlap (try it yourself)
	// though when translating back, the original number is very likely to be lost, due to the fact that bar is forced to be an integer, cutting all the fp that comes after
	// to anyone wondering, the peak loss is around 2.745% (or gamma of 7) and occurs when foo=5

	if (r==252) r=255; // translation error checking (turns out 255%7!=0)
	if (g==252) g=255;

	// no more crazy bitwise logic
    return (r<<16)|(g<<8)|(b)|0xFF000000; // oring the numbers rather than adding as the byte the channels are writing to are GUARANTEED to be 0
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
	return ((rgLUT[r]<<16)|(rgLUT[g]<<8)|(bLUT[b]))|0xFF000000; // same thing with or but now with LUTs
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
	PD out={width,height}; // no need to specify NULL for pixels as does it for me (i love c)
	allocPixelMemory(&out);
	
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
	int8_t high=(data->width)>>8; // moving it right 8 times is the same as dividing by 256 (2**8=256)
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

#ifdef BSC_SIZE_MANIPULATION // it is what it says on the box

// a vector which stores 2 uint16_t values...
typedef struct {
	uint16_t v0;
       	uint16_t v1; // also, 2+2=4 so we are not wasting any memory on padding
} u16v2;

// used for cropImage()
typedef struct {
	uint16_t v0; // first x
	uint16_t v1; // first y
	uint16_t v2; // second x
	uint16_t v3; // second y
} u16v4;

// a
typedef struct {
	float v0;
	float v1;
} fv2;

// turns a coordinate to index
uint32_t coordToPos(PD* data, u16v2 pos) {
	return pos.v1*data->width+pos.v0; // yw+x (or you could do xh+y+1 but you are adding an extra operation and an extra variable (which too needs to be retrieved from meory))
}

// good thing i wrote this type of thing in CIMG
PD scaleBy(PD* data, fv2 scale) {
	PD out={(uint16_t) data->width*scale.v0,(uint16_t) data->height*scale.v1}; // casting a float to an int type pretty much just floors it (as far as i am concerned)
	allocPixelMemory(&out);
	for (uint16_t x=0; x<out.width; x++) { // width of new struct
		uint16_t iX=x/scale.v0; // x of the input PD
		for (uint16_t y=0; y<out.height; y++) { // Y are we doing this again? *cricket sound effect*
			uint16_t iY=y/scale.v1;
			out.pixels[coordToPos(&out,(u16v2) {x,y})]=data->pixels[coordToPos(data,(u16v2) {iX,iY})]; // this should work
		}
	}
	return out;
}

// returns pixels in data from first coord in the u16v4 to the second coord
PD cropImage(PD* data, u16v4 crop) {
	u16v2 topLeft, bottomRight; // we will start at top left and slowly go to bottom right, copying the pixels in the process
	
	if (crop.v0>crop.v2) {
		topLeft.v0=crop.v2;
		bottomRight.v0=crop.v0;
	} else {
		topLeft.v0=crop.v0;
		bottomRight.v0=crop.v2;
	}

	if (crop.v1>crop.v3) {
		topLeft.v1=crop.v3;
		bottomRight.v1=crop.v1;
	} else {
		topLeft.v1=crop.v1;
		bottomRight.v1=crop.v3;
	}
	
	PD out={bottomRight.v0-topLeft.v0+1,bottomRight.v1-topLeft.v1+1}; // +1 because if the coords are the same, it will give a 0-size PD
	allocPixelMemory(&out); // dont forget to alloc memory!

	uint32_t index=0; // index of pixel we want to write to in out
	u16v2 pos={0}; // current data pos
	for (uint16_t x=topLeft.v0; x<=bottomRight.v0; x++) { // <= as < will produce nothing when tl.v0==br.v0 (which is bad)
		pos.v0=x;
		for (uint16_t y=topLeft.v1; y<=bottomRight.v1; y++) {
			pos.v1=y;
			out.pixels[index]=data->pixels[coordToPos(data,pos)];
			index++;
		}
	}

	return out;
}

#endif

#endif
