////////////////////////////////////////////////////////
//                                                    //
// OpenGL Grasshopper                                 //
//                                                    //
// Aaron Benson                                       //
//                                                    //
////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <stack>
#include <math.h>
#include <sstream>
#include <algorithm>

#define GL_GLEXT_PROTOTYPES

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <GL/gl.h>
#endif

# define M_PI           3.14159265358979323846  

using namespace std;

#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtx\transform.hpp" 

// First triangle.
float vertices[] = { -0.05, -0.05, 0.0,
0.05, -0.05, 0.0,
0.05, 0.05, 0.0,
-0.05, 0.05, 0.0, };

GLubyte tindices[6];

// VBO that contains interleaved positions and colors.
GLuint vboHandle[1];
GLuint indexVBO;

GLuint vboHandleCircle[1];
GLuint indexVBOCircle;

// Simple conversion from degrees to rad.
float degToRad(float deg) {
	return deg * M_PI / 180.0;
}

// Initialize grasshopper arm angles.
float angleTRarm1 = degToRad(40.0f), angleTRarm2 = degToRad(-90.0f);
float angleBRarm1 = degToRad(40.0f), angleBRarm2 = degToRad(-90.0f);
float angleTLarm1 = degToRad(140), angleTLarm2 = degToRad(90);
float angleBLarm1 = degToRad(140), angleBLarm2 = degToRad(90);

// Initialize minimum and maximum angles for grasshopper arms.
float angleArmR1Max = degToRad(85);
float angleArmR1Min = degToRad(-40);
float angleArmR2Min = degToRad(-130);
float angleArmL1Min = degToRad(95);
float angleArmL1Max = degToRad(220);
float angleArmL2Max = degToRad(130);

// Initialize scaling variable for grasshopper head.
float headScale = 1;

// Create model for the grasshopper and scene.
glm::mat4 modelGrasshopper = glm::mat4(1.0f);
glm::mat4 modelScene = glm::mat4(1.0f);

vector<glm::mat4> mat_list;
stack<glm::mat4> mat_stack;

vector<glm::mat4> mat_list_scene;
stack<glm::mat4> mat_stack_scene;

//
// Creates VBO objects and send the triangle vertices/colors to the graphics card.
// 
void InitVBO() {

	// Create an interleaved VBO object and bind the first handle.
	glGenBuffers(1, vboHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[0]);

	// Allocate space and copy the position data over and then clean up.
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 12, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	tindices[0] = 0;   tindices[1] = 1;   tindices[2] = 2;
	tindices[3] = 0;   tindices[4] = 2;   tindices[5] = 3;

	// Load the index data and then clean up.
	glGenBuffers(1, &indexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte)* 6, tindices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	float size = 0.02f;

	// Create circle vertex array and initialize vertex points.
	GLfloat* coordCircle = new GLfloat[68];
	int coordIndex = 0;
	coordCircle[coordIndex++] = 0.0f;
	coordCircle[coordIndex++] = 0.0f;
	coordCircle[coordIndex++] = 1.0f * size;
	coordCircle[coordIndex++] = 0.0f;

	// Determine the rest of the points on the circle and add them to the array.
	float angleIncrement = 2.0f * M_PI / 32.0f;
	float x = 1.0f, y = 0.0f;
	for (int k = 1; k < 32; ++k) {
		float xTemp = cos(angleIncrement) * x - sin(angleIncrement) * y;
		y = sin(angleIncrement) * x + cos(angleIncrement) * y;
		x = xTemp;
		coordCircle[coordIndex++] = x * size;
		coordCircle[coordIndex++] = y * size;
	}
	coordCircle[coordIndex++] = 1.0f * size;
	coordCircle[coordIndex++] = 0.0f;

	// Generate buffer for the circle, bind the buffer to the handle, and send it to the card.
	glGenBuffers(1, vboHandleCircle);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleCircle[0]);
	glBufferData(GL_ARRAY_BUFFER, 68 * sizeof(GLfloat), coordCircle, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] coordCircle;
}

//
// Draws a square on screen.
// 
void draw_square(glm::mat4* m, float color[3]) {

	glLoadMatrixf((GLfloat*)m);

	glPointSize(10);
	glColor3f(color[0], color[1], color[2]);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (char*)NULL + 0);
}

//
// Draws a circle on screen.
// 
void draw_circle(glm::mat4* m, float color[3]) {

	glLoadMatrixf((GLfloat*)m);

	glPointSize(10);
	glColor3f(color[0], color[1], color[2]);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 34);
}


//
// OpenGL main display function responsible for drawing everything on screen.
// 
void display() {

	// Make background blue.
	glClearColor(0.35294, 0.998, 0.8706, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	// Enable the vertex array.
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);

	// Tells OpenGL how to walk through the vertex array: number of coordinates per vertex (3 here), type of the coordinates, stride between consecutive vertices, and pointers to the first coordinate.
	glVertexPointer(3, GL_FLOAT, 0, 0);

	float color[3];

	color[0] = 0; color[1] = 0.93; color[2] = 0.1;

	mat_stack.push(modelGrasshopper);
	modelGrasshopper = glm::scale(modelGrasshopper, glm::vec3(0.8f, 2.6f, 1.0f));
	draw_square(&modelGrasshopper, color);
	modelGrasshopper = mat_stack.top();  mat_stack.pop();

	mat_stack.push(modelGrasshopper);

	//////////////////////////////
	//    Right Arm Top
	//////////////////////////////

	color[0] = 0.01; color[1] = 1; color[2] = 0.3;
	modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(0.032f, 0.105f, 0.0f));
	modelGrasshopper = glm::rotate(modelGrasshopper, max(min(angleTRarm1, angleArmR1Max), angleArmR1Min), glm::vec3(0.0f, 0.0f, 1.0f));
	modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(0.09f, 0.0f, 0.0f));

	mat_stack.push(modelGrasshopper);
	modelGrasshopper = glm::scale(modelGrasshopper, glm::vec3(2.0f, 0.2f, 1.0f));
	draw_square(&modelGrasshopper, color);
	modelGrasshopper = mat_stack.top();  mat_stack.pop();

	color[0] = 0.306; color[1] = 0.7; color[2] = 0.19;
	modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(0.1f, 0.0f, 0.0f));
	modelGrasshopper = glm::rotate(modelGrasshopper, max(min(angleTRarm2, 0.0f), angleArmR2Min), glm::vec3(0.0f, 0.0f, 1.0f));
	modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(0.085f, 0.0f, 0.0f));
	mat_stack.push(modelGrasshopper);

	modelGrasshopper = glm::scale(modelGrasshopper, glm::vec3(1.8f, 0.18f, 1.0f));
	draw_square(&modelGrasshopper, color);
	modelGrasshopper = mat_stack.top();  mat_stack.pop();

	modelGrasshopper = mat_stack.top();  mat_stack.pop();
	mat_stack.push(modelGrasshopper);

	//////////////////////////////
	//    Right Arm Bottom
	//////////////////////////////

	color[0] = 0.01; color[1] = 1; color[2] = 0.3;
	modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(0.032f, -0.105f, 0.0f));
	modelGrasshopper = glm::rotate(modelGrasshopper, max(min(angleBRarm1, angleArmR1Max), angleArmR1Min), glm::vec3(0.0f, 0.0f, 1.0f));
	modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(0.09f, 0.0f, 0.0f));

	mat_stack.push(modelGrasshopper);
	modelGrasshopper = glm::scale(modelGrasshopper, glm::vec3(2.0f, 0.2f, 1.0f));
	draw_square(&modelGrasshopper, color);
	modelGrasshopper = mat_stack.top();  mat_stack.pop();

	color[0] = 0.306; color[1] = 0.7; color[2] = 0.19;
	modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(0.1f, 0.0f, 0.0f));
	modelGrasshopper = glm::rotate(modelGrasshopper, max(min(angleBRarm2, 0.0f), angleArmR2Min), glm::vec3(0.0f, 0.0f, 1.0f));
	modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(0.085f, 0.0f, 0.0f));
	mat_stack.push(modelGrasshopper);

	modelGrasshopper = glm::scale(modelGrasshopper, glm::vec3(1.8f, 0.18f, 1.0f));
	draw_square(&modelGrasshopper, color);
	modelGrasshopper = mat_stack.top();  mat_stack.pop();

	modelGrasshopper = mat_stack.top();  mat_stack.pop();
	mat_stack.push(modelGrasshopper);

	//////////////////////////////
	//    Left Arm Top
	//////////////////////////////

	color[0] = 0.01; color[1] = 1; color[2] = 0.3;
	modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(-0.032f, 0.105f, 0.0f));
	modelGrasshopper = glm::rotate(modelGrasshopper, max(min(angleTLarm1, angleArmL1Max), angleArmL1Min), glm::vec3(0.0f, 0.0f, 1.0f));
	modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(0.09f, 0.0f, 0.0f));

	mat_stack.push(modelGrasshopper);
	modelGrasshopper = glm::scale(modelGrasshopper, glm::vec3(2.0f, 0.2f, 1.0f));
	draw_square(&modelGrasshopper, color);
	modelGrasshopper = mat_stack.top();  mat_stack.pop();

	color[0] = 0.306; color[1] = 0.7; color[2] = 0.19;
	modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(0.1f, 0.0f, 0.0f));
	modelGrasshopper = glm::rotate(modelGrasshopper, min(max(angleTLarm2, 0.0f), angleArmL2Max), glm::vec3(0.0f, 0.0f, 1.0f));
	modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(0.085f, 0.0f, 0.0f));
	mat_stack.push(modelGrasshopper);

	modelGrasshopper = glm::scale(modelGrasshopper, glm::vec3(1.8f, 0.18f, 1.0f));
	draw_square(&modelGrasshopper, color);
	modelGrasshopper = mat_stack.top();  mat_stack.pop();

	modelGrasshopper = mat_stack.top();  mat_stack.pop();
	mat_stack.push(modelGrasshopper);

	//////////////////////////////
	//    Left Arm Bottom
	//////////////////////////////

	color[0] = 0.01; color[1] = 1; color[2] = 0.3;
	modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(-0.032f, -0.105f, 0.0f));
	modelGrasshopper = glm::rotate(modelGrasshopper, max(min(angleBLarm1, angleArmL1Max), angleArmL1Min), glm::vec3(0.0f, 0.0f, 1.0f));
	modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(0.09f, 0.0f, 0.0f));

	mat_stack.push(modelGrasshopper);
	modelGrasshopper = glm::scale(modelGrasshopper, glm::vec3(2.0f, 0.2f, 1.0f));
	draw_square(&modelGrasshopper, color);
	modelGrasshopper = mat_stack.top();  mat_stack.pop();

	color[0] = 0.306; color[1] = 0.7; color[2] = 0.19;
	modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(0.1f, 0.0f, 0.0f));
	modelGrasshopper = glm::rotate(modelGrasshopper, min(max(angleBLarm2, 0.0f), angleArmL2Max), glm::vec3(0.0f, 0.0f, 1.0f));
	modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(0.085f, 0.0f, 0.0f));
	mat_stack.push(modelGrasshopper);

	modelGrasshopper = glm::scale(modelGrasshopper, glm::vec3(1.8f, 0.18f, 1.0f));
	draw_square(&modelGrasshopper, color);
	modelGrasshopper = mat_stack.top();  mat_stack.pop();

	modelGrasshopper = mat_stack.top();  mat_stack.pop();
	mat_stack.push(modelGrasshopper);

	//////////////////////////////
	//    Head
	//////////////////////////////

	color[0] = 0; color[1] = 0.43137; color[2] = 0.01569;
	modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(0.0f, 0.13f, 0.0f));
	modelGrasshopper = glm::scale(modelGrasshopper, glm::vec3(1.2f * headScale, 1.5f * headScale, 1.0f));
	modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(0.0f, 0.02f * headScale, 0.0f));
	draw_square(&modelGrasshopper, color);

	modelGrasshopper = mat_stack.top();  mat_stack.pop();
	mat_stack.push(modelGrasshopper);


	//////////////////////////////
	//    Draw the scene
	//////////////////////////////

	// Draw tree trunk.
	color[0] = 0.29412; color[1] = 0.3098; color[2] = 0;
	mat_stack_scene.push(modelScene);
	modelScene = glm::scale(modelScene, glm::vec3(3.0f, 12.0f, 1.0f));
	modelScene = glm::translate(modelScene, glm::vec3(0.2f, 0.0f, 0.0f));
	draw_square(&modelScene, color);
	modelScene = mat_stack_scene.top();  mat_stack_scene.pop();
	mat_stack_scene.push(modelScene);

	// Draw grass.
	color[0] = 0; color[1] = 0.89; color[2] = 0.32549;
	mat_stack_scene.push(modelScene);
	modelScene = glm::scale(modelScene, glm::vec3(20.0f, 5.0f, 1.0f));
	modelScene = glm::translate(modelScene, glm::vec3(0.0f, -0.17f, 0.0f));
	draw_square(&modelScene, color);
	modelScene = mat_stack_scene.top();  mat_stack_scene.pop();
	mat_stack_scene.push(modelScene);

	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandleCircle[0]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// Draw leaves.
	color[0] = 0.0627; color[1] = 0.8117; color[2] = 0.023;
	mat_stack_scene.push(modelScene);
	modelScene = glm::scale(modelScene, glm::vec3(40.0f, 33.0f, 1.0f));
	modelScene = glm::translate(modelScene, glm::vec3(0.015f, 0.03f, 0.0f));
	draw_circle(&modelScene, color);
	modelScene = mat_stack_scene.top();  mat_stack_scene.pop();
	mat_stack_scene.push(modelScene);

	// Draw apple.
	color[0] = 1; color[1] = 0; color[2] = 0;
	mat_stack_scene.push(modelScene);
	modelScene = glm::scale(modelScene, glm::vec3(2.0f, 2.0f, 1.0f));
	modelScene = glm::translate(modelScene, glm::vec3(0.04f, 0.25f, 0.0f));
	draw_circle(&modelScene, color);
	modelScene = mat_stack_scene.top();  mat_stack_scene.pop();
	mat_stack_scene.push(modelScene);
	glDisableVertexAttribArray(0);

	glutSwapBuffers();

}

//
// OpenGL main key function responsible for user input.
// 
void mykey(unsigned char key, int x, int y) {

	float d_angle = degToRad(15);
	float rot_offset = degToRad(5);

	// Basic Movement Controls.
	if (key == 'q') exit(1);
	if (key == 'T')
		modelGrasshopper = glm::rotate(modelGrasshopper, -d_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	if (key == 't')
		modelGrasshopper = glm::rotate(modelGrasshopper, d_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	if (key == 'r')
		modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(0.02f, 0.0f, 0.0f));
	if (key == 'l')
		modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(-0.02f, 0.0f, 0.0f));
	if (key == 'f')
		modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(0.0f, 0.02f, 0.0f));
	if (key == 'b')
		modelGrasshopper = glm::translate(modelGrasshopper, glm::vec3(0.0f, -0.02f, 0.0f));

	// Controls for top right arm.
	if (key == '9')
		angleTRarm1 = angleTRarm1 > angleArmR1Max ? angleArmR1Max : angleTRarm1 + rot_offset;
	if (key == '(')
		angleTRarm1 = angleTRarm1 < angleArmR1Min ? angleArmR1Min : angleTRarm1 - rot_offset;
	if (key == '0')
		angleTRarm2 = 0 < angleTRarm2 ? 0 : angleTRarm2 + rot_offset;
	if (key == ')')
		angleTRarm2 = angleTRarm2 < angleArmR2Min ? angleArmR2Min : angleTRarm2 - rot_offset;

	// Controls for bottom right arm.
	if (key == 'o')
		angleBRarm1 = angleBRarm1 > angleArmR1Max ? angleArmR1Max : angleBRarm1 + rot_offset;
	if (key == 'O')
		angleBRarm1 = angleBRarm1 < angleArmR1Min ? angleArmR1Min : angleBRarm1 - rot_offset;
	if (key == 'p')
		angleBRarm2 = 0 < angleBRarm2 ? 0 : angleBRarm2 + rot_offset;
	if (key == 'P')
		angleBRarm2 = angleBRarm2 < angleArmR2Min ? angleArmR2Min : angleBRarm2 - rot_offset;

	// Controls for top left arm.
	if (key == '*')
		angleTLarm1 = angleTLarm1 > angleArmL1Max ? angleArmL1Max : angleTLarm1 + rot_offset;
	if (key == '8')
		angleTLarm1 = angleTLarm1 < angleArmL1Min ? angleArmL1Min : angleTLarm1 - rot_offset;
	if (key == '7')
		angleTLarm2 = angleTLarm2 < 0 ? 0 : angleTLarm2 - rot_offset;
	if (key == '&')
		angleTLarm2 = angleTLarm2 > angleArmL2Max ? angleArmL2Max : angleTLarm2 + rot_offset;

	// Controls for bottom left arm.
	if (key == 'I')
		angleBLarm1 = angleBLarm1 > angleArmL1Max ? angleArmL1Max : angleBLarm1 + rot_offset;
	if (key == 'i')
		angleBLarm1 = angleBLarm1 < angleArmL1Min ? angleArmL1Min : angleBLarm1 - rot_offset;
	if (key == 'u')
		angleBLarm2 = angleBLarm2 < 0 ? 0 : angleBLarm2 - rot_offset;
	if (key == 'U')
		angleBLarm2 = angleBLarm2 > angleArmL2Max ? angleArmL2Max : angleBLarm2 + rot_offset;

	// Controls for head.
	if (key == 'h')
		headScale = headScale <= 0.9 ? headScale : headScale - 0.05;
	if (key == 'H')
		headScale = headScale >= 1.5 ? headScale : headScale + 0.05;

	glutPostRedisplay();
}

//
// Main method. Initialize and control OpenGL objects.
// 
int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);

	glutCreateWindow("OpenGL Grasshopper - Aaron Benson");
	glutDisplayFunc(display);
	glutKeyboardFunc(mykey);

	mat_list.clear();

#ifdef __APPLE__
#else
	glewInit();
#endif


	InitVBO();

	glutMainLoop();

}