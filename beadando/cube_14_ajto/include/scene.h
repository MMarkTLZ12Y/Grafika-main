// scene.h

#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "texture.h"
#include "utils.h"

#include <obj/model.h>
#include <stdbool.h>

#define NUM_TREES 10

// Constants for the flower
#define FLOWER_BASE_HEIGHT 1.5f
#define FLOWER_BOB_AMPLITUDE 0.2f
#define FLOWER_BOB_FREQUENCY 2.0f
#define FLOWER_SPIN_SPEED 45.0f
#define FLOWER_SIZE 1.0f

// Constants for the wood sprites around trees
#define NUM_WOOD_SPRITES_PER_TREE 5
#define WOOD_SPRITE_SIZE 0.3f
#define WOOD_SPRITE_ORBIT_RADIUS 0.5f
#define WOOD_SPRITE_VERTICAL_OFFSET 0.4f
#define WOOD_SPRITE_SPIN_SPEED 60.0f

// ÚJ: Falak száma megnövelve az ajtónyílás miatt
#define NUM_WALL_SEGMENTS 5 // Eredetileg 4 volt

typedef struct Wall {
    vec3 position; // Center of the wall segment
    vec3 size;     // Full dimensions (width, depth, height) of the segment
} Wall;

typedef struct Scene
{
    Model cube;
    Model tree;
    Model lady_figure;
    Material material;
    GLuint texture_id;
    GLuint ground_texture_id;
    Wall walls[NUM_WALL_SEGMENTS]; // Megnövelt méret
    vec3 tree_positions[NUM_TREES];
    
    vec3 lady_figure_position;
    vec3 lady_figure_start_pos;
    vec3 lady_figure_end_pos;
    float lady_figure_speed;
    bool lady_figure_moving_to_end;

    GLuint flower_texture_id;
    vec3 flower_position;
    float flower_spin_angle;
    double flower_bob_timer;

    GLuint wood_texture_id;
    float wood_sprite_common_spin_angle;

} Scene;

void init_scene(Scene* scene);
void set_lighting();
void set_material(const Material* material);
void update_scene(Scene* scene, double elapsed_time);
void render_scene(const Scene* scene, const Camera* camera);
void draw_origin();
void draw_ground(GLuint texture_id);
void draw_wall(const Wall* wall); // This function remains the same
void render_billboard_sprite(GLuint texture_id, const vec3* world_pos, float size, float spin_angle, const Camera* camera);
bool check_collision(const Camera* camera, const Wall* walls, int num_walls); // Added num_walls parameter

#endif /* SCENE_H */