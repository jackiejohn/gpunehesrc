#define _CRT_SECURE_NO_WARNINGS 1
//					Based On Lesson 6
//					=================
//
// All Code Is The Same, Except For Where I Have ( NEW )
// ( CHANGE ) Means I Have Changed The Line For This Tutorial.

#pragma comment(lib, "Glu32.lib")

#include <windows.h>											// Header File For Windows
#include <stdio.h>												// Header File For Standard Input/Output
#include <gl\gl.h>												// Header File For The OpenGL32 Library
#include <gl\glu.h>												// Header File For The GLu32 Library
#include "texture.h"											// Header File Containing Our Texture Structure ( NEW )
#include "baseapp.h"
#include "tga.h"

class Lesson33 {
public:
HDC			hDC = NULL;											// Private GDI Device Context
HGLRC		hRC = NULL;											// Permanent Rendering Context
HWND		hWnd = NULL;											// Holds Our Window Handle
HINSTANCE	hInstance;											// Holds The Instance Of The Application

bool	keys[256];												// Array Used For The Keyboard Routine
bool	active = TRUE;											// Window Active Flag Set To TRUE By Default
bool	fullscreen = TRUE;										// Fullscreen Flag Set To Fullscreen Mode By Default

float	spin;													// Spin Variable

Texture texture[2];												// Storage For 2 Textures ( NEW )

public:
	bool LoadTGA(Texture * texture, char * filename)				// Load a TGA file
	{
		FILE * fTGA;												// File pointer to texture file
		fTGA = fopen(filename, "rb");								// Open file for reading

		if (fTGA == NULL)											// If it didn't open....
		{
			MessageBox(NULL, "Could not open texture file", "ERROR", MB_OK);	// Display an error message
			return false;														// Exit function
		}

		if (fread(&tgaheader, sizeof(TGAHeader), 1, fTGA) == 0)					// Attempt to read 12 byte header from file
		{
			MessageBox(NULL, "Could not read file header", "ERROR", MB_OK);		// If it fails, display an error message 
			if (fTGA != NULL)													// Check to seeiffile is still open
			{
				fclose(fTGA);													// If it is, close it
			}
			return false;														// Exit function
		}

		if (memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)				// See if header matches the predefined header of 
		{																		// an Uncompressed TGA image
			LoadUncompressedTGA(texture, filename, fTGA);						// If so, jump to Uncompressed TGA loading code
		}
		else if (memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)		// See if header matches the predefined header of
		{																		// an RLE compressed TGA image
			LoadCompressedTGA(texture, filename, fTGA);							// If so, jump to Compressed TGA loading code
		}
		else																	// If header matches neither type
		{
			MessageBox(NULL, "TGA file be type 2 or type 10 ", "Invalid Image", MB_OK);	// Display an error
			fclose(fTGA);
			return false;																// Exit function
		}
		return true;															// All went well, continue on
	}

	bool LoadUncompressedTGA(Texture * texture, char * filename, FILE * fTGA)	// Load an uncompressed TGA (note, much of this code is based on NeHe's 
	{																			// TGA Loading code nehe.gamedev.net)
		if (fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)					// Read TGA header
		{
			MessageBox(NULL, "Could not read info header", "ERROR", MB_OK);		// Display error
			if (fTGA != NULL)													// if file is still open
			{
				fclose(fTGA);													// Close it
			}
			return false;														// Return failular
		}

		texture->width = tga.header[1] * 256 + tga.header[0];					// Determine The TGA Width	(highbyte*256+lowbyte)
		texture->height = tga.header[3] * 256 + tga.header[2];					// Determine The TGA Height	(highbyte*256+lowbyte)
		texture->bpp = tga.header[4];										// Determine the bits per pixel
		tga.Width = texture->width;										// Copy width into local structure						
		tga.Height = texture->height;										// Copy height into local structure
		tga.Bpp = texture->bpp;											// Copy BPP into local structure

		if ((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp != 32)))	// Make sure all information is valid
		{
			MessageBox(NULL, "Invalid texture information", "ERROR", MB_OK);	// Display Error
			if (fTGA != NULL)													// Check if file is still open
			{
				fclose(fTGA);													// If so, close it
			}
			return false;														// Return failed
		}

		if (texture->bpp == 24)													// If the BPP of the image is 24...
			texture->type = GL_RGB;											// Set Image type to GL_RGB
		else																	// Else if its 32 BPP
			texture->type = GL_RGBA;											// Set image type to GL_RGBA

		tga.bytesPerPixel = (tga.Bpp / 8);									// Compute the number of BYTES per pixel
		tga.imageSize = (tga.bytesPerPixel * tga.Width * tga.Height);		// Compute the total amout ofmemory needed to store data
		texture->imageData = (GLubyte *)malloc(tga.imageSize);					// Allocate that much memory

		if (texture->imageData == NULL)											// If no space was allocated
		{
			MessageBox(NULL, "Could not allocate memory for image", "ERROR", MB_OK);	// Display Error
			fclose(fTGA);														// Close the file
			return false;														// Return failed
		}

		if (fread(texture->imageData, 1, tga.imageSize, fTGA) != tga.imageSize)	// Attempt to read image data
		{
			MessageBox(NULL, "Could not read image data", "ERROR", MB_OK);		// Display Error
			if (texture->imageData != NULL)										// If imagedata has data in it
			{
				free(texture->imageData);										// Delete data from memory
			}
			fclose(fTGA);														// Close file
			return false;														// Return failed
		}

		// Byte Swapping Optimized By Steve Thomas
		for (GLuint cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
		{
			texture->imageData[cswap] ^= texture->imageData[cswap + 2] ^=
				texture->imageData[cswap] ^= texture->imageData[cswap + 2];
		}

		fclose(fTGA);															// Close file
		return true;															// Return success
	}

	bool LoadCompressedTGA(Texture * texture, char * filename, FILE * fTGA)		// Load COMPRESSED TGAs
	{
		if (fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)					// Attempt to read header
		{
			MessageBox(NULL, "Could not read info header", "ERROR", MB_OK);		// Display Error
			if (fTGA != NULL)													// If file is open
			{
				fclose(fTGA);													// Close it
			}
			return false;														// Return failed
		}

		texture->width = tga.header[1] * 256 + tga.header[0];					// Determine The TGA Width	(highbyte*256+lowbyte)
		texture->height = tga.header[3] * 256 + tga.header[2];					// Determine The TGA Height	(highbyte*256+lowbyte)
		texture->bpp = tga.header[4];										// Determine Bits Per Pixel
		tga.Width = texture->width;										// Copy width to local structure
		tga.Height = texture->height;										// Copy width to local structure
		tga.Bpp = texture->bpp;											// Copy width to local structure

		if ((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp != 32)))	//Make sure all texture info is ok
		{
			MessageBox(NULL, "Invalid texture information", "ERROR", MB_OK);	// If it isnt...Display error
			if (fTGA != NULL)													// Check if file is open
			{
				fclose(fTGA);													// Ifit is, close it
			}
			return false;														// Return failed
		}

		if (texture->bpp == 24)													// If the BPP of the image is 24...
			texture->type = GL_RGB;											// Set Image type to GL_RGB
		else																	// Else if its 32 BPP
			texture->type = GL_RGBA;											// Set image type to GL_RGBA

		tga.bytesPerPixel = (tga.Bpp / 8);									// Compute BYTES per pixel
		tga.imageSize = (tga.bytesPerPixel * tga.Width * tga.Height);		// Compute amout of memory needed to store image
		texture->imageData = (GLubyte *)malloc(tga.imageSize);					// Allocate that much memory

		if (texture->imageData == NULL)											// If it wasnt allocated correctly..
		{
			MessageBox(NULL, "Could not allocate memory for image", "ERROR", MB_OK);	// Display Error
			fclose(fTGA);														// Close file
			return false;														// Return failed
		}

		GLuint pixelcount = tga.Height * tga.Width;							// Nuber of pixels in the image
		GLuint currentpixel = 0;												// Current pixel being read
		GLuint currentbyte = 0;												// Current byte 
		GLubyte * colorbuffer = (GLubyte *)malloc(tga.bytesPerPixel);			// Storage for 1 pixel

		do
		{
			GLubyte chunkheader = 0;											// Storage for "chunk" header

			if (fread(&chunkheader, sizeof(GLubyte), 1, fTGA) == 0)				// Read in the 1 byte header
			{
				MessageBox(NULL, "Could not read RLE header", "ERROR", MB_OK);	// Display Error
				if (fTGA != NULL)												// If file is open
				{
					fclose(fTGA);												// Close file
				}
				if (texture->imageData != NULL)									// If there is stored image data
				{
					free(texture->imageData);									// Delete image data
				}
				return false;													// Return failed
			}

			if (chunkheader < 128)												// If the ehader is < 128, it means the that is the number of RAW color packets minus 1
			{																	// that follow the header
				chunkheader++;													// add 1 to get number of following color values
				for (short counter = 0; counter < chunkheader; counter++)		// Read RAW color values
				{
					if (fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel) // Try to read 1 pixel
					{
						MessageBox(NULL, "Could not read image data", "ERROR", MB_OK);		// IF we cant, display an error

						if (fTGA != NULL)													// See if file is open
						{
							fclose(fTGA);													// If so, close file
						}

						if (colorbuffer != NULL)												// See if colorbuffer has data in it
						{
							free(colorbuffer);												// If so, delete it
						}

						if (texture->imageData != NULL)										// See if there is stored Image data
						{
							free(texture->imageData);										// If so, delete it too
						}

						return false;														// Return failed
					}
					// write to memory
					texture->imageData[currentbyte] = colorbuffer[2];				    // Flip R and B vcolor values around in the process 
					texture->imageData[currentbyte + 1] = colorbuffer[1];
					texture->imageData[currentbyte + 2] = colorbuffer[0];

					if (tga.bytesPerPixel == 4)												// if its a 32 bpp image
					{
						texture->imageData[currentbyte + 3] = colorbuffer[3];				// copy the 4th byte
					}

					currentbyte += tga.bytesPerPixel;										// Increase thecurrent byte by the number of bytes per pixel
					currentpixel++;															// Increase current pixel by 1

					if (currentpixel > pixelcount)											// Make sure we havent read too many pixels
					{
						MessageBox(NULL, "Too many pixels read", "ERROR", NULL);			// if there is too many... Display an error!

						if (fTGA != NULL)													// If there is a file open
						{
							fclose(fTGA);													// Close file
						}

						if (colorbuffer != NULL)												// If there is data in colorbuffer
						{
							free(colorbuffer);												// Delete it
						}

						if (texture->imageData != NULL)										// If there is Image data
						{
							free(texture->imageData);										// delete it
						}

						return false;														// Return failed
					}
				}
			}
			else																			// chunkheader > 128 RLE data, next color reapeated chunkheader - 127 times
			{
				chunkheader -= 127;															// Subteact 127 to get rid of the ID bit
				if (fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)		// Attempt to read following color values
				{
					MessageBox(NULL, "Could not read from file", "ERROR", MB_OK);			// If attempt fails.. Display error (again)

					if (fTGA != NULL)														// If thereis a file open
					{
						fclose(fTGA);														// Close it
					}

					if (colorbuffer != NULL)													// If there is data in the colorbuffer
					{
						free(colorbuffer);													// delete it
					}

					if (texture->imageData != NULL)											// If thereis image data
					{
						free(texture->imageData);											// delete it
					}

					return false;															// return failed
				}

				for (short counter = 0; counter < chunkheader; counter++)					// copy the color into the image data as many times as dictated 
				{																			// by the header
					texture->imageData[currentbyte] = colorbuffer[2];					// switch R and B bytes areound while copying
					texture->imageData[currentbyte + 1] = colorbuffer[1];
					texture->imageData[currentbyte + 2] = colorbuffer[0];

					if (tga.bytesPerPixel == 4)												// If TGA images is 32 bpp
					{
						texture->imageData[currentbyte + 3] = colorbuffer[3];				// Copy 4th byte
					}

					currentbyte += tga.bytesPerPixel;										// Increase current byte by the number of bytes per pixel
					currentpixel++;															// Increase pixel count by 1

					if (currentpixel > pixelcount)											// Make sure we havent written too many pixels
					{
						MessageBox(NULL, "Too many pixels read", "ERROR", NULL);			// if there is too many... Display an error!

						if (fTGA != NULL)													// If there is a file open
						{
							fclose(fTGA);													// Close file
						}

						if (colorbuffer != NULL)												// If there is data in colorbuffer
						{
							free(colorbuffer);												// Delete it
						}

						if (texture->imageData != NULL)										// If there is Image data
						{
							free(texture->imageData);										// delete it
						}

						return false;														// Return failed
					}
				}
			}
		}

		while (currentpixel < pixelcount);													// Loop while there are still pixels left
		fclose(fTGA);																		// Close the file
		return true;																		// return success
	}


	int LoadGLTextures()											// Load Bitmaps And Convert To Textures
	{
		int Status = FALSE;											// Status Indicator

																	// Load The Bitmap, Check For Errors.
		if (LoadTGA(&texture[0], "Data/Uncompressed.tga") &&
			LoadTGA(&texture[1], "Data/Compressed.tga"))
		{
			Status = TRUE;											// Set The Status To TRUE

			for (int loop = 0; loop<2; loop++)						// Loop Through Both Textures
			{
				// Typical Texture Generation Using Data From The TGA ( CHANGE )
				glGenTextures(1, &texture[loop].texID);				// Create The Texture ( CHANGE )
				glBindTexture(GL_TEXTURE_2D, texture[loop].texID);
				glTexImage2D(GL_TEXTURE_2D, 0, texture[loop].bpp / 8, texture[loop].width, texture[loop].height, 0, texture[loop].type, GL_UNSIGNED_BYTE, texture[loop].imageData);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				if (texture[loop].imageData)						// If Texture Image Exists ( CHANGE )
				{
					free(texture[loop].imageData);					// Free The Texture Image Memory ( CHANGE )
				}
			}
		}
		return Status;												// Return The Status
	}

	GLvoid ReSizeGLScene(GLsizei width, GLsizei height)				// Resize And Initialize The GL Window
	{
		if (height == 0)												// Prevent A Divide By Zero By
		{
			height = 1;												// Making Height Equal One
		}

		glViewport(0, 0, width, height);								// Reset The Current Viewport

		glMatrixMode(GL_PROJECTION);								// Select The Projection Matrix
		glLoadIdentity();											// Reset The Projection Matrix

																	// Calculate The Aspect Ratio Of The Window
		gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

		glMatrixMode(GL_MODELVIEW);									// Select The Modelview Matrix
		glLoadIdentity();											// Reset The Modelview Matrix
	}

	int InitGL(GLvoid)												// All Setup For OpenGL Goes Here
	{
		if (!LoadGLTextures())										// Jump To Texture Loading Routine ( NEW )
		{
			return FALSE;											// If Texture Didn't Load Return FALSE
		}

		glEnable(GL_TEXTURE_2D);									// Enable Texture Mapping ( NEW )
		glShadeModel(GL_SMOOTH);									// Enable Smooth Shading
		glClearColor(0.0f, 0.0f, 0.0f, 0.5f);						// Black Background
		glClearDepth(1.0f);											// Depth Buffer Setup
		glEnable(GL_DEPTH_TEST);									// Enables Depth Testing
		glDepthFunc(GL_LEQUAL);										// The Type Of Depth Testing To Do
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Really Nice Perspective Calculations

		return TRUE;												// Initialization Went OK
	}

	int DrawGLScene(GLvoid)											// Here's Where We Do All The Drawing
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			// Clear The Screen And The Depth Buffer
		glLoadIdentity();											// Reset The Modelview Matrix
		glTranslatef(0.0f, 0.0f, -10.0f);								// Translate 20 Units Into The Screen

		spin += 0.05f;												// Increase Spin

		for (int loop = 0; loop<20; loop++)							// Loop Of 20
		{
			glPushMatrix();											// Push The Matrix
			glRotatef(spin + loop*18.0f, 1.0f, 0.0f, 0.0f);				// Rotate On The X-Axis (Up - Down)
			glTranslatef(-2.0f, 2.0f, 0.0f);							// Translate 2 Units Left And 2 Up

			glBindTexture(GL_TEXTURE_2D, texture[0].texID);			// ( CHANGE )
			glBegin(GL_QUADS);										// Draw Our Quad
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
			glEnd();												// Done Drawing The Quad
			glPopMatrix();											// Pop The Matrix

			glPushMatrix();											// Push The Matrix
			glTranslatef(2.0f, 0.0f, 0.0f);							// Translate 2 Units To The Right
			glRotatef(spin + loop*36.0f, 0.0f, 1.0f, 0.0f);				// Rotate On The Y-Axis (Left - Right)
			glTranslatef(1.0f, 0.0f, 0.0f);							// Move One Unit Right

			glBindTexture(GL_TEXTURE_2D, texture[1].texID);			// ( CHANGE )
			glBegin(GL_QUADS);										// Draw Our Quad
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, -1.0f, 0.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
			glEnd();												// Done Drawing The Quad
			glPopMatrix();											// Pop The Matrix
		}
		return TRUE;												// Keep Going
	}

	GLvoid KillGLWindow(GLvoid)										// Properly Kill The Window
	{
		if (fullscreen)												// Are We In Fullscreen Mode?
		{
			ChangeDisplaySettings(NULL, 0);							// If So Switch Back To The Desktop
			ShowCursor(TRUE);										// Show Mouse Pointer
		}

		if (hRC)													// Do We Have A Rendering Context?
		{
			if (!wglMakeCurrent(NULL, NULL))							// Are We Able To Release The DC And RC Contexts?
			{
				MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			}

			if (!wglDeleteContext(hRC))								// Are We Able To Delete The RC?
			{
				MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			}
			hRC = NULL;												// Set RC To NULL
		}

		if (hDC && !ReleaseDC(hWnd, hDC))							// Are We Able To Release The DC
		{
			MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			hDC = NULL;												// Set DC To NULL
		}

		if (hWnd && !DestroyWindow(hWnd))							// Are We Able To Destroy The Window?
		{
			MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			hWnd = NULL;												// Set hWnd To NULL
		}

		if (!UnregisterClass("OpenGL", hInstance))					// Are We Able To Unregister Class
		{
			MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			hInstance = NULL;											// Set hInstance To NULL
		}
	}

	/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
	*	title			- Title To Appear At The Top Of The Window				*
	*	width			- Width Of The GL Window Or Fullscreen Mode				*
	*	height			- Height Of The GL Window Or Fullscreen Mode			*
	*	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
	*	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/

	BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
	{
		GLuint		PixelFormat;									// Holds The Results After Searching For A Match
		WNDCLASS	wc;												// Windows Class Structure
		DWORD		dwExStyle;										// Window Extended Style
		DWORD		dwStyle;										// Window Style
		RECT		WindowRect;										// Grabs Rectangle Upper Left / Lower Right Values
		WindowRect.left = (long)0;									// Set Left Value To 0
		WindowRect.right = (long)width;								// Set Right Value To Requested Width
		WindowRect.top = (long)0;										// Set Top Value To 0
		WindowRect.bottom = (long)height;								// Set Bottom Value To Requested Height

		fullscreen = fullscreenflag;									// Set The Global Fullscreen Flag

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
					fullscreen = FALSE;								// Windowed Mode Selected.  Fullscreen = FALSE
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

		AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);	// Adjust Window To True Requested Size

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
			KillGLWindow();											// Reset The Display
			MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;											// Return FALSE
		}

		static	PIXELFORMATDESCRIPTOR pfd =							// pfd Tells Windows How We Want Things To Be
		{
			sizeof(PIXELFORMATDESCRIPTOR),							// Size Of This Pixel Format Descriptor
			1,														// Version Number
			PFD_DRAW_TO_WINDOW |									// Format Must Support Window
			PFD_SUPPORT_OPENGL |									// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,										// Must Support Double Buffering
			PFD_TYPE_RGBA,											// Request An RGBA Format
			bits,													// Select Our Color Depth
			0, 0, 0, 0, 0, 0,										// Color Bits Ignored
			0,														// No Alpha Buffer
			0,														// Shift Bit Ignored
			0,														// No Accumulation Buffer
			0, 0, 0, 0,												// Accumulation Bits Ignored
			16,														// 16Bit Z-Buffer (Depth Buffer)  
			0,														// No Stencil Buffer
			0,														// No Auxiliary Buffer
			PFD_MAIN_PLANE,											// Main Drawing Layer
			0,														// Reserved
			0, 0, 0													// Layer Masks Ignored
		};

		if (!(hDC = GetDC(hWnd)))										// Did We Get A Device Context?
		{
			KillGLWindow();											// Reset The Display
			MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;											// Return FALSE
		}

		if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))				// Did Windows Find A Matching Pixel Format?
		{
			KillGLWindow();											// Reset The Display
			MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;											// Return FALSE
		}

		if (!SetPixelFormat(hDC, PixelFormat, &pfd))					// Are We Able To Set The Pixel Format?
		{
			KillGLWindow();											// Reset The Display
			MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;											// Return FALSE
		}

		if (!(hRC = wglCreateContext(hDC)))							// Are We Able To Get A Rendering Context?
		{
			KillGLWindow();											// Reset The Display
			MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;											// Return FALSE
		}

		if (!wglMakeCurrent(hDC, hRC))								// Try To Activate The Rendering Context
		{
			KillGLWindow();											// Reset The Display
			MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;											// Return FALSE
		}

		ShowWindow(hWnd, SW_SHOW);									// Show The Window
		SetForegroundWindow(hWnd);									// Slightly Higher Priority
		SetFocus(hWnd);												// Sets Keyboard Focus To The Window
		ReSizeGLScene(width, height);								// Set Up Our Perspective GL Screen

		if (!InitGL())												// Initialize Our Newly Created GL Window
		{
			KillGLWindow();											// Reset The Display
			MessageBox(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;											// Return FALSE
		}

		return TRUE;												// Success
	}


	int WINAPI WinMain(HINSTANCE	hInstance,						// Instance
		HINSTANCE	hPrevInstance,					// Previous Instance
		LPSTR		lpCmdLine,						// Command Line Parameters
		int			nCmdShow)						// Window Show State
	{
		MSG		msg;												// Windows Message Structure
		BOOL	done = FALSE;											// Bool Variable To Exit Loop

																		// Ask The User Which Screen Mode They Prefer
		if (MessageBox(NULL, "Would You Like To Run In Fullscreen Mode?", "Start FullScreen?", MB_YESNO | MB_ICONQUESTION) == IDNO)
		{
			fullscreen = FALSE;										// Windowed Mode
		}

		// Create Our OpenGL Window
		if (!CreateGLWindow("NeHe & Evan 'terminate' Pipho's TGA Loading Tutorial", 640, 480, 16, fullscreen))
		{
			return 0;												// Quit If Window Was Not Created
		}

		while (!done)												// Loop That Runs While done=FALSE
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))				// Is There A Message Waiting?
			{
				if (msg.message == WM_QUIT)							// Have We Received A Quit Message?
				{
					done = TRUE;										// If So done=TRUE
				}
				else												// If Not, Deal With Window Messages
				{
					TranslateMessage(&msg);							// Translate The Message
					DispatchMessage(&msg);							// Dispatch The Message
				}
			}
			else													// If There Are No Messages
			{
				// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
				if ((active && !DrawGLScene()) || keys[VK_ESCAPE])	// Active?  Was There A Quit Received?
				{
					done = TRUE;										// ESC or DrawGLScene Signalled A Quit
				}
				else												// Not Time To Quit, Update Screen
				{
					SwapBuffers(hDC);								// Swap Buffers (Double Buffering)
				}

				if (keys[VK_F1])									// Is F1 Being Pressed?
				{
					keys[VK_F1] = FALSE;								// If So Make Key FALSE
					KillGLWindow();									// Kill Our Current Window
					fullscreen = !fullscreen;							// Toggle Fullscreen / Windowed Mode
																		// Recreate Our OpenGL Window
					if (!CreateGLWindow("NeHe & Evan 'terminate' Pipho's TGA Loading Tutorial", 640, 480, 16, fullscreen))
					{
						return 0;									// Quit If Window Was Not Created
					}
				}
			}
		}

		// Shutdown
		KillGLWindow();												// Kill The Window
		return (msg.wParam);										// Exit The Program
	}


};
