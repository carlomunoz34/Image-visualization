#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "libs/Utils.h"
#include "libs/Transforms.h"
#include <map>

std::map<char, bool> keyMap;

static GLuint programId;

static GLuint vertexPositionLoc, vertexNormalLoc, modelMatrixLoc;
static GLuint projectionMatrixLoc, viewMatrixLoc, modelColorLoc;

static GLuint ambientLightLoc, diffuseLightLoc, lightPositionLoc;
static GLuint materialALoc, materialDLoc, materialSLoc, exponentLoc, cameraLoc;

static Mat4 modelMatrix, projectionMatrix, viewMatrix;

static float angleY = 0, angleZ = 0;

static const bool USE_ILUMINATION = false;

static float ambientLight[] = {0.5, 0.5, 0.5};
static float materialAmbient[] = {0.0, 0.0, 1.0};

static float diffuseLight[] = {1.0, 1.0, 1.0};
static float lightPosition[] = {0.0, 0.0, 1};
static float materialDiffuse[] = {0.6, 0.6, 0.6};
static float materialSpecular[] = {0.7, 0.7, 0.7};
static float exponent = 16;
static float observerX = 0, observerZ = 0;
static float cameraSpeed = 0.05;

static void initShaders() {
	GLuint vShader;
	if (USE_ILUMINATION)
	{
		vShader = compileShader("shaders/gouraud.vsh", GL_VERTEX_SHADER);
	}
	else
	{
		vShader = compileShader("shaders/modelPosition.vsh",
				GL_VERTEX_SHADER);
	}

	if (!shaderCompiled(vShader))
	{
		return;
	}

	GLuint fShader = compileShader("shaders/modelColor.fsh", GL_FRAGMENT_SHADER);
	if (!shaderCompiled(fShader))
	{
		return;
	}

	programId = glCreateProgram();
	glAttachShader(programId, vShader);
	glAttachShader(programId, fShader);
	glLinkProgram(programId);

	glUseProgram(programId);

	vertexPositionLoc = glGetAttribLocation(programId,"vertexPosition");
	modelColorLoc = glGetAttribLocation(programId,"modelColor");
	
	modelMatrixLoc = glGetUniformLocation(programId,"modelMatrix");
	viewMatrixLoc = glGetUniformLocation(programId,"viewMatrix");
	projectionMatrixLoc = glGetUniformLocation(programId,"projectionMatrix");
	
	if (USE_ILUMINATION)
	{
		ambientLightLoc = glGetUniformLocation(programId, "ambientLight");
		diffuseLightLoc = glGetUniformLocation(programId, "diffuseLight");
		lightPositionLoc = glGetUniformLocation(programId, "lightPosition");
		materialALoc = glGetUniformLocation(programId, "materialA");
		materialDLoc = glGetUniformLocation(programId, "materialD");
		materialSLoc = glGetUniformLocation(programId, "materialS");
		exponentLoc = glGetUniformLocation(programId, "exponent");
		cameraLoc = glGetUniformLocation(programId, "camera");

		glUniform3fv(ambientLightLoc, 1, ambientLight);
		glUniform3fv(diffuseLightLoc, 1, diffuseLight);
		glUniform3fv(lightPositionLoc, 1, lightPosition);
		glUniform3fv(materialALoc, 1, materialAmbient);
		glUniform3fv(materialDLoc, 1, materialDiffuse);
		glUniform3fv(materialSLoc, 1, materialSpecular);
		glUniform1f(exponentLoc, exponent);
	}
	
	glEnable(GL_DEPTH_TEST);

	keyMap['w'] = false;
	keyMap['s'] = false;
	keyMap['a'] = false;
	keyMap['d'] = false;
}
float length;
static void displayFunc() {
	if (keyMap['w'])
	{
		observerZ -= cameraSpeed;
	}
	if (keyMap['s'])
	{
		observerZ += cameraSpeed;
	}
	if (keyMap['a'])
	{
		observerX -= cameraSpeed;
	}
	if (keyMap['d'])
	{
		observerX += cameraSpeed;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programId);

	mIdentity(&viewMatrix);
	translate(&viewMatrix, -observerX, 0, -observerZ);
	glUniformMatrix4fv(viewMatrixLoc, 1, true, viewMatrix.values);
	mIdentity(&modelMatrix);
    glUniformMatrix4fv(modelMatrixLoc, 1, GL_TRUE, modelMatrix.values);

	glutSwapBuffers();
}

static void reshapeFunc(int w, int h) {
	if(h == 0) h = 1;
	glViewport(0, 0, w, h);
	float aspect = (float) w / h;
	setPerspective(&projectionMatrix, 45, aspect, -0.1, -500);
	glUniformMatrix4fv(projectionMatrixLoc, 1, true, projectionMatrix.values);
}

static void timerFunc(int id) {
	glutTimerFunc(10, timerFunc, id);
	glutPostRedisplay();
}

static void keyPressedFunc(unsigned char key, int x, int y)
{
	keyMap[key] = true;
}

static void keyReleasedFunc(unsigned char key, int x, int y)
{
	keyMap[key] = false;
}

int main(int argc, char **argv) {
	setbuf(stdout, NULL);
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Cylinder");

	glutDisplayFunc(displayFunc);
	glutReshapeFunc(reshapeFunc);
	glutTimerFunc(10, timerFunc, 1);
	glutKeyboardFunc(keyPressedFunc);
	glutKeyboardUpFunc(keyReleasedFunc);
	glewInit();
	initShaders();
	glClearColor(0, 0, 0, 1.0);
	glutMainLoop();
	return 0;
}