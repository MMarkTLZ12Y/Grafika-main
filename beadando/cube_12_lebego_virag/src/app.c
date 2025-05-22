#include "app.h"
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <GL/glu.h> // Keep for glFrustum, glOrtho etc. if used

void init_app(App* app, int width, int height)
{
    int error_code;
    int inited_loaders;

    app->is_running = false;

    error_code = SDL_Init(SDL_INIT_EVERYTHING);
    if (error_code != 0) {
        printf("[ERROR] SDL initialization error: %s\n", SDL_GetError());
        return;
    }

    app->window = SDL_CreateWindow(
        "Cube!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL);
    if (app->window == NULL) {
        printf("[ERROR] Unable to create the application window!\n");
        return;
    }

    inited_loaders = IMG_Init(IMG_INIT_PNG);
    if (inited_loaders == 0) {
        printf("[ERROR] IMG initialization error: %s\n", IMG_GetError());
        return;
    }

    app->gl_context = SDL_GL_CreateContext(app->window);
    if (app->gl_context == NULL) {
        printf("[ERROR] Unable to create the OpenGL context!\n");
        return;
    }

    init_opengl();
    reshape(width, height);
    init_camera(&(app->camera));
    init_scene(&(app->scene));
    app->uptime = (double)SDL_GetTicks() / 1000;
    app->is_running = true;
    SDL_SetRelativeMouseMode(SDL_TRUE);

    app->help_texture_id = load_texture("assets/textures/help.png");
    app->show_help = false;
}

void init_opengl()
{
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glEnable(GL_AUTO_NORMAL);
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glDisable(GL_CULL_FACE); // This was already here
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}

void reshape(GLsizei width, GLsizei height)
{
    int x, y, w, h;
    double ratio;

    ratio = (double)width / height;
    if (ratio > VIEWPORT_RATIO) {
        w = (int)((double)height * VIEWPORT_RATIO);
        h = height;
        x = (width - w) / 2;
        y = 0;
    }
    else {
        w = width;
        h = (int)((double)width / VIEWPORT_RATIO);
        x = 0;
        y = (height - h) / 2;
    }

    glViewport(x, y, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-.08, .08, -.06, .06, .1, 100);
}

void handle_app_events(App* app)
{
    SDL_Event event;
    static int mouse_x = 0;
    static int mouse_y = 0;
    int x;
    int y;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_ESCAPE:
                        app->is_running = false;
                        break;
                    case SDL_SCANCODE_W:
                        set_camera_speed(&(app->camera), 1);
                        break;
                    case SDL_SCANCODE_F1:
                        app->show_help = !app->show_help;
                        break;
                    case SDL_SCANCODE_S:
                        set_camera_speed(&(app->camera), -1);
                        break;
                    case SDL_SCANCODE_A:
                        set_camera_side_speed(&(app->camera), 1);
                        break;
                    case SDL_SCANCODE_D:
                        set_camera_side_speed(&(app->camera), -1);
                        break;
                    case SDL_SCANCODE_SPACE:
                        if (!app->camera.is_jumping) {
                            app->camera.is_jumping = true;
                            app->camera.vertical_speed = 5.75;
                        }
                        break;
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_W:
                    case SDL_SCANCODE_S:
                        set_camera_speed(&(app->camera), 0);
                        break;
                    case SDL_SCANCODE_A:
                    case SDL_SCANCODE_D:
                        set_camera_side_speed(&(app->camera), 0);
                        break;
                    default:
                        break;
                }
                break;
            case SDL_MOUSEMOTION:
                SDL_GetMouseState(&x, &y);
                rotate_camera(&(app->camera), mouse_x - x, mouse_y - y);
                mouse_x = x;
                mouse_y = y;
                break;
            case SDL_QUIT:
                app->is_running = false;
                break;
            default:
                break;
        }
    }
}

void render_help(App* app) {
    if (!app->show_help || app->help_texture_id == 0) return;

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, app->help_texture_id);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 800, 600, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(100.0f, 100.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(700.0f, 100.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(700.0f, 500.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(100.0f, 500.0f);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
}

void update_app(App* app)
{
    double current_time;
    double elapsed_time;
    vec3 old_position = app->camera.position;

    current_time = (double)SDL_GetTicks() / 1000;
    elapsed_time = current_time - app->uptime;
    app->uptime = current_time;

    update_camera(&(app->camera), elapsed_time);
    
    if (check_collision(&(app->camera), app->scene.walls)) {
        app->camera.position = old_position;
    }

    update_scene(&(app->scene), elapsed_time);
}

void render_app(App* app) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    set_view(&(app->camera));
    render_scene(&(app->scene), &(app->camera)); // Pass camera to render_scene
    glPopMatrix();

    render_help(app);

    SDL_GL_SwapWindow(app->window);
}

void destroy_app(App* app)
{
    if (app->help_texture_id != 0) {
        glDeleteTextures(1, &app->help_texture_id);
    }
    // ÚJ: Virág textúra törlése
    if (app->scene.flower_texture_id != 0) {
        glDeleteTextures(1, &(app->scene.flower_texture_id));
    }

    if (app->gl_context != NULL) {
        SDL_GL_DeleteContext(app->gl_context);
    }
    if (app->window != NULL) {
        SDL_DestroyWindow(app->window);
    }
    SDL_Quit();
}