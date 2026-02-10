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

    // We use a persistent canvas image so the paint stays on screen between frames
    canvas.Resize(framebuffer.width, framebuffer.height);
    canvas.Fill(Color::BLACK);

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
    
    // Create one Entity with a mesh and a model matrix
    Entity* entity = new Entity();
    // Load a mesh from an OBJ
    Mesh* mesh = new Mesh();
    mesh->LoadOBJ("meshes/lee.obj");
    // Assign mesh to entity
    entity->mesh = mesh;
    // Assign a model matrix
    entity->model.MakeTranslationMatrix(0.0f, 0.0f, 0.0f);
    
    lee_mesh = new Mesh();
    lee_mesh->LoadOBJ("meshes/lee.obj");

    e1 = new Entity();
    e1->mesh = lee_mesh;
    e1->base_position = Vector3(-1.5f, 0.0f, 0.0f);
    e1->base_scale = 1.0f;
    e1->speed = 1.0f;

    e2 = new Entity();
    e2->mesh = lee_mesh;
    e2->base_position = Vector3(1.5f, 0.0f, 0.0f);
    e2->base_scale = 1.0f;
    e2->speed = 1.3f;

    e3 = new Entity();
    e3->mesh = lee_mesh;
    e3->base_position = Vector3(0.0f, -0.5f, -1.0f);
    e3->base_scale = 0.6f;
    e3->speed = 0.8f;
    
    // Camera init
    camera.type = Camera::PERSPECTIVE;
    camera.aspect = (float)framebuffer.width / (float)framebuffer.height;
    camera.fov = 60.0f * DEG2RAD;
    camera.near_plane = 0.1f;
    camera.far_plane = 1000.0f;

    camera.center = Vector3(0, 1, 0);
    camera.up = Vector3(0, 1, 0);

    // orbit params (you must have added these in camera.h)
    camera.yaw = 0.0f;
    camera.pitch = 0.0f;
    camera.distance = 5.0f;

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

    switch (mode)
    {
        case 0: // single entity wireframe
            if (single) single->Render(&framebuffer, &camera, Color::WHITE);
            break;

        case 1: // multiple animated entities
        case 2: // same but used to test interactivity
            if (e1) e1->Render(&framebuffer, &camera, Color::WHITE);
            if (e2) e2->Render(&framebuffer, &camera, Color::WHITE);
            if (e3) e3->Render(&framebuffer, &camera, Color::WHITE);
            break;
    }

    framebuffer.Render();
}



// Called after render
void Application::Update(float seconds_elapsed)
{
    if (mode == 1 || mode == 2)
    {
        if (e1) e1->Update(seconds_elapsed);
        if (e2) e2->Update(seconds_elapsed);
        if (e3) e3->Update(seconds_elapsed);
    }
}
//keyboard press event
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
    switch(event.keysym.sym)
    {
        case SDLK_ESCAPE: exit(0); break;

        // MENU: switch between exercises/results
        case SDLK_0: mode = 0; break; // single entity
        case SDLK_1: mode = 1; break; // multiple animated
        case SDLK_2: mode = 2; break; // interactivity test

        // Camera property selection
        case SDLK_n: cam_prop = PROP_NEAR; break;
        case SDLK_f: cam_prop = PROP_FAR; break;
        case SDLK_v: cam_prop = PROP_FOV; break;

        // Modify current property
        case SDLK_PLUS:
        case SDLK_KP_PLUS:
        {
            if (cam_prop == PROP_NEAR) camera.near_plane += 0.05f;
            else if (cam_prop == PROP_FAR) camera.far_plane += 1.0f;
            else if (cam_prop == PROP_FOV) camera.fov += 5.0f * DEG2RAD;

            if (camera.near_plane < 0.01f) camera.near_plane = 0.01f;
            if (camera.far_plane < camera.near_plane + 0.01f) camera.far_plane = camera.near_plane + 0.01f;

            camera.UpdateProjectionMatrix();
            camera.UpdateViewProjectionMatrix();
        } break;

        case SDLK_MINUS:
        case SDLK_KP_MINUS:
        {
            if (cam_prop == PROP_NEAR) camera.near_plane -= 0.05f;
            else if (cam_prop == PROP_FAR) camera.far_plane -= 1.0f;
            else if (cam_prop == PROP_FOV) camera.fov -= 5.0f * DEG2RAD;

            if (camera.near_plane < 0.01f) camera.near_plane = 0.01f;
            if (camera.far_plane < camera.near_plane + 0.01f) camera.far_plane = camera.near_plane + 0.01f;

            camera.UpdateProjectionMatrix();
            camera.UpdateViewProjectionMatrix();
        } break;
    }
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event)
{
    if (event.button != SDL_BUTTON_LEFT)
        return;
    
    // SDL mouse Y goes downwards, but our drawing ended up being upside-down,
    // so we flip Y here to match framebuffer coordinates.
    Vector2 mouse(event.x, (framebuffer.height - 1 - event.y));

    // 1- Check if we clicked a toolbar button
    Button* clicked = nullptr;

    if      (btn_pencil.IsMouseInside(mouse))   clicked = &btn_pencil;
    else if (btn_eraser.IsMouseInside(mouse))   clicked = &btn_eraser;
    else if (btn_line.IsMouseInside(mouse))     clicked = &btn_line;
    else if (btn_rect.IsMouseInside(mouse))     clicked = &btn_rect;
    else if (btn_triangle.IsMouseInside(mouse)) clicked = &btn_triangle;

    else if (btn_clear.IsMouseInside(mouse))    clicked = &btn_clear;
    else if (btn_load.IsMouseInside(mouse))     clicked = &btn_load;
    else if (btn_save.IsMouseInside(mouse))     clicked = &btn_save;

    else if (btn_black.IsMouseInside(mouse))    clicked = &btn_black;
    else if (btn_white.IsMouseInside(mouse))    clicked = &btn_white;
    else if (btn_red.IsMouseInside(mouse))      clicked = &btn_red;
    else if (btn_green.IsMouseInside(mouse))    clicked = &btn_green;
    else if (btn_blue.IsMouseInside(mouse))     clicked = &btn_blue;
    else if (btn_yellow.IsMouseInside(mouse))   clicked = &btn_yellow;
    else if (btn_cyan.IsMouseInside(mouse))     clicked = &btn_cyan;
    else if (btn_pink.IsMouseInside(mouse))     clicked = &btn_pink;

    if (clicked)
    {
        ButtonType t = clicked->type;

        // Tools
        if (t == BTN_PENCIL) currentTool = TOOL_PENCIL;
        else if (t == BTN_ERASER) currentTool = TOOL_ERASER;
        else if (t == BTN_LINE) currentTool = TOOL_LINE;
        else if (t == BTN_RECT) currentTool = TOOL_RECT;
        else if (t == BTN_TRIANGLE) currentTool = TOOL_TRIANGLE;

        // Actions
        else if (t == BTN_CLEAR) canvas.Fill(Color::BLACK);
        else if (t == BTN_LOAD)  canvas.LoadTGA("my_paint.tga");
        else if (t == BTN_SAVE)  canvas.SaveTGA("my_paint.tga");

        // Colors
        else if (t == BTN_COLOR_BLACK)  currentColor = Color(0,0,0);
        else if (t == BTN_COLOR_WHITE)  currentColor = Color(255,255,255);
        else if (t == BTN_COLOR_RED)    currentColor = Color(255,0,0);
        else if (t == BTN_COLOR_GREEN)  currentColor = Color(0,255,0);
        else if (t == BTN_COLOR_BLUE)   currentColor = Color(0,0,255);
        else if (t == BTN_COLOR_YELLOW) currentColor = Color(255,255,0);
        else if (t == BTN_COLOR_CYAN)   currentColor = Color(0,255,255);
        else if (t == BTN_COLOR_PINK)   currentColor = Color(255,0,255);

        return; // clicked UI -> don't draw
    }

    
    // TRIANGLE with 3 clicks
    if (currentTool == TOOL_TRIANGLE)
    {
        if (triangleClicks == 0)      { triA = mouse; triangleClicks = 1; }
        else if (triangleClicks == 1) { triB = mouse; triangleClicks = 2; }
        else // triangleClicks == 2
        {
            triC = mouse;

            // Draw triangle directly to the canvas
            canvas.DrawTriangle(triA, triB, triC, currentColor, fillShapes, currentColor);

            // Reset to start a new triangle
            triangleClicks = 0;
        }

        // do not start drag drawing
        isDrawing = false;
        return;
    }

    // 2- Start drawing on canvas
    isDrawing = true;
    startPos = mouse;
    lastPos = mouse;

    // Pencil/Eraser should paint immediately
    if (currentTool == TOOL_PENCIL)
        canvas.SetPixel((unsigned)mouse.x, (unsigned)mouse.y, currentColor);

    if (currentTool == TOOL_ERASER)
        canvas.SetPixel((unsigned)mouse.x, (unsigned)mouse.y, Color::BLACK);
}


void Application::OnMouseButtonUp(SDL_MouseButtonEvent event)
{
    if (event.button != SDL_BUTTON_LEFT)
        return;
    
    // Same Y flip as before (consistency is everything)
    Vector2 mouse(event.x, (framebuffer.height - 1 - event.y));

    if (!isDrawing)
        return;

    // When releasing the mouse, we commit the shape into the canvas
    if (currentTool == TOOL_LINE)
    {
        canvas.DrawLineDDA(startPos.x, startPos.y, mouse_position.x, mouse_position.y, currentColor);
    }
    else if (currentTool == TOOL_RECT)
    {
        int x = (int)std::min(startPos.x, mouse_position.x);
        int y = (int)std::min(startPos.y, mouse_position.y);
        int w = (int)std::abs(mouse_position.x - startPos.x);
        int h = (int)std::abs(mouse_position.y - startPos.y);

        canvas.DrawRect(x, y, w, h, currentColor, borderWidth, fillShapes, currentColor);
    }

    isDrawing = false;
}


void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
    // Keep mouse_position updated in the same coordinate system as drawing
    mouse_position = Vector2(event.x, (framebuffer.height - 1 - event.y));

    if (!isDrawing)
        return;

    // Pencil/eraser: paint continuously while dragging (we connect lastPos with the currentPos)
    if (currentTool == TOOL_PENCIL)
    {
        canvas.DrawLineDDA(lastPos.x, lastPos.y, mouse_position.x, mouse_position.y, currentColor);
        lastPos = mouse_position;
    }
    else if (currentTool == TOOL_ERASER)
    {
        canvas.DrawLineDDA(lastPos.x, lastPos.y, mouse_position.x, mouse_position.y, Color::BLACK);
        lastPos = mouse_position;
    }
}



void Application::OnWheel(SDL_MouseWheelEvent event)
{
	float dy = event.preciseY;

	// ...
}

void Application::OnFileChanged(const char* filename)
{ 
	Shader::ReloadSingleShader(filename);
}
