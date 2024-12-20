/*
	Author: Annabelle Thompson
*/

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
#include "Terrain/terrain.h"

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera cam(glm::vec3(0.0f, 0.0f, 20.0f));
bool firstMouse = true;
float lastX = 400, lastY = 300;

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720; 

glm::vec3 lightDirection(-0.3f, -1.0f, 2.0f);
float ambientK = 0.6f;
float diffuseK = 0.2f;
float oceanSpecularK = 0.8f;
float oceanShininess = 32;
float grainSpecularK = 0.8f;
float grainShininess = 850;
float sandStrength = 0.4f;
float grainSize = 5.0f;
float rimStrength = 0.2f;
float rimPower = 10.0f;
float rippleStrength = 5.0f;

glm::vec3 dayLightColor(0.898f, 0.863f, 0.757f);
glm::vec3 dayLitColor = glm::vec3(0.925, 0.796, 0.718);
glm::vec3 dayShadeColor = glm::vec3(0.851, 0.631, 0.565);
glm::vec3 daySpecularColor = glm::vec3(0.990, 0.721, 0.451);
glm::vec3 nightLightColor(0.858f, 0.936f, 0.925f);
glm::vec3 nightLitColor = glm::vec3(0.0, 0.098, 0.094);
glm::vec3 nightShadeColor = glm::vec3(0.0, 0.075, 0.067);
glm::vec3 nightSpecularColor = glm::vec3(0.058, 0.917, 0.891);

glm::vec3 lightColor(0.898f, 0.863f, 0.757f);
glm::vec3 litColor = glm::vec3(0.925, 0.796, 0.718);
glm::vec3 shadeColor = glm::vec3(0.851, 0.631, 0.565);
glm::vec3 specularColor = glm::vec3(0.990, 0.721, 0.451);

float x = 0.0;
float y = 0.0;
float z = 0.0;

bool day = false;
bool night = false;
bool tangent = false;


void processInput(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

bool wireFrame = false;
bool pointRender = false;

//terrain config
int width = 36;
int height = 36;
int complexity = 72;
const int size = 10;


int main() {
	printf("Initializing...\n");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sand", NULL, NULL);
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
	Shader tangentShader("assets/shaderAssets/tangentVisualization.vert", "assets/shaderAssets/tangentVisualization.frag", "assets/shaderAssets/tangentVisualization.geom");
	Shader lampShader("assets/shaderAssets/lampVShader.vert", "assets/shaderAssets/lampFShader.frag");

	//-----------------------------------------------------------------------------------------------

	ew::MeshData terrainMeshData;
	ew::MeshData planeMeshData;
	ew::MeshData sphereMeshData;
	ew::MeshData cubeMeshData;
	
	ew::createPlaneXY(6.0f, 6.0f, 4.0f, &planeMeshData);
	ew::createCube(6.0f, &cubeMeshData);
	ew::createSphere(2.0f, 32, &sphereMeshData);
	ew::Mesh terrainMesh[size*2];
	for (int i = 0; i < size * 2; i++) {
		int type = (rand() * i * size * complexity * rand() * width * height) % 5;
		ew::createTerrain(width, height, complexity, &terrainMeshData, type);
		terrainMesh[i] = ew::Mesh(terrainMeshData);
	}

	ew::Mesh planeMesh = ew::Mesh(planeMeshData);
	ew::Mesh cubeMesh = ew::Mesh(cubeMeshData);
	ew::Mesh sphereMesh = ew::Mesh(sphereMeshData);

	Texture2D grainNormals("assets/NormalMaps/grain.jpg", GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, GL_REPEAT, GL_REPEAT, GL_RGB);
	Texture2D shallowRipplesX("assets/NormalMaps/sandShallowX2.jpg", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, GL_RGB);
	Texture2D steepRipplesX("assets/NormalMaps/sandSteepX2.jpg", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, GL_RGB);
	Texture2D shallowRipplesZ("assets/NormalMaps/sandShallowZ2.jpg", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, GL_RGB);
	Texture2D steepRipplesZ("assets/NormalMaps/sandSteepZ.jpg", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, GL_RGB);

	float rotationTime = 0;
	//Render loop
	while (!glfwWindowShouldClose(window)) {
		//update time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//input
		processInput(window);

		//Clear framebuffer
		glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

		ew::DrawMode drawMode = pointRender ? ew::DrawMode::POINTS : ew::DrawMode::TRIANGLES;

		//use shader
		sandShader.Shader::use();

		//day or night
		if (day)
		{
			lightColor = dayLightColor;
			litColor = dayLitColor;
			shadeColor = dayShadeColor;
			specularColor = daySpecularColor;
		}
		else if (night)
		{
			lightColor = nightLightColor;
			litColor = nightLitColor;
			shadeColor = nightShadeColor;
			specularColor = nightSpecularColor;
		}

		//update uniforms
		sandShader.setVec3("uLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		sandShader.setVec3("uLightDirection", lightDirection);
		sandShader.setVec3("uViewPos", cam.getPos());
		sandShader.setVec3("uLightColor", lightColor);
		sandShader.setVec3("uColorSun", litColor);
		sandShader.setVec3("uColorShade", shadeColor);
		sandShader.setVec3("uSpecColor", specularColor);
		sandShader.setFloat("uAmbientK", ambientK);
		sandShader.setFloat("uDiffuseK", diffuseK);
		sandShader.setFloat("uOceanSpecularK", oceanSpecularK);
		sandShader.setFloat ("uOceanShininess", oceanShininess);
		sandShader.setFloat("uGrainSpecularK", grainSpecularK);
		sandShader.setFloat("uGrainShininess", grainShininess);
		sandShader.setFloat("uGrainSize", grainSize);
		sandShader.setFloat("uRimStrength", rimStrength);
		sandShader.setFloat("uRimPower", rimPower);
		sandShader.setFloat("uSteepnessStrength", rippleStrength);
		sandShader.setInt("uNormalMap", 0);
		sandShader.setInt("uShallowX", 1);
		sandShader.setInt("uSteepX", 2);
		sandShader.setInt("uShallowZ", 3);
		sandShader.setInt("uSteepZ", 4);

		grainNormals.Texture2D::bind(0);
		shallowRipplesX.Texture2D::bind(1);
		steepRipplesX.Texture2D::bind(2);
		shallowRipplesZ.Texture2D::bind(3);
		steepRipplesZ.Texture2D::bind(4);

		//camera view
		glm::mat4 projection = glm::perspective(glm::radians(cam.mZoom), 800.0f / 600.0f, 0.1f, 1000.0f);
		sandShader.setMat4("projection", projection);

		glm::mat4 view = cam.getViewMatrix();
		sandShader.setMat4("view", view);


		/*
			Author: Willam Bishop
		*/
		//Draw terrain
		glm::mat4 planeTransform = glm::mat4(1);
		float spacing = (complexity / 2) - (complexity / 10);
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size;j++) {
				planeTransform = glm::translate(planeTransform, glm::vec3((i * spacing), 0.0, (j * spacing)));
				sandShader.setMat4("model", planeTransform);
				terrainMesh[i+j].draw(drawMode);
				if (tangent)
				{
					normalShader.Shader::use();
					normalShader.setMat4("projection", projection);
					normalShader.setMat4("view", view);
					normalShader.setMat4("model", planeTransform);
					terrainMesh[i + j].draw(drawMode);

					tangentShader.Shader::use();
					tangentShader.setMat4("projection", projection);
					tangentShader.setMat4("view", view);
					tangentShader.setMat4("model", planeTransform);
					terrainMesh[i + j].draw(drawMode);
				}
				planeTransform = glm::translate(planeTransform, glm::vec3(-(i * spacing), 0.0, -(j * spacing) ));
				sandShader.Shader::use();
			}
		}
		/*
			End Author: Willam Bishop
		*/
		
		//draw plane
		sandShader.Shader::use();
		planeTransform = glm::mat4(1);
		planeTransform = glm::rotate(planeTransform, glm::radians(x), glm::vec3(1.0f, 0.0f, 0.0f));
		planeTransform = glm::rotate(planeTransform, glm::radians(y), glm::vec3(0.0f, 1.0f, 0.0f));
		planeTransform = glm::rotate(planeTransform, glm::radians(z), glm::vec3(0.0f, 1.0f, 1.0f));
		planeTransform = glm::translate(planeTransform, glm::vec3(-5.0, -5.0, 0.0));
		sandShader.setMat4("model", planeTransform);
		planeMesh.draw(drawMode);

		if (tangent)
		{
			normalShader.Shader::use();
			normalShader.setMat4("projection", projection);
			normalShader.setMat4("view", view);
			normalShader.setMat4("model", planeTransform);
			planeMesh.draw(drawMode);

			tangentShader.Shader::use();
			tangentShader.setMat4("projection", projection);
			tangentShader.setMat4("view", view);
			tangentShader.setMat4("model", planeTransform);
			planeMesh.draw(drawMode);
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
		ImGui::ColorEdit3("Spec Color", &specularColor.r);
		ImGui::SliderFloat("Ambient K", &ambientK, 0.0f, 1.0f);
		ImGui::SliderFloat("Diffuse K", &diffuseK, 0.0f, 1.0f);
		ImGui::SliderFloat("Ocean Specular K", &oceanSpecularK, 0.0f, 1.0f);
		ImGui::SliderFloat("Ocean Shininess", &oceanShininess, 2, 1024);
		ImGui::SliderFloat("Grain Specular K", &grainSpecularK, 0.0f, 1.0f);
		ImGui::SliderFloat("Grain Shininess", &grainShininess, 2, 1024);
		ImGui::SliderFloat("Grain Size", &grainSize, 1.0f, 10.0f);
		ImGui::SliderFloat("X", &x, -90.0f, 90.0f);
		ImGui::SliderFloat("Y", &y, -90.0f, 90.0f);
		ImGui::SliderFloat("Z", &z, -90.0f, 90.0f);
		ImGui::Checkbox("Day", &day);
		ImGui::Checkbox("Night", &night);
		ImGui::Checkbox("Tangent Space", &tangent);
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
