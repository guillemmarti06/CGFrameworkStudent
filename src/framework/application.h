/*  
	+ This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#pragma once

#include "main/includes.h"
#include "framework.h"
#include "image.h"
#include "button.h"
#include "ParticleSystem.h"
#include "mesh.h"
#include "camera.h"
#include "entity.h"

class Application
{
public:

	// Window

	SDL_Window* window = nullptr;
	int window_width;
	int window_height;

	float time;
    int borderWidth = 5;   // initial border thickness (for example)
    bool fillShapes = true;  // toggled with F
    
    // Paint tool state

    Image canvas;                 // Persistent drawing canvas (what the user paints)

    bool isDrawing = false;       // True while mouse is pressed (dragging)
    Vector2 startPos;             // First click position
    Vector2 lastPos;              // Last mouse position (for pencil/eraser)

    // Current selected tool
    enum ToolType { TOOL_PENCIL, TOOL_ERASER, TOOL_LINE, TOOL_RECT, TOOL_TRIANGLE };
    ToolType currentTool = TOOL_PENCIL;
    
    // Triangle (3 clicks)
    int triangleClicks = 0;
    Vector2 triA, triB, triC;


    Color currentColor = Color(255, 255, 255); // Default drawing color (white)

    // UI buttons (toolbar)
    Button btn_pencil, btn_eraser, btn_line, btn_rect, btn_triangle;
    Button btn_clear, btn_load, btn_save;
    Button btn_black, btn_white, btn_red, btn_green, btn_blue, btn_yellow, btn_cyan, btn_pink;

    
    // Icons persistent
    Image img_pencil, img_eraser, img_line, img_rect, img_tri;
    Image img_clear, img_load, img_save;
    Image img_black, img_white, img_red, img_green, img_blue, img_yellow, img_cyan, img_pink;
    
    // for the animation
    ParticleSystem particleSystem;


	// Input
	const Uint8* keystate;
	int mouse_state; // Tells which buttons are pressed
	Vector2 mouse_position; // Last mouse position
	Vector2 mouse_delta; // Mouse movement in the last frame

	void OnKeyPressed(SDL_KeyboardEvent event);
	void OnMouseButtonDown(SDL_MouseButtonEvent event);
	void OnMouseButtonUp(SDL_MouseButtonEvent event);
	void OnMouseMove(SDL_MouseButtonEvent event);
	void OnWheel(SDL_MouseWheelEvent event);
	void OnFileChanged(const char* filename);

	// CPU Global framebuffer
	Image framebuffer;

	// Constructor and main methods
	Application(const char* caption, int width, int height);
	~Application();

	void Init( void );
	void Render( void );
	void Update( float dt );

	// Other methods to control the app
	void SetWindowSize(int width, int height) {
		glViewport( 0,0, width, height );
		this->window_width = width;
		this->window_height = height;
		this->framebuffer.Resize(width, height);
        
        camera.aspect = (float)width / (float)height;
        camera.SetPerspective(camera.fov, camera.aspect, camera.near_plane, camera.far_plane);
        camera.UpdateViewProjectionMatrix();
	}

	Vector2 GetWindowSize()
	{
		int w,h;
		SDL_GetWindowSize(window,&w,&h);
		return Vector2(float(w), float(h));
	}
    
    Entity* single = nullptr;
    Entity* e1 = nullptr;
    Entity* e2 = nullptr;
    Entity* e3 = nullptr;
    Mesh* lee_mesh = nullptr;
    
    Camera camera;
    int mode = 1; // start with single (mode 1)

    bool orbiting = false; // LMB
    bool panning  = false; // RMB
    Vector2 last_mouse;

    // Camera property to edit with N/F/V and +/- (simple)
    enum CameraProp { PROP_NEAR, PROP_FAR, PROP_FOV };
    CameraProp cam_prop = PROP_NEAR;
    
    FloatImage* zbuffer = NULL;
    
    // Lab3 interactivity
    bool useTexture = true;      // T
    bool useZBuffer = true;      // Z
    bool interpolateUV = true;   // C
};
