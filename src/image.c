#include "image.h"


#pragma region Utils
// Retourne un entier à partir d'une chaîne de caractère
int stoi(char* s) {
    return (int)strtol(s,NULL,10);
}
// Retourne une chaîne de caractère à partir d'un entier
char* itos(int i) {
    char* s = malloc(sizeof(char)*64);
    sprintf(s, "%d", i);
    return s;
}
// Ajoute un caractère à la fin d'une chaîne de caractère
char* appendStringWithChar(char* s, char c) {
    int len = strlen(s);
    char* new_s = malloc(len*(sizeof(char)+1)); // Crée un nouveau string un caractère plus grand.
    for(int i = 0 ; i < len ; i++) { // Reconstruit le string d'origine.
        new_s[i]=s[i];
    }
    new_s[len] = c; // Ajoute le caractère souhaité.
    new_s[len+1] = '\0'; // Ajoute le caractère de fin.
    free(s); // Libère la mémoire.
    return new_s; 
}
char* appendStringWithString(char* s, char* a) {
    for(int i = 0 ; i < strlen(a) ; i++)
        s = appendStringWithChar(s, a[i]);
    return s;
}
// Retourne le maximum entre 2 valeurs.
float max_2val(float a, float b) {
    return a>b?a:b;
}
// Retourne le maximum entre 3 valeurs.
float max_3val(float a, float b, float c) {
    return max_2val(a,max_2val(b,c));
}
// Retourne le minimum entre 2 valeurs.
float min_2val(float a, float b) {
    return a<b?a:b;
}
// Retourne le minimum entre 3 valeurs.
float min_3val(float a, float b, float c) {
    return min_2val(a,min_2val(b,c));
}
// Restreint la valeur donnée entre deux autres valeurs.
float clamp(float v, float min, float max) {
    return max_2val(min_2val(v,max), min);
}
#pragma endregion
#pragma region ImageUtils
// Retourne une matrice de pixel vide, d'une taille donnée.
pixel** GetEmptyMat(int sizex, int sizey) {
    pixel** mat = (pixel**)malloc((sizeof(pixel*)+1)*sizex);
    for(int i = 0 ; i < sizex ; i++)
        mat[i] = (pixel*)malloc(sizeof(pixel)*sizey);
    return mat;
}
// Transforme un tableau de pixel linéaire en une matrice de même taille.
pixel** LtoM(pixel* linear, int sizex, int sizey) {
    pixel** mat = GetEmptyMat(sizex, sizey);
    for(int j = 0 ; j < sizey ; j++)
        for(int i = 0 ; i < sizex ; i++)
            mat[i][j] = linear[j*sizex+i];
    return mat;
}
// Retourne un tableau de pixel linéaire vide.
pixel* GetEmptyLin(int sizex, int sizey) {
    return (pixel*)malloc((sizeof(pixel)+1)*sizex*sizey);
}
// Transforme une matrice de pixel en un tableau de pixel linéaire.
pixel* MtoL(pixel** mat, int sizex, int sizey) {
    pixel* lin = (pixel*)malloc((sizeof(pixel)+1)*sizex*sizey);
    for(int j = 0 ; j < sizey ; j++) {
        for(int i = 0 ; i < sizex ; i++) {
            lin[j*sizex+i] = mat[i][j];
        }
    }
    return lin;
}

// Retourne un pixel à partir de 3 valeurs RGB.
pixel pix(unsigned char r, unsigned char v, unsigned char b) {
    pixel p;
    p.r.val = r;
    p.v.val = v;
    p.b.val = b;
    return p;
}
// Retourne la valeur de Rouge d'un pixel.
unsigned char r(pixel p) {
    return p.r.val;
}
// Retourne la valeur de Vert d'un pixel.
unsigned char v(pixel p) {
    return p.v.val;
}
// Retourne la valeur de Bleu d'un pixel.
unsigned char b(pixel p) {
    return p.b.val;
}
// Retourne la valeur de Teinte d'un pixel. (mode TSL)
float t(pixel p) {
    float r,v,b;
    r = (float)p.r.val/255;
    v = (float)p.v.val/255;
    b = (float)p.b.val/255;
    float min, max, delta;
    min = min_3val(r,v,b);
    max = max_3val(r,v,b);
    delta = max-min;
    float hue;
    if(delta==0)
        return 0;
    if(max==r) {
        hue = (v-b)/delta;
    }
    if(max==v) {
        hue = 2+(b-r)/delta;
    }
    if(max==b) {
        hue = 4+(v-r)/delta;
    }
    hue*=60;
    if(hue<=0)
        hue+=360;
    return hue;
}
// Retourne la valeur de saturation d'un pixel. (mode TSL)
float s(pixel p) {
    float r,v,b;
    r = (float)p.r.val/255;
    v = (float)p.v.val/255;
    b = (float)p.b.val/255;
    float min, max, delta;
    min = min_3val(r,v,b);
    max = max_3val(r,v,b);
    delta = max-min;
    if(max == 0)
        return 0;
    else return (delta/max);
}
// Retourne la valeur de luminosité d'un pixel. (mode TSL)
float l(pixel p) {
    float r,v,b;
    r = (float)p.r.val/255;
    v = (float)p.v.val/255;
    b = (float)p.b.val/255;
    float max = max_3val(r,v,b);
    return max;
}
// Retourne le nombre de pixel "légaux" (entendez "faisant parti de l'image") à partir d'une position et d'une taille donnée. (Cette fonction à pour but d'optimiser le calcul du coefficient central du kernel d'accentuation)
int CountLegalPixels(int x, int y, int sizex, int sizey, int size) {
    int length = 2*size+1;
    int legal_size_x=length, legal_size_y=length;
    int xmin=-size+x , xmax=size+x , ymin=-size+y , ymax=size+y;
    if(xmin<0)
        legal_size_x+=xmin;
    if(ymin<0)
        legal_size_y+=ymin;
    if(xmax>=sizex)
        legal_size_x+=(sizex-xmax);
    if(ymax>=sizey)
        legal_size_y+=(sizey-ymax);
    
    return legal_size_x*legal_size_y;
}
// Retourne la valeur d'un pixel, en appliquant un kernel donné sur une image donnée.
pixel ComputeKernel(image* img, float** kernel, int size, int indx, int indy) {
    pixel p = pix(0,0,0);
    int n = 0;
    int r=0,v=0,b=0;
    for(int i = -size ; i < size ; i++) {
        for(int j = -size ; j < size ; j++) {
            if(indx+i < 0 || indy+j < 0 || indx+i >= img->resx || indy+j >= img->resy) {
                continue;
            }
            r+=(int)(((float)(img->pixels[indx+i][indy+j].r.val))*kernel[i+size][j+size]);
            v+=(int)(((float)(img->pixels[indx+i][indy+j].v.val))*kernel[i+size][j+size]);
            b+=(int)(((float)(img->pixels[indx+i][indy+j].b.val))*kernel[i+size][j+size]);
            n++;
        }
    }
    p = pix(clamp(r/n,0,255), clamp(v/n,0,255), clamp(b/n,0,255));
    return p;
}
#pragma endregion

#pragma region ImageParsing
// Retourne une chaîne de caractère correspondant à la première ligne du fichier donné.
char* readLine(FILE* fp) {
    char* line = "\0";
    int val = 0;
    while(val != '\n') {
        val = fgetc(fp);
        if(val != '\n')
            line = appendStringWithChar(line, val);
    }
    return line;
}
// Retourne la taille d'une image à partir de la chaîne de caractère appropriée.
void readImageSize(char* l, int* sizex, int* sizey) {
    char* s1 = "\0";
    char* s2 = "\0";
    int step = 0;
    int len = strlen(l);
    while(step < len && l[step] != ' ') {
        s1 = appendStringWithChar(s1,l[step]);
        step++;
    }
    while(step < len) {
        s2 = appendStringWithChar(s2, l[step]);
        step++;
    }
    *sizex = stoi(s1);
    *sizey = stoi(s2);
}
// Retourne la valeur maximal d'un pixel à partir de la châine de caractère appropriée.
int readDepth(char* l) {
    return stoi(l);
}

// Retourne un tableau de pixel de la taille de l'image à partir des données de l'image.
pixel** readImagePixels(FILE* fp, int sizex, int sizey) {
    pixel* linear_pixs = GetEmptyLin(sizex, sizey);
    for(int i = 0 ; i < sizex*sizey ; i++) {
        unsigned char r,v,b;
        r = stoi(readLine(fp));
        v = stoi(readLine(fp));
        b = stoi(readLine(fp));
        pixel p = pix(r,v,b);
        linear_pixs[i] = p;
    }
    pixel** pixs = LtoM(linear_pixs, sizex, sizey);
    free(linear_pixs);
    return pixs;
}

// Retourne une image à partir d'un fichier ppm.
image* readImage(FILE* fp) {

    char* format;
    char* desc;
    int sizex, sizey;
    int depth;
    pixel** pixels;

    format = readLine(fp);
    desc = readLine(fp);
    readImageSize(readLine(fp),&sizex,&sizey);
    depth = readDepth(readLine(fp));
    pixels = readImagePixels(fp, sizex, sizey);

    image* img = (image*)malloc(sizeof(image));  

    img->format = format;
    img->desc = desc;
    img->resx = sizex;
    img->resy = sizey;
    img->depth = depth;
    img->pixels = pixels;

    return img;

}


#pragma endregion
#pragma region ImagePrinting

// Affiche la taille de l'image donnée.
void printImageSize(image* img) {
    printf("largeur : %d \nhauteur : %d\n", img->resx, img->resy);
}

// Affiche correctement une valeur donnée.
void printVal(int val) {
    if(0 <= val && val <= 9) {
        printf("00%d",val);
    } else if(val <= 99) {
        printf("0%d",val);
    } else {
        printf("%d",val);
    }
}
// Affiche un pixel donné.
void printPixel(pixel p) {
    int r,v,b;
    r = p.r.val;
    v = p.v.val;
    b = p.b.val;

    printVal(r);
    printf(" ");

    printVal(v);
    printf(" ");

    printVal(b);
    printf("   ");
}
// Affiche une image au format ASCII dans la console.
void printImageASCII(image* img) {
    pixel* pixs = MtoL(img->pixels, img->resx, img->resy);
    for(int i = 0 ; i < img->resx*img->resy ; i ++) {
        if(i%img->resx==0&&i!=0)
            printf("\n");
        pixel p = pixs[i];
        printPixel(p);
    }
}

#pragma endregion
#pragma region imageManipulation
// Convertit une image donnée en échelle de gris, puis la retourne.
image* toGray(image* img) {
    printf("Started computing image !\n");
    for(int i = 0 ; i < img->resx ; i++) {
        for(int j = 0 ; j < img->resy ; j++) {
            int g = (int)255*l(img->pixels[i][j]);
            img->pixels[i][j] = pix(g,g,g);
        }
    }
    printf("To gray done !\n");
    return img;
}
// Convertit une image donnée en sepia, puis la retourne.
image* toSepia(image* img) {
    printf("Started computing image !\n");
    for(int i = 0 ; i < img->resx ; i++) {
        for(int j = 0 ; j < img->resy ; j++) {
            pixel p = img->pixels[i][j];
            int tr, tv, tb;
            tr = (int) (0.393*r(p) + 0.769*v(p) + 0.189*b(p));
            tv = (int) (0.349*r(p) + 0.686*v(p) + 0.168*b(p));
            tb = (int) (0.272*r(p) + 0.534*v(p) + 0.131*b(p));
            p = pix(min_2val(255,tr), min_2val(255,tv), min_2val(255,tb));
            img->pixels[i][j] = p;
        }
    }
    printf("To sepia done !\n");
    return img;
}
// Convertit une image donnée en négatif, puis la retourne.
image* toNegative(image* img) {
    printf("Started computing image !\n");
    for(int i = 0 ; i < img->resx ; i++) {
        for(int j = 0 ; j < img->resy ; j++) {
            pixel p = img->pixels[i][j];
            img->pixels[i][j] = pix(img->depth-r(p), img->depth-v(p), img->depth-b(p));
        }
    }
    printf("To negative done !\n");
    return img;
}
// Effectue un seuillage sur une image donnée à partir d'un seuil donné, puis la retourne.
image* Seuillage(image* img, float v) {
    printf("Started computing image !\n");
    for(int i = 0 ; i < img->resx ; i++) {
        for(int j = 0 ; j < img->resy ; j++) {
            pixel p = img->pixels[i][j];
            if(l(p) < v)
                img->pixels[i][j] = pix(0,0,0);
            else img->pixels[i][j] = pix(255,255,255);
        }
    }
    printf("Seuillage done !\n");
    return img;
}
// Effectue une pixelisation sur une image donnée, en fonction d'une fréquence donnée, puis la retourne.
image* Pixelate(image* img, int freq) {
    printf("Started computing image !\n");
    pixel** pixs = (pixel**)malloc((sizeof(pixel)+1)*img->resx);
    for(int i = 0 ; i < img->resx ; i++) {
        pixel* col = (pixel*)malloc(sizeof(pixel)*img->resy);
        for(int j = 0 ; j < img->resy ; j++) {
            pixel p = pix(0,0,0);
            col[j] = p;
        }
        pixs[i] = col;
    }
    int indx=0, indy=0;
    for(int i = 0 ; i < img->resx ; i++) {
        for(int j = 0 ; j < img->resy ; j++) {
            pixel p = img->pixels[indx][indy];
            pixs[i][j]=p;
            if(j%freq==freq-1 && (indy+freq)<img->resy) indy+=freq;
        }
        indy = 0;
        if(i%freq==freq-1 && (indx+freq)<img->resx) indx+=freq;
    }
    img->pixels = pixs;
    printf("Pixelation done !\n");
    return img;
}
// Effectue un lissage sur une image donnée, avec un kernel d'une taille donnée.
image* Lissage(image* img, int size) {
    printf("Started computing image !\n");
    int length = 2*size+1;
    float** kernel = (float**)malloc(sizeof(float*)*(length));
    for(int i = 0 ; i < length ; i++)
        kernel[i] = (float*)malloc(sizeof(float)*(length));
    for(int i = 0 ; i < length ; i++) 
        for(int j = 0 ; j < length ; j++) 
            kernel[i][j] = 1;

    pixel** pixs = GetEmptyMat(img->resx, img->resy);
    for(int i = 0 ; i < img->resx ; i++) 
        for(int j = 0 ; j < img->resy ; j++) 
            pixs[i][j] = ComputeKernel(img, kernel, size, i, j);

    img->pixels = pixs;
    printf("Lissage done !\n");
    return img;
}
// Accentue les couleurs de l'image (a l'effet inverse du lissage).
image* Accentuation(image* img, int size) {
    printf("Started computing image !\n");
    int length = 2*size+1;
    float** kernel = (float**)malloc(sizeof(float*)*(length));
    for(int i = 0 ; i < length ; i++)
        kernel[i] = (float*)malloc(sizeof(float)*(length));
    for(int i = 0 ; i < length ; i++) {
        for(int j = 0 ; j < length ; j++) {
            kernel[i][j] = -1;
        }
    }

    pixel** pixs = GetEmptyMat(img->resx, img->resy);
    for(int i = 0 ; i < img->resx ; i++) {
        for(int j = 0 ; j < img->resy ; j++) {
            kernel[size][size] = CountLegalPixels(i,j, img->resx, img->resy, size)+1;
            pixs[i][j] = ComputeKernel(img, kernel, size, i, j);
        }
    }
            
    img->pixels = pixs;
    printf("Accentuation done !\n");
    return img;
}

#pragma endregion

#pragma region FileManipulation
// Retourne une image à partir du nom d'un fichier ppm.
image* loadImage(char* name) {
    FILE* fp = fopen(name, "r");
    image* img = readImage(fp);
    fclose(fp);
    return img;
}
// Sauvegarde une image donnée dans un fichier ppm de nom donné.
void saveImage(image* img, char* dest) {
    printf("Start saving !\n");
    FILE* fr = fopen(dest, "w+");
    fputs(img->format, fr);
    fputc('\n', fr);
    fputs(img->desc, fr);
    fputc('\n', fr);
    fputs(itos(img->resx), fr);
    fputc(' ', fr);
    fputs(itos(img->resy), fr);
    fputc('\n', fr);
    fputs(itos(img->depth), fr);
    fputc('\n', fr);
    pixel* pixs = MtoL(img->pixels, img->resx, img->resy);
    for(int i = 0 ; i < img->resx * img->resy ; i++) {
        fputs(itos(pixs[i].r.val), fr);
        fputc('\n', fr);
        fputs(itos(pixs[i].v.val), fr);
        fputc('\n', fr);
        fputs(itos(pixs[i].b.val), fr);
        fputc('\n', fr);
    }
    free(pixs);
    printf("Saved !\n");
    fclose(fr);
}
#pragma endregion
