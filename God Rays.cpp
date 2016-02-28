// God Rays.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include "glew.h"
#include <glut.h>
#include "shader_utils.h"


GLuint fbo, fbo_texture, rbo_depth;
GLuint fbo2, fbo_texture2, rbo_depth2;
float rot;
float t;
int frames;

GLdouble LightPos[3];


//Shader Program Globals
GLuint program_postproc, attribute_v_coord_postproc, uniform_fbo_texture;

float FPS()
{
	frames++;
	float elapsedTime = glutGet(GLUT_ELAPSED_TIME);
	float fps = elapsedTime - t;
	if (fps > 1000)
	{
		printf("FPS:%i\n", int(frames * 1000 / fps));
		t = elapsedTime;
		frames = 0;
	}
	return fps;
}

int InitShader()
{
	/* init_resources */
	/* Post-processing */
	/* Compile and link shaders */
	GLint link_ok = GL_FALSE;
	GLint validate_ok = GL_FALSE;

	GLuint vs, fs;
	if ((vs = create_shader("postprocV.txt", GL_VERTEX_SHADER)) == 0) return 0;
	if ((fs = create_shader("postprocF.txt", GL_FRAGMENT_SHADER)) == 0) return 0;

	program_postproc = glCreateProgram();
	glAttachShader(program_postproc, vs);
	glAttachShader(program_postproc, fs);
	glLinkProgram(program_postproc);
	glGetProgramiv(program_postproc, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		fprintf(stderr, "glLinkProgram:");
		print_log(program_postproc);
		return 0;
	}
	glValidateProgram(program_postproc);
	glGetProgramiv(program_postproc, GL_VALIDATE_STATUS, &validate_ok);
	if (!validate_ok) {
		fprintf(stderr, "glValidateProgram:");
		print_log(program_postproc);
	}

	const char* attribute_name;
	attribute_name = "v_coord";
	attribute_v_coord_postproc = glGetAttribLocation(program_postproc, attribute_name);
	if (attribute_v_coord_postproc == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return 0;
	}
	/*
	const char* uniform_name;
	uniform_name = "fbo_texture";
	uniform_fbo_texture = glGetUniformLocation(program_postproc, uniform_name);
	if (uniform_fbo_texture == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
		return 0;
	}
	*/
}

void RenderTeapot()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER, fbo);
	glClearColor(0.1, 0.1, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)(640) / (GLfloat)(480), 1.0f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D	);
	glDisable(GL_LIGHTING);


	glColor3f(1, 1, 1);
	// Light Source
	glPushMatrix();
	glTranslatef(-2, 5, -15);
	glutSolidSphere(0.2, 20, 20);
	glPopMatrix();

	// Calculate the position of the Light on the screen for return values
	double mvd[16], mpd[16];
	int view[4];
	glGetDoublev(GL_PROJECTION_MATRIX, mpd);
	glGetDoublev(GL_MODELVIEW_MATRIX, mvd);
	glGetIntegerv(GL_VIEWPORT, view);

	gluProject(-2, 5, -15, mvd, mpd, view, &LightPos[0], &LightPos[1], &LightPos[2]);
	LightPos[0] = LightPos[0] / view[2];
	LightPos[1] = LightPos[1] / view[3];

	glColor3f(0, 0, 0);
	glPushMatrix();
	glTranslatef(0, 1, -10);
	glRotatef(rot, 1, 1, 1);
	glutSolidTeapot(1.1);
	glPopMatrix();
	
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	rot += 0.1f;
	
	glBindFramebufferEXT(GL_FRAMEBUFFER, fbo2);
	glClearColor(0.1, 0.1, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)(640) / (GLfloat)(480), 1.0f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Light Source
	glColor3f(1, 1, 1);
	glPushMatrix();
	glTranslatef(0, 0, -15);
	//glutSolidSphere(1.5, 20, 20);
	glPopMatrix();

	glColor3f(0.1, 0.1, 0.3);
	glPushMatrix();
	glTranslatef(0, 1, -10);
	glRotatef(rot, 1, 1, 1);
	glutSolidTeapot(1.1);
	glPopMatrix();
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
}

void CreateLightSource()
{
	// Enable Lighting for this OpenGL Program
	//glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { -2.0f, 0.0f, -20.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void UseSurfaceMaterials()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Set Material Properties which will be assigned by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

void Display(void)
{
	RenderTeapot(); // Renders Teapot to back buffer
	FPS(); // Calculates the frames rendered per second
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 640, 0, 480);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo_texture);


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fbo_texture2);

	glUseProgram(program_postproc);

	GLint location1 = glGetUniformLocation(program_postproc, "OccludingFBO");
	glUniform1i(location1, 0);

	GLint location2 = glGetUniformLocation(program_postproc, "ColorFBO");
	glUniform1i(location2, 1);

	// Pass light source screen position to shader
	glUniform2f(glGetUniformLocation(program_postproc, "lightScreenPosition"), LightPos[0], LightPos[1]);

	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(0, 0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(640, 0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(640, 480);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(0, 480);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glUseProgram(0);
	
	glutSwapBuffers();
	glutPostRedisplay();
}

/*
*********************************Framebuffer***********************************
We will create :
-> A framebuffer object
-> Depth buffer stored in a render buffer(necessary to render a 3D scene)
-> Color buffer stored in a texture(with GL_CLAMP_TO_EDGE to avoid default GL_REPEAT's border "warping" effect).
*/
void CreateBackBuffer()
{
	/* Texture */
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &fbo_texture);
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	/* Depth buffer */
	glGenRenderbuffers(1, &rbo_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 640, 480);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	/* Framebuffer to link everything together */
	glGenFramebuffers(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);
	GLenum status;
	if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
		fprintf(stderr, "glCheckFramebufferStatus: error %p", status);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/*
	//////////////////////////////////////////////////////////////////////////////////////////////////
	/* Texture */
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &fbo_texture2);
	glBindTexture(GL_TEXTURE_2D, fbo_texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	/* Depth buffer */
	glGenRenderbuffers(1, &rbo_depth2);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth2);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 640, 480);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	/* Framebuffer to link everything together */
	glGenFramebuffers(1, &fbo2);
	glBindFramebufferEXT(GL_FRAMEBUFFER, fbo2);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture2, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth2);
	GLenum status2;
	if ((status2 = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
		fprintf(stderr, "glCheckFramebufferStatus: error %p", status2);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void CleanUp(void)
{
	/* free_resources */
	glDeleteRenderbuffers(1, &rbo_depth);
	glDeleteTextures(1, &fbo_texture);
	glDeleteFramebuffers(1, &fbo);
	glDeleteFramebuffers(1, &fbo2);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("OpengL Post Processors");
	glewInit();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glutDisplayFunc(Display);
	glClearColor(0, 0, 0, 0);
	CreateBackBuffer();
	InitShader();
	CreateLightSource();
	UseSurfaceMaterials();
	atexit(CleanUp);
	glutMainLoop();
}