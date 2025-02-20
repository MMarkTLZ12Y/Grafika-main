#include <stdio.h>

// Struktúra a téglatest adataihoz
typedef struct {
    double length;  // Hossz
    double width;   // Szélesség
    double height;  // Magasság
} Cuboid;

// Függvény, amely beállítja a téglatest 3 élét
void set_size(Cuboid *c, double length, double width, double height) {
    if (length <= 0 || width <= 0 || height <= 0) {
        printf("Hiba: Az élek hossza nem lehet 0 vagy negatív!\n");
        return;
    }
    c->length = length;
    c->width = width;
    c->height = height;
}

// Függvény a téglatest térfogatának kiszámításához
double calc_volume(const Cuboid *c) {
    return c->length * c->width * c->height;
}

// Függvény a téglatest felszínének kiszámításához
double calc_surface(const Cuboid *c) {
    return 2 * (c->length * c->width + c->width * c->height + c->height * c->length);
}

// Függvény, amely megvizsgálja, hogy van-e négyzet alakú lapja a téglatestnek
int has_square_face(const Cuboid *c) {
    return (c->length == c->width || c->width == c->height || c->height == c->length);
}

int main() {
    Cuboid c;

    // Beállítjuk a téglatest méreteit
    set_size(&c, 4.0, 5.0, -1.0);
    
    // Térfogat és felszín számítása
    printf("Téglatest térfogata: %.2f\n", calc_volume(&c));
    printf("Téglatest felszíne: %.2f\n", calc_surface(&c));

    // Ellenőrizzük, hogy van-e négyzet alakú lapja
    if (has_square_face(&c)) {
        printf("\n-----------------------\nA téglatestnek van négyzet alakú lapja!\n-----------------------\n");
    } else {
        printf("\n-----------------------\nA téglatestnek nincs négyzet alakú lapja!\n-----------------------\n");
    }

    return 0;
}

