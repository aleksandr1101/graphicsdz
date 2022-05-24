// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <unistd.h>

int main( void )
{
	// init things
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow( 1024, 768, "figure", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// end of init

	// figure coordinates & colors--------------------------------------------------------

	GLuint figure_biffer;
	GLuint colors_buffer;

	static const GLfloat figure[] = {
	    -1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
	    1.5f, 0.0f, -0.5f,

	    -1.0f, 0.0f, 1.0f,
	    1.5f, 0.0f, -0.5f,
        0.0f, 0.0f, -1.5f,

	    -1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, -1.5f,
	    -1.5f, 0.0f, -0.5f,

	    -1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
	    0.0f, 1.2f, 0.0f,

        1.0f, 0.0f, 1.0f,
	    1.5f, 0.0f, -0.5f,
	    0.0f, 1.2f, 0.0f,

	    1.5f, 0.0f, -0.5f,
	    0.0f, 0.0f, -1.5f,
	    0.0f, 1.2f, 0.0f,

	    0.0f, 0.0f, -1.5f,
	    -1.5f, 0.0f, -0.5f,
	    0.0f, 1.2f, 0.0f,

	    -1.5f, 0.0f, -0.5f,
	    -1.0f, 0.0f, 1.0f,
	    0.0f, 1.2f, 0.0f,
	};

	// One color for each vertex. They were generated randomly.
	static const GLfloat colors[] = {
		0.945f,  0.719f,  0.592f,
		0.945f,  0.719f,  0.592f,
		0.945f,  0.719f,  0.592f,
		0.945f,  0.719f,  0.592f,
		0.945f,  0.719f,  0.592f,
		0.945f,  0.719f,  0.592f,
		0.945f,  0.719f,  0.592f,
		0.945f,  0.719f,  0.592f,
		0.945f,  0.719f,  0.592f,
		0.543f,  0.021f,  0.978f,
		0.279f,  0.317f,  0.505f,
		0.167f,  0.620f,  0.077f,
		0.347f,  0.857f,  0.137f,
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f,
		0.822f,  0.569f,  0.201f,
		0.435f,  0.602f,  0.223f,
		0.310f,  0.747f,  0.185f,
		0.597f,  0.770f,  0.761f,
		0.559f,  0.436f,  0.730f,
		0.359f,  0.583f,  0.152f,
		0.483f,  0.596f,  0.789f,
	};

	glGenBuffers(1, &figure_biffer);
	glBindBuffer(GL_ARRAY_BUFFER, figure_biffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(figure), figure, GL_STATIC_DRAW);

    glGenBuffers(1, &colors_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, colors_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    // shaders init ----------------------------------------------------------------

	GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );

	// transformation matrixes for triangles
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	glm::mat4 Projection = glm::perspective(glm::radians(70.0f), 4.0f / 3.0f, 0.1f, 100.0f);

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);

    double angle = 0;
    do{

		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Camera matrix
        glm::mat4 View = glm::lookAt(
                glm::vec3(4 * (1 - cos(angle)) * cos(angle),  3 * cos(angle) + 2 * sin(angle), 4 * (1 - cos(angle)) * sin(angle)),
                glm::vec3(1, 0, 1),
                glm::vec3( 0, 1, 0)
        );
        glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUseProgram(programID);

        // vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, figure_biffer);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

        // colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colors_buffer);
        glVertexAttribPointer(
                1,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,
                (void*)0
        );


        glDrawArrays(GL_TRIANGLES, 0, 3 * 8);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

        angle += 0.0001;
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteProgram(programID);
	glDeleteBuffers(1, &colors_buffer);
    glDeleteBuffers(1, &figure_biffer);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
