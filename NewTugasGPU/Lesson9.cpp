#define _CRT_SECURE_NO_WARNINGS 1

#ifndef Lesson9_CLASS
#define Lesson9_CLASS

#include "baseapp.h"
#include "bmp.h"
#include <iostream>

class Lesson9 {
public:
	bool	twinkle;			// Twinkling Stars
	bool	tp;					// 'T' Key Pressed?

	int num = 50;				// Number Of Stars To Draw

	typedef struct				// Create A Structure For Star
	{
		int r, g, b;			// Stars Color
		GLfloat dist,			// Stars Distance From Center
			angle;			// Stars Current Angle
	}
	stars;
	stars star[50];			// Need To Keep Track Of 'num' Stars

	GLfloat	zoom = -15.0f;		// Distance Away From Stars
	GLfloat tilt = 90.0f;			// Tilt The View
	GLfloat	spin;				// Spin Stars

	GLuint	loop;				// General Loop Variable
	GLuint	texture[1];			// Storage For One textures

public:
	AUX_RGBImageRec *LoadBMP(char *Filename)                // Loads A Bitmap Image
	{
		FILE *File = NULL;                                // File Handle

		if (!Filename)                                  // Make Sure A Filename Was Given
		{
			return NULL;                            // If Not Return NULL
		}

		File = fopen(Filename, "r");                       // Check To See If The File Exists

		if (File)                                       // Does The File Exist?
		{
			fclose(File);                           // Close The Handle
			return auxDIBImageLoad(Filename);       // Load The Bitmap And Return A Pointer
		}
		return NULL;                                    // If Load Failed Return NULL
	}

	int LoadGLTextures()                                    // Load Bitmaps And Convert To Textures
	{
		int Status = FALSE;                               // Status Indicator

		AUX_RGBImageRec *TextureImage[1];               // Create Storage Space For The Texture

		memset(TextureImage, 0, sizeof(void *) * 1);        // Set The Pointer To NULL

															// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
		if (TextureImage[0] = LoadBMP("Data/Star.bmp"))
		{
			Status = TRUE;                            // Set The Status To TRUE

			glGenTextures(1, &texture[0]);          // Create One Texture

													// Create Linear Filtered Texture
			glBindTexture(GL_TEXTURE_2D, texture[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
		}
		if (TextureImage[0])                            // If Texture Exists
		{
			if (TextureImage[0]->data)              // If Texture Image Exists
			{
				free(TextureImage[0]->data);    // Free The Texture Image Memory
			}

			free(TextureImage[0]);                  // Free The Image Structure
		}

		return Status;                                  // Return The Status
	}

	int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
	{
		if (!LoadGLTextures())								// Jump To Texture Loading Routine
		{
			return FALSE;									// If Texture Didn't Load Return FALSE
		}

		glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping
		glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
		glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
		glClearDepth(1.0f);									// Depth Buffer Setup
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);					// Set The Blending Function For Translucency
		glEnable(GL_BLEND);

		for (loop = 0; loop<num; loop++)
		{
			star[loop].angle = 0.0f;
			star[loop].dist = (float(loop) / num)*5.0f;
			star[loop].r = rand() % 256;
			star[loop].g = rand() % 256;
			star[loop].b = rand() % 256;
		}
		return TRUE;										// Initialization Went OK
	}

	int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
		glBindTexture(GL_TEXTURE_2D, texture[0]);			// Select Our Texture

		for (loop = 0; loop<num; loop++)						// Loop Through All The Stars
		{
			glLoadIdentity();								// Reset The View Before We Draw Each Star
			glTranslatef(0.0f, 0.0f, zoom);					// Zoom Into The Screen (Using The Value In 'zoom')
			glRotatef(tilt, 1.0f, 0.0f, 0.0f);					// Tilt The View (Using The Value In 'tilt')
			glRotatef(star[loop].angle, 0.0f, 1.0f, 0.0f);		// Rotate To The Current Stars Angle
			glTranslatef(star[loop].dist, 0.0f, 0.0f);		// Move Forward On The X Plane
			glRotatef(-star[loop].angle, 0.0f, 1.0f, 0.0f);	// Cancel The Current Stars Angle
			glRotatef(-tilt, 1.0f, 0.0f, 0.0f);				// Cancel The Screen Tilt

			if (twinkle)
			{
				glColor4ub(star[(num - loop) - 1].r, star[(num - loop) - 1].g, star[(num - loop) - 1].b, 255);
				glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
				glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 0.0f);
				glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
				glEnd();
			}

			glRotatef(spin, 0.0f, 0.0f, 1.0f);
			glColor4ub(star[loop].r, star[loop].g, star[loop].b, 255);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
			glEnd();

			spin += 0.01f;
			star[loop].angle += float(loop) / num;
			star[loop].dist -= 0.01f;
			if (star[loop].dist<0.0f)
			{
				star[loop].dist += 5.0f;
				star[loop].r = rand() % 256;
				star[loop].g = rand() % 256;
				star[loop].b = rand() % 256;
			}
		}
		return TRUE;										// Everything Went OK
	}
	/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
	*	title			- Title To Appear At The Top Of The Window				*
	*	width			- Width Of The GL Window Or Fullscreen Mode				*
	*	height			- Height Of The GL Window Or Fullscreen Mode			*
	*	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
	*	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/

	BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
	{
		GLuint		PixelFormat;			// Holds The Results After Searching For A Match
		WNDCLASS	wc;						// Windows Class Structure
		DWORD		dwExStyle;				// Window Extended Style
		DWORD		dwStyle;				// Window Style
		RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
		WindowRect.left = (long)0;			// Set Left Value To 0
		WindowRect.right = (long)width;		// Set Right Value To Requested Width
		WindowRect.top = (long)0;				// Set Top Value To 0
		WindowRect.bottom = (long)height;		// Set Bottom Value To Requested Height

		fullscreen = fullscreenflag;			// Set The Global Fullscreen Flag

		hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
		wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc Handles Messages
		wc.cbClsExtra = 0;									// No Extra Window Data
		wc.cbWndExtra = 0;									// No Extra Window Data
		wc.hInstance = hInstance;							// Set The Instance
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
		wc.hbrBackground = NULL;									// No Background Required For GL
		wc.lpszMenuName = NULL;									// We Don't Want A Menu
		wc.lpszClassName = "OpenGL";								// Set The Class Name

		if (!RegisterClass(&wc))									// Attempt To Register The Window Class
		{
			MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;											// Return FALSE
		}

		if (fullscreen)												// Attempt Fullscreen Mode?
		{
			DEVMODE dmScreenSettings;								// Device Mode
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Size Of The Devmode Structure
			dmScreenSettings.dmPelsWidth = width;				// Selected Screen Width
			dmScreenSettings.dmPelsHeight = height;				// Selected Screen Height
			dmScreenSettings.dmBitsPerPel = bits;					// Selected Bits Per Pixel
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
			if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
				if (MessageBox(NULL, "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", "NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
				{
					fullscreen = FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
				}
				else
				{
					// Pop Up A Message Box Letting User Know The Program Is Closing.
					MessageBox(NULL, "Program Will Now Close.", "ERROR", MB_OK | MB_ICONSTOP);
					return FALSE;									// Return FALSE
				}
			}
		}

		if (fullscreen)												// Are We Still In Fullscreen Mode?
		{
			dwExStyle = WS_EX_APPWINDOW;								// Window Extended Style
			dwStyle = WS_POPUP;										// Windows Style
			ShowCursor(FALSE);										// Hide Mouse Pointer
		}
		else
		{
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
			dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style
		}

		AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

																		// Create The Window
		if (!(hWnd = CreateWindowEx(dwExStyle,							// Extended Style For The Window
			"OpenGL",							// Class Name
			title,								// Window Title
			dwStyle |							// Defined Window Style
			WS_CLIPSIBLINGS |					// Required Window Style
			WS_CLIPCHILDREN,					// Required Window Style
			0, 0,								// Window Position
			WindowRect.right - WindowRect.left,	// Calculate Window Width
			WindowRect.bottom - WindowRect.top,	// Calculate Window Height
			NULL,								// No Parent Window
			NULL,								// No Menu
			hInstance,							// Instance
			NULL)))								// Dont Pass Anything To WM_CREATE
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}

		static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
		{
			sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
			1,											// Version Number
			PFD_DRAW_TO_WINDOW |						// Format Must Support Window
			PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,							// Must Support Double Buffering
			PFD_TYPE_RGBA,								// Request An RGBA Format
			bits,										// Select Our Color Depth
			0, 0, 0, 0, 0, 0,							// Color Bits Ignored
			0,											// No Alpha Buffer
			0,											// Shift Bit Ignored
			0,											// No Accumulation Buffer
			0, 0, 0, 0,									// Accumulation Bits Ignored
			16,											// 16Bit Z-Buffer (Depth Buffer)  
			0,											// No Stencil Buffer
			0,											// No Auxiliary Buffer
			PFD_MAIN_PLANE,								// Main Drawing Layer
			0,											// Reserved
			0, 0, 0										// Layer Masks Ignored
		};

		if (!(hDC = GetDC(hWnd)))							// Did We Get A Device Context?
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}

		if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}

		if (!SetPixelFormat(hDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}

		if (!(hRC = wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}

		if (!wglMakeCurrent(hDC, hRC))					// Try To Activate The Rendering Context
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}

		ShowWindow(hWnd, SW_SHOW);						// Show The Window
		SetForegroundWindow(hWnd);						// Slightly Higher Priority
		SetFocus(hWnd);									// Sets Keyboard Focus To The Window
		ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

		if (!InitGL())									// Initialize Our Newly Created GL Window
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}

		return TRUE;									// Success
	}

	int WINAPI WinMain(HINSTANCE	hInstance,			// Instance
		HINSTANCE	hPrevInstance,		// Previous Instance
		LPSTR		lpCmdLine,			// Command Line Parameters
		int			nCmdShow)			// Window Show State
	{
		MSG		msg;									// Windows Message Structure
		BOOL	done = FALSE;								// Bool Variable To Exit Loop

															// Ask The User Which Screen Mode They Prefer
		if (MessageBox(NULL, "Would You Like To Run In Fullscreen Mode?", "Start FullScreen?", MB_YESNO | MB_ICONQUESTION) == IDNO)
		{
			fullscreen = FALSE;							// Windowed Mode
		}

		// Create Our OpenGL Window
		if (!CreateGLWindow("NeHe's Animated Blended Textures Tutorial", 640, 480, 16, fullscreen))
		{
			return 0;									// Quit If Window Was Not Created
		}

		while (!done)									// Loop That Runs While done=FALSE
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Is There A Message Waiting?
			{
				if (msg.message == WM_QUIT)				// Have We Received A Quit Message?
				{
					done = TRUE;							// If So done=TRUE
				}
				else									// If Not, Deal With Window Messages
				{
					TranslateMessage(&msg);				// Translate The Message
					DispatchMessage(&msg);				// Dispatch The Message
				}
			}
			else										// If There Are No Messages
			{
				// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
				if ((active && !DrawGLScene()) || keys[VK_ESCAPE])	// Active?  Was There A Quit Received?
				{
					done = TRUE;							// ESC or DrawGLScene Signalled A Quit
				}
				else									// Not Time To Quit, Update Screen
				{
					SwapBuffers(hDC);					// Swap Buffers (Double Buffering)
					if (keys['T'] && !tp)
					{
						tp = TRUE;
						twinkle = !twinkle;
					}
					if (!keys['T'])
					{
						tp = FALSE;
					}

					if (keys[VK_UP])
					{
						tilt -= 0.5f;
					}

					if (keys[VK_DOWN])
					{
						tilt += 0.5f;
					}

					if (keys[VK_PRIOR])
					{
						zoom -= 0.2f;
					}

					if (keys[VK_NEXT])
					{
						zoom += 0.2f;
					}

					if (keys[VK_F1])						// Is F1 Being Pressed?
					{
						keys[VK_F1] = FALSE;					// If So Make Key FALSE
						KillGLWindow();						// Kill Our Current Window
						fullscreen = !fullscreen;				// Toggle Fullscreen / Windowed Mode
																// Recreate Our OpenGL Window
						if (!CreateGLWindow("NeHe's Animated Blended Textures Tutorial", 640, 480, 16, fullscreen))
						{
							return 0;						// Quit If Window Was Not Created
						}
					}
				}
			}
		}

		// Shutdown
		KillGLWindow();									// Kill The Window
		return (msg.wParam);							// Exit The Program
	}

};
#endif