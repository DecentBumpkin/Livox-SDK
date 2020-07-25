// Include standard headers
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h> // Include GLEW
#include <GLFW/glfw3.h> // Include GLFW
#include <glm/glm.hpp> // Include GLM
#include <glm/gtc/matrix_transform.hpp>

#include <OpenGL/shader.hpp>
#include <OpenGL/camera.h>
#include <OpenGL/trackball.h>

#include "lds.h" /*livox_ros::LivoxPointXyzrtl*/

using namespace glm;

typedef struct
{
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
} Point;

class GLScene 
{
public:
	GLScene(){}
	~GLScene();
	int init();
	void updateScreen();
	void updatePointCloud(livox_ros::LivoxPointXyzrtl* dst_point);
	void computeMatricesFromInputs();
	void computeMatricesFromInputsPro();
	inline glm::mat4 getViewMatrix() { return ViewMatrix; }
	inline glm::mat4 getProjectionMatrix() { return ProjectionMatrix; }
private:
	// unsigned int VertexArrayID;
	// unsigned int vertexbuffer;
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
	GLFWwindow* window;
	static TrackBall	trackball_;
	static Camera		camera_;
	static double xpos_, ypos_;
	// static members
	
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

	GLuint VertexArrayID;
	GLuint vertexbuffer;
	GLuint programID;
	GLuint MatrixID;

};