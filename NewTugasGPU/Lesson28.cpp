#define _CRT_SECURE_NO_WARNINGS 1

#ifndef Lesson28_CLASS
#define Lesson28_CLASS

#include "baseapp.h"
#include "bmp.h"
#include <math.h>			// Header File For Math Library Routines
#include <stdio.h>			// Header File For Standard I/O Routines
#include <stdlib.h>			// Header File For Standard Library

typedef struct point_3d {			// Structure for a 3-dimensional point (NEW)
	double x, y, z;
} POINT_3D;

typedef struct bpatch {				// Structure for a 3rd degree bezier patch (NEW)
	POINT_3D	anchors[4][4];			// 4x4 grid of anchor points
	GLuint		dlBPatch;				// Display List for Bezier Patch
	GLuint		texture;				// Texture for the patch
} BEZIER_PATCH;

class Lesson28 {
public:
	DEVMODE			DMsaved;			// Saves the previous screen settings (NEW)

	GLfloat			rotz = 0.0f;		// Rotation about the Z axis
	BEZIER_PATCH	mybezier;			// The bezier patch we're going to use (NEW)
	BOOL			showCPoints = TRUE;	// Toggles displaying the control point grid (NEW)
	int				divs = 7;			// Number of intrapolations (conrols poly resolution) (NEW)
public:
	// Adds 2 points. Don't just use '+' ;)
	POINT_3D pointAdd(POINT_3D p, POINT_3D q) {
		p.x += q.x;		p.y += q.y;		p.z += q.z;
		return p;
	}

	// Multiplies a point and a constant. Don't just use '*'
	POINT_3D pointTimes(double c, POINT_3D p) {
		p.x *= c;	p.y *= c;	p.z *= c;
		return p;
	}

	// Function for quick point creation
	POINT_3D makePoint(double a, double b, double c) {
		POINT_3D p;
		p.x = a;	p.y = b;	p.z = c;
		return p;
	}


	// Calculates 3rd degree polynomial based on array of 4 points
	// and a single variable (u) which is generally between 0 and 1
	POINT_3D Bernstein(float u, POINT_3D *p) {
		POINT_3D	a, b, c, d, r;

		a = pointTimes(pow(u, 3), p[0]);
		b = pointTimes(3 * pow(u, 2)*(1 - u), p[1]);
		c = pointTimes(3 * u*pow((1 - u), 2), p[2]);
		d = pointTimes(pow((1 - u), 3), p[3]);

		r = pointAdd(pointAdd(a, b), pointAdd(c, d));

		return r;
	}

	// Generates a display list based on the data in the patch
	// and the number of divisions
	GLuint genBezier(BEZIER_PATCH patch, int divs) {
		int			u = 0, v;
		float		py, px, pyold;
		GLuint		drawlist = glGenLists(1);		// make the display list
		POINT_3D	temp[4];
		POINT_3D	*last = (POINT_3D*)malloc(sizeof(POINT_3D)*(divs + 1));
		// array of points to mark the first line of polys

		if (patch.dlBPatch != NULL)					// get rid of any old display lists
			glDeleteLists(patch.dlBPatch, 1);

		temp[0] = patch.anchors[0][3];				// the first derived curve (along x axis)
		temp[1] = patch.anchors[1][3];
		temp[2] = patch.anchors[2][3];
		temp[3] = patch.anchors[3][3];

		for (v = 0; v <= divs; v++) {						// create the first line of points
			px = ((float)v) / ((float)divs);			// percent along y axis
														// use the 4 points from the derives curve to calculate the points along that curve
			last[v] = Bernstein(px, temp);
		}

		glNewList(drawlist, GL_COMPILE);				// Start a new display list
		glBindTexture(GL_TEXTURE_2D, patch.texture);	// Bind the texture

		for (u = 1; u <= divs; u++) {
			py = ((float)u) / ((float)divs);			// Percent along Y axis
			pyold = ((float)u - 1.0f) / ((float)divs);		// Percent along old Y axis

			temp[0] = Bernstein(py, patch.anchors[0]);	// Calculate new bezier points
			temp[1] = Bernstein(py, patch.anchors[1]);
			temp[2] = Bernstein(py, patch.anchors[2]);
			temp[3] = Bernstein(py, patch.anchors[3]);

			glBegin(GL_TRIANGLE_STRIP);					// Begin a new triangle strip

			for (v = 0; v <= divs; v++) {
				px = ((float)v) / ((float)divs);			// Percent along the X axis

				glTexCoord2f(pyold, px);				// Apply the old texture coords
				glVertex3d(last[v].x, last[v].y, last[v].z);	// Old Point

				last[v] = Bernstein(px, temp);			// Generate new point
				glTexCoord2f(py, px);					// Apply the new texture coords
				glVertex3d(last[v].x, last[v].y, last[v].z);	// New Point
			}

			glEnd();									// END the triangle srip
		}

		glEndList();								// END the list

		free(last);									// Free the old vertices array
		return drawlist;							// Return the display list
	}

	/************************************************************************************/

	void initBezier(void) {
		mybezier.anchors[0][0] = makePoint(-0.75, -0.75, -0.5);	// set the bezier vertices
		mybezier.anchors[0][1] = makePoint(-0.25, -0.75, 0.0);
		mybezier.anchors[0][2] = makePoint(0.25, -0.75, 0.0);
		mybezier.anchors[0][3] = makePoint(0.75, -0.75, -0.5);
		mybezier.anchors[1][0] = makePoint(-0.75, -0.25, -0.75);
		mybezier.anchors[1][1] = makePoint(-0.25, -0.25, 0.5);
		mybezier.anchors[1][2] = makePoint(0.25, -0.25, 0.5);
		mybezier.anchors[1][3] = makePoint(0.75, -0.25, -0.75);
		mybezier.anchors[2][0] = makePoint(-0.75, 0.25, 0.0);
		mybezier.anchors[2][1] = makePoint(-0.25, 0.25, -0.5);
		mybezier.anchors[2][2] = makePoint(0.25, 0.25, -0.5);
		mybezier.anchors[2][3] = makePoint(0.75, 0.25, 0.0);
		mybezier.anchors[3][0] = makePoint(-0.75, 0.75, -0.5);
		mybezier.anchors[3][1] = makePoint(-0.25, 0.75, -1.0);
		mybezier.anchors[3][2] = makePoint(0.25, 0.75, -1.0);
		mybezier.anchors[3][3] = makePoint(0.75, 0.75, -0.5);
		mybezier.dlBPatch = NULL;
	}

	/*****************************************************************************************/
	// Load Bitmaps And Convert To Textures

	BOOL LoadGLTexture(GLuint *texPntr, char* name)
	{
		BOOL success = FALSE;
		AUX_RGBImageRec *TextureImage = NULL;

		glGenTextures(1, texPntr);						// Generate 1 texture

		FILE* test = NULL;
		TextureImage = NULL;

		test = fopen(name, "r");						// test to see if the file exists
		if (test != NULL) {								// if it does
			fclose(test);									// close the file
			TextureImage = auxDIBImageLoad(name);			// and load the texture
		}

		if (TextureImage != NULL) {						// if it loaded
			success = TRUE;

			// Typical Texture Generation Using Data From The Bitmap
			glBindTexture(GL_TEXTURE_2D, *texPntr);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage->sizeX, TextureImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		if (TextureImage->data)
			free(TextureImage->data);

		return success;
	}

	/************************************************************************************/

	int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
	{
		glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping
		glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
		glClearColor(0.05f, 0.05f, 0.05f, 0.5f);			// Black Background
		glClearDepth(1.0f);									// Depth Buffer Setup
		glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
		glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

		initBezier();											// Initialize the Bezier's control grid
		LoadGLTexture(&(mybezier.texture), "./data/NeHe.bmp");	// Load the texture
		mybezier.dlBPatch = genBezier(mybezier, divs);			// Generate the patch

		return TRUE;										// Initialization Went OK
	}

	/************************************************************************************/

	int DrawGLScene(GLvoid) {								// Here's Where We Do All The Drawing
		int i, j;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
		glLoadIdentity();									// Reset The Current Modelview Matrix
		glTranslatef(0.0f, 0.0f, -4.0f);						// Move Left 1.5 Units And Into The Screen 6.0
		glRotatef(-75.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(rotz, 0.0f, 0.0f, 1.0f);						// Rotate The Triangle On The Z axis ( NEW )

		glCallList(mybezier.dlBPatch);						// Call the Bezier's display list
															// this need only be updated when the patch changes

		if (showCPoints) {									// If drawing the grid is toggled on
			glDisable(GL_TEXTURE_2D);
			glColor3f(1.0f, 0.0f, 0.0f);
			for (i = 0; i<4; i++) {								// draw the horizontal lines
				glBegin(GL_LINE_STRIP);
				for (j = 0; j<4; j++)
					glVertex3d(mybezier.anchors[i][j].x, mybezier.anchors[i][j].y, mybezier.anchors[i][j].z);
				glEnd();
			}
			for (i = 0; i<4; i++) {								// draw the vertical lines
				glBegin(GL_LINE_STRIP);
				for (j = 0; j<4; j++)
					glVertex3d(mybezier.anchors[j][i].x, mybezier.anchors[j][i].y, mybezier.anchors[j][i].z);
				glEnd();
			}
			glColor3f(1.0f, 1.0f, 1.0f);
			glEnable(GL_TEXTURE_2D);
		}

		return TRUE;										// Keep Going
	}

	/************************************************************************************/
	/************************************************************************************/

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

		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &DMsaved); // save the current display state (NEW)

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

		if (!RegisterClass(&wc))									// Attempt To Register The Window Class
		{
			MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;											// Return FALSE
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

	/************************************************************************************/

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
		if (!CreateGLWindow("David Nikdel & NeHe's Bezier Tutorial", 640, 480, 16, fullscreen))
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
				}


				if (keys[VK_LEFT])	rotz -= 0.8f;		// rotate left
				if (keys[VK_RIGHT])	rotz += 0.8f;		// rotate right
				if (keys[VK_UP]) {						// resolution up
					divs++;
					mybezier.dlBPatch = genBezier(mybezier, divs);	// Update the patch
					keys[VK_UP] = FALSE;
				}
				if (keys[VK_DOWN] && divs > 1) {
					divs--;
					mybezier.dlBPatch = genBezier(mybezier, divs);	// Update the patch
					keys[VK_DOWN] = FALSE;
				}
				if (keys[VK_SPACE]) {					// SPACE toggles showCPoints
					showCPoints = !showCPoints;
					keys[VK_SPACE] = FALSE;
				}


				if (keys[VK_F1])						// Is F1 Being Pressed?
				{
					keys[VK_F1] = FALSE;					// If So Make Key FALSE
					KillGLWindow();						// Kill Our Current Window
					fullscreen = !fullscreen;				// Toggle Fullscreen / Windowed Mode
															// Recreate Our OpenGL Window
					if (!CreateGLWindow("David Nikdel & NeHe's Bezier Tutorial", 640, 480, 16, fullscreen))
					{
						return 0;						// Quit If Window Was Not Created
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