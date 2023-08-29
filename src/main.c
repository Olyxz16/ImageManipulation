#include "interface.c"



void main() {

    image* img = loadImage("image.ppm");            // MODIFICATION MANUELLE
    img = Lissage(img,3);
    saveImage(img, "result.ppm");

    /*image* img = askForImage();                   // MODIFICATION AUTOMATIQUE
    img = computeImage(img);
    saveImage(img, "result.ppm");*/
    
}
