#include <glad/glad.h>
#include <GLFW/glfw3.h>



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "camera.h"
#include "object.h"
#include "model.h"

#include <iostream>
#include <fstream>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void AddWall(int x, int y, int z, float rot =0, int sx = 1, int sy = 1, int sz = 1);
void AddDoor(int x, int y, int z, float rot = 0, int sx = 1, int sy = 1, int sz = 1);
void AddFloor(int x, int y, int z, float rot = 0, int sx = 1, int sy = 1, int sz = 1);
void AddLight(int x, int y, int z, float rot = 0, int sx = 1, int sy = 1, int sz = 1);
void AddModel(int x, int y, int z, float rot = 0, int sx = 1, int sy = 1, int sz = 1);


void ReadMap();
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(std::vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int texturePack = 1;


///list of everything
std::vector<Object> walls;
std::vector<Object> doors;
std::vector<Object> floors;
std::vector<Object> lights;
std::vector<Object> nanoSuits;

// lighting
glm::vec3 lightPos(1.2f, 20.0f, 2.0f);

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Gandalfs Engine", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader("resources/shaders/1.model_loading.vs", "resources/shaders/1.model_loading.fs");

	Shader skyboxShader("resources/shaders/6.1.skybox.vs", "resources/shaders/6.1.skybox.fs");

	Shader lightingShader("resources/shaders/2.2.basic_lighting.vs", "resources/shaders/2.2.basic_lighting.fs");
	Shader lampShader("resources/shaders/2.2.lamp.vs", "resources/shaders/2.2.lamp.fs");

	Model ourModel("resources/model/nanosuit/nanosuit.obj");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		 -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 

		//left
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,

		//right
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f	
	};


	float floor_vertices[] = {
	 
		-0.5f, -0.6f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.6f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.6f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
				  
		-0.5f, -0.6f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.6f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,


		//left
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.6f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.6f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.6f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,

		//right
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.6f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f, -0.6f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f, -0.6f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f

	};


	float door_vertices[] = {
		///left pillar
		//front face??
	 -0.5f, -0.5f, -0.17f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	 -0.25f, -0.5f, -0.17f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	 -0.25f,  0.4f, -0.17f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	 -0.25f,  0.4f, -0.17f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	-0.5f,  0.4f, -0.17f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.17f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

	// back face?
	-0.5f, -0.5f,  0.17f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	 -0.25f, -0.5f,  0.17f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
	 -0.25f,  0.4f,  0.17f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	 -0.25f,  0.4f,  0.17f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	-0.5f,  0.4f,  0.17f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.17f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	// left face?
	-0.5f,  0.4f,  0.17f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.4f, -0.17f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	-0.5f, -0.5f, -0.17f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.17f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.17f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.4f,  0.17f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	//right face?
	 -0.25f,  0.4f,  0.17f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	 -0.25f,  0.4f, -0.17f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	 -0.25f, -0.5f, -0.17f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 -0.25f, -0.5f, -0.17f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 -0.25f, -0.5f,  0.17f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	 -0.25f,  0.4f,  0.17f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	 ///right pillar
	 //front face??
	 0.25f, -0.5f, -0.17f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.17f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	 0.5f,  0.4f, -0.17f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	 0.5f,  0.4f, -0.17f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	 0.25f,  0.4f, -0.17f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
	 0.25f, -0.5f, -0.17f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	
	// back face?
	0.25f, -0.5f,  0.17f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.17f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
	 0.5f,  0.4f,  0.17f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	 0.5f,  0.4f,  0.17f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	0.25f,  0.4f,  0.17f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	0.25f, -0.5f,  0.17f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	
	// left face?
	0.25f,  0.4f,  0.17f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	0.25f,  0.4f, -0.17f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	0.25f, -0.5f, -0.17f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	0.25f, -0.5f, -0.17f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	0.25f, -0.5f,  0.17f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	0.25f,  0.4f,  0.17f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	
	//right face?
	 -0.5f,  0.4f,  0.17f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	 -0.5f,  0.4f, -0.17f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	 -0.5f, -0.5f, -0.17f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 -0.5f, -0.5f, -0.17f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 -0.5f, -0.5f,  0.17f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	 -0.5f,  0.4f,  0.17f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	 ///top pillar
	  //front face??
	 -0.5f, 0.4f, -0.17f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	 0.5f,  0.4f, -0.17f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.17f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	 0.5f,  0.5f, -0.17f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	 -0.5f, 0.5f, -0.17f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
	 -0.5f, 0.4f, -0.17f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

	 // back face?
	 -0.5f,  0.4f,  0.17f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	  0.5f,  0.4f,  0.17f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
	  0.5f,  0.5f,  0.17f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	  0.5f,  0.5f,  0.17f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	 -0.5f,  0.5f,  0.17f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	 -0.5f,  0.4f,  0.17f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	 // left face?
	 -0.5f,  0.5f,  0.17f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	 -0.5f,  0.5f, -0.17f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	 -0.5f,  0.4f, -0.17f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 -0.5f,  0.4f, -0.17f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 -0.5f,  0.4f,  0.17f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	 -0.5f,  0.5f,  0.17f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	 //right face?
	  -0.5f,  0.5f,  0.17f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	  -0.5f,  0.5f, -0.17f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	  -0.5f,  0.4f, -0.17f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	  -0.5f,  0.4f, -0.17f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	  -0.5f,  0.4f,  0.17f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	  -0.5f,  0.5f,  0.17f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	  //bottom face
	  -0.5f, 0.4f, -0.17f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
     	 0.5f, 0.4f, -0.17f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
     	 0.5f, 0.4f,  0.17f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     	 0.5f, 0.4f,  0.17f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     	-0.5f, 0.4f,  0.17f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
     	-0.5f, 0.4f, -0.17f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		//top face
		-0.5f, 0.5f, -0.17f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		 0.5f, 0.5f, -0.17f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		 0.5f, 0.5f,  0.17f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		 0.5f, 0.5f,  0.17f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f,  0.17f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.17f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};


	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO, doorVBO, doorVAO, floorVBO, floorVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &doorVAO);
	glGenBuffers(1, &doorVBO);
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);

	

	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//tex co-ordinate attrubute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(floorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//tex co-ordinate attrubute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(doorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, doorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(door_vertices), door_vertices, GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//tex co-ordinate attrubute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);



	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO, lightVBO;
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &lightVBO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);


	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	unsigned int wallTexture = loadTexture("resources/textures/awesomeface.png");
	unsigned int wallSPec = loadTexture("resources/textures/awesomeface.png");

	unsigned int wallD1 = loadTexture("resources/textures/wall1/diff.png");
	unsigned int wallS1 = loadTexture("resources/textures/wall1/spec.png");

	unsigned int wallD2 = loadTexture("resources/textures/wall2/diff.png");
	unsigned int wallS2 = loadTexture("resources/textures/wall2/spec.png");

	unsigned int floorTexD = loadTexture("resources/textures/floor/diff.jpg");
	unsigned int floorTexS = loadTexture("resources/textures/floor/spec.jpg");

	unsigned int floorTexD2 = loadTexture("resources/textures/floor2/diff.jpg");
	unsigned int floorTexS2 = loadTexture("resources/textures/floor2/spec.jpg");

	unsigned int floorTexD3 = loadTexture("resources/textures/floor3/diff.jpg");
	unsigned int floorTexS3 = loadTexture("resources/textures/floor3/spec.jpg");


	unsigned int wallD3 = loadTexture("resources/textures/wall3/diff.png");
	unsigned int wallS3 = loadTexture("resources/textures/wall3/spec.png");
	std::vector<std::string> faces
	{
		"resources/textures/skyboxnn/right.jpg",
		"resources/textures/skyboxnn/right.jpg",
		"resources/textures/skyboxnn/top.jpg",
		"resources/textures/skyboxnn/bottom.jpg",
		"resources/textures/skyboxnn/front.jpg",
		"resources/textures/skyboxnn/front.jpg"
	};
	  unsigned int cubemapTexture = loadCubemap(faces);

	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);


	///list of shit
	//AddWall(0, 0, 0);
	//AddWall(1, 1, 1);
	//AddWall(2, 2, 2, 45);
	//
	ReadMap();
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightingShader.use();
		lightingShader.setInt("amountOfLights", lights.size());
		for (int i = 0; i < lights.size(); i++)
		{
			std::stringstream ss;
			ss << i;
			std::string str = ss.str();

			lightingShader.setVec3("lights[" + str + "].position", lights[i].position);

			// light properties
			lightingShader.setVec3("lights[" + str + "].ambient", 0.2f, 0.2f, 0.2f);
			lightingShader.setVec3("lights[" + str + "].diffuse", 0.5f, 0.5f, 0.5f);
			lightingShader.setVec3("lights[" + str + "].specular", 1.0f, 1.0f, 1.0f);
		}
		lightingShader.setVec3("light.position", lightPos);
		lightingShader.setVec3("viewPos", camera.Position);

		// light properties
		lightingShader.setVec3("light.ambient", 0.04f, 0.04f, 0.04f);
		lightingShader.setVec3("light.diffuse", 0.1f, 0.1f, 0.1f);
		lightingShader.setVec3("light.specular", 0.2f, 0.2f, 0.2f);

		// material properties
		lightingShader.setFloat("material.shininess", 64.0f);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// world transformation
		

		// render the cube


				// render the floor
		if (texturePack == 1)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, floorTexD);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, floorTexS);
		}
		else if(texturePack == 2)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, floorTexD2);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, floorTexS2);
		}
		else if (texturePack == 3)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, floorTexD3);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, floorTexS3);
		}

		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("model", model);
		glBindVertexArray(floorVAO);
		for (unsigned int i = 0; i < floors.size(); i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, floors[i].position);
			model = glm::rotate(model, glm::radians(floors[i].rotation), glm::vec3(0.0f, 1.0f, 0.0f));
			lightingShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		// render the cube
		if (texturePack == 1)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, wallD1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, wallS1);
		}
		else if (texturePack == 2)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, wallD2);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, wallS2);
		}
		else if (texturePack == 3)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, wallD3);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, wallS3);
		}
		glBindVertexArray(cubeVAO);
		for (unsigned int i = 0; i < walls.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, walls[i].position);
			model = glm::rotate(model, glm::radians(walls[i].rotation), glm::vec3(0.0f, 1.0f, 0.0f));
			lightingShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);

		}




		glBindVertexArray(doorVAO);
		for (unsigned int i = 0; i < doors.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, doors[i].position);
			model = glm::rotate(model, glm::radians(doors[i].rotation), glm::vec3(0.0f, 1.0f, 0.0f));
			lightingShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 72+6+6);

		}

		for (unsigned int i = 0; i < nanoSuits.size(); i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(nanoSuits[i].position.x, -0.5, nanoSuits[i].position.z)); // translate it down so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.05f));	// it's a bit too big for our scene, so scale it down
			lightingShader.setMat4("model", model);
			ourModel.Draw(lightingShader);

		}


		//ourShader.use();
		//// view/projection transformations
		//
		//ourShader.setMat4("projection", projection);
		//ourShader.setMat4("view", view);


		// render the loaded model
	



		//glDrawArrays(GL_TRIANGLES, 0, 36);


		// also draw the lamp object
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lampShader.setMat4("model", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		for (unsigned int i = 0; i < lights.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, lights[i].position);
			model = glm::scale(model, glm::vec3(0.05f)); // a smaller cube
			model = glm::rotate(model, glm::radians(lights[i].rotation), glm::vec3(0.0f, 1.0f, 0.0f));
			lampShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);

		}

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default




		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		texturePack = 1;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		texturePack = 2;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		texturePack = 3;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
//
void ReadMap()
{
	std::ifstream file("resources/map.txt");
	std::string str;
	int x, y = 0;
	while (std::getline(file, str)) {
		for (int i = 0; i < str.length(); i++) {
			if (str[i] == 'W') {
				AddWall(i,0,y);
			
			}
			if (str[i] == 'D')
			{
				AddDoor(i, 0, y);
				AddFloor(i, 0, y);
			}
			if (str[i] == 'd')
			{
				AddDoor(i, 0, y, 90.0f);
				AddFloor(i, 0, y);
			}
			if (str[i] == 'O') {
				AddFloor(i, 0, y);
			}
			if (str[i] == 'l') {
				AddLight(i, 0, y);
				AddFloor(i, 0, y);
			}
			if (str[i] == 'M') {
			
				AddModel(i, 0, y);
				AddFloor(i, 0, y);
			}
			
		}
		y++;
	}
}
//
void AddWall(int x, int y, int z, float rot , int sx , int sy , int sz )
{
	Object newObject = {};
	glm::vec3 pos = glm::vec3(x, y, z);
	glm::vec3 scale = glm::vec3(sx, sy, sz);
	newObject.position = pos;
	newObject.rotation = rot;
	newObject.scale = scale;
	walls.push_back(newObject);

}

void AddFloor(int x, int y, int z, float rot, int sx, int sy, int sz) {
	Object newObject = {};
	glm::vec3 pos = glm::vec3(x, y, z);
	glm::vec3 scale = glm::vec3(sx, sy, sz);
	newObject.position = pos;
	newObject.rotation = rot;
	newObject.scale = scale;
	floors.push_back(newObject);
}

void AddModel(int x, int y, int z, float rot, int sx, int sy, int sz)
{
	Object newObject = {};
	glm::vec3 pos = glm::vec3(x, y, z);
	glm::vec3 scale = glm::vec3(sx, sy, sz);
	newObject.position = pos;
	newObject.rotation = rot;
	newObject.scale = scale;
	nanoSuits.push_back(newObject);

}
void AddLight(int x, int y, int z, float rot, int sx, int sy, int sz)
{
	Object newObject = {};
	glm::vec3 pos = glm::vec3(x, y, z);
	glm::vec3 scale = glm::vec3(sx, sy, sz);
	newObject.position = pos;
	newObject.rotation = rot;
	newObject.scale = scale;
	lights.push_back(newObject);

}
//
void AddDoor(int x, int y, int z, float rot, int sx, int sy, int sz)
{
	Object newObject = {};
	glm::vec3 pos = glm::vec3(x, y, z);
	glm::vec3 scale = glm::vec3(sx, sy, sz);
	newObject.position = pos;
	newObject.rotation = rot;
	newObject.scale = scale;
	doors.push_back(newObject);

}
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


unsigned int loadCubemap(std:: vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}