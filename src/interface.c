#include "interface.h"
#include "image.c"




image* askForImage() {
    printf("Donnez le nom de l'image a traiter : ");
    char* name;
    scanf("%s",name);
    int len = strlen(name);
    if(len<4 || (name[len-4]!='.' && name[len-3]!='p' && name[len-2]!='p' && name[len-1]!='m')) {
        name = appendStringWithString(name, ".ppm");
    }
    image* img = loadImage(name);
    return img; 
}
void displayAllFunctions() {
    printf("1: Niveau de gris\n");
    printf("2: Sepia\n");
    printf("3: Negatif\n");
    printf("4: Seuillage\n");
    printf("5: Pixelisation\n");
    printf("6: Filtre de lissage\n");
    printf("7: Filtre d'accentuation\n");
}
int askForInt(int default_val) {
    int val;
    printf("Rentrez la valeur de la modification (valeur par défaut : %d) : ", default_val);
    scanf("%d",&val);
    printf("\n");
    return val;
}
float askForFloat(float default_val) {
    float val;
    printf("Rentrez la valeur de la modification (valeur pas défaut : %f) : ", default_val);
    scanf("%d",&val);
    printf("\n");
    return val;
}
image* computeImage(image* img) {
    printf("Tapez l'id de la commande a effectuer\n\n");
    displayAllFunctions();
    printf("\n");
    printf("Id commande : ");
    int fun;
    scanf("%d",&fun);
    printf("\n");
    switch(fun) {
        case 1: return toGray(img); break;
        case 2: return toSepia(img); break;
        case 3: return toNegative(img); break;
        case 4: return Seuillage(img, askForFloat(0.5)); break;
        case 5: return Pixelate(img, askForInt(5)); break;
        case 6: return Lissage(img, askForInt(1)); break;
        case 7: return Accentuation(img, askForInt(1)); break;
        default: perror("Erreur: id de fonction invalide");
    }
}


