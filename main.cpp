#include "icg_common.h"
#include "_mesh/Mesh.h"
#include "_mesh/TerrainMesh.h"
#include "_mesh/Skybox.h"

#include "OpenGLImage/EigenVisualizer.h"

using namespace EigenVisualizer;

int window_width = 640;
int window_height = 640;

Mesh mesh;
TerrainMesh terrain;
Skybox skybox;

typedef Eigen::Transform<float, 3, Eigen::Affine> Transform;

//mouse panning helpers
bool rotating = false;
bool scaling = false;
const double lim = 0.5;
const double RadPerPixel = 0.01;
const double MovePerPixel = 0.001;
int lastx; int lasty;

vec3 dirVec(-0.8607, 1.1486, -2.177);

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

void RotateZ(vec3 *dir, float rot)
{
	double cosPhi = (double)cos(rot);
	double sinPhi = (double)sin(rot);
//need to learn proper way to mult vec3 by matrix
	float xnew = 0;
	xnew += (*dir).x()*cosPhi;
	xnew += (*dir).y() * -sinPhi;
	float ynew = 0;
	ynew += (*dir).x() * sinPhi;
	ynew += (*dir).y() * cosPhi;
	float znew = 0;
	znew += (*dir).z() * 1;

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
//	float ZRot = (ynew - yold) * RadPerPixel;
	RotateX(&dirVec, XRot);
	RotateY(&dirVec, YRot);
//	RotateZ(&dirVec, ZRot);
	VIEW = Eigen::lookAt(dirVec, vec3(0, 0, 0), vec3(0, 1, 0));
	lastx = xnew; lasty = ynew;
}

void CamZoom(int xnew, int ynew, int xold, int yold)
{
	float length = sqrt(dirVec.x() * dirVec.x() + dirVec.y() * dirVec.y());
	float newLength = length + (ynew - yold) * MovePerPixel;
	if (ynew > yold)
	{
		MODEL(0, 3) += 0.2;
		MODEL(1, 3) += 0.2;
		MODEL(2, 3) += 0.2;
	}
	else if (ynew < yold)
	{
		MODEL(0, 3) += -0.2;
		MODEL(1, 3) += -0.2;
		MODEL(2, 3) += -0.2;
	}
	lastx = xnew; lasty = ynew;
}

void mousemove(int x, int y)
{
	if (rotating)
		CamRotate(x, y, lastx, lasty);
	else if (scaling)
		CamZoom(x, y, lastx, lasty);
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

    glClearColor(0.5,0.5,0.5, /*solid*/1.0 );    
    glEnable(GL_DEPTH_TEST);
//    mesh.init();
	terrain.init(500, 500);
	skybox.init(700, 700);

	//setup viewing matrices;
	MODEL = mat4::Identity();
	VIEW = Eigen::lookAt(dirVec, vec3(0, 0, 0), vec3(0, 1, 0));
	PROJ = Eigen::perspective(45.0f, window_width / (float)window_height, 0.1f, 100.0f);
}

void display(){
    opengp::update_title_fps("Assignment 3 and 4");   
    glViewport(0,0,window_width,window_height);    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	


	glUseProgram(0);
	//viewing matrices	
	GLuint pid = terrain.getProgramID();
	glUseProgram(pid);
	glUniformMatrix4fv(glGetUniformLocation(pid, "MODEL"), 1, GL_FALSE, MODEL.data());
	glUniformMatrix4fv(glGetUniformLocation(pid, "VIEW"), 1, GL_FALSE, VIEW.data());
	glUniformMatrix4fv(glGetUniformLocation(pid, "PROJ"), 1, GL_FALSE, PROJ.data());
	terrain.draw();
	glUseProgram(0);
//	skybox.draw();
}

void cleanup(){}

void keyboard(int key, int action){
    if(action != GLFW_PRESS) return; ///< only act on release
    switch(key){
        case '0': break;
		case 'w': case 'W':
			printf("%f, %f, %f", dirVec.x(), dirVec.y(), dirVec.z());
			break;
			
        default: break;
    }
}

int main(int, char**){
    glfwInitWindowSize(window_width, window_height);
    glfwCreateWindow();
    glfwDisplayFunc(display);
	glfwSetMouseButtonCallback(selection_button);
	glfwSetMousePosCallback(mousemove);
    glfwSetKeyCallback(keyboard);
    init();
    keyboard(GLFW_KEY_KP_1, 0);
    glfwMainLoop();
    cleanup();
    return EXIT_SUCCESS;
}
