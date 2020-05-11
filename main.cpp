#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "libs/Utils.h"
#include "libs/Transforms.h"
#include <map>
#include <vector>

#include "libs/image.h"

std::map<char, bool> keyMap;

static GLuint programId;

static GLuint vertexArrayId;
static GLuint bufferId[3];

static GLuint vertexPositionLoc, vertexNormalLoc, modelMatrixLoc;
static GLuint projectionMatrixLoc, viewMatrixLoc, modelColorLoc;
static Mat4 modelMatrix, projectionMatrix, viewMatrix;

static GLuint ambientLightLoc, diffuseLightLoc, lightPositionLoc;
static GLuint materialALoc, materialDLoc, materialSLoc, exponentLoc, cameraLoc;

static float ambientLight[] = {0.5, 0.5, 0.5};
static float materialAmbient[] = {0.0, 0.0, 1.0};
static float diffuseLight[] = {1.0, 1.0, 1.0};
static float lightPosition[] = {0.0, 0.0, 1};
static float materialDiffuse[] = {0.6, 0.6, 0.6};
static float materialSpecular[] = {0.7, 0.7, 0.7};
static float exponent = 16;

static float observerX = 0, observerZ = 0;
static float cameraSpeed = 0.05;

std::vector<float> vertexes;
std::vector<float> colors;
std::vector<float> normals;
const float START_Z = -10;

static void initShaders() {
	GLuint vShader = compileShader("shaders/gouraud.vsh", GL_VERTEX_SHADER);
	//GLuint vShader = compileShader("shaders/modelPosition.vsh", GL_VERTEX_SHADER);
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
	vertexNormalLoc = glGetAttribLocation(programId, "vertexNormal");
	
	modelMatrixLoc = glGetUniformLocation(programId,"modelMatrix");
	viewMatrixLoc = glGetUniformLocation(programId,"viewMatrix");
	projectionMatrixLoc = glGetUniformLocation(programId,"projectionMatrix");

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

	glEnable(GL_DEPTH_TEST);

	keyMap['w'] = false;
	keyMap['s'] = false;
	keyMap['a'] = false;
	keyMap['d'] = false;
}

static void displayFunc() 
{
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

	glBindVertexArray(vertexArrayId);
	glUseProgram(programId);
	glDrawArrays(GL_POINT, 0, vertexes.size());

	glutSwapBuffers();
}

static void reshapeFunc(int w, int h) 
{
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

static void createImage(Image img)
{
	glPointSize(10);
	float x = -(img.width / 2.0);
	float y = (img.height / 2.0);

	printf("startX: %0.0f, startY: %0.0f\n", x, y);

	int i = 0, j = 0;

	for (i = 0; i < img.width; i++, x++)
	{
		for (j = 0; j < img.height; j++, y--)
		{
			//printf("i: %i, j: %i, img: %u\n", i, j, img.get(i, j, 0));
			// Add vertexes
			vertexes.push_back(x);
			vertexes.push_back(y);
			vertexes.push_back(START_Z);

			// Add colors
			colors.push_back(img.get(i, j, 0) / 255.0);
			colors.push_back(img.get(i, j, 1) / 255.0);
			colors.push_back(img.get(i, j, 2) / 255.0);

			// Add normals
			normals.push_back(0);
			normals.push_back(0);
			normals.push_back(1);
		}
	}
	printf("%lu, %i, %i\n", vertexes.size(), i, j);

	glGenVertexArrays(1, &vertexArrayId);
    glBindVertexArray(vertexArrayId);

    glGenBuffers(3, bufferId);

	vertexes = std::vector<float>(3, 0.5);
	colors = std::vector<float>(3, 0.5);

	// Positions
    glBindBuffer(GL_ARRAY_BUFFER, bufferId[0]);
    glBufferData(GL_ARRAY_BUFFER, vertexes.size() * sizeof(float),
            &vertexes[0], GL_STATIC_DRAW);
    glVertexAttribPointer(vertexPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexPositionLoc);

	// Colors
    glBindBuffer(GL_ARRAY_BUFFER, bufferId[1]);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float),
            &colors[0], GL_STATIC_DRAW);
    glVertexAttribPointer(modelColorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(modelColorLoc);

	// Normals
    glBindBuffer(GL_ARRAY_BUFFER, bufferId[2]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float),
            &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(vertexNormalLoc, 3, GL_FLOAT, 0, 0, 0);
    glEnableVertexAttribArray(vertexNormalLoc);
}

int main(int argc, char **argv) {
	std::string path = "0010.png";
	Image img(&path[0]);

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
	createImage(img);
	glClearColor(0, 0, 0, 1.0);
	glutMainLoop();

	return 0;
}