#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h"
#include "camera.h"
#include "entity.h"

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	int w,h;
	SDL_GetWindowSize(window,&w,&h);
    
	this->mouse_state = 0;
	this->time = 0.f;
	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(nullptr);

	this->framebuffer.Resize(w, h);
}

Application::~Application()
{
}

void Application::Init(void)
{
    std::cout << "Initiating app..." << std::endl;
    
    // ALL THESE IMAGES AND ANIMATION IS FROM LAB1!! BUT WE KEPT IT (JUST INITIALIZED, it does not disturb)

    // We use a persistent canvas image so the paint stays on screen between frames
    canvas.Resize(framebuffer.width, framebuffer.height);
    canvas.Fill(Color::BLACK);
    
    zbuffer = new FloatImage();
    zbuffer->Resize(window_width, window_height);


    // Load UI icons (stored inside /res/images)
    // here we pass "images/..." (NOT "res/images/...") because utils already adds /res.
    img_pencil.LoadPNG("images/pencil.png");
    img_eraser.LoadPNG("images/eraser.png");
    img_line.LoadPNG("images/line.png");
    img_rect.LoadPNG("images/rectangle.png");
    img_tri.LoadPNG("images/triangle.png");
    img_clear.LoadPNG("images/clear.png");
    img_load.LoadPNG("images/load.png");
    img_save.LoadPNG("images/save.png");

    img_black.LoadPNG("images/black.png");
    img_white.LoadPNG("images/white.png");
    img_red.LoadPNG("images/red.png");
    img_green.LoadPNG("images/green.png");
    img_blue.LoadPNG("images/blue.png");
    img_yellow.LoadPNG("images/yellow.png");
    img_cyan.LoadPNG("images/cyan.png");
    img_pink.LoadPNG("images/pink.png");

    // Toolbar positioning: place icons at the bottom of the screen
    // We decided to put buttons on the "bottom" of our canvas coordinates system (as in the example of the guidelines)
    // Because we flip mouse Y (SDL vs framebuffer), y=10 ends up being "down" for our paint coords.
    int y = 10;
    int x = 10;
    int step = 45;

    // Tools
    btn_pencil   = Button(img_pencil, Vector2(x, y), BTN_PENCIL);   x += step;
    btn_eraser   = Button(img_eraser, Vector2(x, y), BTN_ERASER);   x += step;
    btn_line     = Button(img_line,   Vector2(x, y), BTN_LINE);     x += step;
    btn_rect     = Button(img_rect,   Vector2(x, y), BTN_RECT);     x += step;
    btn_triangle = Button(img_tri,    Vector2(x, y), BTN_TRIANGLE); x += step;

    x += 20; // gap

    // Utilities
    btn_clear = Button(img_clear, Vector2(x, y), BTN_CLEAR); x += step;
    btn_load  = Button(img_load,  Vector2(x, y), BTN_LOAD);  x += step;
    btn_save  = Button(img_save,  Vector2(x, y), BTN_SAVE);  x += step;

    x += 20; // gap

    // Colors
    btn_black  = Button(img_black,  Vector2(x, y), BTN_COLOR_BLACK);  x += step;
    btn_white  = Button(img_white,  Vector2(x, y), BTN_COLOR_WHITE);  x += step;
    btn_red    = Button(img_red,    Vector2(x, y), BTN_COLOR_RED);    x += step;
    btn_green  = Button(img_green,  Vector2(x, y), BTN_COLOR_GREEN);  x += step;
    btn_blue   = Button(img_blue,   Vector2(x, y), BTN_COLOR_BLUE);   x += step;
    btn_yellow = Button(img_yellow, Vector2(x, y), BTN_COLOR_YELLOW); x += step;
    btn_cyan   = Button(img_cyan,   Vector2(x, y), BTN_COLOR_CYAN);   x += step;
    btn_pink   = Button(img_pink,   Vector2(x, y), BTN_COLOR_PINK);   x += step;

    
    // Animation system init (we want it ready even if we start in paint mode)
    particleSystem.Init(framebuffer.width, framebuffer.height);
    
    single = new Entity();
    // we adjusted to 0.8 so that when you zoom you still see the shape correctly
    single->model.MakeTranslationMatrix(0.0f, 0.8f, 0.0f);
    
    // use lee text/mesh for single
    Mesh* lee_mesh = new Mesh();
    lee_mesh->LoadOBJ("meshes/lee.obj");
    single->mesh = lee_mesh;
    Image* tex_lee = new Image();
    tex_lee->LoadTGA("textures/lee_color_specular.tga", true);
    single->texture = tex_lee;
    //Lab5
    single->material = new Material();
    single->material->shader = Shader::Get("shaders/raster.vs", "shaders/raster.fs");
    single->material->color_texture = Texture::Get("textures/lee_color_specular.tga");

    //MULTIPLE ENTITIES
    
    //Create first entity, on the left
    e1 = new Entity();
    e1->base_position = Vector3(-1.0f, 0.8f, 0.0f);
    e1->base_scale = 1.5f;
    e1->speed = 0.5f;
    
    // use lee text/mesh for e1 aswell
    e1->mesh = lee_mesh;
    e1->texture = tex_lee;
    e1->material = new Material();
    e1->material->shader = Shader::Get("shaders/raster.vs", "shaders/raster.fs");
    e1->material->color_texture = Texture::Get("textures/lee_color_specular.tga");
    
    // Create second entity, same as first bit on the right
    e2 = new Entity();
    e2->base_position = Vector3(1.0f, 0.8f, 0.0f);
    e2->base_scale = 1.5f;
    e2->speed = 0.5f;
    
    // use anna mesh and text for second entity
    Mesh* mesh_anna = new Mesh();
    mesh_anna->LoadOBJ("meshes/anna.obj");
    e2->mesh = mesh_anna;
    Image* tex_anna = new Image();
    tex_anna->LoadTGA("textures/anna_color_specular.tga", true);
    e2->texture = tex_anna;
    e2->material = new Material();
    e2->material->shader = Shader::Get("shaders/raster.vs", "shaders/raster.fs");
    e2->material->color_texture = Texture::Get("textures/anna_color_specular.tga");

    
    // Create third entity, a bit smaller, centered, and rotating slower than the other 2
    e3 = new Entity();
    e3->base_position = Vector3(0.0f, 0.5f, -1.0f);
    e3->base_scale = 1.5f;
    e3->speed = 0.3f;
    
    //use cleo mesh/text for third entity
    Mesh* mesh_cleo = new Mesh();
    mesh_cleo->LoadOBJ("meshes/cleo.obj");
    e3->mesh = mesh_cleo;
    Image* tex_cleo = new Image();
    tex_cleo->LoadTGA("textures/cleo_color_specular.tga", true);
    e3->texture = tex_cleo;
    e3->material = new Material();
    e3->material->shader = Shader::Get("shaders/raster.vs", "shaders/raster.fs");
    e3->material->color_texture = Texture::Get("textures/cleo_color_specular.tga");
    
    // Camera init, set the values
    camera.type = Camera::PERSPECTIVE;
    camera.aspect = (float)framebuffer.width / (float)framebuffer.height;
    camera.fov = 60.0f * DEG2RAD;
    camera.near_plane = 0.3f;
    camera.far_plane = 40.0f;

    camera.center = Vector3(0, 1, 0);
    camera.up = Vector3(0, 1, 0);

    // orbit params
    camera.yaw = 0.0f;
    camera.pitch = 0.0f;
    camera.distance = 2.0f;

    // compute eye
    camera.eye.x = camera.center.x + cosf(camera.pitch) * sinf(camera.yaw) * camera.distance;
    camera.eye.y = camera.center.y + sinf(camera.pitch) * camera.distance;
    camera.eye.z = camera.center.z + cosf(camera.pitch) * cosf(camera.yaw) * camera.distance;

    camera.UpdateViewMatrix();
    camera.UpdateProjectionMatrix();
    camera.UpdateViewProjectionMatrix();
    
    // LAB 4: full screen quad
    quad = new Mesh();
    quad->CreateQuad();
    shader = Shader::Get("shaders/quad.vs", "shaders/quad.fs");
    image_texture = Texture::Get("images/fruits.png");
}

void Application::Render()
{
    // Lab 5 placeholder
    if (current_lab == 5)
    {
        glDisable(GL_DEPTH_TEST);

        if (!shader || !quad)
            return;

        shader->Enable();
        shader->SetFloat("u_mode", 0.0f);
        shader->SetFloat("u_aspect", (float)window_width / (float)window_height);
        shader->SetFloat("u_time", time);

        if (image_texture)
            shader->SetTexture("u_texture", image_texture);

        quad->Render();
        shader->Disable();
        return;
    }

    // Task 4: GPU mesh
    if (current_task == 4)
    {
        glEnable(GL_DEPTH_TEST);

        if (single)
            single->Render(&camera);

        return;
    }

    // Tasks 1, 2, 3: full screen quad
    glDisable(GL_DEPTH_TEST);

    if (!shader || !quad)
        return;

    float mode = 0.0f;

    if (current_task == 1)
    {
        mode = (float)current_subtask; // 0..5
    }
    else if (current_task == 2)
    {
        mode = 6.0f + (float)current_subtask; // 6..11
    }
    else if (current_task == 3)
    {
        if (current_subtask == 0)
            mode = 12.0f; // a
        else if (current_subtask == 1)
            mode = 13.0f; // b
        else if (current_subtask == 2)
            mode = 12.0f; // c
        else if (current_subtask == 3)
            mode = 13.0f; // d
        else if (current_subtask == 4)
            mode = 12.0f; // e
        else
            mode = 13.0f; // f
    }

    shader->Enable();
    shader->SetFloat("u_mode", mode);
    shader->SetFloat("u_aspect", (float)window_width / (float)window_height);
    shader->SetFloat("u_time", time);

    if (image_texture)
        shader->SetTexture("u_texture", image_texture);

    quad->Render();
    shader->Disable();
}

// Called after render
void Application::Update(float seconds_elapsed)
{
    time += seconds_elapsed;
}

//keyboard press event
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
    switch (event.keysym.sym)
    {
        case SDLK_ESCAPE:
            exit(0);
            break;

        // Show Tasks 1 to 4
        case SDLK_1:
            current_task = 1;
            break;

        case SDLK_2:
            current_task = 2;
            break;

        case SDLK_3:
            current_task = 3;
            break;

        case SDLK_4:
            current_task = 4;
            break;

        // Show subtasks a to f for Tasks 1, 2, 3
        case SDLK_a:
            current_subtask = 0;
            break;

        case SDLK_b:
            current_subtask = 1;
            break;

        case SDLK_c:
            current_subtask = 2;
            break;

        case SDLK_d:
            current_subtask = 3;
            break;

        case SDLK_e:
            current_subtask = 4;
            break;

        case SDLK_f:
            current_subtask = 5;
            break;

        // Change between lab 4 and lab 5 scenes
        case SDLK_l:
            if (current_lab == 4)
                current_lab = 5;
            else
                current_lab = 4;
            break;
    }
}

// Control the mouse button up/down with simple booleans

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event)
{
    last_mouse = Vector2((float)event.x, (float)event.y);

    if (event.button == SDL_BUTTON_LEFT)
        orbiting = true;

    if (event.button == SDL_BUTTON_RIGHT)
        panning = true;
}



void Application::OnMouseButtonUp(SDL_MouseButtonEvent event)
{
    if (event.button == SDL_BUTTON_LEFT)
        orbiting = false;

    if (event.button == SDL_BUTTON_RIGHT)
        panning = false;
}



void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
    Vector2 mouse((float)event.x, (float)event.y);
    Vector2 delta = mouse - last_mouse;
    last_mouse = mouse;

    // Sensitivity (simple)
    const float rot_speed = 0.005f;
    const float pan_speed = 0.01f;

    if (orbiting)
    {
        camera.yaw   -= delta.x * rot_speed;
        camera.pitch -= delta.y * rot_speed;

        // Clamp pitch to avoid flip
        if (camera.pitch >  1.5f) camera.pitch =  1.5f;
        if (camera.pitch < -1.5f) camera.pitch = -1.5f;
    }

    if (panning)
    {
        // Build camera right & up (based on the current view)
        Vector3 forward = (camera.center - camera.eye);
        forward.Normalize();

        Vector3 right = forward.Cross(camera.up);
        right.Normalize();

        Vector3 up2 = right.Cross(forward);
        up2.Normalize();

        Vector3 move;

        move = right;
        move = move * (-delta.x * pan_speed);
        camera.center = camera.center + move;

        move = up2;
        move = move * ( delta.y * pan_speed);
        camera.center = camera.center + move;
    }

    // Recompute eye from yaw/pitch/distance around center
    if (orbiting || panning)
    {
        camera.eye.x = camera.center.x + cosf(camera.pitch) * sinf(camera.yaw) * camera.distance;
        camera.eye.y = camera.center.y + sinf(camera.pitch) * camera.distance;
        camera.eye.z = camera.center.z + cosf(camera.pitch) * cosf(camera.yaw) * camera.distance;

        camera.UpdateViewMatrix();
        camera.UpdateViewProjectionMatrix();
    }
}


void Application::OnWheel(SDL_MouseWheelEvent event)
{
    // wheel.y: +1 forward, -1 backward
    // here we are basically computing the zoom of the wheel of the mouse
    float zoom_factor = 1.0f - event.y * 0.1f;
    camera.distance *= zoom_factor;

    if (camera.distance < 0.5f) camera.distance = 0.5f;
    if (camera.distance > 50.0f) camera.distance = 50.0f;

    camera.eye.x = camera.center.x + cosf(camera.pitch) * sinf(camera.yaw) * camera.distance;
    camera.eye.y = camera.center.y + sinf(camera.pitch) * camera.distance;
    camera.eye.z = camera.center.z + cosf(camera.pitch) * cosf(camera.yaw) * camera.distance;

    camera.UpdateViewMatrix();
    camera.UpdateViewProjectionMatrix();
}


void Application::OnFileChanged(const char* filename)
{ 
	Shader::ReloadSingleShader(filename);
}
