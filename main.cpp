#include "icg_common.h"
#include "_mesh/Mesh.h"
#include "_mesh/TerrainMesh.h"

int window_width = 640;
int window_height = 640;

Mesh mesh;
TerrainMesh terrain;

typedef Eigen::Transform<float, 3, Eigen::Affine> Transform;


bool mouseMove = false;

//Viewing matrices
mat4 VIEW;
mat4 PROJ;
mat4 MODEL;


void mousemove(int x, int y)
{
	if (!mouseMove)
		return;
	cout << "x = " << x << endl;
	cout << "y = " << y << endl;

}

void selection_button(int button, int action)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		mouseMove = true;
		int x = 0, y = 0;
		glfwGetMousePos(&x, &y);
		mousemove(x, y);
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		mouseMove = false;
	}
}
void init(){

    glClearColor(1,1,1, /*solid*/1.0 );    
    glEnable(GL_DEPTH_TEST);
 //   mesh.init();
	terrain.init(16,16);

	//setup viewing matrices;
	float near = 0.25;  //near and far planes;
	float far = 3.5;

}

void display(){
    opengp::update_title_fps("Assignment 3 and 4");   
    glViewport(0,0,window_width,window_height);    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	mesh.draw();
	
	terrain.draw();
}

void cleanup(){}

void keyboard(int key, int action){
    if(action != GLFW_RELEASE) return; ///< only act on release
    switch(key){
        case '0': break;
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
