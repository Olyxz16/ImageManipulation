#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>


void main() {

    int sizex, sizey;

    int** tab = (int**)malloc(sizeof(int*)*sizex);
    for(int i = 0 ; i < sizex ; i++)
        tab[i] = (int*)malloc(sizeof(int)*sizey);

}