#include "icg_common.h"
#include "_mesh/Mesh.h"
#include "_mesh/TerrainMesh.h"
#include "_mesh/Skybox.h"
#include "_mesh/Water.h"
#include "FrameBuffer.h"
#include "OpenGLImage/EigenVisualizer.h"

using namespace EigenVisualizer;

int window_width = 1280;
int window_height = 640;

FrameBuffer fb(window_width, window_height);

Mesh mesh;
TerrainMesh terrain;
Skybox skybox;
Water water;
typedef Eigen::Transform<float, 3, Eigen::Affine> Transform;

bool keypress = false;
int lastkey = -1;
//mouse panning helpers
bool rotating = false;
bool scaling = false;
const double lim = 0.5;
const double RadPerPixel = 0.01;
const double MovePerPixel = 0.001;
int lastx; int lasty;

//vectors used for camera matrix transforms
vec3 dirVec(100, 0, 100);  //camera orientation
vec3 eye(0, 8, 0);				//camera translation in world space

//Viewing matrices
mat4 VIEW;
mat4 PROJ;
mat4 MODEL;

void RotateY(vec3 *dir, float rot)
{
	float cosPhi = (float)cos(rot);
	float sinPhi = (float)sin(rot);
	float xnew = 0;
	xnew += (*dir).x()*cosPhi;
	xnew += (*dir).z() * sinPhi;
	float ynew = 0;
	ynew += (*dir).y() * 1;
	ynew += (*dir).z() * sinPhi;
	float znew = 0;
	znew += (*dir).x() * -sinPhi;
	znew += (*dir).z() * cosPhi;

	(*dir).x() = xnew; (*dir).y() = ynew; (*dir).z() = znew;
}

void RotateX(vec3 *dir, float rot)
{
	double cosPhi = (double)cos(rot);
	double sinPhi = (double)sin(rot);
	//need to learn proper way to mult vec3 by matrix
	float xnew = 0;
	xnew += (*dir).x();
	float ynew = 0;
	ynew += (*dir).y() * cosPhi;
	ynew += (*dir).z() * sinPhi;
	float znew = 0;
	znew += (*dir).y() * -sinPhi;
	znew += (*dir).z() * cosPhi;

	(*dir).x() = xnew; (*dir).y() = ynew; (*dir).z() = znew;
}

void CamRotate(int xnew, int ynew, int xold, int yold)
{
	float XRot = (ynew - yold) * RadPerPixel;
	float YRot = (xnew - xold) * RadPerPixel;
	RotateX(&dirVec, XRot);
	RotateY(&dirVec, YRot);
	VIEW = Eigen::lookAt(eye, dirVec, vec3(0, 1, 0));
	lastx = xnew; lasty = ynew;
}


void mousemove(int x, int y)
{
	if (rotating)
		CamRotate(x, y, lastx, lasty);
	//else if (scaling)
	//	CamZoom(x, y, lastx, lasty);
}

void selection_button(int button, int action)
{
	//Left mouse button is for rotating
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (!rotating)
		{
			rotating = true;
			glfwGetMousePos(&lastx, &lasty);
		}
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		rotating = false;
	//right mouse button scales
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		if (!scaling)
		{
			scaling = true;
			glfwGetMousePos(&lastx, &lasty);
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		scaling = false;
}

void init(){

	glClearColor(0.5, 0.5, 0.5, /*solid*/1.0);
	glEnable(GL_DEPTH_TEST);

	//Set up the framebuffer object to be used as the texture
	GLuint fb_tex = fb.init();
	terrain.init(500, 500);
	skybox.init(700, 700);	
	water.init(500, 500);

	//setup viewing matrices;
	MODEL = mat4::Identity();
	VIEW = Eigen::lookAt(eye, dirVec, vec3(0, 1, 0));
	PROJ = Eigen::perspective(45.0f, window_width / (float)window_height, 0.1f, 1100.0f);
}

void display(){
	opengp::update_title_fps("Assignment 3 and 4");
	glViewport(0, 0, window_width, window_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLuint pid;

	vec3 cam_pos_mirror = eye;
	cam_pos_mirror(2) = -eye(2);
	mat4 view_mirror = Eigen::lookAt(cam_pos_mirror, dirVec, vec3(0, 1, 0));
	//	fb.bind();
	pid = skybox.getProgramID();
	glUseProgram(pid);
	glUniformMatrix4fv(glGetUniformLocation(pid, "MODEL"), 1, GL_FALSE, MODEL.data());
	glUniformMatrix4fv(glGetUniformLocation(pid, "VIEW"), 1, GL_FALSE, VIEW.data());
	glUniformMatrix4fv(glGetUniformLocation(pid, "PROJ"), 1, GL_FALSE, PROJ.data());
	skybox.draw();
	glUseProgram(0);

	pid = terrain.getProgramID();
	glUseProgram(pid);
	glUniformMatrix4fv(glGetUniformLocation(pid, "MODEL"), 1, GL_FALSE, MODEL.data());
	glUniformMatrix4fv(glGetUniformLocation(pid, "VIEW"), 1, GL_FALSE, VIEW.data());
	glUniformMatrix4fv(glGetUniformLocation(pid, "PROJ"), 1, GL_FALSE, PROJ.data());
	terrain.draw();
	//	fb.unbind();
	glUseProgram(0);

	//viewing matrices	
	pid = water.getProgramID();
	glUseProgram(pid);
	glUniformMatrix4fv(glGetUniformLocation(pid, "MODEL"), 1, GL_FALSE, MODEL.data());
	glUniformMatrix4fv(glGetUniformLocation(pid, "VIEW"), 1, GL_FALSE, VIEW.data());
	glUniformMatrix4fv(glGetUniformLocation(pid, "PROJ"), 1, GL_FALSE, PROJ.data());
	water.draw();
	glUseProgram(0);

}

void cleanup(){}

void keyboard(int key, int action)
{
	if (action == GLFW_RELEASE)
		return; ///< only act on PRESS

	float delta = 0.02;  ///the step amount for wasd
	vec3 xaxis = dirVec.normalized().cross(vec3(0, 1, 0));
	switch (key){
	case '0': break;
	case 'w': case 'W':
		eye += dirVec*delta;
		dirVec += dirVec*delta;
		break;
	case 's': case 'S':
		eye -= dirVec*delta;
		dirVec -= dirVec*delta;
		break;
	case 'a': case 'A':
		eye -= xaxis;
		dirVec -= xaxis;
		break;
	case 'd': case 'D':
		eye += xaxis;
		dirVec += xaxis;
		break;
	case 'i': case 'I':
		printf("eye = %f, %f, %f dirVec = %f, %f, %f \n", eye.x(), eye.y(), eye.z(), dirVec.x(), dirVec.y(), dirVec.z());
		break;
	default: break;

	}
	VIEW = Eigen::lookAt(eye, dirVec, vec3(0, 1, 0));
}

int main(int, char**){
	glfwInitWindowSize(window_width, window_height);
	glfwCreateWindow();	glfwEnable(GLFW_KEY_REPEAT);
	glfwDisplayFunc(display);

	glfwSetMouseButtonCallback(selection_button);
	glfwSetMousePosCallback(mousemove);
	glfwSetKeyCallback(keyboard);
	init();
	//	keyboard(GLFW_KEY_KP_1, 0);
	glfwMainLoop();
	cleanup();
	return EXIT_SUCCESS;
}
