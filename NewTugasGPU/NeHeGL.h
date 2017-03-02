/********************
*                   *
*   NeHeGL Header   *
*                   *
**********************************************************************************
*                                                                                *
*	You Need To Provide The Following Functions:                                 *
*                                                                                *
*	BOOL Initialize (GL_Window* window, Keys* keys);                             *
*		Performs All Your Initialization                                         *
*		Returns TRUE If Initialization Was Successful, FALSE If Not              *
*		'window' Is A Parameter Used In Calls To NeHeGL                          *
*		'keys' Is A Structure Containing The Up/Down Status Of keys              *
*                                                                                *
*	void Deinitialize (void);                                                    *
*		Performs All Your DeInitialization                                       *
*                                                                                *
*	void Update (DWORD milliseconds);                                            *
*		Perform Motion Updates                                                   *
*		'milliseconds' Is The Number Of Milliseconds Passed Since The Last Call  *
*		With Whatever Accuracy GetTickCount() Provides                           *
*                                                                                *
*	void Draw (void);                                                            *
*		Perform All Your Scene Drawing                                           *
*                                                                                *
*********************************************************************************/

#ifndef GL_FRAMEWORK__INCLUDED
#define GL_FRAMEWORK__INCLUDED

#include <windows.h>								// Header File For Windows

typedef struct {									// Structure For Keyboard Stuff
	BOOL keyDown[256];								// Holds TRUE / FALSE For Each Key
} Keys;												// Keys

typedef struct {									// Contains Information Vital To Applications
	HINSTANCE		hInstance;						// Application Instance
	const char*		className;						// Application ClassName
} Application;										// Application

typedef struct {									// Window Creation Info
	Application*		application;				// Application Structure
	char*				title;						// Window Title
	int					width;						// Width
	int					height;						// Height
	int					bitsPerPixel;				// Bits Per Pixel
	BOOL				isFullScreen;				// FullScreen?
} GL_WindowInit;									// GL_WindowInit

typedef struct {									// Contains Information Vital To A Window
	Keys*				keys;						// Key Structure
	HWND				hWnd;						// Window Handle
	HDC					hDC;						// Device Context
	HGLRC				hRC;						// Rendering Context
	GL_WindowInit		init;						// Window Init
	BOOL				isVisible;					// Window Visible?
	DWORD				lastTickCount;				// Tick Counter
} GL_Window;										// GL_Window

void TerminateApplication(GL_Window* window);		// Terminate The Application

void ToggleFullscreen(GL_Window* window);			// Toggle Fullscreen / Windowed Mode

													// These Are The Function You Must Provide
BOOL Initialize(GL_Window* window, Keys* keys);	// Performs All Your Initialization
void Deinitialize(void);							// Performs All Your DeInitialization
void Update(DWORD milliseconds);					// Perform Motion Updates
void Draw(void);									// Perform All Your Scene Drawing
void Selection(void);								// Perform Selection

extern int mouse_x;
extern int mouse_y;

//lesson 35
BOOL Initialize35(GL_Window* window, Keys* keys);	// Performs All Your Initialization
void Deinitialize35(void);							// Performs All Your DeInitialization
void Update35(DWORD milliseconds);					// Perform Motion Updates
void Draw35(void);									// Perform All Your Scene Drawing

//lesson 36										//lesson 35
BOOL Initialize36(GL_Window* window, Keys* keys);	// Performs All Your Initialization
void Deinitialize36(void);							// Performs All Your DeInitialization
void Update36(DWORD milliseconds);					// Perform Motion Updates
void Draw36(void);									// Perform All Your Scene Drawing

//lesson 37
BOOL Initialize37(GL_Window* window, Keys* keys);	// Performs All Your Initialization
void Deinitialize37(void);							// Performs All Your DeInitialization
void Update37(DWORD milliseconds);					// Perform Motion Updates
void Draw37(void);								// Perform All Your Scene Drawing

//lesson 38
BOOL Initialize38(GL_Window* window, Keys* keys);	// Performs All Your Initialization
void Deinitialize38(void);							// Performs All Your DeInitialization
void Update38(DWORD milliseconds);					// Perform Motion Updates
void Draw38(void);								// Perform All Your Scene Drawing

//lesson 39
BOOL Initialize39(GL_Window* window, Keys* keys);	// Performs All Your Initialization
void Deinitialize39(void);							// Performs All Your DeInitialization
void Update39(DWORD milliseconds);					// Perform Motion Updates
void Draw39(void);								// Perform All Your Scene Drawing
											// GL_FRAMEWORK__INCLUDED

//lesson 40
BOOL Initialize40(GL_Window* window, Keys* keys);	// Performs All Your Initialization
void Deinitialize40(void);							// Performs All Your DeInitialization
void Update40(DWORD milliseconds);					// Perform Motion Updates
void Draw40(void);								// Perform All Your Scene Drawing

//lesson 41
BOOL Initialize41(GL_Window* window, Keys* keys);	// Performs All Your Initialization
void Deinitialize41(void);							// Performs All Your DeInitialization
void Update41(DWORD milliseconds);					// Perform Motion Updates
void Draw41(void);								// Perform All Your Scene Drawing

//lesson 42
BOOL Initialize42(GL_Window* window, Keys* keys);	// Performs All Your Initialization
void Deinitialize42(void);							// Performs All Your DeInitialization
void Update42(float milliseconds);
void Draw42(void);								// Perform All Your Scene Drawing

//lesson 45
BOOL Initialize45(GL_Window* window, Keys* keys);	// Performs All Your Initialization
void Deinitialize45(void);							// Performs All Your DeInitialization
void Update45(DWORD milliseconds);
void Draw45(void);								// Perform All Your Scene Drawing

//lesson 46
BOOL Initialize46(GL_Window* window, Keys* keys);	// Performs All Your Initialization
void Deinitialize46(void);							// Performs All Your DeInitialization
void Update46(DWORD milliseconds);
void Draw46(void);								// Perform All Your Scene Drawing

//lesson 47
BOOL Initialize47(GL_Window* window, Keys* keys);	// Performs All Your Initialization
void Deinitialize47(void);							// Performs All Your DeInitialization
void Update47(float milliseconds);
void Draw47(void);								// Perform All Your Scene Drawing

												//lesson 48
BOOL Initialize48(GL_Window* window, Keys* keys);	// Performs All Your Initialization
void Deinitialize48(void);							// Performs All Your DeInitialization
void Update48(DWORD milliseconds);
void Draw48(void);								// Perform All Your Scene Drawing
#endif	
