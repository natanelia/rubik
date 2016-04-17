#define _USE_MATH_DEFINES

// Include standard headers
#include <cstdio>
#include <cstdlib>
#include <cmath>


// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

using namespace glm;

#include <common/shader.hpp>
#include "cube.hpp"

#define NO_ROTATE 0
#define ROTATE_TOP 1
#define ROTATE_RIGHT 2
#define ROTATE_BOTTOM 3
#define ROTATE_LEFT 4



// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
static GLfloat vd[] = {
     1.0f, 1.0f, 1.0f, // Right
     1.0f, 1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,

     1.0f, 1.0f, 1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f,-1.0f, 1.0f,
     
    -1.0f,-1.0f,-1.0f, // Left
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
     
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
     
    -1.0f, 1.0f,-1.0f, // Top
     1.0f, 1.0f,-1.0f,
     1.0f, 1.0f, 1.0f,
     
    -1.0f, 1.0f,-1.0f,
     1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
     
    -1.0f,-1.0f,-1.0f, // Bottom
    -1.0f,-1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     
    -1.0f,-1.0f,-1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f,-1.0f,-1.0f,

    -1.0f,-1.0f, 1.0f, // Back
     1.0f, 1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
     
     1.0f,-1.0f,-1.0f, // Top
     1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
     
    -1.0f,-1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f
};

// One color for each vertex. They were generated randomly.
static GLfloat cd[] = {
    1.0f,  0.0f,  0.0f, // Right
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,

    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,

    1.0f,  0.5f,  0.0f, // Left
    1.0f,  0.5f,  0.0f,
    1.0f,  0.5f,  0.0f,

    1.0f,  0.5f,  0.0f,
    1.0f,  0.5f,  0.0f,
    1.0f,  0.5f,  0.0f,

    1.0f,  1.0f,  0.0f, // Top
    1.0f,  1.0f,  0.0f,
    1.0f,  1.0f,  0.0f,

    1.0f,  1.0f,  0.0f,
    1.0f,  1.0f,  0.0f,
    1.0f,  1.0f,  0.0f,

    0.0f,  1.0f,  0.0f, // Bottom
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,

    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,

    0.0f,  0.25f, 1.0f, // Back
    0.0f,  0.25f, 1.0f,
    0.0f,  0.25f, 1.0f,

    0.0f,  0.25f, 1.0f,
    0.0f,  0.25f, 1.0f,
    0.0f,  0.25f, 1.0f,

    0.5f,  0.0f,  1.0f, // Front
    0.5f,  0.0f,  1.0f,
    0.5f,  0.0f,  1.0f,

    0.5f,  0.0f,  1.0f,
    0.5f,  0.0f,  1.0f,
    0.5f,  0.0f,  1.0f
};

// Projection matrix : 45deg Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
// Camera matrix
glm::mat4 ViewMatrix = glm::lookAt(glm::vec3(0,0,-12), glm::vec3(0,0,0), glm::vec3(0,-1,0));

int main( void )
{
    // Initialise GLFW
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

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "Rubrik", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
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

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );

    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    Cube cubes[3][3][3];
    cubes[0][0][0] = Cube("111", vd, cd);
    cubes[0][0][1] = Cube("112", vd, cd);
    cubes[0][0][2] = Cube("113", vd, cd);

    cubes[0][1][0] = Cube("121", vd, cd);
    cubes[0][1][1] = Cube("122", vd, cd);
    cubes[0][1][2] = Cube("123", vd, cd);

    cubes[0][2][0] = Cube("131", vd, cd);
    cubes[0][2][1] = Cube("132", vd, cd);
    cubes[0][2][2] = Cube("133", vd, cd);

    cubes[1][0][0] = Cube("211", vd, cd);
    cubes[1][0][1] = Cube("212", vd, cd);
    cubes[1][0][2] = Cube("213", vd, cd);

    cubes[1][1][0] = Cube("221", vd, cd);
    // cubes[1][1][1] = Cube("222", vd, cd);
    cubes[1][1][2] = Cube("223", vd, cd);

    cubes[1][2][0] = Cube("231", vd, cd);
    cubes[1][2][1] = Cube("232", vd, cd);
    cubes[1][2][2] = Cube("233", vd, cd);

    cubes[2][0][0] = Cube("311", vd, cd);
    cubes[2][0][1] = Cube("312", vd, cd);
    cubes[2][0][2] = Cube("313", vd, cd);

    cubes[2][1][0] = Cube("321", vd, cd);
    cubes[2][1][1] = Cube("322", vd, cd);
    cubes[2][1][2] = Cube("323", vd, cd);

    cubes[2][2][0] = Cube("331", vd, cd);
    cubes[2][2][1] = Cube("332", vd, cd);
    cubes[2][2][2] = Cube("333", vd, cd);


    int nbFrames = 0;
    int doAnimate = NO_ROTATE;
    bool keyLock = false;

    double lastTime = glfwGetTime();
    double startTime = glfwGetTime();
    double lastFrameTime = lastTime;

    double currentTime;
    do {
        currentTime = glfwGetTime();
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !keyLock) {
            printf("KEY D PRESSED\n");
            doAnimate = ROTATE_RIGHT;

            startTime = currentTime;
            lastFrameTime = currentTime;
            keyLock = true;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !keyLock) {
            printf("KEY A PRESSED\n");
            doAnimate = ROTATE_LEFT;

            startTime = currentTime;
            lastFrameTime = currentTime;
            keyLock = true;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !keyLock) {
            printf("KEY W PRESSED\n");
            doAnimate = ROTATE_TOP;

            startTime = currentTime;
            lastFrameTime = currentTime;
            keyLock = true;
        }

        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && !keyLock) {
            printf("KEY X PRESSED\n");
            doAnimate = ROTATE_BOTTOM;

            startTime = currentTime;
            lastFrameTime = currentTime;
            keyLock = true;
        }


        float deltaTime = (float)(currentTime - lastFrameTime);
        lastFrameTime = currentTime;
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
            // printf and reset
            printf("%f ms/frame\n", 1000.0/double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }

        if (currentTime - startTime <= 1.0131f && doAnimate != NO_ROTATE) {
            glm::vec3 __rotation;
            switch (doAnimate) {
                case ROTATE_RIGHT:
                    __rotation = glm::vec3(0.0f, -M_PI * deltaTime, 0.0f);
                    break;
                case ROTATE_LEFT:
                    __rotation = glm::vec3(0.0f, M_PI * deltaTime, 0.0f);
                    break;
                case ROTATE_TOP:
                    __rotation = glm::vec3(-M_PI * deltaTime, 0.0f, 0.0f);
                    break;
                case ROTATE_BOTTOM:
                    __rotation = glm::vec3(M_PI * deltaTime, 0.0f, 0.0f);
                    break;
            }

            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 1; j++) {
                    for (int k = 0; k < 3; k++) {
                       cubes[i][j][k].rotate(__rotation);
                    }
                }
            }
        } else {
            keyLock = false;
            doAnimate = NO_ROTATE;
        }

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                   cubes[i][j][k].draw(MatrixID, ProjectionMatrix, ViewMatrix);
                }
            }
        }

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        fflush(stdout);
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO and shader
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}


