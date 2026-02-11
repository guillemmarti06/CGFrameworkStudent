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

    //MULTIPLE ENTITIES
    
    //Create first entity, on the left
    e1 = new Entity();
    e1->base_position = Vector3(-1.5f, 0.8f, 0.0f);
    e1->base_scale = 1.3f;
    e1->speed = 1.3f;
    
    // use lee text/mesh for e1 aswell
    e1->mesh = lee_mesh;
    e1->texture = tex_lee;
    
    // Create second entity, same as first bit on the right
    e2 = new Entity();
    e2->base_position = Vector3(1.5f, 0.8f, 0.0f);
    e2->base_scale = 1.3f;
    e2->speed = 1.3f;
    
    // use anna mesh and text for second entity
    Mesh* mesh_anna = new Mesh();
    mesh_anna->LoadOBJ("mveshes/anna.obj");
    e2->mesh = mesh_anna;
    Image* tex_anna = new Image();
    tex_anna->LoadTGA("textures/anna_color_specular.tga", true);
    e2->texture = tex_anna;

    
    // Create third entity, a bit smaller, centered, and rotating slower than the other 2
    e3 = new Entity();
    e3->base_position = Vector3(0.0f, 0.5f, -1.0f);
    e3->base_scale = 1.0f;
    e3->speed = 0.8f;
    
    //use cleo mesh/text for third entity
    Mesh* mesh_cleo = new Mesh();
    mesh_cleo->LoadOBJ("meshes/cleo.obj");
    e3->mesh = mesh_cleo;
    Image* tex_cleo = new Image();
    tex_cleo->LoadTGA("textures/cleo_color_specular.tga", true);
    e3->texture = tex_cleo;
    
    // Camera init
    camera.type = Camera::PERSPECTIVE;
    camera.aspect = (float)framebuffer.width / (float)framebuffer.height;
    camera.fov = 60.0f * DEG2RAD;
    camera.near_plane = 0.3f;
    camera.far_plane = 40.0f;

    camera.center = Vector3(0, 1, 0);
    camera.up = Vector3(0, 1, 0);

    // orbit params (you must have added these in camera.h)
    camera.yaw = 0.0f;
    camera.pitch = 0.0f;
    camera.distance = 2.5f;

    // compute eye
    camera.eye.x = camera.center.x + cosf(camera.pitch) * sinf(camera.yaw) * camera.distance;
    camera.eye.y = camera.center.y + sinf(camera.pitch) * camera.distance;
    camera.eye.z = camera.center.z + cosf(camera.pitch) * cosf(camera.yaw) * camera.distance;

    camera.UpdateViewMatrix();
    camera.UpdateProjectionMatrix();
    camera.UpdateViewProjectionMatrix();
}

// Render one frame
void Application::Render()
{
    framebuffer.Fill(Color::BLACK);

    // Clear zbuffer
    if (zbuffer)
        zbuffer->Fill(1e9f);

    // Apply global interactivity to all entities
    // we use auto basically to avoid doing the same lines of code for each entity
    // in this way we can simplify the code, make it much more readable and shorter
    auto applySettings = [&](Entity* e)
    {
        if (!e) return;

        e->useTexture = useTexture;
        e->useZBuffer = useZBuffer;
        e->interpolateUV = interpolateUV;

        if (wireframe)
            e->mode = Entity::eRenderMode::WIREFRAME;
        else
            e->mode = interpolateUV ? Entity::eRenderMode::TRIANGLES_INTERPOLATED
                                    : Entity::eRenderMode::TRIANGLES;
    };

    applySettings(single);
    applySettings(e1);
    applySettings(e2);
    applySettings(e3);

    FloatImage* zb = useZBuffer ? zbuffer : NULL;

    // Now control the change between modes
    if (mode == 1)
    {
        if (single) single->Render(&framebuffer, &camera, zb);
    }
    else if (mode == 2)
    {
        if (e1) e1->Render(&framebuffer, &camera, zb);
        if (e2) e2->Render(&framebuffer, &camera, zb);
        if (e3) e3->Render(&framebuffer, &camera, zb);
    }

    framebuffer.Render();
}


// Called after render
void Application::Update(float seconds_elapsed)
{
    if (mode == 2)
    {
        if (e1) e1->Update(seconds_elapsed);
        if (e2) e2->Update(seconds_elapsed);
        if (e3) e3->Update(seconds_elapsed);
    }
}

//keyboard press event
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
    switch (event.keysym.sym)
    {
        case SDLK_ESCAPE:
            exit(0);
            break;

        // LAB 2 modes
        case SDLK_1:
            mode = 1; // SINGLE ENTITY
            break;

        case SDLK_2:
            mode = 2; // MULTIPLE ANIMATED ENTITIES
            break;

        // select property of the camer
        case SDLK_n:
            cam_prop = PROP_NEAR;
            break;

        case SDLK_f:
            cam_prop = PROP_FAR;
            break;

        case SDLK_v:
            cam_prop = PROP_FOV;
            break;
            
        // LAB 3 interactivity
        case SDLK_t:
            useTexture = !useTexture;
            break;
        
        case SDLK_z:
            useZBuffer = !useZBuffer;
            break;
        
        case SDLK_c:
            interpolateUV = !interpolateUV;
            break;
        
        case SDLK_w:
            wireframe = !wireframe;
            break;
            
        // increase
        case SDLK_PLUS:
        case SDLK_KP_PLUS:
        {
            if (cam_prop == PROP_NEAR)
                camera.near_plane += 0.1f;
            else if (cam_prop == PROP_FAR)
                camera.far_plane += 5.0f;
            else if (cam_prop == PROP_FOV)
                camera.fov += 5.0f * DEG2RAD;

            // clamps
            if (camera.near_plane < 0.05f)
                camera.near_plane = 0.05f;

            if (camera.far_plane < camera.near_plane + 1.0f)
                camera.far_plane = camera.near_plane + 1.0f;

            float minFov = 10.0f * DEG2RAD;
            float maxFov = 170.0f * DEG2RAD;
            if (camera.fov < minFov) camera.fov = minFov;
            if (camera.fov > maxFov) camera.fov = maxFov;

            camera.SetPerspective(camera.fov, camera.aspect, camera.near_plane, camera.far_plane);
            camera.UpdateViewProjectionMatrix();
        }
        break;

        // decrease
        case SDLK_MINUS:
        case SDLK_KP_MINUS:
        {
            if (cam_prop == PROP_NEAR)
                camera.near_plane -= 0.1f;
            else if (cam_prop == PROP_FAR)
                camera.far_plane -= 5.0f;
            else if (cam_prop == PROP_FOV)
                camera.fov -= 5.0f * DEG2RAD;

            // --- CLAMPS ---
            if (camera.near_plane < 0.05f)
                camera.near_plane = 0.05f;

            if (camera.far_plane < camera.near_plane + 1.0f)
                camera.far_plane = camera.near_plane + 1.0f;

            float minFov = 10.0f * DEG2RAD;
            float maxFov = 170.0f * DEG2RAD;
            if (camera.fov < minFov) camera.fov = minFov;
            if (camera.fov > maxFov) camera.fov = maxFov;

            camera.SetPerspective(camera.fov, camera.aspect, camera.near_plane, camera.far_plane);
            camera.UpdateViewProjectionMatrix();
        }
        break;
    }
}


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
