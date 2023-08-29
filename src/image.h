#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>


typedef struct octet {
    unsigned char val;
} octet;

typedef struct pixel {
    octet r,v,b;
} pixel;

typedef struct image {
    char* format;
    char* desc;
    int resx, resy;
    int depth;
    pixel** pixels;
} image;