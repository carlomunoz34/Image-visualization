
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "libs/Utils.h"
#include "libs/Transforms.h"
#include <map>
#include <vector>
#include <math.h>


#include "libs/image.h"

#define toRadians(deg) deg * M_PI / 180.0
std::string path = "cat.3.jpg";

std::map<char, bool> keyMap;

typedef float vec3[3];

const float CAMERA_ROTATION_SPEED = 0.5;
const unsigned RESET = UINT32_MAX;

static GLuint programId;
static GLuint cubeVA;
static GLuint vertexArrayId;
static GLuint bufferId[4];

static GLuint vertexPositionLoc, vertexNormalLoc, modelMatrixLoc;
static GLuint projectionMatrixLoc, viewMatrixLoc, modelColorLoc;
static Mat4 modelMatrix, projectionMatrix, viewMatrix;

static GLuint ambientLightLoc, diffuseLightLoc, lightPositionLoc;
static GLuint materialALoc, materialDLoc, materialSLoc, exponentLoc, cameraLoc;

static float ambientLight[] = {0.5, 0.5, 0.5};
static float materialAmbient[] = {1.0, 1.0, 1.0};
static float diffuseLight[] = {1.0, 1.0, 1.0};
static float lightPosition[] = {0.0, 0.0, 1};
static float materialDiffuse[] = {0.6, 0.6, 0.6};
static float materialSpecular[] = {0.7, 0.7, 0.7};
static float exponent = 16;

static float cameraSpeed = 0.1;
static float cameraX = 0;
static float cameraZ = 15;

static float mouseDiffX = 0;
static float mouseDiffY = 0;

static float cameraAngleX = 0;
static float cameraAngleY = 0;

bool firstMove = true;
int const MARGIN = 10;

std::vector<float> vertexes;
std::vector<float> colors;
std::vector<float> normals;
std::vector<GLuint> indexes;
const float START_Z = 0;

// This scalar multiplies the grey value so the image can have
// a more notorius relief.
const float SCALAR = 5;

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
	cameraLoc = glGetUniformLocation(programId, "cameraPosition");
	exponentLoc = glGetUniformLocation(programId, "exponent");
	materialALoc = glGetUniformLocation(programId, "materialA");
	materialDLoc = glGetUniformLocation(programId, "materialD");
	materialSLoc = glGetUniformLocation(programId, "materialS");

	glUniform3fv(ambientLightLoc, 1, ambientLight);
	glUniform3fv(diffuseLightLoc, 1, diffuseLight);
	glUniform3fv(lightPositionLoc, 1, lightPosition);
	glUniform3fv(materialALoc, 1, materialAmbient);
	glUniform3fv(materialDLoc, 1, materialDiffuse);
	glUniform3fv(materialSLoc, 1, materialSpecular);
	glUniform1f(exponentLoc, exponent);

	keyMap['w'] = false;
	keyMap['s'] = false;
	keyMap['a'] = false;
	keyMap['d'] = false;
}


static void displayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Actualizar posicion de la camara
	if (keyMap['w'])
	{
		cameraZ -= cameraSpeed;
	}
	if (keyMap['s'])
	{
		cameraZ += cameraSpeed;
	}
	if (keyMap['a'])
	{
		cameraX -= cameraSpeed;
	}
	if (keyMap['d'])
	{
		cameraX += cameraSpeed;
	}

	glUseProgram(programId);

	glUniform3f(cameraLoc, cameraX, 0, cameraZ); //cmx 0 cmZ

	glUniformMatrix4fv(projectionMatrixLoc, 1, true, projectionMatrix.values);
	mIdentity(&viewMatrix);

	translate(&viewMatrix, -cameraX, 0, -cameraZ);

	glUniformMatrix4fv(viewMatrixLoc, 1, true, viewMatrix.values);


	mIdentity(&modelMatrix);

	translate(&modelMatrix, 0, 0, -100);
	rotateX(&modelMatrix, mouseDiffY * CAMERA_ROTATION_SPEED);
	rotateY(&modelMatrix, mouseDiffX * CAMERA_ROTATION_SPEED);


	glBindVertexArray(vertexArrayId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId[2]);
	glUniformMatrix4fv(modelMatrixLoc, 1, true, modelMatrix.values);
	
	glDrawElements(GL_TRIANGLE_STRIP, indexes.size(), GL_UNSIGNED_INT, 0);

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

void onMouseMove(int x, int y)
{
	int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
	int windowWidth = glutGet(GLUT_WINDOW_WIDTH);

	if (firstMove)
	{
		firstMove = false;
		glutWarpPointer(windowWidth / 2, windowHeight / 2);

		return;
	}

	mouseDiffX = x - windowWidth / 2;
	mouseDiffY = y - windowHeight / 2;

	glutPostRedisplay();

	if (x < MARGIN)
	{
		glutWarpPointer(MARGIN, y);
	}
	else if (x > windowWidth - MARGIN)
	{
		glutWarpPointer(windowWidth - MARGIN, y);
	}

	if (y < MARGIN)
	{
		glutWarpPointer(x, MARGIN);
	}
	else if (y > windowHeight - MARGIN)
	{
		glutWarpPointer(x, windowHeight - MARGIN);
	}
}



static void createImage(Image img)
{
	// Separation between each vertex
	const float SEPARATION = 0.1;
	float x = -(img.width / 2.0) * SEPARATION;
	float y = (img.height / 2.0) * SEPARATION;

	int i = 0, j = 0;

	for (i = 0; i < img.height; i++, y -= SEPARATION)
	{
		x = -(img.width / 2.0) * SEPARATION;
		for (j = 0; j < img.width; j++, x += SEPARATION)
		{
			float red, green, blue, grey;
			red = img.get(i, j, 0) / 255.0;
			green = img.get(i, j, 1) / 255.0;
			blue = img.get(i, j, 2) / 255.0;

			grey = (red + green + blue) / 3;

			// Add vertexes
			vertexes.push_back(x);
			vertexes.push_back(y);
			vertexes.push_back(START_Z + (grey * SCALAR));

			// Add colors
			colors.push_back(red);
			colors.push_back(green);
			colors.push_back(blue);

			// Add normals
			normals.push_back(0);
			normals.push_back(0);
			normals.push_back(1);
		}
	}

	// Calculate indexes
	for (int i = 0; i < img.height - 1; i++)
	{
		for (int j = 0; j < img.width; j++)
		{
			indexes.push_back((GLuint) (i * img.width + j));
			indexes.push_back((GLuint) ((i + 1) * img.width + j));
		}
		indexes.push_back((GLuint) RESET);
	}

	glGenVertexArrays(1, &vertexArrayId);
    glBindVertexArray(vertexArrayId);

    glGenBuffers(3, bufferId);

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

	// Indexes
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint),
            &indexes[0], GL_STATIC_DRAW);

	// Normals
    glBindBuffer(GL_ARRAY_BUFFER, bufferId[3]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float),
            &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(vertexNormalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexNormalLoc);

	glPrimitiveRestartIndex(RESET);
    glEnable(GL_PRIMITIVE_RESTART);

}

int main(int argc, char **argv) {
	Image img(&path[0]);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Image");

	glutDisplayFunc(displayFunc);
	glutReshapeFunc(reshapeFunc);
	glutTimerFunc(10, timerFunc, 1);

	glutKeyboardFunc(keyPressedFunc);
	glutKeyboardUpFunc(keyReleasedFunc);

	glewInit();
	glEnable(GL_DEPTH_TEST);
	initShaders();

	//glutSetCursor(GLUT_CURSOR_NONE);
	glutPassiveMotionFunc(onMouseMove);
	//glutMotionFunc(mouseRotation);


	createImage(img);

	glClearColor(0, 0, 0, 1.0);
	glutMainLoop();

	return 0;
}