#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 

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

    // Create a persistent canvas with the same size as the framebuffer
    canvas.Resize(framebuffer.width, framebuffer.height);
    canvas.Fill(Color::BLACK);

    // Load button icons from res/images/ (as described in the lab). :contentReference[oaicite:18]{index=18}
    Image img_pencil;   img_pencil.LoadPNG("res/images/pencil.png");
    Image img_eraser;   img_eraser.LoadPNG("res/images/eraser.png");
    Image img_line;     img_line.LoadPNG("res/images/line.png");
    Image img_rect;     img_rect.LoadPNG("res/images/rectangle.png");
    Image img_tri;      img_tri.LoadPNG("res/images/triangle.png");
    Image img_clear;    img_clear.LoadPNG("res/images/clear.png");
    Image img_load;     img_load.LoadPNG("res/images/load.png");
    Image img_save;     img_save.LoadPNG("res/images/save.png");

    Image img_black;    img_black.LoadPNG("res/images/black.png");
    Image img_white;    img_white.LoadPNG("res/images/white.png");
    Image img_red;      img_red.LoadPNG("res/images/red.png");
    Image img_green;    img_green.LoadPNG("res/images/green.png");
    Image img_blue;     img_blue.LoadPNG("res/images/blue.png");
    Image img_yellow;   img_yellow.LoadPNG("res/images/yellow.png");
    Image img_cyan;     img_cyan.LoadPNG("res/images/cyan.png");
    Image img_pink;     img_pink.LoadPNG("res/images/pink.png");

    // Toolbar positioning: place icons at the bottom of the screen
    int y = (int)framebuffer.height - 50; // simple fixed bar height
    int x = 10;
    int step = 45;

    buttons.clear();
    buttons.push_back(Button(img_pencil, Vector2((float)x, (float)y), BTN_PENCIL)); x += step;
    buttons.push_back(Button(img_eraser, Vector2((float)x, (float)y), BTN_ERASER)); x += step;
    buttons.push_back(Button(img_line,   Vector2((float)x, (float)y), BTN_LINE));   x += step;
    buttons.push_back(Button(img_rect,   Vector2((float)x, (float)y), BTN_RECT));   x += step;
    buttons.push_back(Button(img_tri,    Vector2((float)x, (float)y), BTN_TRIANGLE)); x += step;

    x += 20; // small gap
    buttons.push_back(Button(img_clear,  Vector2((float)x, (float)y), BTN_CLEAR)); x += step;
    buttons.push_back(Button(img_load,   Vector2((float)x, (float)y), BTN_LOAD));  x += step;
    buttons.push_back(Button(img_save,   Vector2((float)x, (float)y), BTN_SAVE));  x += step;

    x += 20; // colors group
    buttons.push_back(Button(img_black,  Vector2((float)x, (float)y), BTN_COLOR_BLACK)); x += step;
    buttons.push_back(Button(img_white,  Vector2((float)x, (float)y), BTN_COLOR_WHITE)); x += step;
    buttons.push_back(Button(img_red,    Vector2((float)x, (float)y), BTN_COLOR_RED)); x += step;
    buttons.push_back(Button(img_green,  Vector2((float)x, (float)y), BTN_COLOR_GREEN)); x += step;
    buttons.push_back(Button(img_blue,   Vector2((float)x, (float)y), BTN_COLOR_BLUE)); x += step;
    buttons.push_back(Button(img_yellow, Vector2((float)x, (float)y), BTN_COLOR_YELLOW)); x += step;
    buttons.push_back(Button(img_cyan,   Vector2((float)x, (float)y), BTN_COLOR_CYAN)); x += step;
    buttons.push_back(Button(img_pink,   Vector2((float)x, (float)y), BTN_COLOR_PINK)); x += step;
}

// Render one frame
void Application::Render()
{
    framebuffer.Fill(Color::BLACK);

    // 1) Draw what the user has already painted
    framebuffer.DrawImage(canvas, 0, 0);

    // 2) If user is dragging, show a preview for shapes (line/rect/triangle)
    if (app_mode == 1 && isDrawing)
    {
        if (currentTool == TOOL_LINE)
        {
            framebuffer.DrawLineDDA((int)startPos.x, (int)startPos.y, (int)mouse_position.x, (int)mouse_position.y, currentColor);
        }
        else if (currentTool == TOOL_RECT)
        {
            int x = (int)std::min(startPos.x, mouse_position.x);
            int y = (int)std::min(startPos.y, mouse_position.y);
            int w = (int)std::abs(mouse_position.x - startPos.x);
            int h = (int)std::abs(mouse_position.y - startPos.y);

            framebuffer.DrawRect(x, y, w, h, currentColor, borderWidth, fillShapes, currentColor);
        }
        else if (currentTool == TOOL_TRIANGLE)
        {
            // Simple triangle preview: base is start->mouse, third point fixed above the base midpoint
            Vector2 a = startPos;
            Vector2 b = mouse_position;
            Vector2 mid((a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f);
            Vector2 c(mid.x, mid.y - 80.0f);

            framebuffer.DrawTriangle(a, b, c, Color(255,255,255), fillShapes, currentColor);
        }
    }

    // 3) Draw toolbar buttons at the bottom
    for (const Button& b : buttons)
        b.Render(framebuffer);

    framebuffer.Render();
}


// Called after render
void Application::Update(float seconds_elapsed)
{
    
}

//keyboard press event 
void Application::OnKeyPressed( SDL_KeyboardEvent event )
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch(event.keysym.sym) {
		case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app
            
        case SDLK_1: mode = 1; break; // Paint
        case SDLK_2: mode= 2; break; // Animation
            
        // Increase border width
        case SDLK_PLUS:
        case SDLK_KP_PLUS:
            borderWidth++;
            break;

        // Decrease border width
        case SDLK_MINUS:
        case SDLK_KP_MINUS:
            if (borderWidth > 1)
                borderWidth--;
            break;
            
        // Fill Shapes toggle
        case SDLK_f:
            fillShapes = !fillShapes;
            break;
	}
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event)
{
    if (event.button != SDL_BUTTON_LEFT)
        return;

    Vector2 mouse((float)event.x, (float)event.y);

    // 1) Check if we clicked a toolbar button
    for (const Button& b : buttons)
    {
        if (b.IsMouseInside(mouse))
        {
            // Handle button action
            if (b.type == BTN_PENCIL) currentTool = TOOL_PENCIL;
            else if (b.type == BTN_ERASER) currentTool = TOOL_ERASER;
            else if (b.type == BTN_LINE) currentTool = TOOL_LINE;
            else if (b.type == BTN_RECT) currentTool = TOOL_RECT;
            else if (b.type == BTN_TRIANGLE) currentTool = TOOL_TRIANGLE;
            else if (b.type == BTN_CLEAR) canvas.Fill(Color::BLACK);
            else if (b.type == BTN_LOAD)  canvas.LoadPNG("res/images/fruits.png"); // simple demo load
            else if (b.type == BTN_SAVE)  canvas.SaveTGA("my_paint.tga");          // simple demo save

            // Color selection
            else if (b.type == BTN_COLOR_BLACK)  currentColor = Color(0,0,0);
            else if (b.type == BTN_COLOR_WHITE)  currentColor = Color(255,255,255);
            else if (b.type == BTN_COLOR_RED)    currentColor = Color(255,0,0);
            else if (b.type == BTN_COLOR_GREEN)  currentColor = Color(0,255,0);
            else if (b.type == BTN_COLOR_BLUE)   currentColor = Color(0,0,255);
            else if (b.type == BTN_COLOR_YELLOW) currentColor = Color(255,255,0);
            else if (b.type == BTN_COLOR_CYAN)   currentColor = Color(0,255,255);
            else if (b.type == BTN_COLOR_PINK)   currentColor = Color(255,0,255);

            return; // IMPORTANT: do not start drawing if we clicked UI
        }
    }

    // 2) Start drawing on canvas
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

    if (!isDrawing)
        return;

    // When releasing the mouse, we "commit" the shape into the canvas
    if (currentTool == TOOL_LINE)
    {
        canvas.DrawLineDDA((int)startPos.x, (int)startPos.y, (int)mouse_position.x, (int)mouse_position.y, currentColor);
    }
    else if (currentTool == TOOL_RECT)
    {
        int x = (int)std::min(startPos.x, mouse_position.x);
        int y = (int)std::min(startPos.y, mouse_position.y);
        int w = (int)std::abs(mouse_position.x - startPos.x);
        int h = (int)std::abs(mouse_position.y - startPos.y);

        canvas.DrawRect(x, y, w, h, currentColor, borderWidth, fillShapes, currentColor);
    }
    else if (currentTool == TOOL_TRIANGLE)
    {
        Vector2 a = startPos;
        Vector2 b = mouse_position;
        Vector2 mid((a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f);
        Vector2 c(mid.x, mid.y - 80.0f);

        canvas.DrawTriangle(a, b, c, Color(255,255,255), fillShapes, currentColor);
    }

    isDrawing = false;
}


void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
    mouse_position = Vector2((float)event.x, (float)event.y);

    if (!isDrawing)
        return;

    // Pencil/eraser: draw small line segments while dragging (paint-like)
    if (currentTool == TOOL_PENCIL)
    {
        canvas.DrawLineDDA((int)lastPos.x, (int)lastPos.y, (int)mouse_position.x, (int)mouse_position.y, currentColor);
        lastPos = mouse_position;
    }
    else if (currentTool == TOOL_ERASER)
    {
        canvas.DrawLineDDA((int)lastPos.x, (int)lastPos.y, (int)mouse_position.x, (int)mouse_position.y, Color::BLACK);
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
