// scene.h

#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "texture.h" // Szükséges a GLuint-hez
#include "utils.h"   // Szükséges a Material és vec3 típusokhoz

#include <obj/model.h> // Szükséges a Model típushoz
#include <stdbool.h>   // Szükséges a bool típushoz

#define NUM_TREES 10 // Fák száma

// Constants for the flower
#define FLOWER_BASE_HEIGHT 1.5f
#define FLOWER_BOB_AMPLITUDE 0.2f
#define FLOWER_BOB_FREQUENCY 2.0f
#define FLOWER_SPIN_SPEED 45.0f // Degrees per second
#define FLOWER_SIZE 1.0f

typedef struct Wall {
    vec3 position;
    vec3 size;
} Wall;

typedef struct Scene
{
    Model cube;
    Model tree;
    Model lady_figure; // A figura modellje
    Material material;
    GLuint texture_id;
    GLuint ground_texture_id;
    Wall walls[4];
    vec3 tree_positions[NUM_TREES];
    
    // Lady figure animation parameters
    vec3 lady_figure_position;
    vec3 lady_figure_start_pos;
    vec3 lady_figure_end_pos;
    float lady_figure_speed;
    bool lady_figure_moving_to_end;

    // Flower properties
    GLuint flower_texture_id;
    vec3 flower_position;         // Current position, Z will be updated by bobbing
    float flower_spin_angle;      // Current rotation around its vertical axis
    double flower_bob_timer;       // Timer for the bobbing animation
} Scene;

/**
 * Initialize the scene.
 */
void init_scene(Scene* scene);

/**
 * Set the lighting properties.
 */
void set_lighting();

/**
 * Set the material properties for an object.
 */
void set_material(const Material* material);

/**
 * Update the scene.
 */
void update_scene(Scene* scene, double elapsed_time);

/**
 * Render the scene objects.
 */
void render_scene(const Scene* scene, const Camera* camera); // Pass camera for billboarding

/**
 * Draw the origin lines (axes).
 */
void draw_origin();

/**
 * Draw the ground quad.
 */
void draw_ground(GLuint texture_id);

/**
 * Draw a single wall.
 */
void draw_wall(const Wall* wall);

/**
 * Draw the floating flower.
 */
void render_flower(const Scene* scene, const Camera* camera); // Pass camera for billboarding

/**
 * Check for collision between the camera and the walls.
 */
bool check_collision(const Camera* camera, const Wall* walls);

#endif /* SCENE_H */