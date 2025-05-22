#include "scene.h"
#include <obj/load.h>
#include <obj/draw.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h> // Szükséges a rand()-hoz, ha véletlenszerű pozíciókat szeretnénk
#include <time.h>   // Szükséges a srand()-hoz

void init_scene(Scene* scene)
{
    // Modellek betöltése
    if (!load_model(&(scene->cube), "assets/models/cube.obj")) {
        printf("ERROR: Failed to load model: assets/models/cube.obj\n");
    }
    if (!load_model(&(scene->tree), "assets/models/tree.obj")) {
        printf("ERROR: Failed to load model: assets/models/tree.obj\n");
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

    // Falak inicializálása (egy 10x10-es doboz, 2 egység magas falakkal)
    // A falak közepe a megadott pozícióban van.
    // A méretek: x (vastagság X mentén), y (vastagság Y mentén), z (magasság Z mentén)
    // A jelenlegi koordináta-rendszerben:
    // X: jobbra/balra
    // Y: előre/hátra
    // Z: fel/le
    float wall_thickness = 0.2f;
    float room_size = 10.0f;
    float wall_height = 2.0f;
    float half_room = room_size / 2.0f;
    float half_thickness = wall_thickness / 2.0f;
    scene->walls[0] = (Wall){{-half_room - half_thickness, 0.0f, wall_height / 2.0f}, {wall_thickness, room_size + wall_thickness*2, wall_height}};
    scene->walls[1] = (Wall){{half_room + half_thickness, 0.0f, wall_height / 2.0f}, {wall_thickness, room_size + wall_thickness*2, wall_height}};
    scene->walls[2] = (Wall){{0.0f, -half_room - half_thickness, wall_height / 2.0f}, {room_size, wall_thickness, wall_height}};
    scene->walls[3] = (Wall){{0.0f, half_room + half_thickness, wall_height / 2.0f}, {room_size, wall_thickness, wall_height}};


    // Alapértelmezett anyag beállítása (pl. a kockákhoz)
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

    // ÚJ: Fák pozícióinak inicializálása
    // Lehetnek fix pozíciók, vagy generálhatunk véletlenszerűeket
    // srand(time(NULL)); // Véletlenszám-generátor inicializálása (ha szükséges)

    // Fix pozíciók példaként:
    scene->tree_positions[0] = (vec3){  5.0f,  5.0f, 0.0f };
    scene->tree_positions[1] = (vec3){ -6.0f,  3.0f, 0.0f };
    scene->tree_positions[2] = (vec3){  7.0f, -4.0f, 0.0f };
    scene->tree_positions[3] = (vec3){ -8.0f, -6.0f, 0.0f };
    scene->tree_positions[4] = (vec3){  0.0f,  8.0f, 0.0f }; // Ez a falakon belül lehet
    scene->tree_positions[5] = (vec3){ 12.0f, 12.0f, 0.0f }; // Ez biztosan kívül
    scene->tree_positions[6] = (vec3){-15.0f,  0.0f, 0.0f };
    scene->tree_positions[7] = (vec3){  0.0f,-13.0f, 0.0f };
    scene->tree_positions[8] = (vec3){ 10.0f, -9.0f, 0.0f };
    scene->tree_positions[9] = (vec3){ -3.0f, 11.0f, 0.0f };

    /*
    // Vagy véletlenszerű pozíciók egy tartományban:
    // A talaj mérete -ground_size-tól +ground_size-ig terjed, azaz 2*ground_size
    // A draw_ground-ban ground_size = 30.0f;
    float spawn_range = 25.0f; // Legyenek a ground_size-on belül, de ne a legszélén
    for (int i = 0; i < NUM_TREES; ++i) {
        scene->tree_positions[i].x = ((float)rand() / RAND_MAX) * 2.0f * spawn_range - spawn_range;
        scene->tree_positions[i].y = ((float)rand() / RAND_MAX) * 2.0f * spawn_range - spawn_range;
        scene->tree_positions[i].z = 0.0f; // Talajon
    }
    */
}


void set_lighting()
{
    // Fényforrás beállításai
    float ambient_light[] = { 0.3f, 0.3f, 0.3f, 1.0f }; // Környezeti fény (hogy ne legyenek teljesen fekete árnyékok)
    float diffuse_light[] = { 0.8f, 0.8f, 0.8f, 1.0f }; // Diffúz fény (fő fényforrás)
    float specular_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };// Csillanás színe
    float position[] = { 3.0f, 3.0f, 5.0f, 1.0f };    // Fényforrás pozíciója (x, y, z, w - w=1 pontszerű, w=0 irányfény)

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
}

void set_material(const Material* material)
{
    // Anyag tulajdonságainak beállítása
    // OpenGL régi, fix funkciós pipeline anyagmodelljét használjuk
    float ambient_material_color[] = {
        material->ambient.red,
        material->ambient.green,
        material->ambient.blue,
        1.0f // Alpha komponens, általában 1.0 átlátszatlan objektumokhoz
    };

    float diffuse_material_color[] = {
        material->diffuse.red,
        material->diffuse.green,
        material->diffuse.blue,
        1.0f
    };

    float specular_material_color[] = {
        material->specular.red,
        material->specular.green,
        material->specular.blue,
        1.0f
    };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_material_color);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess); // Shininess egy float, nem tömb
}

void update_scene(Scene* scene)
{
    // Jelenleg nincs mit frissíteni a jeleneten minden képkockában.
    // Ide kerülhetnének pl. animációk, fizikai szimulációk frissítése.
    (void)scene; // Hogy a fordító ne panaszkodjon a nem használt paraméterre
}

void draw_ground(GLuint texture_id)
{
    glBindTexture(GL_TEXTURE_2D, texture_id); // Textúra kötése
    // A textúrázásnak engedélyezve kell lennie (glEnable(GL_TEXTURE_2D))

    // Anyag beállítása a talajhoz (ha eltérőt szeretnénk, mint az alap)
    // Itt feltételezzük, hogy a hívó helyen van beállítva, vagy az alapértelmezett jó.
    // Pl. egy szürkésebb, kevésbé csillogó anyag:
    /*
    Material ground_material;
    ground_material.ambient.red = 0.2f; ground_material.ambient.green = 0.2f; ground_material.ambient.blue = 0.2f;
    ground_material.diffuse.red = 0.6f; ground_material.diffuse.green = 0.6f; ground_material.diffuse.blue = 0.6f;
    ground_material.specular.red = 0.1f; ground_material.specular.green = 0.1f; ground_material.specular.blue = 0.1f;
    ground_material.shininess = 5.0f;
    set_material(&ground_material);
    */

    // Egy nagy négyzet a talajnak, Z=0 síkon
    // A textúra koordináták (0,0) és (1,1) között vannak, hogy a teljes textúra megjelenjen.
    // Ha ismétlődő mintát szeretnénk, a koordinátákat növelhetnénk (pl. 0,0-tól 10,10-ig).
    // Ehhez a textúra paramétereinél GL_REPEAT-et kell beállítani (glTexParameteri).
    float ground_size = 30.0f;
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f); // Normális felfelé mutat (a helyes megvilágításhoz)
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-ground_size, -ground_size, 0.0f);
        glTexCoord2f(10.0f, 0.0f); glVertex3f( ground_size, -ground_size, 0.0f); // Textúra ismétlődéshez
        glTexCoord2f(10.0f, 10.0f); glVertex3f( ground_size,  ground_size, 0.0f);
        glTexCoord2f(0.0f, 10.0f); glVertex3f(-ground_size,  ground_size, 0.0f);
    glEnd();
}

void draw_wall(const Wall* wall)
{
    glPushMatrix();
    glTranslatef(wall->position.x, wall->position.y, wall->position.z);
    glScalef(wall->size.x, wall->size.y, wall->size.z); // Méretezzük a falat

    // Egyszerű fal szín, textúra nélkül
    glDisable(GL_TEXTURE_2D); // Biztosan kikapcsoljuk a textúrázást a falhoz
    
    // Egyedi anyag a falaknak (szürke, matt)
    Material wall_material;
    wall_material.ambient.red = 0.2f; wall_material.ambient.green = 0.2f; wall_material.ambient.blue = 0.2f;
    wall_material.diffuse.red = 0.5f; wall_material.diffuse.green = 0.5f; wall_material.diffuse.blue = 0.55f; // Enyhén kékes szürke
    wall_material.specular.red = 0.0f; wall_material.specular.green = 0.0f; wall_material.specular.blue = 0.0f;
    wall_material.shininess = 0.0f;
    set_material(&wall_material);

    // Egy egységkockát rajzolunk, amit a glScalef méretez
    // Fontos, hogy a normálisok helyesek legyenek a megvilágításhoz.
    glBegin(GL_QUADS);
        // Front face (Z+)
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-0.5f, -0.5f, 0.5f); glVertex3f(0.5f, -0.5f, 0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
        // Back face (Z-)
        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glVertex3f(0.5f, 0.5f, -0.5f); glVertex3f(0.5f, -0.5f, -0.5f);
        // Top face (Y+)
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-0.5f, 0.5f, -0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f); glVertex3f(0.5f, 0.5f, -0.5f);
        // Bottom face (Y-)
        glNormal3f(0.0f, -1.0f, 0.0f);
        glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(0.5f, -0.5f, -0.5f);
        glVertex3f(0.5f, -0.5f, 0.5f); glVertex3f(-0.5f, -0.5f, 0.5f);
        // Right face (X+)
        glNormal3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.5f, -0.5f, -0.5f); glVertex3f(0.5f, 0.5f, -0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f); glVertex3f(0.5f, -0.5f, 0.5f);
        // Left face (X-)
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glEnd();
    
    glEnable(GL_TEXTURE_2D); // Visszakapcsoljuk, hátha a következő objektum textúrázott
    glPopMatrix();
}

bool check_collision(const Camera* camera, const Wall* walls)
{
    // Egyszerű AABB (Axis-Aligned Bounding Box) ütközésvizsgálat
    // A kamerát egy pontnak tekintjük egyelőre, vagy egy kis sugarú gömbnek.
    // A falak téglatestek.

    // Kamera bounding box-a (leegyszerűsítve egy pontra vagy kis kockára)
    float cam_radius = 0.3f; // Kamera "mérete" az ütközéshez
    float cam_min_x = camera->position.x - cam_radius;
    float cam_max_x = camera->position.x + cam_radius;
    float cam_min_y = camera->position.y - cam_radius;
    float cam_max_y = camera->position.y + cam_radius;
    float cam_min_z = camera->position.z - cam_radius; // Figyelembe véve a kamera magasságát
    float cam_max_z = camera->position.z + cam_radius;


    for (int i = 0; i < 4; i++) {
        const Wall* wall = &walls[i];
        
        // Fal bounding box-a
        float wall_min_x = wall->position.x - wall->size.x / 2.0f;
        float wall_max_x = wall->position.x + wall->size.x / 2.0f;
        float wall_min_y = wall->position.y - wall->size.y / 2.0f;
        float wall_max_y = wall->position.y + wall->size.y / 2.0f;
        float wall_min_z = wall->position.z - wall->size.z / 2.0f;
        float wall_max_z = wall->position.z + wall->size.z / 2.0f;

        // Ütközés ellenőrzése
        bool overlap_x = cam_max_x > wall_min_x && cam_min_x < wall_max_x;
        bool overlap_y = cam_max_y > wall_min_y && cam_min_y < wall_max_y;
        bool overlap_z = cam_max_z > wall_min_z && cam_min_z < wall_max_z;

        if (overlap_x && overlap_y && overlap_z) {
            return true; // Ütközés történt
        }
    }
    return false; // Nincs ütközés
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

    // ÚJ: Fák rajzolása ciklussal
    Material tree_material;
    tree_material.ambient.red = 0.1f; tree_material.ambient.green = 0.05f; tree_material.ambient.blue = 0.02f;
    tree_material.diffuse.red = 0.5f; tree_material.diffuse.green = 0.35f; tree_material.diffuse.blue = 0.2f;
    tree_material.specular.red = 0.05f; tree_material.specular.green = 0.05f; tree_material.specular.blue = 0.0f;
    tree_material.shininess = 5.0f;
    
    set_material(&tree_material); // Fa anyagának beállítása egyszer a ciklus előtt
    glDisable(GL_TEXTURE_2D);     // Textúrázás kikapcsolása a fákhoz

    for (int i = 0; i < NUM_TREES; ++i) {
        glPushMatrix();
        // Elmozdítjuk a fát a tárolt pozíciójára
        glTranslatef(
            scene->tree_positions[i].x,
            scene->tree_positions[i].y,
            scene->tree_positions[i].z
        );
        // Itt lehetne méretezni is, ha kellene: glScalef(meret_x, meret_y, meret_z);
        draw_model(&(scene->tree)); // Ugyanazt a fa modellt rajzoljuk ki
        glPopMatrix();
    }

    glEnable(GL_TEXTURE_2D); // Textúrázás visszakapcsolása
}

void draw_origin()
{
    // Fények és textúrázás kikapcsolása a tengelyvonalakhoz, hogy fix színűek legyenek
    GLboolean lighting_enabled;
    GLboolean texture_2d_enabled;
    glGetBooleanv(GL_LIGHTING, &lighting_enabled);
    glGetBooleanv(GL_TEXTURE_2D, &texture_2d_enabled);

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    glBegin(GL_LINES);
        // X tengely (piros)
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);

        // Y tengely (zöld)
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);

        // Z tengely (kék)
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();

    // Előző állapot visszaállítása
    if (lighting_enabled) glEnable(GL_LIGHTING);
    if (texture_2d_enabled) glEnable(GL_TEXTURE_2D);
    
    // Szín visszaállítása fehérre, hogy ne befolyásolja a következő rajzolásokat,
    // ha azok nem használnak anyagokat (pl. glColor után)
    glColor3f(1.0f, 1.0f, 1.0f);
}