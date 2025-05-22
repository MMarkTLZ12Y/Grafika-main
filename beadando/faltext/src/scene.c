#include "scene.h"
#include <obj/load.h>
#include <obj/draw.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void init_scene(Scene* scene)
{
    // Modellek
    if (!load_model(&(scene->cube), "assets/models/cube.obj")) { printf("Cube model load error\n"); }
    if (!load_model(&(scene->tree), "assets/models/tree.obj")) { printf("Tree model load error\n"); }
    if (!load_model(&(scene->lady_figure), "assets/models/lady_figure.obj")) { printf("Lady figure model load error\n"); }

    // Textúrák
    scene->texture_id = load_texture("assets/textures/cube.png");
    scene->ground_texture_id = load_texture("assets/textures/grass.png");
    scene->flower_texture_id = load_texture("assets/textures/virag.png");
    scene->wood_texture_id = load_texture("assets/textures/wood5.png");
    scene->wall_texture_id = load_texture("assets/textures/wall2.png"); // <--- ÚJ SOR (pl. brick.png)

    if (scene->wood_texture_id == 0) {
        printf("ERROR: Failed to load texture: assets/textures/wall2.png\n");
    }

    // Falak inicializálása
    float wall_thickness = 0.2f;
    float room_size = 10.0f;      // Original room dimension along X and Y
    float wall_height = 2.0f;
    float half_room = room_size / 2.0f;
    float half_thickness = wall_thickness / 2.0f;

    // Door parameters
    float door_width = 1.5f;
    // float door_height = 1.8f; // Not directly used if doorway goes to full wall_height
                                // We are making a full height opening.

    // Wall 0: Left wall (-X side)
    scene->walls[0] = (Wall){
        {-half_room - half_thickness, 0.0f, wall_height / 2.0f},                               // position
        {wall_thickness, room_size + wall_thickness * 2, wall_height}                           // size
    };

    // Wall 1: Right wall (+X side)
    scene->walls[1] = (Wall){
        {half_room + half_thickness, 0.0f, wall_height / 2.0f},                                // position
        {wall_thickness, room_size + wall_thickness * 2, wall_height}                           // size
    };

    // Wall 2: Back wall (-Y side)
    scene->walls[2] = (Wall){
        {0.0f, -half_room - half_thickness, wall_height / 2.0f},                                // position
        {room_size, wall_thickness, wall_height}                                                // size (full width)
    };

    // Wall 3 & 4: Front wall (+Y side) - SPLIT FOR DOORWAY
    // This wall originally spanned from x = -half_room to x = +half_room at y = half_room + half_thickness
    // Total length of this wall section was room_size.
    // We center the door.
    float wall_segment_length = (room_size - door_width) / 2.0f;

    // Wall 3: Left segment of the front wall
    scene->walls[3] = (Wall){
        {-half_room + (wall_segment_length / 2.0f), half_room + half_thickness, wall_height / 2.0f}, // position
        {wall_segment_length, wall_thickness, wall_height}                                             // size
    };
    
    // Wall 4: Right segment of the front wall
    scene->walls[4] = (Wall){
        {half_room - (wall_segment_length / 2.0f), half_room + half_thickness, wall_height / 2.0f},   // position
        {wall_segment_length, wall_thickness, wall_height}                                             // size
    };


    // Material
    scene->material.ambient.red = 0.2f; scene->material.ambient.green = 0.2f; scene->material.ambient.blue = 0.0f;
    scene->material.diffuse.red = 1.0f; scene->material.diffuse.green = 1.0f; scene->material.diffuse.blue = 0.0f;
    scene->material.specular.red = 0.1f; scene->material.specular.green = 0.1f; scene->material.specular.blue = 0.0f;
    scene->material.shininess = 10.0f;

    // Fák
    scene->tree_positions[0] = (vec3){  5.0f,  5.0f, 0.0f };
    scene->tree_positions[1] = (vec3){ -6.0f,  3.0f, 0.0f };
    // ... (rest of tree positions)
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

// ... (set_lighting, set_material, update_scene, draw_ground, draw_wall, render_billboard_sprite, draw_origin are unchanged) ...
void set_lighting() { 
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

void set_material(const Material* material) { 
    float ambient_material_color[] = { material->ambient.red, material->ambient.green, material->ambient.blue, 1.0f };
    float diffuse_material_color[] = { material->diffuse.red, material->diffuse.green, material->diffuse.blue, 1.0f };
    float specular_material_color[] = { material->specular.red, material->specular.green, material->specular.blue, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_material_color);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);
}

void update_scene(Scene* scene, double elapsed_time) {
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
    scene->flower_bob_timer += elapsed_time;
    scene->flower_position.z = FLOWER_BASE_HEIGHT + sin(scene->flower_bob_timer * FLOWER_BOB_FREQUENCY) * FLOWER_BOB_AMPLITUDE;
    scene->flower_spin_angle += FLOWER_SPIN_SPEED * (float)elapsed_time;
    scene->flower_spin_angle = fmod(scene->flower_spin_angle, 360.0f);
    scene->wood_sprite_common_spin_angle += WOOD_SPRITE_SPIN_SPEED * (float)elapsed_time;
    scene->wood_sprite_common_spin_angle = fmod(scene->wood_sprite_common_spin_angle, 360.0f);
}

void draw_ground(GLuint texture_id) { 
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

// Módosítsd a draw_wall függvény definícióját, hogy megkapja a textúra ID-t
void draw_wall(const Wall* wall, GLuint texture_id) { // <--- MÓDOSÍTOTT PARAMÉTER
    glPushMatrix();
    glTranslatef(wall->position.x, wall->position.y, wall->position.z);
    glScalef(wall->size.x, wall->size.y, wall->size.z);

    // glDisable(GL_TEXTURE_2D); // <--- EZT A SORT TÖRÖLD VAGY KOMMENTELD KI

    // Anyag beállítása - a textúra színeinek jobb érvényesülése érdekében a diffúz színt fehérre állítjuk
    Material wall_material;
    wall_material.ambient.red = 0.5f; wall_material.ambient.green = 0.5f; wall_material.ambient.blue = 0.5f; // Lehet kicsit sötétebb, hogy az árnyékok látszódjanak
    wall_material.diffuse.red = 1.0f; wall_material.diffuse.green = 1.0f; wall_material.diffuse.blue = 1.0f; // Fehér, hogy a textúra színe domináljon
    wall_material.specular.red = 0.1f; wall_material.specular.green = 0.1f; wall_material.specular.blue = 0.1f; // Enyhe csillogás
    wall_material.shininess = 5.0f;
    set_material(&wall_material);

    glEnable(GL_TEXTURE_2D); // <--- Biztosítjuk, hogy a textúrázás engedélyezve van
    glBindTexture(GL_TEXTURE_2D, texture_id); // <--- A FAL TEXTÚRÁJÁNAK BEKÖTÉSE

    // Opcionális: Textúra ismétlődés beállítása, ha a fal nagyobb, mint a textúra
    // Ezt a load_texture-ben vagy itt is meg lehetne tenni, de az alapértelmezett általában jó.
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // A textúra koordinátákat a fal méretétől függően is beállíthatod az ismétlődéshez,
    // pl. X irányban size.x / textúra_egység_mérete alkalommal ismétlődjön.
    // Most egyszerű 0-1 koordinátákat használunk minden lapra.
    // Ez azt eredményezi, hogy a textúra kifeszül minden egyes fal lapra.
    // Ha ismétlődő mintát szeretnél, a glTexCoord2f értékeket kell módosítani (pl. 0.0f-tól 5.0f-ig).
    // A textúra koordináták (u,v): (0,0) bal alsó, (1,0) jobb alsó, (1,1) jobb felső, (0,1) bal felső.
    // OpenGL-ben a textúrák függőleges koordinátája (v) gyakran alulról felfelé nő.

    glBegin(GL_QUADS);
    // Front face (Z+)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);

    // Back face (Z-)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f); // A textúra hátulról "tükrözve" jelenhet meg így, vagy fordítsd meg a koordinátákat
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);

    // Top face (Y+)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);

    // Bottom face (Y-)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);

    // Right face (X+)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);

    // Left face (X-)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glEnd();

    // glEnable(GL_TEXTURE_2D); // <--- EZT A SORT IS TÖRÖLD VAGY KOMMENTELD KI, ha a fenti disable-t kivetted
                                 // Mert ha nem tiltottad le, nem kell újra engedélyezni.
                                 // A textúrázás általánosan engedélyezve van az init_opengl-ben.
    glPopMatrix();
}

void render_billboard_sprite(GLuint texture_id, const vec3* world_pos, float size, float spin_angle, const Camera* camera) {
    if (texture_id == 0) return;
    (void)camera; 
    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE); 
    glPushMatrix();
    glTranslatef(world_pos->x, world_pos->y, world_pos->z);
    GLfloat modelview_matrix[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);
    float scale_x = sqrt(modelview_matrix[0]*modelview_matrix[0] + modelview_matrix[1]*modelview_matrix[1] + modelview_matrix[2]*modelview_matrix[2]);
    float scale_y = sqrt(modelview_matrix[4]*modelview_matrix[4] + modelview_matrix[5]*modelview_matrix[5] + modelview_matrix[6]*modelview_matrix[6]);
    float scale_z = sqrt(modelview_matrix[8]*modelview_matrix[8] + modelview_matrix[9]*modelview_matrix[9] + modelview_matrix[10]*modelview_matrix[10]);
    modelview_matrix[0] = scale_x; modelview_matrix[1] = 0.0f;    modelview_matrix[2] = 0.0f;
    modelview_matrix[4] = 0.0f;    modelview_matrix[5] = scale_y; modelview_matrix[6] = 0.0f;
    modelview_matrix[8] = 0.0f;    modelview_matrix[9] = 0.0f;    modelview_matrix[10] = scale_z;
    glLoadMatrixf(modelview_matrix);
    glRotatef(spin_angle, 0.0f, 0.0f, 1.0f);
    glScalef(size / 2.0f, size / 2.0f, size / 2.0f); 
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); 
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 0.0f); 
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, -1.0f, 0.0f); 
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f, 0.0f); 
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f, 0.0f); 
    glEnd();
    glPopMatrix();
    glPopAttrib();
}

void draw_origin() {
    GLboolean lighting_enabled; glGetBooleanv(GL_LIGHTING, &lighting_enabled);
    GLboolean texture_2d_enabled; glGetBooleanv(GL_TEXTURE_2D, &texture_2d_enabled);
    glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
    glBegin(GL_LINES);
        glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
        glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();
    if (lighting_enabled) glEnable(GL_LIGHTING); if (texture_2d_enabled) glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
}

// MODIFIED: check_collision now takes num_walls
bool check_collision(const Camera* camera, const Wall* walls, int num_walls)
{
    float cam_radius = 0.3f;
    float cam_min_x = camera->position.x - cam_radius;
    float cam_max_x = camera->position.x + cam_radius;
    float cam_min_y = camera->position.y - cam_radius;
    float cam_max_y = camera->position.y + cam_radius;
    float cam_min_z = camera->position.z - cam_radius;
    float cam_max_z = camera->position.z + cam_radius;

    for (int i = 0; i < num_walls; i++) { // Use num_walls
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
 
void render_scene(const Scene* scene, const Camera* camera)
{
    set_material(&(scene->material)); // Alapértelmezett anyag, ha más nincs megadva
    set_lighting();
    draw_origin();

    // Talaj
    glEnable(GL_TEXTURE_2D); // Ez jó itt általánosan
    glPushMatrix();
    draw_ground(scene->ground_texture_id);
    glPopMatrix();

    // Falak - MODIFIED LOOP
    for (int i = 0; i < NUM_WALL_SEGMENTS; i++) {
        draw_wall(&scene->walls[i], scene->wall_texture_id); // <--- ÁTADJUK A TEXTÚRA ID-T
    }

    // Kockák
    // set_material(&(scene->material)); // Ezt már a render_scene elején megtettük
    glBindTexture(GL_TEXTURE_2D, scene->texture_id);
    // glEnable(GL_TEXTURE_2D); // Már engedélyezve van
    glPushMatrix(); glTranslatef(-2.0, -2.0, 0.5); draw_model(&(scene->cube)); glPopMatrix();
    glPushMatrix(); glTranslatef( 2.0, -2.0, 0.5); draw_model(&(scene->cube)); glPopMatrix();

    // Fák és a körülöttük lebegő fa sprite-ok
    Material tree_material;
    tree_material.ambient.red = 0.4f; tree_material.ambient.green = 0.25f; tree_material.ambient.blue  = 0.15f;
    tree_material.diffuse.red = 0.5f; tree_material.diffuse.green = 0.35f; tree_material.diffuse.blue  = 0.2f;
    tree_material.specular.red = 0.05f; tree_material.specular.green= 0.05f; tree_material.specular.blue = 0.0f;
    tree_material.shininess = 5.0f;

    for (int i = 0; i < NUM_TREES; ++i) {
        set_material(&tree_material);
        glDisable(GL_TEXTURE_2D);
        glPushMatrix();
        glTranslatef(scene->tree_positions[i].x, scene->tree_positions[i].y, scene->tree_positions[i].z);
        draw_model(&(scene->tree));
        glPopMatrix();

        for (int j = 0; j < NUM_WOOD_SPRITES_PER_TREE; ++j) {
            vec3 sprite_pos;
            float angle_offset = (2.0f * M_PI / NUM_WOOD_SPRITES_PER_TREE) * j;
            sprite_pos.x = scene->tree_positions[i].x + WOOD_SPRITE_ORBIT_RADIUS * cos(angle_offset);
            sprite_pos.y = scene->tree_positions[i].y + WOOD_SPRITE_ORBIT_RADIUS * sin(angle_offset);
            sprite_pos.z = scene->tree_positions[i].z + WOOD_SPRITE_VERTICAL_OFFSET;
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
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(scene->lady_figure_position.x, scene->lady_figure_position.y, scene->lady_figure_position.z);
    draw_model(&(scene->lady_figure));
    glPopMatrix();

    // Központi virág
    render_billboard_sprite(scene->flower_texture_id, &(scene->flower_position), FLOWER_SIZE, scene->flower_spin_angle, camera);

    glEnable(GL_TEXTURE_2D);
}