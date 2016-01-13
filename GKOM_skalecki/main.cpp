#define M_PI 3.14159f
#include <iostream>
#include <glut.h>

using namespace std;

const float cameraRadius = 12.0f;
const float cameraLevel = 5.0f;
const float headRadius = 1.25f;
const float springRadius = 0.5f;
const float maxHeadLevel = 15.0f;
const int springSegments = 500;

void initRendering() {
	glClearColor(0, 0, 0, 0);
}

inline float toRadians(float angleInDegrees) {

	return angleInDegrees * M_PI / 180.0f;
}

void getPositionOnSphere(GLfloat *result, float r, float angleHorizontal, float angleVertical) {

	angleVertical = toRadians(angleVertical);
	angleHorizontal = toRadians(angleHorizontal);

	result[0] = r * cos(angleVertical) * cos(angleHorizontal);
	result[1] = r * sin(angleVertical);
	result[2] = r * cos(angleVertical) * sin(angleHorizontal);
}

void drawSphere(float headRadius, float radius, float angleHorizontal, float angleVertical) {

	GLfloat *vec = new GLfloat[3];
	glPushMatrix();
	getPositionOnSphere(vec, headRadius, angleHorizontal, angleVertical);
	glTranslatef(vec[0], vec[1], vec[2]);
	glutSolidSphere(radius, 10, 10);
	glPopMatrix();
	delete[] vec;
}

void drawClown(float headLevel) {

	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	float springLevel = headLevel - headRadius / 1.2f;
	float xVariance = headLevel / 5.0f - headRadius / 2.0f;
	GLfloat *vec = new GLfloat[3];

	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(0.5f, 0.5f, 0.5f);
	for (int i = 0; i < springSegments; i++) {
		float x = springRadius * sin(i * 100.0f / springSegments);
		float y1 = i * springLevel / springSegments;
		float y2 = i * springLevel / springSegments + 0.05f;
		float z = springRadius * cos(i * 100.0f / springSegments);
		float springXVariance = (y1 / springLevel) * (y1 / springLevel) * xVariance;
		glVertex3f(x + springXVariance, y1, z);
		glVertex3f(x + springXVariance, y2, z);
	}
	glEnd();
	
	glPushMatrix();
	glColor3f(0.2f, 0.3f, 0.2f);
	glTranslatef(0.0f, headRadius + 0.5f, 0.0f);
	glutSolidCube(2 * headRadius + 1.0f);
	glPopMatrix();
	
	glColor3f(1.0f, 0.8f, 0.8f);

	GLfloat light_position[] = { 5.0, 5.0, -5.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	// Draw Head
	glTranslatef(xVariance, headLevel, 0.0f);
	glutSolidSphere(headRadius, 50, 30);

	glColor3f(0.0f, 0.0f, 0.0f);
	drawSphere(headRadius, 0.1f, -15.0f, 25.0f);
	drawSphere(headRadius, 0.1f, 15.0f, 25.0f);

	glColor3f(1.0f, 0.0f, 0.0f);
	drawSphere(headRadius, 0.3f, 0.0f, 0.0f);
	drawSphere(headRadius, 0.6f, -75.0f, 35.0f);
	drawSphere(headRadius, 0.6f, 75.0f, 35.0f);

	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (float i = -30; i <= 30; i += 1.0f) {
		getPositionOnSphere(vec, headRadius + 0.025f, i, -30.0f + i * i / 50.0f);
		glVertex3fv(vec);
		getPositionOnSphere(vec, headRadius + 0.025f, i, -35.0f + i * i / 50.0f);
		glVertex3fv(vec);
	}	
	glEnd();	

	delete[] vec;
}

void updateCamera(float angle, float headLevel) {
	
	float x = sin(angle) * cameraRadius;
	float z = cos(angle) * cameraRadius;

	gluLookAt(   x, cameraLevel,    z,
			  0.0f, headLevel, 0.0f,
		      0.0f, 1.0f, 0.0f);
}

void drawGround() {

	// Draw ground
	glColor3f(0.3f, 0.4f, 0.3f);
	glBegin(GL_QUADS);
	glVertex3f(-10.0f, 0.0f, -10.0f);
	glVertex3f(-10.0f, 0.0f, 10.0f);
	glVertex3f(10.0f, 0.0f, 10.0f);
	glVertex3f(10.0f, 0.0f, -10.0f);
	glEnd();
}

void renderScene(void) {	

	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	float pause = cos(timeSinceStart / 500.0f) < 0.0f ? 1.0f : 0.0f;
	float timeVariable = cos(timeSinceStart / 500.0f);
	float headLevel = headRadius + timeVariable * timeVariable * pause * maxHeadLevel / 2.0f;
	float cameraAngle = timeSinceStart / 1000.0f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	// Set the camera
	updateCamera(cameraAngle, headLevel);

	drawGround();
			
	drawClown(headLevel);

	glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27)
		exit(0);
}


void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;
	float ratio = 1.0* w / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45, ratio, 1, 1000);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {

	// init GLUT and create window

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("KGOM animation");

	glClearColor(0.2f, 0.0f, 0.3f, 1.0f);
	glEnable(GL_LIGHT0);
	glShadeModel (GL_SMOOTH);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);

	// register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);
	glutKeyboardFunc(processNormalKeys);

	// OpenGL init
	glEnable(GL_DEPTH_TEST);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}