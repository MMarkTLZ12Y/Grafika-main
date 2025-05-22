#include "scene.h"
#include <obj/load.h>
#include <obj/draw.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h> // For sin, cos, fmod, M_PI

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void init_scene(Scene* scene)
{
    // Modellek
    if (!load_model(&(scene->cube), "assets/models/cube.obj")) { /* ... */ }
    if (!load_model(&(scene->tree), "assets/models/tree.obj")) { /* ... */ }
    if (!load_model(&(scene->lady_figure), "assets/models/lady_figure.obj")) { /* ... */ }

    // Textúrák
    scene->texture_id = load_texture("assets/textures/cube.png");
    scene->ground_texture_id = load_texture("assets/textures/grass.png");
    scene->flower_texture_id = load_texture("assets/textures/virag.png");
    scene->wood_texture_id = load_texture("assets/textures/wood5.png");
    if (scene->wood_texture_id == 0) {
        printf("ERROR: Failed to load texture: assets/textures/wood.png\n");
    }

    // Falak
    float wall_thickness = 0.2f; float room_size = 10.0f; float wall_height = 2.0f;
    float half_room = room_size / 2.0f; float half_thickness = wall_thickness / 2.0f;
    scene->walls[0] = (Wall){{-half_room - half_thickness, 0.0f, wall_height / 2.0f}, {wall_thickness, room_size + wall_thickness*2, wall_height}};
    scene->walls[1] = (Wall){{half_room + half_thickness, 0.0f, wall_height / 2.0f}, {wall_thickness, room_size + wall_thickness*2, wall_height}};
    scene->walls[2] = (Wall){{0.0f, -half_room - half_thickness, wall_height / 2.0f}, {room_size, wall_thickness, wall_height}};
    scene->walls[3] = (Wall){{0.0f, half_room + half_thickness, wall_height / 2.0f}, {room_size, wall_thickness, wall_height}};

    // Material
    scene->material.ambient.red = 0.2f; scene->material.ambient.green = 0.2f; scene->material.ambient.blue = 0.0f;
    scene->material.diffuse.red = 1.0f; scene->material.diffuse.green = 1.0f; scene->material.diffuse.blue = 0.0f;
    scene->material.specular.red = 0.1f; scene->material.specular.green = 0.1f; scene->material.specular.blue = 0.0f;
    scene->material.shininess = 10.0f;

    // Fák
    scene->tree_positions[0] = (vec3){  5.0f,  5.0f, 0.0f };
    scene->tree_positions[1] = (vec3){ -6.0f,  3.0f, 0.0f };
    scene->tree_positions[2] = (vec3){  7.0f, -4.0f, 0.0f };
    scene->tree_positions[3] = (vec3){ -8.0f, -6.0f, 0.0f };
    scene->tree_positions[4] = (vec3){  0.0f,  8.0f, 0.0f };
    scene->tree_positions[5] = (vec3){ 12.0f, 12.0f, 0.0f };
    scene->tree_positions[6] = (vec3){-15.0f,  0.0f, 0.0f };
    scene->tree_positions[7] = (vec3){  0.0f,-13.0f, 0.0f };
    scene->tree_positions[8] = (vec3){ 10.0f, -9.0f, 0.0f };
    scene->tree_positions[9] = (vec3){ -3.0f, 11.0f, 0.0f };

    // Figura
    scene->lady_figure_position = (vec3){ 3.0f, 3.0f, 0.0f };
    scene->lady_figure_start_pos = (vec3){ 3.0f, 3.0f, 0.0f };
    scene->lady_figure_end_pos   = (vec3){ -3.0f, 3.0f, 0.0f };
    scene->lady_figure_speed     = 1.0f;
    scene->lady_figure_moving_to_end = true;

    // Virág
    scene->flower_position.x = 0.0f; scene->flower_position.y = 0.0f;
    scene->flower_position.z = FLOWER_BASE_HEIGHT;
    scene->flower_spin_angle = 0.0f;
    scene->flower_bob_timer = 0.0;

    // Fa sprite-ok
    scene->wood_sprite_common_spin_angle = 0.0f;
}


void set_lighting() { /* ... unchanged ... */ 
    float model_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);

    float ambient_light0[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    float diffuse_light0[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    float specular_light0[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    float position0[] = { 1.0f, 1.0f, 7.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light0);
    glLightfv(GL_LIGHT0, GL_POSITION, position0);

    float ambient_light1[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
    float diffuse_light1[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
    float specular_light1[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float position1[]       = { -3.0f, -3.0f, 4.0f, 1.0f };

    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient_light1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse_light1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular_light1);
    glLightfv(GL_LIGHT1, GL_POSITION, position1);
    glEnable(GL_LIGHT1);
}
void set_material(const Material* material) { /* ... unchanged ... */ 
    float ambient_material_color[] = {
        material->ambient.red, material->ambient.green, material->ambient.blue, 1.0f
    };
    float diffuse_material_color[] = {
        material->diffuse.red, material->diffuse.green, material->diffuse.blue, 1.0f
    };
    float specular_material_color[] = {
        material->specular.red, material->specular.green, material->specular.blue, 1.0f
    };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_material_color);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);
}

void update_scene(Scene* scene, double elapsed_time)
{
    // Lady figure
    vec3 current_target_pos;
    if (scene->lady_figure_moving_to_end) current_target_pos = scene->lady_figure_end_pos;
    else current_target_pos = scene->lady_figure_start_pos;
    vec3 dir_to_target = vec3_subtract(current_target_pos, scene->lady_figure_position);
    float dist_to_target = vec3_length(dir_to_target);
    float epsilon = scene->lady_figure_speed * (float)elapsed_time * 0.5f;
    if (epsilon < 0.001f) epsilon = 0.001f;
    if (dist_to_target < epsilon || dist_to_target == 0.0f) {
        scene->lady_figure_position = current_target_pos;
        scene->lady_figure_moving_to_end = !scene->lady_figure_moving_to_end;
    } else {
        vec3 move_dir_norm = vec3_normalize(dir_to_target);
        float move_amount = scene->lady_figure_speed * (float)elapsed_time;
        if (move_amount > dist_to_target) move_amount = dist_to_target;
        scene->lady_figure_position = vec3_add(scene->lady_figure_position, vec3_scale(move_dir_norm, move_amount));
    }

    // Virág
    scene->flower_bob_timer += elapsed_time;
    scene->flower_position.z = FLOWER_BASE_HEIGHT + sin(scene->flower_bob_timer * FLOWER_BOB_FREQUENCY) * FLOWER_BOB_AMPLITUDE;
    scene->flower_spin_angle += FLOWER_SPIN_SPEED * (float)elapsed_time;
    scene->flower_spin_angle = fmod(scene->flower_spin_angle, 360.0f);

    // Fa sprite-ok közös forgása
    scene->wood_sprite_common_spin_angle += WOOD_SPRITE_SPIN_SPEED * (float)elapsed_time;
    scene->wood_sprite_common_spin_angle = fmod(scene->wood_sprite_common_spin_angle, 360.0f);
}

void draw_ground(GLuint texture_id) { /* ... unchanged ... */ 
    glBindTexture(GL_TEXTURE_2D, texture_id);
    float ground_size = 30.0f;
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-ground_size, -ground_size, 0.0f);
        glTexCoord2f(10.0f, 0.0f); glVertex3f( ground_size, -ground_size, 0.0f);
        glTexCoord2f(10.0f, 10.0f); glVertex3f( ground_size,  ground_size, 0.0f);
        glTexCoord2f(0.0f, 10.0f); glVertex3f(-ground_size,  ground_size, 0.0f);
    glEnd();
}
void draw_wall(const Wall* wall) { /* ... unchanged ... */ 
    glPushMatrix();
    glTranslatef(wall->position.x, wall->position.y, wall->position.z);
    glScalef(wall->size.x, wall->size.y, wall->size.z);

    glDisable(GL_TEXTURE_2D);
    Material wall_material;
    wall_material.ambient.red = 0.2f; wall_material.ambient.green = 0.2f; wall_material.ambient.blue = 0.2f;
    wall_material.diffuse.red = 0.5f; wall_material.diffuse.green = 0.5f; wall_material.diffuse.blue = 0.55f;
    wall_material.specular.red = 0.0f; wall_material.specular.green = 0.0f; wall_material.specular.blue = 0.0f;
    wall_material.shininess = 0.0f;
    set_material(&wall_material);

    glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-0.5f, -0.5f, 0.5f); glVertex3f(0.5f, -0.5f, 0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glVertex3f(0.5f, 0.5f, -0.5f); glVertex3f(0.5f, -0.5f, -0.5f);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-0.5f, 0.5f, -0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f); glVertex3f(0.5f, 0.5f, -0.5f);
        glNormal3f(0.0f, -1.0f, 0.0f);
        glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(0.5f, -0.5f, -0.5f);
        glVertex3f(0.5f, -0.5f, 0.5f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glNormal3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.5f, -0.5f, -0.5f); glVertex3f(0.5f, 0.5f, -0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f); glVertex3f(0.5f, -0.5f, 0.5f);
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glEnd();
    
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}
bool check_collision(const Camera* camera, const Wall* walls) { /* ... unchanged ... */ 
    float cam_radius = 0.3f;
    float cam_min_x = camera->position.x - cam_radius;
    float cam_max_x = camera->position.x + cam_radius;
    float cam_min_y = camera->position.y - cam_radius;
    float cam_max_y = camera->position.y + cam_radius;
    float cam_min_z = camera->position.z - cam_radius;
    float cam_max_z = camera->position.z + cam_radius;

    for (int i = 0; i < 4; i++) {
        const Wall* wall = &walls[i];
        float wall_min_x = wall->position.x - wall->size.x / 2.0f;
        float wall_max_x = wall->position.x + wall->size.x / 2.0f;
        float wall_min_y = wall->position.y - wall->size.y / 2.0f;
        float wall_max_y = wall->position.y + wall->size.y / 2.0f;
        float wall_min_z = wall->position.z - wall->size.z / 2.0f;
        float wall_max_z = wall->position.z + wall->size.z / 2.0f;

        bool overlap_x = cam_max_x > wall_min_x && cam_min_x < wall_max_x;
        bool overlap_y = cam_max_y > wall_min_y && cam_min_y < wall_max_y;
        bool overlap_z = cam_max_z > wall_min_z && cam_min_z < wall_max_z;

        if (overlap_x && overlap_y && overlap_z) {
            return true;
        }
    }
    return false;
}


// ÁLTALÁNOS BILLBOARD SPRITE RENDERELŐ FÜGGVÉNY
void render_billboard_sprite(GLuint texture_id, const vec3* world_pos, float size, float spin_angle, const Camera* camera) {
    if (texture_id == 0) return;
    (void)camera; // camera pointer might be used for more advanced billboarding, but not in this simple version.

    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE); // Fontos az átlátszó textúrák helyes rendereléséhez

    glPushMatrix();
    // Pozicionálás a világban
    glTranslatef(world_pos->x, world_pos->y, world_pos->z);

    // Billboarding: Orientálás a kamera felé
    // Ez a legegyszerűbb forma, ami a modellnézeti mátrix forgási részét nullázza.
    GLfloat modelview_matrix[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

    // Forgási rész nullázása (a modellnézeti mátrixban)
    // Megtartjuk a skálázást (modelview_matrix[0], [5], [10]), ha van
    float scale_x = sqrt(modelview_matrix[0]*modelview_matrix[0] + modelview_matrix[1]*modelview_matrix[1] + modelview_matrix[2]*modelview_matrix[2]);
    float scale_y = sqrt(modelview_matrix[4]*modelview_matrix[4] + modelview_matrix[5]*modelview_matrix[5] + modelview_matrix[6]*modelview_matrix[6]);
    float scale_z = sqrt(modelview_matrix[8]*modelview_matrix[8] + modelview_matrix[9]*modelview_matrix[9] + modelview_matrix[10]*modelview_matrix[10]);

    modelview_matrix[0] = scale_x; modelview_matrix[1] = 0.0f;    modelview_matrix[2] = 0.0f;
    modelview_matrix[4] = 0.0f;    modelview_matrix[5] = scale_y; modelview_matrix[6] = 0.0f;
    modelview_matrix[8] = 0.0f;    modelview_matrix[9] = 0.0f;    modelview_matrix[10] = scale_z;
    glLoadMatrixf(modelview_matrix);

    // Saját forgás a sprite "felfelé" (most képernyő Z) tengelye körül
    glRotatef(spin_angle, 0.0f, 0.0f, 1.0f);
    
    // Méretezés
    glScalef(size / 2.0f, size / 2.0f, size / 2.0f); // A quad -1 és 1 között van

    // Quad rajzolása
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // Teljes szín, ha nincs anyag
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 0.0f); // Bal alsó
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, -1.0f, 0.0f); // Jobb alsó
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f, 0.0f); // Jobb felső
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f, 0.0f); // Bal felső
    glEnd();

    glPopMatrix();
    glPopAttrib();
}
 
void render_scene(const Scene* scene, const Camera* camera)
{
    set_material(&(scene->material));
    set_lighting();
    draw_origin();

    // Talaj
    glEnable(GL_TEXTURE_2D); // Biztosítjuk, hogy a textúrázás be van kapcsolva
    glPushMatrix();
    draw_ground(scene->ground_texture_id);
    glPopMatrix();

    // Falak
    for (int i = 0; i < 4; i++) {
        draw_wall(&scene->walls[i]);
    }

    // Kockák
    set_material(&(scene->material)); // Alap material
    glBindTexture(GL_TEXTURE_2D, scene->texture_id); // Kocka textúra
    glEnable(GL_TEXTURE_2D);
    glPushMatrix(); glTranslatef(-2.0, -2.0, 0.5); draw_model(&(scene->cube)); glPopMatrix();
    glPushMatrix(); glTranslatef( 2.0, -2.0, 0.5); draw_model(&(scene->cube)); glPopMatrix();

    // Fák és a körülöttük lebegő fa sprite-ok
    Material tree_material;
    tree_material.ambient.red = 0.4f; tree_material.ambient.green = 0.25f; tree_material.ambient.blue  = 0.15f;
    tree_material.diffuse.red = 0.5f; tree_material.diffuse.green = 0.35f; tree_material.diffuse.blue  = 0.2f;
    tree_material.specular.red = 0.05f; tree_material.specular.green= 0.05f; tree_material.specular.blue = 0.0f;
    tree_material.shininess = 5.0f;

    for (int i = 0; i < NUM_TREES; ++i) {
        // Fa modell
        set_material(&tree_material);
        glDisable(GL_TEXTURE_2D); // Fa modell nem textúrázott
        glPushMatrix();
        glTranslatef(scene->tree_positions[i].x, scene->tree_positions[i].y, scene->tree_positions[i].z);
        draw_model(&(scene->tree));
        glPopMatrix();

        // Fa körüli lebegő fa sprite-ok
        for (int j = 0; j < NUM_WOOD_SPRITES_PER_TREE; ++j) {
            vec3 sprite_pos;
            float angle_offset = (2.0f * M_PI / NUM_WOOD_SPRITES_PER_TREE) * j; // Szög eltolás minden sprite-hoz

            sprite_pos.x = scene->tree_positions[i].x + WOOD_SPRITE_ORBIT_RADIUS * cos(angle_offset);
            sprite_pos.y = scene->tree_positions[i].y + WOOD_SPRITE_ORBIT_RADIUS * sin(angle_offset);
            sprite_pos.z = scene->tree_positions[i].z + WOOD_SPRITE_VERTICAL_OFFSET; // Magasság a fa aljától

            render_billboard_sprite(scene->wood_texture_id, &sprite_pos, WOOD_SPRITE_SIZE, scene->wood_sprite_common_spin_angle, camera);
        }
    }

    // Figura
    Material lady_material;
    lady_material.ambient.red = 0.4f; lady_material.ambient.green = 0.2f; lady_material.ambient.blue = 0.3f;
    lady_material.diffuse.red = 0.8f; lady_material.diffuse.green = 0.4f; lady_material.diffuse.blue = 0.6f;
    lady_material.specular.red = 0.3f; lady_material.specular.green = 0.3f; lady_material.specular.blue = 0.3f;
    lady_material.shininess = 30.0f;
    set_material(&lady_material);
    glDisable(GL_TEXTURE_2D); // Figura nem textúrázott
    glPushMatrix();
    glTranslatef(scene->lady_figure_position.x, scene->lady_figure_position.y, scene->lady_figure_position.z);
    draw_model(&(scene->lady_figure));
    glPopMatrix();

    // Központi virág
    render_billboard_sprite(scene->flower_texture_id, &(scene->flower_position), FLOWER_SIZE, scene->flower_spin_angle, camera);

    glEnable(GL_TEXTURE_2D); // Általános textúrázás visszakapcsolása
}

void draw_origin() { /* ... unchanged ... */ 
    GLboolean lighting_enabled;
    GLboolean texture_2d_enabled;
    glGetBooleanv(GL_LIGHTING, &lighting_enabled);
    glGetBooleanv(GL_TEXTURE_2D, &texture_2d_enabled);

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    glBegin(GL_LINES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();

    if (lighting_enabled) glEnable(GL_LIGHTING);
    if (texture_2d_enabled) glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
}