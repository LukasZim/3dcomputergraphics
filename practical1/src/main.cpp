// Disable compiler warnings in third-party code (which we cannot change).
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <glad/glad.h>
// Include glad before glfw3.
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
DISABLE_WARNINGS_POP()
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <filesystem>
#include <framework/mesh.h>
#include <framework/trackball.h>
#include <framework/window.h>
#include <iostream>
#include <memory>
#include <numeric>
#include <span>
#include <string>
#include <vector>

//START READING HERE!!!

//////Predefined global variables

//Use the enum values to define different rendering modes
//The mode is used by the function display and the mode is
//chosen during execution with the keys 1-9
enum class DisplayModeType {
	TRIANGLE,
	FACE,
	CUBE,
	ARM,
	MESH,
	SNAKE
};

DisplayModeType displayMode = DisplayModeType::TRIANGLE;
constexpr glm::ivec2 resolution{ 800, 800 };
std::unique_ptr<Window> pWindow;
std::unique_ptr<Trackball> pTrackball;

glm::vec4 lightPos{ 1.0f, 1.0f, 0.4f, 1.0f };
Mesh mesh;

//Declare your own global variables here:
int myVariableThatServesNoPurpose;
float xvariable = 0;

float angle_arm = 0.0f;
float length_arm = 2.0f;
float angle_forearm = 0.0f;
float length_forearm = 2.0f;

float angle_hand = 0.0f;
float length_hand = 2.0f;

float snake_var = 0.0f;

////////// Draw Functions

//function to draw coordinate axes with a certain length (1 as a default)
void drawCoordSystem(const float length = 1)
{
	//draw simply colored axes

	//remember all states of the GPU
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	//deactivate the lighting state
	glDisable(GL_LIGHTING);
	//draw axes
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(length, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, length, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, length);
	glEnd();

	//reset to previous state
	glPopAttrib();
}

/**
 * Several drawing functions for you to work on
*/

void drawTriangle()
{
	// A simple example of a drawing function for a triangle
	//1) Try changing its color to red
	//2) Try changing its vertex positions
	//3) Add a second triangle in blue
	//4) Add a global variable (initialized at 0), which represents the
	//   x-coordinate of the first vertex of each triangle
	//5) Go to the function animate and increment this variable
	//   by a small value - observe the animation.

	glColor3f(1, 0, 0);
	glNormal3f(0, 0, 1);
	glBegin(GL_TRIANGLES);
	glVertex3f(xvariable, 0, 0);
	glVertex3f(1, 0, 0);
	glVertex3f(1, 1, 0);


	glColor3f(0, 0, 1);
	glVertex3f(xvariable, 0, 1);
	glVertex3f(1, 0, 1);
	glVertex3f(1, 1, 1);
	glEnd();
}

void drawUnitFace(const glm::mat4& transformMatrix)
{
	//1) Draw a unit quad in the x,y plane oriented along the z axis
	//2) Make sure the orientation of the vertices is positive (counterclock wise)
	//3) What happens if the order is inversed?
	//4) Transform the quad by the given transformation matrix.
	//
	// For more information on how to use glm (OpenGL Mathematics Library), see:
	// https://github.com/g-truc/glm/blob/master/manual.md#section1
	//
	// The documentation of the various matrix transforms can be found here:
	// https://glm.g-truc.net/0.9.9/api/a00247.html
	//
	// Please note that the glm matrix operations transform an existing matrix.
	// To get just a rotation/translation/scale matrix you can pass an identity
	// matrix (glm::mat4(1.0f)). Also, keep in mind that the rotation angle is
	// specified in radians. You can use glm::radians(angleInDegrees) to convert
	// from radians to degrees.
	//
	// For example (rotate 90 degrees around the x axis):
	// glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::degrees(90), glm::vec3(1, 0, 0));


	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(glm::value_ptr(transformMatrix));
	glNormal3f(0, 0, 1);
	//glColor3f(1, 1, 1);

	glBegin(GL_TRIANGLES);
	glVertex3f(-.5, .5, 0);
	glVertex3f(-.5, -.5, 0);
	glVertex3f(.5, .5, 0);
	glVertex3f(-.5, -.5, 0);
	glVertex3f(.5, -.5, 0);
	glVertex3f(.5, .5, 0);

	glEnd();
	glPopMatrix();
	glFlush();
}

void drawUnitCube(const glm::mat4& transformMatrix)
{
	//1) Draw a cube using your function drawUnitFace. Use glm::translate(Matrix, Vector)
	//   and glm::rotate(Matrix, Angle, Vector) to create the transformation matrices
	//   passed to drawUnitFace.
	//2) Transform your cube by the given transformation matrix.

	glPushMatrix();
	glMultMatrixf(glm::value_ptr(transformMatrix));

	// draw first face
	drawUnitFace(glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0)), glm::vec3(-.5, 0, .5)));

	// second face
	drawUnitFace(glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0)), glm::vec3(0, 0, 1)));

	// third face
	drawUnitFace(glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 1, 0)), glm::vec3(.5, 0, .5)));

	// fourth face
	drawUnitFace(glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0)), glm::vec3(0, 0, 0)));

	// fifth face
	drawUnitFace(glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0)), glm::vec3(.5, 0, .5)));

	// sixth face
	drawUnitFace(glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0)), glm::vec3(.5, 0, .5)));

	glPopMatrix();
}

void drawArm()
{
	// Produce a three-unit arm (upperarm, forearm, hand) making use of your function
	// drawUnitCube to define each of them
	//1) Define 3 global variables that control the angles between the arm parts and add
	//   cases to the keyboard function to control these values

	//2) Use these variables to define your arm.
	//   Use glm::scale(Matrix, Vector) to achieve different arm lengths.
	//   Use glm::rotate(Matrix, Angle, Vector) to correctly place the elements

	//3 Optional) make an animated snake out of these boxes
	//(an arm with 10 joints that moves using the animate function)


	// arm
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glm::mat4 i = glm::mat4(1.0f);
	//glm::mat4 armMatrix = glm::translate(glm::scale(glm::rotate(i, glm::radians(angle_arm), glm::vec3(0,0,1)), glm::vec3(length_arm, 1.0f, 1.0f)), glm::vec3(0.5f, 0, 0));
	glm::mat4 armMatrix1 = glm::rotate(glm::translate(i, glm::vec3(0.0f, 0, 0)), glm::radians(angle_arm), glm::vec3(0, 0, 1));
	glm::mat4 armMatrix = glm::scale(armMatrix1, glm::vec3(length_arm, 1.0f, 1.0f));
	//glm::mat4 armMatrix = glm::translate(glm::rotate(glm::scale(i, glm::vec3(length_arm, 1.0f, 1.0f)), glm::radians(angle_arm), glm::vec3(0, 0, 1)), glm::vec3(0.5f, 0,0));
	drawUnitCube(armMatrix);
	glMultMatrixf(glm::value_ptr(armMatrix1));

	// forearm
	glPushMatrix();
	glColor3f(0.0f, 1.0f, 0.0f);
	//glm::mat4 foreArmMatrix = glm::translate(glm::scale(glm::rotate(i, glm::radians(angle_forearm), glm::vec3(0, 0, 1)), glm::vec3(length_forearm, 1.0f, 1.0f)), glm::vec3(length_arm, 0, 0));
	glm::mat4 foreArmMatrix1 = glm::rotate(glm::translate(i, glm::vec3(length_arm, 0, 0)), glm::radians(angle_forearm), glm::vec3(0, 0, 1));
	glm::mat4 foreArmMatrix = glm::scale(foreArmMatrix1, glm::vec3(length_forearm, 1.0f, 1.0f));
	drawUnitCube(foreArmMatrix);
	glMultMatrixf(glm::value_ptr(foreArmMatrix1));

	// hand
	glColor3f(0.0f, 0.0f, 1.0f);
	glm::mat4 handMatrix1 = glm::rotate(glm::translate(i, glm::vec3(length_forearm, 0, 0)), glm::radians(angle_hand), glm::vec3(0, 0, 1));
	glm::mat4 handMatrix = glm::scale(handMatrix1, glm::vec3(length_hand, 1.0f, 1.0f));
	drawUnitCube(handMatrix);

	glPopMatrix();
	glPopMatrix();

	glm::mat4 test = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	glm::mat4 test2 = glm::scale(glm::rotate(test, glm::radians(angle_arm), glm::vec3(0, 0, 1)), glm::vec3(length_arm, 1, 1));
	glm::mat4 offset = glm::translate(test2, glm::vec3(0.5, 0, 0));
	glColor3f(1, 0, 0);
	//drawUnitCube(offset);
	glm::mat4 lowerarmOffset = glm::translate(glm::rotate(test, glm::radians(angle_arm), glm::vec3(0, 0, 1)), glm::vec3(length_arm, 0, 0));
	glm::mat4 lowerarm1 = glm::scale(glm::rotate(lowerarmOffset, glm::radians(angle_forearm), glm::vec3(0, 0, 1)), glm::vec3(length_forearm, 1, 1));
	glm::mat4 lowerarm2 = glm::translate(lowerarm1, glm::vec3(0.5, 0, 0));
	glColor3f(0, 0, 1);
	//drawUnitCube(lowerarm2);


	//glm::mat4 offset = glm::scale(glm::translate(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.5 , 0, 0)), glm::radians(angle_arm), glm::vec3(0,0,1)), glm::vec3(-1, 0, 0)), glm::vec3(length_arm,1,1));
	//drawUnitCube(offset);

	glm::mat4 offset2 =
		glm::translate(
			glm::rotate(
				glm::translate(
					glm::rotate(
						glm::translate(
							glm::mat4(1.0f),
							glm::vec3(0.5, 0, 0)
						),
						glm::radians(angle_forearm),
						glm::vec3(0, 0, 1)
					),
					glm::vec3(-0.5, 0, 0)
				),
				glm::radians(angle_arm),
				glm::vec3(0, 0, 1)
			),
			glm::vec3(length_arm, 0, 0)
		);
	//drawUnitCube(offset2);

}




void drawSnakeRec(int depth) {
	if (depth == 0) {
		return;
	}
	glPushMatrix();
	glm::mat4 i = glm::mat4(1.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glm::mat4 handMatrix1 = glm::rotate(glm::translate(i, glm::vec3(length_arm, 0, 0)), glm::radians(45 * sin(snake_var + depth * 1.5f)), glm::vec3(0, 0, 1));
	glm::mat4 handMatrix = glm::scale(handMatrix1, glm::vec3(length_arm, 1.0f, 1.0f));
	drawUnitCube(handMatrix);
	glMultMatrixf(glm::value_ptr(handMatrix1));
	drawSnakeRec(depth - 1);
	glPopMatrix();
	return;
}

void drawSnake() {
	drawSnakeRec(20);
}

void drawLight()
{
	//1) Draw a cube at the light's position lightPos using your drawUnitCube function.
	//   To make the light source bright, follow the drawCoordSystem function
	//   To deactivate the lighting temporarily and draw it in yellow.

	//2) Make the light position controllable via the keyboard function

	//3) Add normal information to all your faces of the previous functions
	//   and observe the shading after pressing 'L' to activate the lighting.
	//   You can use 'l' to turn it off again.

	//4) OPTIONAL
	//   Draw a sphere (consisting of triangles) instead of a cube.
	bool light = glIsEnabled(GL_LIGHTING);
	glDisable(GL_LIGHTING);
	glColor3f(1, 1, 0);
	drawUnitCube(glm::scale(glm::translate(glm::mat4(1), glm::vec3(lightPos)), glm::vec3(0.2)));

	if (light)
		glEnable(GL_LIGHTING);

}

void drawTriangleFromMesh(Mesh mesh, int index) {
	auto triangle = mesh.triangles[index];
	auto p1 = mesh.vertices[triangle.x];
	auto p2 = mesh.vertices[triangle.y];
	auto p3 = mesh.vertices[triangle.z];
	glm::vec3 normal = glm::normalize(p1.normal + p2.normal + p3.normal);
	glColor3f(1, 0, 0);
	glNormal3f(normal.x, normal.y, normal.z);
	glVertex3f(p1.position.x, p1.position.y, p1.position.z);
	glVertex3f(p2.position.x, p2.position.y, p2.position.z);
	glVertex3f(p3.position.x, p3.position.y, p3.position.z);
}

void drawMesh()
{
	//1) Use the mesh data structure;
	//   Each triangle is defined with 3 consecutive indices in the meshTriangles table.
	//   These indices correspond to vertices stored in the meshVertices table.
	//   Provide a function that draws these triangles.

	//2) Compute the normals of these triangles

	//3) Try computing a normal per vertex as the average of the adjacent face normals.
	//   Call glNormal3f with the corresponding values before each vertex.
	//   What do you observe with respect to the lighting?

	//4) Try loading your own model (export it from Blender as a Wavefront obj) and replace the provided mesh file.
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < mesh.triangles.size(); i++) {

		
		//drawTriangleFromMesh(mesh, i);
		auto triangle = mesh.triangles[i];
		auto p1 = mesh.vertices[triangle.x];
		auto p2 = mesh.vertices[triangle.y];
		auto p3 = mesh.vertices[triangle.z];
		glm::vec3 normal = glm::normalize(p1.normal + p2.normal + p3.normal);
		glColor3f(1, 0, 0);
		glNormal3f(normal.x, normal.y, normal.z);
		glVertex3f(p1.position.x, p1.position.y, p1.position.z);
		glVertex3f(p2.position.x, p2.position.y, p2.position.z);
		glVertex3f(p3.position.x, p3.position.y, p3.position.z);
	}
	glEnd();
}

void display()
{
	//set the light to the right position
	glLightfv(GL_LIGHT0, GL_POSITION, glm::value_ptr(lightPos));
	drawLight();

	switch (displayMode) {
	case DisplayModeType::TRIANGLE:
		drawCoordSystem();
		drawTriangle();
		break;
	case DisplayModeType::FACE:
		drawCoordSystem();
		drawUnitFace(glm::scale(glm::mat4(1.0f), glm::vec3(2))); // mat4(1.0f) = identity matrix
		break;
		// ...
	case DisplayModeType::CUBE:
		drawCoordSystem();
		glColor3f(0, 1, 0);
		drawUnitCube(glm::mat4(1.0f)); // mat4(1.0f) = identity matrix
		break;
	case DisplayModeType::ARM:
		drawCoordSystem();
		drawArm(); // mat4(1.0f) = identity matrix
		break;
	case DisplayModeType::MESH:
		drawCoordSystem();
		drawMesh();
		break;
	case DisplayModeType::SNAKE:
		drawCoordSystem();
		drawSnake();
	default:
		break;
	}
}

/**
 * Animation
 */
void animate()
{
	xvariable += .01;
	snake_var += 0.01f;
}

// Take keyboard input into account.
void keyboard(int key, int /* scancode */, int /* action */, int /* mods */)
{
	glm::dvec2 cursorPos = pWindow->getCursorPos();
	std::cout << "Key " << key << " pressed at " << cursorPos.x << ", " << cursorPos.y << "\n";

	switch (key) {
	case GLFW_KEY_1: {
		displayMode = DisplayModeType::TRIANGLE;
		break;
	}
	case GLFW_KEY_2: {
		displayMode = DisplayModeType::FACE;
		break;
	}
	case GLFW_KEY_3: {
		displayMode = DisplayModeType::CUBE;
		break;
	}
	case GLFW_KEY_4: {
		displayMode = DisplayModeType::ARM;
		break;
	}
	case GLFW_KEY_5: {
		displayMode = DisplayModeType::MESH;
		break;
	}	
	case GLFW_KEY_6: {
		displayMode = DisplayModeType::SNAKE;
		break;
	}
	case GLFW_KEY_ESCAPE: {
		pWindow->close();
		break;
	}
	case GLFW_KEY_Q: {
		angle_arm += 1;
		break;
	}
	case GLFW_KEY_A: {
		angle_arm -= 1;
		break;
	}
	case GLFW_KEY_W: {
		angle_forearm += 1;
		break;
	}
	case GLFW_KEY_S: {
		angle_forearm -= 1;
		break;
	}
	case GLFW_KEY_E: {
		angle_hand += 1;
		break;
	}
	case GLFW_KEY_D: {
		angle_hand -= 1;
		break;
	}
	case GLFW_KEY_Z: {
		length_arm -= 0.1f;
		break;
	}
	case GLFW_KEY_X: {
		length_arm += 0.1f;
		break;
	}
	case GLFW_KEY_L: {
		// Turn lighting on.
		if (pWindow->isKeyPressed(GLFW_KEY_LEFT_SHIFT) || pWindow->isKeyPressed(GLFW_KEY_RIGHT_SHIFT)) {
			glEnable(GL_LIGHTING);
			std::cout << "enabled" << "\n";
		}
		else {
			glDisable(GL_LIGHTING);
			std::cout << "disabled" << "\n";
		}
		break;
	}
	};
}

//Nothing needed below this point
//STOP READING //STOP READING //STOP READING
//STOP READING //STOP READING //STOP READING
//STOP READING //STOP READING //STOP READING
//STOP READING //STOP READING //STOP READING

void displayInternal(void);
void reshape(const glm::ivec2&);
void init()
{
	// Initialize viewpoint
	pTrackball->printHelp();
	reshape(resolution);

	glDisable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);

	//int MatSpec [4] = {1,1,1,1};
	//   glMaterialiv(GL_FRONT_AND_BACK,GL_SPECULAR,MatSpec);
	//   glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,10);

	// Enable Depth test
	glEnable(GL_DEPTH_TEST);

	// Draw frontfacing polygons as filled.
	glPolygonMode(GL_FRONT, GL_FILL);
	// Draw backfacing polygons as outlined.
	glPolygonMode(GL_BACK, GL_LINE);
	glShadeModel(GL_SMOOTH);
	mesh = loadMesh("David.obj", true)[0];
}

// Program entry point. Everything starts here.
int main(int /* argc */, char** argv)
{
	pWindow = std::make_unique<Window>(argv[0], resolution, OpenGLVersion::GL2);
	pTrackball = std::make_unique<Trackball>(pWindow.get(), glm::radians(50.0f));
	pWindow->registerKeyCallback(keyboard);
	pWindow->registerWindowResizeCallback(reshape);

	init();

	while (!pWindow->shouldClose()) {
		pWindow->updateInput();

		animate();
		displayInternal();

		pWindow->swapBuffers();
	}
}

// OpenGL helpers. You don't need to touch these.
void displayInternal(void)
{
	// Clear screen
	glViewport(0, 0, pWindow->getWindowSize().x, pWindow->getWindowSize().y);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Load identity matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Load up view transforms
	const glm::mat4 viewTransform = pTrackball->viewMatrix();
	glMultMatrixf(glm::value_ptr(viewTransform));

	// Your rendering function
	animate();
	display();
}
void reshape(const glm::ivec2& size)
{
	// Called when the window is resized.
	// Update the viewport and projection matrix.
	glViewport(0, 0, size.x, size.y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	const glm::mat4 perspectiveMatrix = pTrackball->projectionMatrix();
	glLoadMatrixf(glm::value_ptr(perspectiveMatrix));
	glMatrixMode(GL_MODELVIEW);
}
