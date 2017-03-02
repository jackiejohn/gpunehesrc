#define _CRT_SECURE_NO_WARNINGS 1

#ifndef Lesson10_CLASS
#define Lesson10_CLASS

#include "baseapp.h"
#include "bmp.h"
#include <stdio.h>
#include <math.h>
#include <malloc.h> 

class Lesson10 {
public:
	bool	blend;				// Blending ON/OFF
	bool	bp;					// B Pressed?
	bool	fp;					// F Pressed?

	const float piover180 = 0.0174532925f;
	float heading;
	float xpos;
	float zpos;

	GLfloat	yrot;				// Y Rotation
	GLfloat walkbias = 0;
	GLfloat walkbiasangle = 0;
	GLfloat lookupdown = 0.0f;
	GLfloat	z = 0.0f;				// Depth Into The Screen

	GLuint	filter;				// Which Filter To Use
	GLuint	texture[3];			// Storage For 3 Textures

	typedef struct tagVERTEX
	{
		float x, y, z;
		float u, v;
	} VERTEX;

	typedef struct tagTRIANGLE
	{
		VERTEX vertex[3];
	} TRIANGLE;

	typedef struct tagSECTOR
	{
		int numtriangles;
		TRIANGLE* triangle;
	} SECTOR;

	SECTOR sector1;				// Our Model Goes Here:


public:

	void readstr(FILE *f, char *string)
	{
		do
		{
			fgets(string, 255, f);
		} while ((string[0] == '/') || (string[0] == '\n'));
		return;
	}

	void SetupWorld()
	{
		float x, y, z, u, v;
		int numtriangles;
		FILE *filein;
		char oneline[255];
		filein = fopen("E:/learn/NewTugasGPU/NewTugasGPU/Data/World.txt", "rt");				// File To Load World Data From

		readstr(filein, oneline);
		sscanf(oneline, "NUMPOLLIES %d\n", &numtriangles);

		sector1.triangle = new TRIANGLE[numtriangles];
		sector1.numtriangles = numtriangles;
		for (int loop = 0; loop < numtriangles; loop++)
		{
			for (int vert = 0; vert < 3; vert++)
			{
				readstr(filein, oneline);
				sscanf(oneline, "%f %f %f %f %f", &x, &y, &z, &u, &v);
				sector1.triangle[loop].vertex[vert].x = x;
				sector1.triangle[loop].vertex[vert].y = y;
				sector1.triangle[loop].vertex[vert].z = z;
				sector1.triangle[loop].vertex[vert].u = u;
				sector1.triangle[loop].vertex[vert].v = v;
			}
		}
		fclose(filein);
		return;
	}

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
		if (TextureImage[0] = LoadBMP("E:/learn/NewTugasGPU/NewTugasGPU/Data/Mud.bmp"))
		{
			Status = TRUE;                            // Set The Status To TRUE

			glGenTextures(3, &texture[0]);          // Create Three Textures

													// Create Nearest Filtered Texture
			glBindTexture(GL_TEXTURE_2D, texture[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);

			// Create Linear Filtered Texture
			glBindTexture(GL_TEXTURE_2D, texture[1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);

			// Create MipMapped Texture
			glBindTexture(GL_TEXTURE_2D, texture[2]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
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
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);					// Set The Blending Function For Translucency
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// This Will Clear The Background Color To Black
		glClearDepth(1.0);									// Enables Clearing Of The Depth Buffer
		glDepthFunc(GL_LESS);								// The Type Of Depth Test To Do
		glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
		glShadeModel(GL_SMOOTH);							// Enables Smooth Color Shading
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

		SetupWorld();

		return TRUE;										// Initialization Went OK
	}

	int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
		glLoadIdentity();									// Reset The View

		GLfloat x_m, y_m, z_m, u_m, v_m;
		GLfloat xtrans = -xpos;
		GLfloat ztrans = -zpos;
		GLfloat ytrans = -walkbias - 0.25f;
		GLfloat sceneroty = 360.0f - yrot;

		int numtriangles;

		glRotatef(lookupdown, 1.0f, 0, 0);
		glRotatef(sceneroty, 0, 1.0f, 0);

		glTranslatef(xtrans, ytrans, ztrans);

		glBindTexture(GL_TEXTURE_2D, texture[0]);//////////////////////////////

		numtriangles = sector1.numtriangles;

		// Process Each Triangle
		for (int loop_m = 0; loop_m < numtriangles; loop_m++)
		{
			glBegin(GL_TRIANGLES);
			glNormal3f(0.0f, 0.0f, 1.0f);
			x_m = sector1.triangle[loop_m].vertex[0].x;
			y_m = sector1.triangle[loop_m].vertex[0].y;
			z_m = sector1.triangle[loop_m].vertex[0].z;
			u_m = sector1.triangle[loop_m].vertex[0].u;
			v_m = sector1.triangle[loop_m].vertex[0].v;
			glTexCoord2f(u_m, v_m); glVertex3f(x_m, y_m, z_m);

			x_m = sector1.triangle[loop_m].vertex[1].x;
			y_m = sector1.triangle[loop_m].vertex[1].y;
			z_m = sector1.triangle[loop_m].vertex[1].z;
			u_m = sector1.triangle[loop_m].vertex[1].u;
			v_m = sector1.triangle[loop_m].vertex[1].v;
			glTexCoord2f(u_m, v_m); glVertex3f(x_m, y_m, z_m);

			x_m = sector1.triangle[loop_m].vertex[2].x;
			y_m = sector1.triangle[loop_m].vertex[2].y;
			z_m = sector1.triangle[loop_m].vertex[2].z;
			u_m = sector1.triangle[loop_m].vertex[2].u;
			v_m = sector1.triangle[loop_m].vertex[2].v;
			glTexCoord2f(u_m, v_m); glVertex3f(x_m, y_m, z_m);
			glEnd();
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
		if (!CreateGLWindow("Lionel Brits & NeHe's 3D World Tutorial", 640, 480, 16, fullscreen))
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
					if (keys['B'] && !bp)
					{
						bp = TRUE;
						blend = !blend;
						if (!blend)
						{
							glDisable(GL_BLEND);
							glEnable(GL_DEPTH_TEST);
						}
						else
						{
							glEnable(GL_BLEND);
							glDisable(GL_DEPTH_TEST);
						}
					}
					if (!keys['B'])
					{
						bp = FALSE;
					}

					if (keys['F'] && !fp)
					{
						fp = TRUE;
						filter += 1;
						if (filter>2)
						{
							filter = 0;
						}
					}
					if (!keys['F'])
					{
						fp = FALSE;
					}

					if (keys[VK_PRIOR])
					{
						z -= 0.02f;
					}

					if (keys[VK_NEXT])
					{
						z += 0.02f;
					}

					if (keys[VK_UP])
					{

						xpos -= (float)sin(heading*piover180) * 0.05f;
						zpos -= (float)cos(heading*piover180) * 0.05f;
						if (walkbiasangle >= 359.0f)
						{
							walkbiasangle = 0.0f;
						}
						else
						{
							walkbiasangle += 10;
						}
						walkbias = (float)sin(walkbiasangle * piover180) / 20.0f;
					}

					if (keys[VK_DOWN])
					{
						xpos += (float)sin(heading*piover180) * 0.05f;
						zpos += (float)cos(heading*piover180) * 0.05f;
						if (walkbiasangle <= 1.0f)
						{
							walkbiasangle = 359.0f;
						}
						else
						{
							walkbiasangle -= 10;
						}
						walkbias = (float)sin(walkbiasangle * piover180) / 20.0f;
					}

					if (keys[VK_RIGHT])
					{
						heading -= 1.0f;
						yrot = heading;
					}

					if (keys[VK_LEFT])
					{
						heading += 1.0f;
						yrot = heading;
					}

					if (keys[VK_PRIOR])
					{
						lookupdown -= 1.0f;
					}

					if (keys[VK_NEXT])
					{
						lookupdown += 1.0f;
					}

					if (keys[VK_F1])						// Is F1 Being Pressed?
					{
						keys[VK_F1] = FALSE;					// If So Make Key FALSE
						KillGLWindow();						// Kill Our Current Window
						fullscreen = !fullscreen;				// Toggle Fullscreen / Windowed Mode
																// Recreate Our OpenGL Window
						if (!CreateGLWindow("Lionel Brits & NeHe's 3D World Tutorial", 640, 480, 16, fullscreen))
						{
							return 0;						// Quit If Window Was Not Created
						}
					}
				}
			}
		}

		// Shutdown
		KillGLWindow();										// Kill The Window
		return (msg.wParam);								// Exit The Program
	}
};
#endif