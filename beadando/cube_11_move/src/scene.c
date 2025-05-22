#include "scene.h"
#include <obj/load.h>
#include <obj/draw.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h> // For fabs, etc. if needed, utils.h includes it for sqrtf

void init_scene(Scene* scene)
{
    // Modellek betöltése
    if (!load_model(&(scene->cube), "assets/models/cube.obj")) {
        printf("ERROR: Failed to load model: assets/models/cube.obj\n");
    }
    if (!load_model(&(scene->tree), "assets/models/tree.obj")) {
        printf("ERROR: Failed to load model: assets/models/tree.obj\n");
    }
    if (!load_model(&(scene->lady_figure), "assets/models/lady_figure.obj")) {
        printf("ERROR: Failed to load model: assets/models/lady_figure.obj\n");
    }

    // Textúrák betöltése
    scene->texture_id = load_texture("assets/textures/cube.png");
    if (scene->texture_id == 0) {
        printf("ERROR: Failed to load texture: assets/textures/cube.png\n");
    }
    scene->ground_texture_id = load_texture("assets/textures/grass.png");
    if (scene->ground_texture_id == 0) {
        printf("ERROR: Failed to load texture: assets/textures/grass.png\n");
    }

    // Falak inicializálása
    float wall_thickness = 0.2f;
    float room_size = 10.0f;
    float wall_height = 2.0f;
    float half_room = room_size / 2.0f;
    float half_thickness = wall_thickness / 2.0f;
    scene->walls[0] = (Wall){{-half_room - half_thickness, 0.0f, wall_height / 2.0f}, {wall_thickness, room_size + wall_thickness*2, wall_height}};
    scene->walls[1] = (Wall){{half_room + half_thickness, 0.0f, wall_height / 2.0f}, {wall_thickness, room_size + wall_thickness*2, wall_height}};
    scene->walls[2] = (Wall){{0.0f, -half_room - half_thickness, wall_height / 2.0f}, {room_size, wall_thickness, wall_height}};
    scene->walls[3] = (Wall){{0.0f, half_room + half_thickness, wall_height / 2.0f}, {room_size, wall_thickness, wall_height}};

    // Alapértelmezett anyag beállítása
    scene->material.ambient.red = 0.2f;
    scene->material.ambient.green = 0.2f;
    scene->material.ambient.blue = 0.0f;
    scene->material.diffuse.red = 1.0f;
    scene->material.diffuse.green = 1.0f;
    scene->material.diffuse.blue = 0.0f;
    scene->material.specular.red = 0.1f;
    scene->material.specular.green = 0.1f;
    scene->material.specular.blue = 0.0f;
    scene->material.shininess = 10.0f;

    // Fák pozícióinak inicializálása
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

    // Figura pozíciójának és animációjának inicializálása
    scene->lady_figure_position = (vec3){ 3.0f, 3.0f, 0.0f }; // Kezdeti pozíció
    
    // Mozgási pálya beállítása
    scene->lady_figure_start_pos = (vec3){ 3.0f, 3.0f, 0.0f };    // Vonal kezdőpontja
    scene->lady_figure_end_pos   = (vec3){ -3.0f, 3.0f, 0.0f };   // Vonal végpontja (mozgás X tengely mentén)
    scene->lady_figure_speed     = 1.0f;                         // Sebesség (egység/másodperc)
    scene->lady_figure_moving_to_end = true;                     // Kezdetben a végpont felé mozog
}


void set_lighting()
{
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

void set_material(const Material* material)
{
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
    // Lady figure mozgásának frissítése
    vec3 current_target_pos;
    if (scene->lady_figure_moving_to_end) {
        current_target_pos = scene->lady_figure_end_pos;
    } else {
        current_target_pos = scene->lady_figure_start_pos;
    }

    vec3 direction_to_target = vec3_subtract(current_target_pos, scene->lady_figure_position);
    float distance_to_target = vec3_length(direction_to_target);

    // Kis hibahatár az összehasonlításhoz (pl. 0.01 egység)
    float epsilon = scene->lady_figure_speed * (float)elapsed_time * 0.5f; // Dinamikusabb epsilon
    if (epsilon < 0.001f) epsilon = 0.001f;


    if (distance_to_target < epsilon || distance_to_target == 0.0f) {
        // Cél elérve (vagy nagyon közel van)
        scene->lady_figure_position = current_target_pos; // Pontosan a célpozícióba helyezzük
        scene->lady_figure_moving_to_end = !scene->lady_figure_moving_to_end; // Irányváltás
    } else {
        // Mozgás a cél felé
        vec3 move_direction_normalized = vec3_normalize(direction_to_target);
        float move_amount = scene->lady_figure_speed * (float)elapsed_time;

        // Ne lépje túl a célt ebben a lépésben
        if (move_amount > distance_to_target) {
            move_amount = distance_to_target;
        }
        
        scene->lady_figure_position = vec3_add(
            scene->lady_figure_position,
            vec3_scale(move_direction_normalized, move_amount)
        );
    }
}

void draw_ground(GLuint texture_id)
{
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

void draw_wall(const Wall* wall)
{
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

bool check_collision(const Camera* camera, const Wall* walls)
{
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
 
void render_scene(const Scene* scene)
{
    set_material(&(scene->material));
    set_lighting();
    
    draw_origin();

    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    draw_ground(scene->ground_texture_id);
    glPopMatrix();

    for (int i = 0; i < 4; i++) {
        draw_wall(&scene->walls[i]);
    }

    set_material(&(scene->material));
    glBindTexture(GL_TEXTURE_2D, scene->texture_id);
    glEnable(GL_TEXTURE_2D);

    glPushMatrix();
    glTranslatef(-2.0, -2.0, 0.5);
    draw_model(&(scene->cube));
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.0, -2.0, 0.5);
    draw_model(&(scene->cube));
    glPopMatrix();

    Material tree_material;
    tree_material.ambient.red   = 0.4f; tree_material.ambient.green = 0.25f;tree_material.ambient.blue  = 0.15f;
    tree_material.diffuse.red   = 0.5f; tree_material.diffuse.green = 0.35f;tree_material.diffuse.blue  = 0.2f;
    tree_material.specular.red  = 0.05f;tree_material.specular.green= 0.05f;tree_material.specular.blue = 0.0f;
    tree_material.shininess     = 5.0f;
    set_material(&tree_material);
    glDisable(GL_TEXTURE_2D);

    for (int i = 0; i < NUM_TREES; ++i) {
        glPushMatrix();
        glTranslatef(
            scene->tree_positions[i].x,
            scene->tree_positions[i].y,
            scene->tree_positions[i].z
        );
        draw_model(&(scene->tree));
        glPopMatrix();
    }

    Material lady_material;
    lady_material.ambient.red = 0.4f; lady_material.ambient.green = 0.2f; lady_material.ambient.blue = 0.3f;
    lady_material.diffuse.red = 0.8f; lady_material.diffuse.green = 0.4f; lady_material.diffuse.blue = 0.6f;
    lady_material.specular.red = 0.3f; lady_material.specular.green = 0.3f; lady_material.specular.blue = 0.3f;
    lady_material.shininess = 30.0f;

    set_material(&lady_material);
    glDisable(GL_TEXTURE_2D);

    glPushMatrix();
    // Use the updated lady_figure_position for rendering
    glTranslatef(
        scene->lady_figure_position.x,
        scene->lady_figure_position.y,
        scene->lady_figure_position.z
    );
    // glScalef(0.8f, 0.8f, 0.8f);
    draw_model(&(scene->lady_figure));
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);
}

void draw_origin()
{
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