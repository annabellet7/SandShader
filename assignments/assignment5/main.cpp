#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <ew/mesh.h>
#include <ew/procGen.h>
#include <ew/external/stb_image.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Shader/Shader.h"
#include "Texture/Texture.h"
#include "Camera/Camera.h"

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera cam(glm::vec3(0.0f, 0.0f, 10.0f));
bool firstMouse = true;
float lastX = 400, lastY = 300;

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720; 

glm::vec3 lightDirection(4.5f, 5.0f, 1.0f);
glm::vec3 lightColor(0.898f, 0.863f, 0.757f);
float ambientK = 0.25f;
float diffuseK = 0.2f;
float specularK = 1.0f;
int shininess = 32;
float sandStrength = 0.4f;
float grainSize = 5.0f;
float rimStrength = 3.0f;
float rimPower = 4.5f;

void processInput(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

bool wireFrame = false;
bool pointRender = false;

int main() {
	printf("Initializing...\n");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	//Initialization goes here!
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	Shader sandShader("assets/shaderAssets/basicLightingVShader.vert", "assets/shaderAssets/basicLightingFShader.frag");
	Shader normalShader("assets/shaderAssets/normalVisualization.vert", "assets/shaderAssets/normalVisualization.frag", "assets/shaderAssets/normalVisualization.geom");
	Shader lampShader("assets/shaderAssets/lampVShader.vert", "assets/shaderAssets/lampFShader.frag");

	//-----------------------------------------------------------------------------------------------

	ew::MeshData planeMeshData;
	ew::MeshData sphereMeshData;
	ew::MeshData cubeMeshData;
	ew::createPlaneXY(6.0f, 6.0f, 1, &planeMeshData);
	ew::createCube(1.0f, &cubeMeshData);
	ew::createSphere(2.0f, 32, &sphereMeshData);
	ew::Mesh planeMesh = ew::Mesh(planeMeshData);
	ew::Mesh cubeMesh = ew::Mesh(cubeMeshData);
	ew::Mesh sphereMesh = ew::Mesh(sphereMeshData);

	Texture2D grainNormals("assets/NormalMaps/grainNormals.png", GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, GL_REPEAT, GL_REPEAT, GL_RGB);
	Texture2D webTexture("assets/Textures/web.jpg", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, GL_RGB);

	//set active shader and set textures to units
	sandShader.Shader::use();
	glUniform1i(glGetUniformLocation(sandShader.getProgram(), "normalMap"), 0);
	glUniform1i(glGetUniformLocation(sandShader.getProgram(), "texture1"), 1);

	float rotationTime = 0;
	//Render loop
	while (!glfwWindowShouldClose(window)) {
		//update time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		rotationTime += deltaTime;

		//input
		processInput(window);

		//Clear framebuffer
		glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

		ew::DrawMode drawMode = pointRender ? ew::DrawMode::POINTS : ew::DrawMode::TRIANGLES;
		grainNormals.Texture2D::bind(0);
		webTexture.Texture2D::bind(1);

		//use shader
		sandShader.Shader::use();
		sandShader.setVec3("uLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		sandShader.setVec3("uLightDirection", lightDirection);
		sandShader.setVec3("uViewPos", cam.getPos());
		sandShader.setVec3("uLightColor", lightColor);
		sandShader.setFloat("uAmbientK", ambientK);
		sandShader.setFloat("uDiffuseK", diffuseK);
		sandShader.setFloat("uSpecularK", specularK);
		sandShader.setInt("uShininess", shininess);
		sandShader.setFloat("uSandStrength", sandStrength);
		sandShader.setFloat("uGrainSize", grainSize);
		sandShader.setFloat("uRimStrength", rimStrength);
		sandShader.setFloat("uRimPower", rimPower);

		//update uniform
		//time
		float time = glfwGetTime();
		int timeLoc = glGetUniformLocation(sandShader.mId, "uTime");
		glUniform1f(timeLoc, time);

		//camera view
		glm::mat4 projection = glm::perspective(glm::radians(cam.mZoom), 800.0f / 600.0f, 0.1f, 1000.0f);
		sandShader.setMat4("projection", projection);

		glm::mat4 view = cam.getViewMatrix();
		sandShader.setMat4("view", view);

		//Draw plane
		/*glm::mat4 planeTransform = glm::mat4(1);
		planeTransform = glm::rotate(planeTransform, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		planeTransform = glm::translate(planeTransform, glm::vec3(-5.0, -5.0, 0.0));
		sandShader.setMat4("model", planeTransform);
		planeMesh.draw(drawMode);
		
		normalShader.Shader::use();
		normalShader.setMat4("projection", projection); 
		normalShader.setMat4("view", view);  
		normalShader.setMat4("model", planeTransform);
		planeMesh.draw(drawMode);*/

		////draw
		{
			//Draw sphere
			glm::mat4 transform = glm::mat4(1);
			//planeTransform = glm::rotate(planeTransform, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			transform = glm::translate(transform, glm::vec3(0.0, 0.0, 0.0));
			sandShader.setMat4("model", transform);
			sphereMesh.draw(drawMode);

			//normals
			normalShader.Shader::use();
			normalShader.setMat4("projection", projection); 
			normalShader.setMat4("view", view);  
			normalShader.setMat4("model", transform); 
			sphereMesh.draw(drawMode);
		}


		//light cube
		lampShader.Shader::use();
		lampShader.setVec3("uLightColor", lightColor);
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightDirection);
		model = glm::scale(model, glm::vec3(0.2f));
		lampShader.setMat4("model", model);

		cubeMesh.draw(drawMode);

		//draw imgui
		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		//imgui window
		ImGui::Begin("Settings");
		ImGui::DragFloat3("Light Position", &lightDirection.x, 0.1f);
		ImGui::ColorEdit3("Light Color", &lightColor.r);
		ImGui::SliderFloat("Ambient K", &ambientK, 0.0f, 1.0f);
		ImGui::SliderFloat("Diffuse K", &diffuseK, 0.0f, 1.0f);
		ImGui::SliderFloat("Specular K", &specularK, 0.0f, 1.0f);
		ImGui::SliderInt("Shininess", &shininess, 2, 1024);
		ImGui::End();

		//render imgui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	printf("Shutting down...");
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	//speed
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		cam.keyboardInput(SPRINT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		cam.keyboardInput(WALK, deltaTime);
	}
	
	//direction
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cam.keyboardInput(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cam.keyboardInput(BACKWARD, deltaTime); 
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cam.keyboardInput(LEFT, deltaTime); 
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cam.keyboardInput(RIGHT, deltaTime); 
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		cam.keyboardInput(UP, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		cam.keyboardInput(DOWN, deltaTime);
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		float xOffset = xpos - lastX;
		float yOffset = ypos - lastY;
		lastX = xpos;
		lastY = ypos;

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		cam.mouseMoveInput(xOffset, yOffset);
	}
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstMouse = true;
	}
	
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	cam.mouseWheelInput(yOffset);
}
