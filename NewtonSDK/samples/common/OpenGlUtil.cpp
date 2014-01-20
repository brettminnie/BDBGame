//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// 
//********************************************************************

// OpenGlUtil.cpp: implementation of the OpenGlUtil class.
//
//////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "OpenGlUtil.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


// open gl callbacks

static int windowWidth; 
static int windowHeight;
static RenderLoop g_render;

static int xOrigin = 10000;	
static int yOrigin = 10000;



// index for the texture we'll load for the cube
GLuint g_cubeTexture;
GLuint g_ballTexture;
GLuint g_floorTexture;
GLuint g_buggyTexture;
GLuint g_tireTexture;


  
// Rotate the cube by 4 degrees and force a redisplay.
static void Animate()
{
	glutPostRedisplay();
} 


//	Reset the viewport for window changes
static void Reshape(int width, int height)
{
	if (height == 0)
		return;
	glViewport(0, 0, (GLsizei) width, (GLsizei) height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, width/height, 0.2, 500.0);
	
	glMatrixMode(GL_MODELVIEW);
} 


//	Clear and redraw the scene.
static void Display()
{
	
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	g_render ();
	
	// draw everything and swap the display buffer
	glFlush();
	glutSwapBuffers();
} 


static void CaptureMousePos (int button, int state, int x, int y)
{
	static bool firstTime = false;
	MOUSE_POINT point;
	
	if (firstTime) {
		GetCursorPos(point);
		xOrigin = point.x - x; 
		yOrigin = point.y - y;
	}
	firstTime = true;
}


static void MouseScreenToClient (int& x, int& y)
{
	x = max (min (x - xOrigin, windowWidth), 0);
	y = max (min (y - yOrigin, windowHeight), 0);
}



void GetCursorPos(MOUSE_POINT& point)
{
	#ifdef _MSC_VER
		POINT p;
		GetCursorPos (&p);	
		point.x = p.x;
		point.y = p.y;
	#else
		Point p;
		GetMouse (&p);
		point.x = p.h;
		point.y = p.v;
	#endif
}


//	Loads the texture from the specified file and stores it in iTexture. Note
//	that we're using the GLAUX library here, which is generally discouraged,
//	but in this case spares us having to write a bitmap loading routine.
void LoadTexture(char *filename, GLuint &texture)
{
	#pragma pack(1)
	struct TGAHEADER
	{
		GLbyte	identsize;              // Size of ID field that follows header (0)
		GLbyte	colorMapType;           // 0 = None, 1 = paletted
		GLbyte	imageType;              // 0 = none, 1 = indexed, 2 = rgb, 3 = grey, +8=rle
		unsigned short	colorMapStart;  // First colour map entry
		unsigned short	colorMapLength; // Number of colors
		unsigned char 	colorMapBits;   // bits per palette entry
		unsigned short	xstart;         // image x origin
		unsigned short	ystart;         // image y origin
		unsigned short	width;          // width in pixels
		unsigned short	height;         // height in pixels
		GLbyte	bits;                   // bits per pixel (8 16, 24, 32)
		GLbyte	descriptor;             // image descriptor
	};
	#pragma pack(8)

	#define BYTE_SWAP(x) x = ((x) >> 8) + ((x) << 8)


	FILE *pFile;				// File pointer
	TGAHEADER tgaHeader;		// TGA file header
	unsigned lImageSize;		// Size in bytes of image
	short sDepth;				// Pixel depth;
	GLbyte	*pBits = NULL;      // Pointer to bits
	char fullPathName[256];
	
	GLint iWidth;
	GLint iHeight;
	GLint iComponents;
	GLenum eFormat;
    
    // Default/Failed values
    eFormat = GL_RGBA;
    iComponents = GL_RGB8;

	texture = 0;
	
	#ifdef _MSC_VER		
		sprintf (fullPathName, "%s", filename);
	#else
		sprintf (fullPathName, "../../../%s", filename);
	#endif	
	
    // Attempt to open the fil
    pFile = fopen(fullPathName, "rb");
    if(pFile == NULL) {
		return;
	}
    
	  _ASSERTE (sizeof (TGAHEADER) == 18);
    // Read in header (binary) // sizeof(TGAHEADER) = 18
    fread(&tgaHeader, 18, 1, pFile);
    
    // Do byte swap for big vs little endian
	#ifndef _M_IX86
		BYTE_SWAP(tgaHeader.colorMapStart);
		BYTE_SWAP(tgaHeader.colorMapLength);
		BYTE_SWAP(tgaHeader.xstart);
		BYTE_SWAP(tgaHeader.ystart);
		BYTE_SWAP(tgaHeader.width);
		BYTE_SWAP(tgaHeader.height);
	#endif


        
    // Get width, height, and depth of texture
    iWidth = tgaHeader.width;
    iHeight = tgaHeader.height;
    sDepth = tgaHeader.bits / 8;

    
    // Put some validity checks here. Very simply, I only understand
    // or care about 8, 24, or 32 bit targa's.
    if(tgaHeader.bits != 8 && tgaHeader.bits != 24 && tgaHeader.bits != 32) {
		fclose(pFile);
		return;
	}


    // Calculate size of image buffer
    lImageSize = tgaHeader.width * tgaHeader.height * sDepth;
    
    // Allocate memory and check for success
    pBits = (GLbyte *)malloc(lImageSize * sizeof(GLbyte));
    if(pBits == NULL) {
		fclose(pFile);
		return;
	}


    
    // Read in the bits
    // Check for read error. This should catch RLE or other 
    // weird formats that I don't want to recognize
    if(fread(pBits, lImageSize, 1, pFile) != 1)  {
		fclose(pFile);
        free(pBits);
        return;
   }

    
    // Set OpenGL format expected
	#ifdef _M_IX86
		switch(sDepth)
		{
			case 3:     // Most likely case
				eFormat = GL_RGB;
				iComponents = GL_RGB8;
				break;
			case 4:
				eFormat = GL_BGRA_EXT;
				iComponents = GL_RGBA8;
				break;
			case 1:
				eFormat = GL_LUMINANCE;
				iComponents = GL_LUMINANCE8;
				break;
		};
	#else
		switch(sDepth)
		{
			case 3:     // Most likely case
				eFormat = GL_RGBA;
				iComponents = GL_RGB8;
				break;
			case 4:
				eFormat = GL_BGRA;
				iComponents = GL_RGBA8;
				break;
			case 1:
				eFormat = GL_LUMINANCE;
				iComponents = GL_LUMINANCE8;
				break;
		};	
	#endif
        

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
			
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBits);

    // Done with File
    fclose(pFile);
	free(pBits);        

} 


void InitOpenGl(
	int argc, 
	char **argv, 
	const char *title,
	RenderLoop renderFnt)
{
	// initilize opengl	
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	windowWidth = 800; 
	windowHeight = 600;
	
	// Create the window
	glutInitWindowSize(windowWidth, windowHeight);
//	glutInitWindowPosition(400, 350);
	glutCreateWindow(title);

	// uncomment this to run in full screen (very slow)
//	glutFullScreen();

	// set the background color
	glClearColor(0.5, 0.5, 0.5, 0.0);
	
	// set the shading model
	glShadeModel(GL_SMOOTH);
	
	// set up a single white light
	
	GLfloat lightColor[] = { 0.7f, 0.7f, 0.7f, 0.0 };
	GLfloat lightAmbientColor[] = { 0.125f, 0.125f, 0.125f, 0.0 };
	GLfloat lightPoition[] = { 500.0f, 200.0f, 500.0f, 0.0 };
	
	glLightfv(GL_LIGHT0, GL_POSITION, lightPoition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	
	// load the texture for the cube
	LoadTexture("newton.tga", g_cubeTexture);

	// load the texture for the cube
	LoadTexture("earthmap.tga", g_ballTexture);

	// load the texture for the font
	LoadTexture("floor.tga", g_floorTexture);

	// load the texture for the font
	LoadTexture("buggy_sk.tga", g_buggyTexture);


	// load the texture for the font
	LoadTexture("tyre.tga", g_tireTexture);

	
	// make the modelview matrix active, and initialize it
	glMatrixMode(GL_MODELVIEW);

	
	// Register the event callback functions
	glutDisplayFunc(Display); 
	glutReshapeFunc(Reshape);
	
//#ifndef _MSC_VER	
	glutMouseFunc(CaptureMousePos);
//#endif
//	glutKeyboardFunc(keyboard);
	glutIdleFunc(Animate);

	g_render = renderFnt;



}


void SetCamera (const dVector& eye, const dVector& target)
{
	// set up the view orientation looking at the origin from slightly above
	// and to the left
	glLoadIdentity();
	//gluLookAt(0.0, 1.0, 6.0, 0.0, 0.0, 0.0,	0.0, 1.0, 0.0);
	gluLookAt(eye.m_x, eye.m_y, eye.m_z, target.m_x, target.m_y, target.m_z, 0.0, 1.0, 0.0);

	glMatrixMode(GL_MODELVIEW);

}


void Print (const dVector& color, dFloat x, dFloat y, const char *fmt, ... )
{	
	int i;
	int len;
	char string[1024];
	
	va_list argptr;
	
	va_start (argptr, fmt);
	vsprintf (string, fmt, argptr);
	va_end( argptr );

	glColor3f(color.m_x, color.m_y, color.m_z);
	glDisable (GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, windowWidth, 0, windowHeight );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glRasterPos3f(x, windowHeight - 15 - y, 0);
	len = (int) strlen (string );
	for (i = 0; i < len; i++) {
		glutBitmapCharacter (GLUT_BITMAP_TIMES_ROMAN_24, string[i] );
	}
	
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable( GL_DEPTH_TEST );

}



dVector ScreenToWorld (int screenx, int screeny, int screenz)
{
	//Where the values will be stored
	GLint viewport[4]; 

	//Retrieves the viewport and stores it in the variable
	glGetIntegerv(GL_VIEWPORT, viewport); 

	//Where the 16 doubles of the matrix are to be stored
	GLdouble modelview[16]; 

	//Retrieve the matrix
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview); 

	GLdouble projection[16]; 
	glGetDoublev(GL_PROJECTION_MATRIX, projection);

	GLdouble winX = 0.0;
	GLdouble winY = 0.0;
	GLdouble winZ = 0.0; //The coordinates to pass in

	MouseScreenToClient (screenx, screeny);

	winX = dFloat(screenx); //Store the x coord
	winY = dFloat(screeny); //Store the y coord
	winZ = dFloat(screenz); //Store the Z coord
	

	//Now windows coordinates start with (0,0) being at the top left whereas OpenGL coords start lower left so we have to do this to convert it: 
	//Remember we got viewport value before 
	winY = (dFloat)viewport[3] - winY; 

	GLdouble objx;
	GLdouble objy;
	GLdouble objz;
	gluUnProject (winX, winY, winZ, modelview, projection, viewport, &objx, &objy, &objz);

	//gluProject(objx, objy, objz, modelview, projection, viewport, &winX, &winY, &winZ);


	return dVector (dFloat(objx), dFloat(objy), dFloat(objz));
}


int dGetKeyState(int key)
{
	#ifdef _MSC_VER	
		return GetAsyncKeyState (key);
	#else
	
		UInt32 state;
		static bool haveKeyMap = false;
		static char virtualKeyCode[128];
		
		if (!haveKeyMap) {
			haveKeyMap = true;
			state = 0;
			memset (virtualKeyCode, 0, sizeof (virtualKeyCode));
			// this is an adbomination, the function, doe not agree with the function GetKey
			//for (int i = 0; i < 128; i ++) {
				//int val = KeyTranslate (NULL, i, &state);
				//virtualKeyCode[val] = i;
			//}
			
			// hardcoding the key mapping, if some one figure out how to make thois work please let me know
			virtualKeyCode[VK_ESCAPE] = 46;       // scappe key
			virtualKeyCode[VK_LBUTTON] = 35;
			virtualKeyCode[VK_F1] = 0x63;
			virtualKeyCode[VK_F2] = 0x61;
			virtualKeyCode[VK_F3] = 0x7b;
			
			virtualKeyCode[int('a')] = 25;
			virtualKeyCode[int('A')] = 25;
			virtualKeyCode[int('d')] = 27;
			virtualKeyCode[int('D')] = 27;
			virtualKeyCode[int('w')] = 22;
			virtualKeyCode[int('W')] = 22;
			virtualKeyCode[int('s')] = 26;
			virtualKeyCode[int('S')] = 26;
		}
	
		if (key == VK_LBUTTON) {
			if (StillDown()){
				return 0x8000;
			}
			return 0;
		} 
					
		KeyMap theKeys;
		GetKeys (theKeys);
			
		int index;
		index = 0;
		
		for (int i = 0; i <4; i ++) {
			if (theKeys[i]) {
			   for (int mask = theKeys[i]; mask; mask >>= 1) {
					index ++;
			   }
			   if (virtualKeyCode[key] == index) {
					return 0x8000;
			   }
			}
			index += 32;
		}
		//statte = theKey.
		return 0;
	#endif

}




unsigned dRand ()
{
	#define RAND_MUL 31415821u
	static unsigned randSeed = 0;
 	randSeed = RAND_MUL * randSeed + 1; 
	return randSeed & dRAND_MAX;
}


void ShowMousePicking (const dVector& p0, const dVector& p1)
{

	dFloat radius = 0.25f;
	// set the color of the cube's surface
	GLfloat cubeColor[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, cubeColor);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cubeColor);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0);


	// set up the cube's texture
	glDisable(GL_TEXTURE_2D);


	GLUquadricObj *pObj;
	glPushMatrix();
	dMatrix matrix (GetIdentityMatrix());
	matrix.m_posit = p0;
	glMultMatrix(&matrix[0][0]);
	
	// Get a new Quadric off the stack
	pObj = gluNewQuadric();				
	gluQuadricTexture(pObj, true);						
	gluSphere(pObj, radius, 10, 10);					
	glPopMatrix();

	glPushMatrix();
	matrix.m_posit = p1;
	glMultMatrix(&matrix[0][0]);
	gluSphere(pObj, radius, 10, 10);					
	gluDeleteQuadric(pObj);	
	glPopMatrix();

	dVector dir (p1 - p0);
	dFloat lenght (dir % dir);
	if (lenght > 1.0e-2f) {
	
		glPushMatrix();
		
		lenght = sqrt (lenght);
		dMatrix align (dgYawMatrix(0.5f * 3.1426f));
		align.m_posit.m_x = -lenght * 0.5f;
		matrix = align * dgGrammSchmidt(dir);
		matrix.m_posit += (p1 + p0).Scale (0.5f);
		glMultMatrix(&matrix[0][0]);
		
		// Get a new Quadric off the stack
		pObj = gluNewQuadric();				
		gluCylinder(pObj, radius * 0.5f, radius * 0.5f, lenght, 10, 2);
		gluDeleteQuadric(pObj);	
		glPopMatrix();
	}

}

