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
// Switch between modes
    
    switch(mode)
    {
        case 1: // Paint
            // mouse input → draw primitives
            break;

        case 2: // Animation
            // particle system, animation, etc.
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
