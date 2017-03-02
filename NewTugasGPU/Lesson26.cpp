#define _CRT_SECURE_NO_WARNINGS 1

#ifndef Lesson26_CLASS
#define Lesson26_CLASS

#include "baseapp.h"
#include <stdio.h>			// Header File For Standard Input/Output
#include "bmp.h"

class Lesson26 {
public:
	// Light Parameters
	const GLfloat LightAmb[4] = { 0.7f, 0.7f, 0.7f, 1.0f };	// Ambient Light
	const GLfloat LightDif[4] = { 1.0f, 1.0f, 1.0f, 1.0f };	// Diffuse Light
	const GLfloat LightPos[4] = { 4.0f, 4.0f, 6.0f, 1.0f };	// Light Position

	GLUquadricObj	*q;										// Quadratic For Drawing A Sphere

	GLfloat		xrot = 0.0f;						// X Rotation
	GLfloat		yrot = 0.0f;						// Y Rotation
	GLfloat		xrotspeed = 0.0f;						// X Rotation Speed
	GLfloat		yrotspeed = 0.0f;						// Y Rotation Speed
	GLfloat		zoom = -7.0f;						// Depth Into The Screen
	GLfloat		height = 2.0f;						// Height Of Ball From Floor

	GLuint		texture[3];									// 3 Textures


public:
	AUX_RGBImageRec *LoadBMP(char *Filename)				// Loads A Bitmap Image
	{
		FILE *File = NULL;									// File Handle

		if (!Filename)										// Make Sure A Filename Was Given
		{
			return NULL;									// If Not Return NULL
		}

		File = fopen(Filename, "r");							// Check To See If The File Exists

		if (File)											// Does The File Exist?
		{
			fclose(File);									// Close The Handle
			return auxDIBImageLoad(Filename);				// Load The Bitmap And Return A Pointer
		}

		return NULL;										// If Load Failed Return NULL
	}

	int LoadGLTextures()                                    // Load Bitmaps And Convert To Textures
	{
		int Status = FALSE;									// Status Indicator
		AUX_RGBImageRec *TextureImage[3];					// Create Storage Space For The Textures
		memset(TextureImage, 0, sizeof(void *) * 3);			// Set The Pointer To NULL
		if ((TextureImage[0] = LoadBMP("Data/EnvWall.bmp")) &&// Load The Floor Texture
			(TextureImage[1] = LoadBMP("Data/Ball.bmp")) &&	// Load the Light Texture
			(TextureImage[2] = LoadBMP("Data/EnvRoll.bmp")))	// Load the Wall Texture
		{
			Status = TRUE;									// Set The Status To TRUE
			glGenTextures(3, &texture[0]);					// Create The Texture
			for (int loop = 0; loop<3; loop++)				// Loop Through 5 Textures
			{
				glBindTexture(GL_TEXTURE_2D, texture[loop]);
				glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[loop]->sizeX, TextureImage[loop]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[loop]->data);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			for (int loop = 0; loop<3; loop++)					// Loop Through 5 Textures
			{
				if (TextureImage[loop])						// If Texture Exists
				{
					if (TextureImage[loop]->data)			// If Texture Image Exists
					{
						free(TextureImage[loop]->data);		// Free The Texture Image Memory
					}
					free(TextureImage[loop]);				// Free The Image Structure 
				}
			}
		}
		return Status;										// Return The Status
	}

	int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
	{
		if (!LoadGLTextures())								// If Loading The Textures Failed
		{
			return FALSE;									// Return False
		}
		glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
		glClearColor(0.2f, 0.5f, 1.0f, 1.0f);				// Background
		glClearDepth(1.0f);									// Depth Buffer Setup
		glClearStencil(0);									// Clear The Stencil Buffer To 0
		glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
		glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
		glEnable(GL_TEXTURE_2D);							// Enable 2D Texture Mapping

		glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmb);			// Set The Ambient Lighting For Light0
		glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDif);			// Set The Diffuse Lighting For Light0
		glLightfv(GL_LIGHT0, GL_POSITION, LightPos);		// Set The Position For Light0

		glEnable(GL_LIGHT0);								// Enable Light 0
		glEnable(GL_LIGHTING);								// Enable Lighting

		q = gluNewQuadric();								// Create A New Quadratic
		gluQuadricNormals(q, GL_SMOOTH);					// Generate Smooth Normals For The Quad
		gluQuadricTexture(q, GL_TRUE);						// Enable Texture Coords For The Quad

		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);	// Set Up Sphere Mapping
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);	// Set Up Sphere Mapping

		return TRUE;										// Initialization Went OK
	}

	void DrawObject()										// Draw Our Ball
	{
		glColor3f(1.0f, 1.0f, 1.0f);						// Set Color To White
		glBindTexture(GL_TEXTURE_2D, texture[1]);			// Select Texture 2 (1)
		gluSphere(q, 0.35f, 32, 16);						// Draw First Sphere

		glBindTexture(GL_TEXTURE_2D, texture[2]);			// Select Texture 3 (2)
		glColor4f(1.0f, 1.0f, 1.0f, 0.4f);					// Set Color To White With 40% Alpha
		glEnable(GL_BLEND);									// Enable Blending
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);					// Set Blending Mode To Mix Based On SRC Alpha
		glEnable(GL_TEXTURE_GEN_S);							// Enable Sphere Mapping
		glEnable(GL_TEXTURE_GEN_T);							// Enable Sphere Mapping

		gluSphere(q, 0.35f, 32, 16);						// Draw Another Sphere Using New Texture
															// Textures Will Mix Creating A MultiTexture Effect (Reflection)
		glDisable(GL_TEXTURE_GEN_S);						// Disable Sphere Mapping
		glDisable(GL_TEXTURE_GEN_T);						// Disable Sphere Mapping
		glDisable(GL_BLEND);								// Disable Blending
	}

	void DrawFloor()										// Draws The Floor
	{
		glBindTexture(GL_TEXTURE_2D, texture[0]);			// Select Texture 1 (0)
		glBegin(GL_QUADS);									// Begin Drawing A Quad
		glNormal3f(0.0, 1.0, 0.0);						// Normal Pointing Up
		glTexCoord2f(0.0f, 1.0f);					// Bottom Left Of Texture
		glVertex3f(-2.0, 0.0, 2.0);					// Bottom Left Corner Of Floor

		glTexCoord2f(0.0f, 0.0f);					// Top Left Of Texture
		glVertex3f(-2.0, 0.0, -2.0);					// Top Left Corner Of Floor

		glTexCoord2f(1.0f, 0.0f);					// Top Right Of Texture
		glVertex3f(2.0, 0.0, -2.0);					// Top Right Corner Of Floor

		glTexCoord2f(1.0f, 1.0f);					// Bottom Right Of Texture
		glVertex3f(2.0, 0.0, 2.0);					// Bottom Right Corner Of Floor
		glEnd();											// Done Drawing The Quad
	}

	int DrawGLScene(GLvoid)									// Draw Everything
	{
		// Clear Screen, Depth Buffer & Stencil Buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// Clip Plane Equations
		double eqr[] = { 0.0f,-1.0f, 0.0f, 0.0f };			// Plane Equation To Use For The Reflected Objects

		glLoadIdentity();									// Reset The Modelview Matrix
		glTranslatef(0.0f, -0.6f, zoom);					// Zoom And Raise Camera Above The Floor (Up 0.6 Units)
		glColorMask(0, 0, 0, 0);								// Set Color Mask
		glEnable(GL_STENCIL_TEST);							// Enable Stencil Buffer For "marking" The Floor
		glStencilFunc(GL_ALWAYS, 1, 1);						// Always Passes, 1 Bit Plane, 1 As Mask
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);			// We Set The Stencil Buffer To 1 Where We Draw Any Polygon
															// Keep If Test Fails, Keep If Test Passes But Buffer Test Fails
															// Replace If Test Passes
		glDisable(GL_DEPTH_TEST);							// Disable Depth Testing
		DrawFloor();										// Draw The Floor (Draws To The Stencil Buffer)
															// We Only Want To Mark It In The Stencil Buffer
		glEnable(GL_DEPTH_TEST);							// Enable Depth Testing
		glColorMask(1, 1, 1, 1);								// Set Color Mask to TRUE, TRUE, TRUE, TRUE
		glStencilFunc(GL_EQUAL, 1, 1);						// We Draw Only Where The Stencil Is 1
															// (I.E. Where The Floor Was Drawn)
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);				// Don't Change The Stencil Buffer
		glEnable(GL_CLIP_PLANE0);							// Enable Clip Plane For Removing Artifacts
															// (When The Object Crosses The Floor)
		glClipPlane(GL_CLIP_PLANE0, eqr);					// Equation For Reflected Objects
		glPushMatrix();										// Push The Matrix Onto The Stack
		glScalef(1.0f, -1.0f, 1.0f);					// Mirror Y Axis
		glLightfv(GL_LIGHT0, GL_POSITION, LightPos);	// Set Up Light0
		glTranslatef(0.0f, height, 0.0f);				// Position The Object
		glRotatef(xrot, 1.0f, 0.0f, 0.0f);				// Rotate Local Coordinate System On X Axis
		glRotatef(yrot, 0.0f, 1.0f, 0.0f);				// Rotate Local Coordinate System On Y Axis
		DrawObject();									// Draw The Sphere (Reflection)
		glPopMatrix();										// Pop The Matrix Off The Stack
		glDisable(GL_CLIP_PLANE0);							// Disable Clip Plane For Drawing The Floor
		glDisable(GL_STENCIL_TEST);							// We Don't Need The Stencil Buffer Any More (Disable)
		glLightfv(GL_LIGHT0, GL_POSITION, LightPos);		// Set Up Light0 Position
		glEnable(GL_BLEND);									// Enable Blending (Otherwise The Reflected Object Wont Show)
		glDisable(GL_LIGHTING);								// Since We Use Blending, We Disable Lighting
		glColor4f(1.0f, 1.0f, 1.0f, 0.8f);					// Set Color To White With 80% Alpha
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// Blending Based On Source Alpha And 1 Minus Dest Alpha
		DrawFloor();										// Draw The Floor To The Screen
		glEnable(GL_LIGHTING);								// Enable Lighting
		glDisable(GL_BLEND);								// Disable Blending
		glTranslatef(0.0f, height, 0.0f);					// Position The Ball At Proper Height
		glRotatef(xrot, 1.0f, 0.0f, 0.0f);					// Rotate On The X Axis
		glRotatef(yrot, 0.0f, 1.0f, 0.0f);					// Rotate On The Y Axis
		DrawObject();										// Draw The Ball
		xrot += xrotspeed;									// Update X Rotation Angle By xrotspeed
		yrot += yrotspeed;									// Update Y Rotation Angle By yrotspeed
		glFlush();											// Flush The GL Pipeline
		return TRUE;										// Everything Went OK
	}

	void ProcessKeyboard()									// Process Keyboard Results
	{
		if (keys[VK_RIGHT])		yrotspeed += 0.08f;			// Right Arrow Pressed (Increase yrotspeed)
		if (keys[VK_LEFT])		yrotspeed -= 0.08f;			// Left Arrow Pressed (Decrease yrotspeed)
		if (keys[VK_DOWN])		xrotspeed += 0.08f;			// Down Arrow Pressed (Increase xrotspeed)
		if (keys[VK_UP])		xrotspeed -= 0.08f;			// Up Arrow Pressed (Decrease xrotspeed)

		if (keys['A'])			zoom += 0.05f;				// 'A' Key Pressed ... Zoom In
		if (keys['Z'])			zoom -= 0.05f;				// 'Z' Key Pressed ... Zoom Out

		if (keys[VK_PRIOR])		height += 0.03f;				// Page Up Key Pressed Move Ball Up
		if (keys[VK_NEXT])		height -= 0.03f;				// Page Down Key Pressed Move Ball Down
	}
	/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
	*	title			- Title To Appear At The Top Of The Window				*
	*	width			- Width Of The GL Window Or Fullscreen Mode				*
	*	height			- Height Of The GL Window Or Fullscreen Mode			*
	*	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
	*	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/

	BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
	{
		GLuint		PixelFormat;							// Holds The Results After Searching For A Match
		WNDCLASS	wc;										// Windows Class Structure
		DWORD		dwExStyle;								// Window Extended Style
		DWORD		dwStyle;								// Window Style

		fullscreen = fullscreenflag;							// Set The Global Fullscreen Flag

		hInstance = GetModuleHandle(NULL);		// Grab An Instance For Our Window
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
		wc.lpfnWndProc = (WNDPROC)WndProc;			// WndProc Handles Messages
		wc.cbClsExtra = 0;							// No Extra Window Data
		wc.cbWndExtra = 0;							// No Extra Window Data
		wc.hInstance = hInstance;					// Set The Instance
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);	// Load The Default Icon
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);	// Load The Arrow Pointer
		wc.hbrBackground = NULL;							// No Background Required For GL
		wc.lpszMenuName = NULL;							// We Don't Want A Menu
		wc.lpszClassName = "OpenGL";						// Set The Class Name

		if (!RegisterClass(&wc))							// Attempt To Register The Window Class
		{
			MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;									// Return FALSE
		}

		if (fullscreen)										// Attempt Fullscreen Mode?
		{
			DEVMODE dmScreenSettings;						// Device Mode
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);	// Size Of The Devmode Structure
			dmScreenSettings.dmPelsWidth = width;		// Selected Screen Width
			dmScreenSettings.dmPelsHeight = height;		// Selected Screen Height
			dmScreenSettings.dmBitsPerPel = bits;			// Selected Bits Per Pixel
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
			if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
				if (MessageBox(NULL, "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", "NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
				{
					fullscreen = FALSE;						// Windowed Mode Selected.  Fullscreen = FALSE
				}
				else
				{
					// Pop Up A Message Box Letting User Know The Program Is Closing.
					MessageBox(NULL, "Program Will Now Close.", "ERROR", MB_OK | MB_ICONSTOP);
					return FALSE;							// Return FALSE
				}
			}
		}

		if (fullscreen)										// Are We Still In Fullscreen Mode?
		{
			dwExStyle = WS_EX_APPWINDOW;						// Window Extended Style
			dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;	// Windows Style
			ShowCursor(FALSE);								// Hide Mouse Pointer
		}
		else
		{
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;	// Window Extended Style
			dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;	// Windows Style
		}

		// Create The Window
		if (!(hWnd = CreateWindowEx(dwExStyle,				// Extended Style For The Window
			"OpenGL",				// Class Name
			title,					// Window Title
			dwStyle,				// Window Style
			0, 0,					// Window Position
			width, height,			// Selected Width And Height
			NULL,					// No Parent Window
			NULL,					// No Menu
			hInstance,				// Instance
			NULL)))					// Dont Pass Anything To WM_CREATE
		{
			KillGLWindow();									// Reset The Display
			MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;									// Return FALSE
		}

		static	PIXELFORMATDESCRIPTOR pfd =					// pfd Tells Windows How We Want Things To Be
		{
			sizeof(PIXELFORMATDESCRIPTOR),					// Size Of This Pixel Format Descriptor
			1,												// Version Number
			PFD_DRAW_TO_WINDOW |							// Format Must Support Window
			PFD_SUPPORT_OPENGL |							// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,								// Must Support Double Buffering
			PFD_TYPE_RGBA,									// Request An RGBA Format
			bits,											// Select Our Color Depth
			0, 0, 0, 0, 0, 0,								// Color Bits Ignored
			0,												// No Alpha Buffer
			0,												// Shift Bit Ignored
			0,												// No Accumulation Buffer
			0, 0, 0, 0,										// Accumulation Bits Ignored
			16,												// 16Bit Z-Buffer (Depth Buffer)  
			1,												// Use Stencil Buffer ( * Important * )
			0,												// No Auxiliary Buffer
			PFD_MAIN_PLANE,									// Main Drawing Layer
			0,												// Reserved
			0, 0, 0											// Layer Masks Ignored
		};

		if (!(hDC = GetDC(hWnd)))								// Did We Get A Device Context?
		{
			KillGLWindow();									// Reset The Display
			MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;									// Return FALSE
		}

		if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))		// Did Windows Find A Matching Pixel Format?
		{
			KillGLWindow();									// Reset The Display
			MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;									// Return FALSE
		}

		if (!SetPixelFormat(hDC, PixelFormat, &pfd))			// Are We Able To Set The Pixel Format?
		{
			KillGLWindow();									// Reset The Display
			MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;									// Return FALSE
		}

		if (!(hRC = wglCreateContext(hDC)))					// Are We Able To Get A Rendering Context?
		{
			KillGLWindow();									// Reset The Display
			MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;									// Return FALSE
		}

		if (!wglMakeCurrent(hDC, hRC))						// Try To Activate The Rendering Context
		{
			KillGLWindow();									// Reset The Display
			MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;									// Return FALSE
		}

		ShowWindow(hWnd, SW_SHOW);							// Show The Window
		SetForegroundWindow(hWnd);							// Slightly Higher Priority
		SetFocus(hWnd);										// Sets Keyboard Focus To The Window
		ReSizeGLScene(width, height);						// Set Up Our Perspective GL Screen

		if (!InitGL())										// Initialize Our Newly Created GL Window
		{
			KillGLWindow();									// Reset The Display
			MessageBox(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;									// Return FALSE
		}

		return TRUE;										// Success
	}

	int WINAPI WinMain(HINSTANCE	hInstance,				// Instance
		HINSTANCE	hPrevInstance,			// Previous Instance
		LPSTR		lpCmdLine,				// Command Line Parameters
		int			nCmdShow)				// Window Show State
	{
		MSG		msg;										// Windows Message Structure
		BOOL	done = FALSE;									// Bool Variable To Exit Loop

																// Ask The User Which Screen Mode They Prefer
		if (MessageBox(NULL, "Would You Like To Run In Fullscreen Mode?", "Start FullScreen?", MB_YESNO | MB_ICONQUESTION) == IDNO)
		{
			fullscreen = FALSE;								// Windowed Mode
		}

		// Create Our OpenGL Window
		if (!CreateGLWindow("Banu Octavian & NeHe's Stencil & Reflection Tutorial", 640, 480, 32, fullscreen))
		{
			return 0;										// Quit If Window Was Not Created
		}

		while (!done)										// Loop That Runs While done=FALSE
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))		// Is There A Message Waiting?
			{
				if (msg.message == WM_QUIT)					// Have We Received A Quit Message?
				{
					done = TRUE;								// If So done=TRUE
				}
				else										// If Not, Deal With Window Messages
				{
					TranslateMessage(&msg);					// Translate The Message
					DispatchMessage(&msg);					// Dispatch The Message
				}
			}
			else											// If There Are No Messages
			{
				// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
				if (active)									// Program Active?
				{
					if (keys[VK_ESCAPE])					// Was Escape Pressed?
					{
						done = TRUE;							// ESC Signalled A Quit
					}
					else									// Not Time To Quit, Update Screen
					{
						DrawGLScene();						// Draw The Scene
						SwapBuffers(hDC);					// Swap Buffers (Double Buffering)

						ProcessKeyboard();					// Processed Keyboard Presses
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