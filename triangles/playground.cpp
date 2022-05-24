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

	window = glfwCreateWindow( 1024, 768, "two triangles", NULL, NULL);
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

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// end of init

	// triangle coordinates --------------------------------------------------------

	GLuint triag_buffer1;
	GLuint triag_buffer2;

	static const GLfloat triag1[] = {
            -0.5f, 0.0f, 0.0f,
            2.0f, -1.0f, -0.7f,
            2.0f,  1.0f, 0.7f,
    };

    static const GLfloat triag2[] = {
            0.5f, 0.0f, 0.0f,
            -2.0f, 0.7f, -1.0f,
            -2.0f,  -0.7f, 1.0f,
    };

	glGenBuffers(1, &triag_buffer1);
	glBindBuffer(GL_ARRAY_BUFFER, triag_buffer1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triag1), triag1, GL_STATIC_DRAW);

    glGenBuffers(1, &triag_buffer2);
    glBindBuffer(GL_ARRAY_BUFFER, triag_buffer2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triag2), triag2, GL_STATIC_DRAW);

    // shaders init ----------------------------------------------------------------

	GLuint programID1 = LoadShaders( "SimpleTransform.vertexshader", "SingleColor1.fragmentshader" );
    GLuint programID2 = LoadShaders( "SimpleTransform.vertexshader", "SingleColor2.fragmentshader" );

	// transformation matrixes for triangles
	GLuint MatrixID1 = glGetUniformLocation(programID1, "MVP");
    GLuint MatrixID2 = glGetUniformLocation(programID2, "MVP");

	glm::mat4 Projection = glm::perspective(glm::radians(70.0f), 4.0f / 3.0f, 0.1f, 100.0f);

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);

    double angle = 0;
    do{

		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT );
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Camera matrix
        glm::mat4 View = glm::lookAt(
                glm::vec3(4 * (1 - cos(angle)) * cos(angle),  3 * cos(angle) + 2 * sin(angle), 4 * (1 - cos(angle)) * sin(angle)),
                glm::vec3(1, 0, 1),
                glm::vec3( 0, 1, 0)
        );
        glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

        // print triangle 1
		glUseProgram(programID1);
		glUniformMatrix4fv(MatrixID1, 1, GL_FALSE, &MVP[0][0]);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, triag_buffer1);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle
		glDisableVertexAttribArray(0);

        // print triangle 2
        glUseProgram(programID2);
        glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, triag_buffer2);
        glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,
                (void*)0
        );
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

        angle += 0.0001;
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteProgram(programID1);
    glDeleteProgram(programID2);
	glDeleteBuffers(1, &triag_buffer1);
    glDeleteBuffers(1, &triag_buffer2);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
