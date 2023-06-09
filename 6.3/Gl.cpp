#include "glew.h"
#include "glut.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "Header.h"

GLfloat width, height;
static GLubyte* pImage = NULL;
static GLubyte* rotatedImage = NULL;
static GLint iWidth, iHeight, iComponents;
static GLenum eFormat;

static GLint iRenderMode = 1;
#pragma pack(8)

void SetupRC(void)
{

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	pImage = (GLubyte*)gltLoadTGA("2.tga",
		&iWidth, &iHeight, &iComponents, &eFormat);

}
void ShutdownRC(void)
{

	free(pImage);
}


void ProcessMenu(int value)
{
	if (value == 0)
		
		gltWriteTGA("ScreenShot1.tga");
	else
		
		iRenderMode = value;

	glutPostRedisplay();
}

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	GLubyte* rotatedImage = new GLubyte[iWidth * iHeight * 3]; 
	for (int y = 0; y < iHeight; y++) {
		for (int x = 0; x < iWidth; x++) {
			int srcIndex = (y * iWidth + x) * 3;
			int destIndex = ((iWidth - x - 1) * iHeight + y) * 3;
			rotatedImage[destIndex] = pImage[srcIndex];
			rotatedImage[destIndex + 1] = pImage[srcIndex + 1];
			rotatedImage[destIndex + 2] = pImage[srcIndex + 2];
		}
	}
		
		glRasterPos2i(width/3, height);
		glPixelZoom(1.2, -1.2);
		glDrawPixels(iHeight, iWidth, eFormat, GL_UNSIGNED_BYTE, rotatedImage);
		delete[] rotatedImage; 
		glutSwapBuffers();
}
void ChangeSize(int w, int h)
{
	
		if (h == 0)
			h = 1;
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	
	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
	
	gluOrtho2D(0.0f, (GLfloat)w, 0.0, (GLfloat)h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GL_DOUBLE);
	glutInitWindowSize(1500, 1000);
	glutCreateWindow("OpenGL Image Operations");
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);

	SetupRC(); 
	glutMainLoop(); 
		ShutdownRC(); 
		return 0;
}
