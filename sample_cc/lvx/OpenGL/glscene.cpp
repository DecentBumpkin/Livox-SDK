#include <OpenGL/glscene.hpp>
#include <unistd.h> /*usleep*/

using namespace glm;

int GLScene::init(){

	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow( 1024, 768, "OpenGL GLScene", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
    glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE); // When sticky keys mode is enabled, the pollable state of a key will remain GLFW_PRESS
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); /* cursor view option*/

	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); // Accept fragment if it closer to the camera than the former one
	glEnable(GL_CULL_FACE); // Cull triangles which normal is not towards the camera

	// GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	programID = LoadShaders( "shader/SimpleVertexShader.vertexshader", "shader/SimpleFragmentShader.fragmentshader" );
	MatrixID = glGetUniformLocation(programID, "MVP");

	// Point point_cloud_buffer[] = {
	// 	{-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f},
	// 	{0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f} };
	// printf("show size %lu\n",sizeof(Point));

	livox_ros::LivoxPointXyzrtl* pc_buffer_ptr = new livox_ros::LivoxPointXyzrtl[96]; /* lds.h */
	livox_ros::LivoxPointXyzrtl pc_buffer[96];

	/* You're calling sizeof on sobel_x, and sobel_x is a pointer. 
	This means 8 is the sizeof a pointer on your system (presumably a 64bit machine)*/
	printf("sizeof: %lu %lu %lu %lu %lu\n",
		sizeof(pc_buffer_ptr), sizeof(pc_buffer), 
		sizeof(pc_buffer_ptr[0]), sizeof(pc_buffer[0]), 
		sizeof(livox_ros::LivoxPointXyzrtl));

	// GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pc_buffer), pc_buffer, GL_STREAM_DRAW);
	// glBufferSubData(GL_ARRAY_BUFFER, 0, 48 * sizeof(livox_ros::LivoxPointXyzrtl), pc_buffer + 48);
	
	delete[] pc_buffer_ptr;

	// 1rst attribute buffer : vertices
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(livox_ros::LivoxPointXyzrtl),(void*)0 );
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(livox_ros::LivoxPointXyzrtl),(void*)(3*sizeof(GL_FLOAT)) );
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glPointSize(3);

	printf("GL Initialized!\n");

}


void GLScene::computeMatricesFromInputs(){
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, GLScene::cursor_position_callback);
	static double lastTime = glfwGetTime();
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);
	lastTime = currentTime;

    ProjectionMatrix = glm::perspective<float>(glm::radians(camera_.get_zoom()), 4.0f/3.0f, 0.1f, 100.0f);
    ViewMatrix = camera_.get_view_matrix() * trackball_.get_rotation_matrix();
}

// static members
void GLScene::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if(button == GLFW_MOUSE_BUTTON_RIGHT){
		if(action == GLFW_PRESS){
			printf("moving camera...\n");
			camera_.process_mouse_right_click(xpos_, ypos_);
		}
		else if(action == GLFW_RELEASE){
			printf("release camera...\n");
			camera_.process_mouse_right_release(xpos_, ypos_);
		}
	}
	if(button == GLFW_MOUSE_BUTTON_LEFT){
		if(action == GLFW_PRESS){
			printf("moving trackball...\n");
			trackball_.process_mouse_left_click(xpos_, ypos_);
		}
		else if(action == GLFW_RELEASE){
			printf("release trackball...\n");
			trackball_.process_mouse_left_release(xpos_, ypos_);
		}
	}
}
void GLScene::scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
	// printf("Mouse wheel x: %5.2lf, y: %5.2lf\n", xoffset, yoffset);
	// Point newPoint[] = {
	// 	{-1.0f, 4.0f, 0.0f, 1.0f, 1.0f, 1.0f},
	// 	{0.0f, 4.0f, 0.0f, 1.0f, 1.0f, 1.0f},
	// 	{1.0f, 4.0f, 0.0f, 1.0f, 1.0f, 1.0f}
	// };
	// glBufferSubData(GL_ARRAY_BUFFER, 6*sizeof(Point), 3*sizeof(Point), newPoint);
	camera_.process_mouse_scroll(yoffset);
}
void GLScene::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	// printf("Cursor pos  x: %5.2lf, y: %5.2lf\n", xpos, ypos);
	camera_.process_mouse_right_movement(xpos, ypos);
	trackball_.process_mouse_left_movement(xpos, ypos);
	xpos_ = xpos;
	ypos_ = ypos;
}
void GLScene::updateScreen(){

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);
// Check if the ESC key was pressed or the window was closed

	glPointSize(3);
    glBindVertexArray(VertexArrayID); /* in tutorial */

	// // Use our shader
	glUseProgram(programID);

	glfwPollEvents();
	// Compute the MVP matrix from keyboard and mouse input
	computeMatricesFromInputs();
	glm::mat4 ProjectionMatrix = getProjectionMatrix();
	glm::mat4 ViewMatrix = getViewMatrix();
	glm::mat4 ModelMatrix = glm::mat4(1.0);
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	
	glDrawArrays(GL_POINTS, 0, 10);

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();

}

void GLScene::updatePointCloud(livox_ros::LivoxPointXyzrtl* dst_point){
	// printf("update point!\n");
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 96 * sizeof(livox_ros::LivoxPointXyzrtl), dst_point);

}

GLScene::~GLScene(){
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);// Check if the ESC key was pressed or the window was closed
	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);
	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

TrackBall GLScene::trackball_ = TrackBall(1024,768);
Camera GLScene::camera_;
double GLScene::xpos_;
double GLScene::ypos_;