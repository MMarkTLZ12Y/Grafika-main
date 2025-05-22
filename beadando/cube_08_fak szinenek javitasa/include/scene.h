// scene.h

#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "texture.h" // Szükséges a GLuint-hez
#include "utils.h"   // Szükséges a Material és vec3 típusokhoz

#include <obj/model.h> // Szükséges a Model típushoz
#include <stdbool.h>   // Szükséges a bool típushoz

#define NUM_TREES 10 // Fák száma

typedef struct Wall {
    vec3 position;
    vec3 size;
} Wall;

typedef struct Scene
{
    Model cube;
    Model tree; // A fa modell tárolására
    Material material; // Általános anyag, pl. a kockákhoz
    GLuint texture_id; // Kocka textúrája
    GLuint ground_texture_id; // Talaj textúrája
    Wall walls[4]; // A falak tömbje
    vec3 tree_positions[NUM_TREES]; // ÚJ: A fák pozíciói
} Scene;

/**
 * Initialize the scene.
 * Betölti a modelleket, textúrákat és beállítja az alapértelmezett anyagokat.
 */
void init_scene(Scene* scene);

/**
 * Set the lighting properties.
 * Beállítja a fényforrásokat (ambient, diffuse, specular, position).
 */
void set_lighting();

/**
 * Set the material properties for an object.
 * Beállítja az aktuális rajzolási anyag tulajdonságait.
 */
void set_material(const Material* material);

/**
 * Update the scene.
 * Jelenleg üres, de később animációkhoz vagy dinamikus elemekhez használható.
 */
void update_scene(Scene* scene);

/**
 * Render the scene objects.
 * Kirajzolja a jelenet összes elemét (modellek, talaj, falak).
 */
void render_scene(const Scene* scene);

/**
 * Draw the origin lines (axes).
 * Segédfüggvény az X, Y, Z tengelyek kirajzolásához.
 */
void draw_origin();

/**
 * Draw the ground quad.
 * Kirajzol egy textúrázott négyszöget talajként.
 */
void draw_ground(GLuint texture_id);

/**
 * Draw a single wall.
 * Kirajzol egy falat a megadott pozícióval és mérettel.
 */
void draw_wall(const Wall* wall);

/**
 * Check for collision between the camera and the walls.
 * Ellenőrzi, hogy a kamera ütközik-e valamelyik fallal.
 */
bool check_collision(const Camera* camera, const Wall* walls);

#endif /* SCENE_H */