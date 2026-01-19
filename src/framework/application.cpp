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
}

// Render one frame
void Application::Render(void)
{
// TRY OUT the draw line function (IT WORKS WELL)
    
    framebuffer.Fill(Color::BLACK);
    int x = framebuffer.width / 2;
    int y = framebuffer.height / 2;
    Color color(255, 255, 255);
    
    switch(current_task)
        {
            case 1: // Task 2.1.1 (Drawing Lines)
                framebuffer.DrawLineDDA(x, y, x + 100 * cos(time), y + 100 * sin(time), color);
                break;

            case 2: // Task 2.1.2 (Drawing Rectangles)
                break;

            case 3: // Task 2.
                break;
        }

     

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
        case SDLK_1: current_task = 1; break; // DDA line
        case SDLK_2: current_task = 2; break; // next task
        case SDLK_3: current_task = 3; break; // next task
	}
}

void Application::OnMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {

	}
}

void Application::OnMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {

	}
}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
	
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
