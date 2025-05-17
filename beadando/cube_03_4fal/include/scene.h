#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "texture.h"

#include <obj/model.h>

typedef struct Wall {
    vec3 position;
    vec3 size;
} Wall;

typedef struct Scene
{
    Model cube;
    Material material;
    GLuint texture_id;
    GLuint ground_texture_id;
    Wall walls[4]; // Négy fal
} Scene;


void init_scene(Scene* scene);
void set_lighting();
void set_material(const Material* material);
void update_scene(Scene* scene);
void render_scene(const Scene* scene);
void draw_origin();
void draw_ground(GLuint texture_id);
void draw_wall(const Wall* wall);
bool check_collision(const Camera* camera, const Wall* wall);

#endif /* SCENE_H */