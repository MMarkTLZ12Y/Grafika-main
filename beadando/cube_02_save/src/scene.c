#include "scene.h"

#include <obj/load.h>
#include <obj/draw.h>
#include <GL/gl.h>
#include <stdio.h>

void init_scene(Scene* scene)
{
    load_model(&(scene->cube), "assets/models/cube.obj");
    scene->texture_id = load_texture("assets/textures/cube.png");
    scene->ground_texture_id = load_texture("assets/textures/grass.png");

    // Initialize wall
    scene->wall.position.x = 0.0f;
    scene->wall.position.y = 5.0f;
    scene->wall.position.z = 0.5f;
    scene->wall.size.x = 10.0f;
    scene->wall.size.y = 0.5f;
    scene->wall.size.z = 2.0f;

    glBindTexture(GL_TEXTURE_2D, scene->texture_id);

    scene->material.ambient.red = 0.0;
    scene->material.ambient.green = 0.0;
    scene->material.ambient.blue = 0.0;

    scene->material.diffuse.red = 1.0;
    scene->material.diffuse.green = 1.0;
    scene->material.diffuse.blue = 0.0;

    scene->material.specular.red = 0.0;
    scene->material.specular.green = 0.0;
    scene->material.specular.blue = 0.0;

    scene->material.shininess = 0.0;

    printf("Cube texture ID: %d\n", scene->texture_id);
    printf("Ground texture ID: %d\n", scene->ground_texture_id);
}

void set_lighting()
{
    float ambient_light[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float diffuse_light[] = { 1.0f, 1.0f, 1.0, 1.0f };
    float specular_light[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float position[] = { 0.0f, 0.0f, 10.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
}

void set_material(const Material* material)
{
    float ambient_material_color[] = {
        material->ambient.red,
        material->ambient.green,
        material->ambient.blue
    };

    float diffuse_material_color[] = {
        material->diffuse.red,
        material->diffuse.green,
        material->diffuse.blue
    };

    float specular_material_color[] = {
        material->specular.red,
        material->specular.green,
        material->specular.blue
    };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &(material->shininess));
}

void update_scene(Scene* scene)
{
}

void draw_ground(GLuint texture_id)
{
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-30.0f, -30.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(30.0f, -30.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(30.0f, 30.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-30.0f, 30.0f, 0.0f);
    glEnd();
}

void draw_wall(const Wall* wall)
{
    glPushMatrix();
    glTranslatef(wall->position.x, wall->position.y, wall->position.z);
    glScalef(wall->size.x, wall->size.y, wall->size.z);
    
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.2f, 0.4f, 0.8f);
    
    glBegin(GL_QUADS);
        // Front face
        glVertex3f(-0.5f, -0.5f, 0.5f);
        glVertex3f(0.5f, -0.5f, 0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f);
        
        // Back face
        glVertex3f(-0.5f, -0.5f, -0.5f);
        glVertex3f(-0.5f, 0.5f, -0.5f);
        glVertex3f(0.5f, 0.5f, -0.5f);
        glVertex3f(0.5f, -0.5f, -0.5f);
        
        // Top face
        glVertex3f(-0.5f, 0.5f, -0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f);
        glVertex3f(0.5f, 0.5f, -0.5f);
        
        // Bottom face
        glVertex3f(-0.5f, -0.5f, -0.5f);
        glVertex3f(0.5f, -0.5f, -0.5f);
        glVertex3f(0.5f, -0.5f, 0.5f);
        glVertex3f(-0.5f, -0.5f, 0.5f);
        
        // Left face
        glVertex3f(-0.5f, -0.5f, -0.5f);
        glVertex3f(-0.5f, -0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, -0.5f);
        
        // Right face
        glVertex3f(0.5f, -0.5f, -0.5f);
        glVertex3f(0.5f, 0.5f, -0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f);
        glVertex3f(0.5f, -0.5f, 0.5f);
    glEnd();
    
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

bool check_collision(const Camera* camera, const Wall* wall)
{
    float camera_radius = 0.3f;
    
    if (fabs(camera->position.x - wall->position.x) < (wall->size.x/2 + camera_radius)) {
        if (fabs(camera->position.y - wall->position.y) < (wall->size.y/2 + camera_radius)) {
            if (fabs(camera->position.z - wall->position.z) < (wall->size.z/2 + camera_radius)) {
                return true;
            }
        }
    }
    return false;
}

void render_scene(const Scene* scene)
{
    set_material(&(scene->material));
    set_lighting();
    draw_origin();

    glPushMatrix();
    glTranslatef(0.0, 0.0, -0.5);
    draw_ground(scene->ground_texture_id);
    glPopMatrix();

    draw_wall(&scene->wall);

    glBindTexture(GL_TEXTURE_2D, scene->texture_id);
    
    glPushMatrix();
    glTranslatef(-1.0, 0.0, 0.0);
    draw_model(&(scene->cube));
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.0, 0.0, 0.0);
    draw_model(&(scene->cube));
    glPopMatrix();
}

void draw_origin()
{
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1, 0);
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);
    glEnd();
}