#include <stdio.h>

typedef struct {
    double length;  
    double width;   
    double height;  
} Cuboid;

void set_size(Cuboid *c, double length, double width, double height) {
    if (length <= 0 || width <= 0 || height <= 0) {
        printf("Hiba: Az elek hossza nem lehet 0 vagy negativ!\n");
        return;
    }
    c->length = length;
    c->width = width;
    c->height = height;
}

double calc_volume(const Cuboid *c) {
    return c->length * c->width * c->height;
}

double calc_surface(const Cuboid *c) {
    return 2 * (c->length * c->width + c->width * c->height + c->height * c->length);
}

int has_square_face(const Cuboid *c) {
    return (c->length == c->width || c->width == c->height || c->height == c->length);
}

int main() {
    Cuboid c;

    set_size(&c, 4.0, 5.0, -1.0);
    
    printf("Téglatest térfogata: %.2f\n", calc_volume(&c));
    printf("Téglatest felszíne: %.2f\n", calc_surface(&c));

    if (has_square_face(&c)) {
        printf("\n-----------------------\nA teglatestnek van negyzet alaku lapja!\n-----------------------\n");
    } else {
        printf("\n-----------------------\nA teglatestnek nincs negyzet alaku lapja!\n-----------------------\n");
    }

    return 0;
}

