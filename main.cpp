#define _USE_MATH_DEFINES

// Include standard headers
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>


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
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include "cube.hpp"

#define NO_ROTATE 0

#define ROTATE_LEFT_TO_TOP 1
#define ROTATE_LEFT_TO_BOTTOM 2
#define ROTATE_CENTER_TO_TOP 3
#define ROTATE_CENTER_TO_BOTTOM 4
#define ROTATE_RIGHT_TO_TOP 5
#define ROTATE_RIGHT_TO_BOTTOM 6

#define ROTATE_TOP_TO_LEFT 7
#define ROTATE_TOP_TO_RIGHT 8
#define ROTATE_MIDDLE_TO_LEFT 9
#define ROTATE_MIDDLE_TO_RIGHT 10
#define ROTATE_BOTTOM_TO_LEFT 11
#define ROTATE_BOTTOM_TO_RIGHT 12

// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices

/*
 Represents a point light
 */
struct Light {
    glm::vec3 position;
    glm::vec3 intensities; //a.k.a. the color of the light
};

Light gLight;

static GLfloat ambientLight;

static GLfloat vd[] = {
     1.0f, 1.0f, 1.0f, // Right
     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f,-1.0f,

     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,

    -1.0f,-1.0f,-1.0f, // Left
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,

    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,

    -1.0f, 1.0f,-1.0f, // Top
     1.0f, 1.0f, 1.0f,
     1.0f, 1.0f,-1.0f,

    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,

    -1.0f,-1.0f,-1.0f, // Bottom
     1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,

     1.0f,-1.0f, 1.0f,
     1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,

    -1.0f,-1.0f, 1.0f, // Back
     1.0f, 1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,

    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,

     1.0f,-1.0f,-1.0f, // Front
     1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,

    -1.0f,-1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f
};

// One color for each vertex. They were generated randomly.
// Right - Left - Top - Bottom - Back - Front (each got 6 rows)
static GLfloat gd[] = {
    0.668104f, 1.0f-0.000013f, //RIGHT 1
    0.335973f, 1.0f-0.335903f,
    0.667979f, 1.0f-0.335851f,

    0.335973f, 1.0f-0.335903f, //RIGHT 2
    0.668104f, 1.0f-0.000013f,
    0.336098f, 1.0f-0.000071f,

    0.000059f, 1.0f-0.000004f, //LEFT 1
    0.000103f, 1.0f-0.336048f,
    0.335973f, 1.0f-0.335903f,

    0.000059f, 1.0f-0.000004f, //LEFT 2
    0.335973f, 1.0f-0.335903f,
    0.336098f, 1.0f-0.000071f,

    0.000103f, 1.0f-0.336048f, //TOP 2
    0.336024f, 1.0f-0.671877f,
    0.335973f, 1.0f-0.335903f,

    0.000103f, 1.0f-0.336048f, //TOP 1
    0.000004f, 1.0f-0.671870f,
    0.336024f, 1.0f-0.671877f,

    0.667979f, 1.0f-0.335851f, //BOTTOM 1
    0.336024f, 1.0f-0.671877f,
    0.667969f, 1.0f-0.671889f,

    0.667979f, 1.0f-0.335851f, //BOTTOM 2
    0.335973f, 1.0f-0.335903f,
    0.336024f, 1.0f-0.671877f,

    1.000023f, 1.0f-0.000013f, //BACK 1
    0.667979f, 1.0f-0.335851f,
    0.999958f, 1.0f-0.336064f,

    1.000023f, 1.0f-0.000013f, //BACK 2
    0.668104f, 1.0f-0.000013f,
    0.667979f, 1.0f-0.335851f,

    1.000004f, 1.0f-0.671847f, //FRONT 1
    0.999958f, 1.0f-0.336064f,
    0.667979f, 1.0f-0.335851f,

    0.667969f, 1.0f-0.671889f, //FRONT 2
    1.000004f, 1.0f-0.671847f,
    0.667979f, 1.0f-0.335851f
};

static GLfloat nd[] = {
    1.0f, 0.0f, 0.0f, // Right
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,

    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,

   -1.0f, 0.0f, 0.0f, // Left
   -1.0f, 0.0f, 0.0f,
   -1.0f, 0.0f, 0.0f,

   -1.0f, 0.0f, 0.0f,
   -1.0f, 0.0f, 0.0f,
   -1.0f, 0.0f, 0.0f,

    0.0f, 1.0f, 0.0f, // Top
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,

    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,

    0.0f,-1.0f, 0.0f, // Bottom
    0.0f,-1.0f, 0.0f,
    0.0f,-1.0f, 0.0f,

    0.0f,-1.0f, 0.0f,
    0.0f,-1.0f, 0.0f,
    0.0f,-1.0f, 0.0f,

    0.0f, 0.0f, 1.0f, // Back
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,

    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,

    0.0f, 0.0f,-1.0f, // Front
    0.0f, 0.0f,-1.0f,
    0.0f, 0.0f,-1.0f,

    0.0f, 0.0f,-1.0f,
    0.0f, 0.0f,-1.0f,
    0.0f, 0.0f,-1.0f
};

int main( void )
{

    // Projection matrix : 45deg Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    glm::mat4 ViewMatrix = glm::lookAt(glm::vec3(8,8,12), glm::vec3(0,0,0), glm::vec3(0,1,0));

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
    window = glfwCreateWindow( 1024, 768, "Rubik", NULL, NULL);
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
    glClearColor(0.5f, 0.55f, 1.0f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );



//    GLuint Texture = loadDDS("uvtemplate.DDS");

//    // Get a handle for our "myTextureSampler" uniform
//    GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");


    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    int cubesCode[3][3][3];
    cubesCode[0][0][0] = 111;
    cubesCode[0][0][1] = 112;
    cubesCode[0][0][2] = 113;

    cubesCode[0][1][0] = 121;
    cubesCode[0][1][1] = 122;
    cubesCode[0][1][2] = 123;

    cubesCode[0][2][0] = 131;
    cubesCode[0][2][1] = 132;
    cubesCode[0][2][2] = 133;

    cubesCode[1][0][0] = 211;
    cubesCode[1][0][1] = 212;
    cubesCode[1][0][2] = 213;

    cubesCode[1][1][0] = 221;
    // cubesCode[1][1][1] = 222;
    cubesCode[1][1][2] = 223;

    cubesCode[1][2][0] = 231;
    cubesCode[1][2][1] = 232;
    cubesCode[1][2][2] = 233;

    cubesCode[2][0][0] = 311;
    cubesCode[2][0][1] = 312;
    cubesCode[2][0][2] = 313;

    cubesCode[2][1][0] = 321;
    cubesCode[2][1][1] = 322;
    cubesCode[2][1][2] = 323;

    cubesCode[2][2][0] = 331;
    cubesCode[2][2][1] = 332;
    cubesCode[2][2][2] = 333;

    Cube cubes[3][3][3];
    cubes[0][0][0] = Cube("111", vd, gd, nd);
    cubes[0][0][1] = Cube("112", vd, gd, nd);
    cubes[0][0][2] = Cube("113", vd, gd, nd);

    cubes[0][1][0] = Cube("121", vd, gd, nd);
    cubes[0][1][1] = Cube("122", vd, gd, nd);
    cubes[0][1][2] = Cube("123", vd, gd, nd);

    cubes[0][2][0] = Cube("131", vd, gd, nd);
    cubes[0][2][1] = Cube("132", vd, gd, nd);
    cubes[0][2][2] = Cube("133", vd, gd, nd);

    cubes[1][0][0] = Cube("211", vd, gd, nd);
    cubes[1][0][1] = Cube("212", vd, gd, nd);
    cubes[1][0][2] = Cube("213", vd, gd, nd);

    cubes[1][1][0] = Cube("221", vd, gd, nd);
    // cubes[1][1][1] = Cube("222", vd, gd, nd);
    cubes[1][1][2] = Cube("223", vd, gd, nd);

    cubes[1][2][0] = Cube("231", vd, gd, nd);
    cubes[1][2][1] = Cube("232", vd, gd, nd);
    cubes[1][2][2] = Cube("233", vd, gd, nd);

    cubes[2][0][0] = Cube("311", vd, gd, nd);
    cubes[2][0][1] = Cube("312", vd, gd, nd);
    cubes[2][0][2] = Cube("313", vd, gd, nd);

    cubes[2][1][0] = Cube("321", vd, gd, nd);
    cubes[2][1][1] = Cube("322", vd, gd, nd);
    cubes[2][1][2] = Cube("323", vd, gd, nd);

    cubes[2][2][0] = Cube("331", vd, gd, nd);
    cubes[2][2][1] = Cube("332", vd, gd, nd);
    cubes[2][2][2] = Cube("333", vd, gd, nd);


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

        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && !keyLock) {
            printf("KEY Q PRESSED\n");
            doAnimate = ROTATE_TOP_TO_LEFT;

            startTime = currentTime;
            lastFrameTime = currentTime;
            keyLock = true;
        }

        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && !keyLock) {
            printf("KEY W PRESSED\n");
            doAnimate = ROTATE_TOP_TO_RIGHT;

            startTime = currentTime;
            lastFrameTime = currentTime;
            keyLock = true;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !keyLock) {
            printf("KEY A PRESSED\n");
            doAnimate = ROTATE_MIDDLE_TO_LEFT;

            startTime = currentTime;
            lastFrameTime = currentTime;
            keyLock = true;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !keyLock) {
            printf("KEY S PRESSED\n");
            doAnimate = ROTATE_MIDDLE_TO_RIGHT;

            startTime = currentTime;
            lastFrameTime = currentTime;
            keyLock = true;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !keyLock) {
            printf("KEY Z PRESSED\n");
            doAnimate = ROTATE_BOTTOM_TO_LEFT;

            startTime = currentTime;
            lastFrameTime = currentTime;
            keyLock = true;
        }

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && !keyLock) {
            printf("KEY X PRESSED\n");
            doAnimate = ROTATE_BOTTOM_TO_RIGHT;

            startTime = currentTime;
            lastFrameTime = currentTime;
            keyLock = true;
        }

        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && !keyLock) {
            printf("KEY U PRESSED\n");
            doAnimate = ROTATE_LEFT_TO_TOP;

            startTime = currentTime;
            lastFrameTime = currentTime;
            keyLock = true;
        }

        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && !keyLock) {
            printf("KEY J PRESSED\n");
            doAnimate = ROTATE_LEFT_TO_BOTTOM;

            startTime = currentTime;
            lastFrameTime = currentTime;
            keyLock = true;
        }

        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && !keyLock) {
            printf("KEY I PRESSED\n");
            doAnimate = ROTATE_CENTER_TO_TOP;

            startTime = currentTime;
            lastFrameTime = currentTime;
            keyLock = true;
        }

        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && !keyLock) {
            printf("KEY K PRESSED\n");
            doAnimate = ROTATE_CENTER_TO_BOTTOM;

            startTime = currentTime;
            lastFrameTime = currentTime;
            keyLock = true;
        }

        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && !keyLock) {
            printf("KEY O PRESSED\n");
            doAnimate = ROTATE_RIGHT_TO_TOP;

            startTime = currentTime;
            lastFrameTime = currentTime;
            keyLock = true;
        }

        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !keyLock) {
            printf("KEY L PRESSED\n");
            doAnimate = ROTATE_RIGHT_TO_BOTTOM;

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
                case ROTATE_LEFT_TO_TOP:
                    __rotation = glm::vec3(-M_PI * deltaTime, 0.0f, 0.0f); // TO_TOP

                    for (int i = 0; i < 1; i++) {
                        for (int j = 0; j < 3; j++) {
                            for (int k = 0; k < 3; k++) {
                                cubes[i][j][k].rotate(__rotation);
                            }
                        }
                    }

                    break;
                case ROTATE_LEFT_TO_BOTTOM:
                    __rotation = glm::vec3(M_PI * deltaTime, 0.0f, 0.0f); // TO_BOTTOM

                    for (int i = 0; i < 1; i++) {
                        for (int j = 0; j < 3; j++) {
                            for (int k = 0; k < 3; k++) {
                                cubes[i][j][k].rotate(__rotation);
                            }
                        }
                    }

                    break;
                case ROTATE_CENTER_TO_TOP:
                    __rotation = glm::vec3(-M_PI * deltaTime, 0.0f, 0.0f); // TO_TOP

                    for (int i = 1; i < 2; i++) {
                        for (int j = 0; j < 3; j++) {
                            for (int k = 0; k < 3; k++) {
                                cubes[i][j][k].rotate(__rotation);
                            }
                        }
                    }

                    break;
                case ROTATE_CENTER_TO_BOTTOM:
                    __rotation = glm::vec3(M_PI * deltaTime, 0.0f, 0.0f); // TO_BOTTOM

                    for (int i = 1; i < 2; i++) {
                        for (int j = 0; j < 3; j++) {
                            for (int k = 0; k < 3; k++) {
                                cubes[i][j][k].rotate(__rotation);
                            }
                        }
                    }

                    break;
                case ROTATE_RIGHT_TO_TOP:
                    __rotation = glm::vec3(-M_PI * deltaTime, 0.0f, 0.0f); // TO_TOP

                    for (int i = 2; i < 3; i++) {
                        for (int j = 0; j < 3; j++) {
                            for (int k = 0; k < 3; k++) {
                               cubes[i][j][k].rotate(__rotation);
                            }
                        }
                    }

                    break;
                case ROTATE_RIGHT_TO_BOTTOM:
                    __rotation = glm::vec3(M_PI * deltaTime, 0.0f, 0.0f); // TO_BOTTOM

                    for (int i = 2; i < 3; i++) {
                        for (int j = 0; j < 3; j++) {
                            for (int k = 0; k < 3; k++) {
                                cubes[i][j][k].rotate(__rotation);
                            }
                        }
                    }

                    break;
                case ROTATE_TOP_TO_LEFT:
                    __rotation = glm::vec3(0.0f, M_PI * deltaTime, 0.0f); // TO_LEFT

                    for (int i = 0; i < 3; i++) {
                        for (int j = 0; j < 1; j++) {
                            for (int k = 0; k < 3; k++) {
                                cubes[i][j][k].rotate(__rotation);
                            }
                        }
                    }

                    break;
                case ROTATE_TOP_TO_RIGHT:
                    __rotation = glm::vec3(0.0f, -M_PI * deltaTime, 0.0f); // TO_RIGHT

                    for (int i = 0; i < 3; i++) {
                        for (int j = 0; j < 1; j++) {
                            for (int k = 0; k < 3; k++) {
                                cubes[i][j][k].rotate(__rotation);
                            }
                        }
                    }

                    break;
                case ROTATE_MIDDLE_TO_LEFT:
                    __rotation = glm::vec3(0.0f, M_PI * deltaTime, 0.0f); // TO_LEFT

                    for (int i = 0; i < 3; i++) {
                        for (int j = 1; j < 2; j++) {
                            for (int k = 0; k < 3; k++) {
                                cubes[i][j][k].rotate(__rotation);
                            }
                        }
                    }

                    break;
                case ROTATE_MIDDLE_TO_RIGHT:
                    __rotation = glm::vec3(0.0f, -M_PI * deltaTime, 0.0f); // TO_RIGHT

                    for (int i = 0; i < 3; i++) {
                        for (int j = 1; j < 2; j++) {
                            for (int k = 0; k < 3; k++) {
                               cubes[i][j][k].rotate(__rotation);
                            }
                        }
                    }

                    break;
                case ROTATE_BOTTOM_TO_LEFT:
                    __rotation = glm::vec3(0.0f, M_PI * deltaTime, 0.0f); // TO_LEFT

                    for (int i = 0; i < 3; i++) {
                        for (int j = 2; j < 3; j++) {
                            for (int k = 0; k < 3; k++) {
                               cubes[i][j][k].rotate(__rotation);
                            }
                        }
                    }

                    break;
                case ROTATE_BOTTOM_TO_RIGHT:
                    __rotation = glm::vec3(0.0f, -M_PI * deltaTime, 0.0f); // TO_RIGHT

                    for (int i = 0; i < 3; i++) {
                        for (int j = 2; j < 3; j++) {
                            for (int k = 0; k < 3; k++) {
                                cubes[i][j][k].rotate(__rotation);
                            }
                        }
                    }

                    break;
            }
        } else {

            int temp_cubesCode[3][3];
            Cube temp_cube[3][3][3];
            switch (doAnimate) {
                case ROTATE_LEFT_TO_TOP:
                    for(int i = 0; i < 1; i++){
                        for(int j = 0; j < 3; j++){
                            for(int k = 0; k < 3; k++){
                                temp_cube[i][j][k] = Cube("111",vd, gd, nd);
                                temp_cube[i][j][k].setColorBack(cubes[i][2-k][j].getColorBack());
                                temp_cube[i][j][k].setColorFront(cubes[i][2-k][j].getColorFront());
                                temp_cube[i][j][k].setColorLeft(cubes[i][2-k][j].getColorLeft());
                                temp_cube[i][j][k].setColorRight(cubes[i][2-k][j].getColorRight());
                                temp_cube[i][j][k].setColorBottom(cubes[i][2-k][j].getColorBottom());
                                temp_cube[i][j][k].setColorTop(cubes[i][2-k][j].getColorTop());
                                temp_cube[i][j][k].rotateColor(ROTATE_BOTTOM);
                            }
                        }
                    }
                    for(int i = 0; i < 1; i++){
                        for(int j = 0; j < 3; j++){
                            for(int k = 0; k < 3; k++){
                                cubes[i][j][k] = Cube(std::to_string(i+1)+std::to_string(j+1)+std::to_string(k+1),vd, gd, nd);
                                cubes[i][j][k].setColorBack(temp_cube[i][j][k].getColorBack());
                                cubes[i][j][k].setColorFront(temp_cube[i][j][k].getColorFront());
                                cubes[i][j][k].setColorLeft(temp_cube[i][j][k].getColorLeft());
                                cubes[i][j][k].setColorRight(temp_cube[i][j][k].getColorRight());
                                cubes[i][j][k].setColorBottom(temp_cube[i][j][k].getColorBottom());
                                cubes[i][j][k].setColorTop(temp_cube[i][j][k].getColorTop());
                            }
                        }
                    }
                    break;

                case ROTATE_LEFT_TO_BOTTOM:
                    for(int i = 0; i < 1; i++){
                        for(int j = 0; j < 3; j++){
                            for(int k = 0; k < 3; k++){
                                temp_cube[i][j][k] = Cube("111",vd, gd, nd);
                                temp_cube[i][j][k].setColorBack(cubes[i][k][2-j].getColorBack());
                                temp_cube[i][j][k].setColorFront(cubes[i][k][2-j].getColorFront());
                                temp_cube[i][j][k].setColorLeft(cubes[i][k][2-j].getColorLeft());
                                temp_cube[i][j][k].setColorRight(cubes[i][k][2-j].getColorRight());
                                temp_cube[i][j][k].setColorBottom(cubes[i][k][2-j].getColorBottom());
                                temp_cube[i][j][k].setColorTop(cubes[i][k][2-j].getColorTop());
                                temp_cube[i][j][k].rotateColor(ROTATE_TOP);
                            }
                        }
                    }
                    for(int i = 0; i < 1; i++){
                        for(int j = 0; j < 3; j++){
                            for(int k = 0; k < 3; k++){
                                cubes[i][j][k] = Cube(std::to_string(i+1)+std::to_string(j+1)+std::to_string(k+1),vd, gd, nd);
                                cubes[i][j][k].setColorBack(temp_cube[i][j][k].getColorBack());
                                cubes[i][j][k].setColorFront(temp_cube[i][j][k].getColorFront());
                                cubes[i][j][k].setColorLeft(temp_cube[i][j][k].getColorLeft());
                                cubes[i][j][k].setColorRight(temp_cube[i][j][k].getColorRight());
                                cubes[i][j][k].setColorBottom(temp_cube[i][j][k].getColorBottom());
                                cubes[i][j][k].setColorTop(temp_cube[i][j][k].getColorTop());
                            }
                        }
                    }
                    break;

                case ROTATE_CENTER_TO_TOP:
                    for(int i = 1; i < 2; i++){
                        for(int j = 0; j < 3; j++){
                            for(int k = 0; k < 3; k++){
                                temp_cube[i][j][k] = Cube("111",vd, gd, nd);
                                temp_cube[i][j][k].setColorBack(cubes[i][2-k][j].getColorBack());
                                temp_cube[i][j][k].setColorFront(cubes[i][2-k][j].getColorFront());
                                temp_cube[i][j][k].setColorLeft(cubes[i][2-k][j].getColorLeft());
                                temp_cube[i][j][k].setColorRight(cubes[i][2-k][j].getColorRight());
                                temp_cube[i][j][k].setColorBottom(cubes[i][2-k][j].getColorBottom());
                                temp_cube[i][j][k].setColorTop(cubes[i][2-k][j].getColorTop());
                                temp_cube[i][j][k].rotateColor(ROTATE_BOTTOM);
                            }
                        }
                    }
                    for(int i = 1; i < 2; i++){
                        for(int j = 0; j < 3; j++){
                            for(int k = 0; k < 3; k++){
                                cubes[i][j][k] = Cube(std::to_string(i+1)+std::to_string(j+1)+std::to_string(k+1),vd, gd, nd);
                                cubes[i][j][k].setColorBack(temp_cube[i][j][k].getColorBack());
                                cubes[i][j][k].setColorFront(temp_cube[i][j][k].getColorFront());
                                cubes[i][j][k].setColorLeft(temp_cube[i][j][k].getColorLeft());
                                cubes[i][j][k].setColorRight(temp_cube[i][j][k].getColorRight());
                                cubes[i][j][k].setColorBottom(temp_cube[i][j][k].getColorBottom());
                                cubes[i][j][k].setColorTop(temp_cube[i][j][k].getColorTop());
                            }
                        }
                    }
                    break;

                case ROTATE_CENTER_TO_BOTTOM:
                    for(int i = 1; i < 2; i++){
                        for(int j = 0; j < 3; j++){
                            for(int k = 0; k < 3; k++){
                                temp_cube[i][j][k] = Cube("111",vd, gd, nd);
                                temp_cube[i][j][k].setColorBack(cubes[i][k][2-j].getColorBack());
                                temp_cube[i][j][k].setColorFront(cubes[i][k][2-j].getColorFront());
                                temp_cube[i][j][k].setColorLeft(cubes[i][k][2-j].getColorLeft());
                                temp_cube[i][j][k].setColorRight(cubes[i][k][2-j].getColorRight());
                                temp_cube[i][j][k].setColorBottom(cubes[i][k][2-j].getColorBottom());
                                temp_cube[i][j][k].setColorTop(cubes[i][k][2-j].getColorTop());
                                temp_cube[i][j][k].rotateColor(ROTATE_TOP);
                            }
                        }
                    }
                    for(int i = 1; i < 2; i++){
                        for(int j = 0; j < 3; j++){
                            for(int k = 0; k < 3; k++){
                                cubes[i][j][k] = Cube(std::to_string(i+1)+std::to_string(j+1)+std::to_string(k+1),vd, gd, nd);
                                cubes[i][j][k].setColorBack(temp_cube[i][j][k].getColorBack());
                                cubes[i][j][k].setColorFront(temp_cube[i][j][k].getColorFront());
                                cubes[i][j][k].setColorLeft(temp_cube[i][j][k].getColorLeft());
                                cubes[i][j][k].setColorRight(temp_cube[i][j][k].getColorRight());
                                cubes[i][j][k].setColorBottom(temp_cube[i][j][k].getColorBottom());
                                cubes[i][j][k].setColorTop(temp_cube[i][j][k].getColorTop());
                            }
                        }
                    }
                    break;

                case ROTATE_RIGHT_TO_TOP:
                    for(int i = 2; i < 3; i++){
                        for(int j = 0; j < 3; j++){
                            for(int k = 0; k < 3; k++){
                                temp_cube[i][j][k] = Cube("111",vd, gd, nd);
                                temp_cube[i][j][k].setColorBack(cubes[i][2-k][j].getColorBack());
                                temp_cube[i][j][k].setColorFront(cubes[i][2-k][j].getColorFront());
                                temp_cube[i][j][k].setColorLeft(cubes[i][2-k][j].getColorLeft());
                                temp_cube[i][j][k].setColorRight(cubes[i][2-k][j].getColorRight());
                                temp_cube[i][j][k].setColorBottom(cubes[i][2-k][j].getColorBottom());
                                temp_cube[i][j][k].setColorTop(cubes[i][2-k][j].getColorTop());
                                temp_cube[i][j][k].rotateColor(ROTATE_BOTTOM);
                            }
                        }
                    }
                    for(int i = 2; i < 3; i++){
                        for(int j = 0; j < 3; j++){
                            for(int k = 0; k < 3; k++){
                                cubes[i][j][k] = Cube(std::to_string(i+1)+std::to_string(j+1)+std::to_string(k+1),vd, gd, nd);
                                cubes[i][j][k].setColorBack(temp_cube[i][j][k].getColorBack());
                                cubes[i][j][k].setColorFront(temp_cube[i][j][k].getColorFront());
                                cubes[i][j][k].setColorLeft(temp_cube[i][j][k].getColorLeft());
                                cubes[i][j][k].setColorRight(temp_cube[i][j][k].getColorRight());
                                cubes[i][j][k].setColorBottom(temp_cube[i][j][k].getColorBottom());
                                cubes[i][j][k].setColorTop(temp_cube[i][j][k].getColorTop());
                            }
                        }
                    }
                    break;

                case ROTATE_RIGHT_TO_BOTTOM:
                    for(int i = 2; i < 3; i++){
                        for(int j = 0; j < 3; j++){
                            for(int k = 0; k < 3; k++){
                                temp_cube[i][j][k] = Cube("111",vd, gd, nd);
                                temp_cube[i][j][k].setColorBack(cubes[i][k][2-j].getColorBack());
                                temp_cube[i][j][k].setColorFront(cubes[i][k][2-j].getColorFront());
                                temp_cube[i][j][k].setColorLeft(cubes[i][k][2-j].getColorLeft());
                                temp_cube[i][j][k].setColorRight(cubes[i][k][2-j].getColorRight());
                                temp_cube[i][j][k].setColorBottom(cubes[i][k][2-j].getColorBottom());
                                temp_cube[i][j][k].setColorTop(cubes[i][k][2-j].getColorTop());
                                temp_cube[i][j][k].rotateColor(ROTATE_TOP);
                            }
                        }
                    }
                    for(int i = 2; i < 3; i++){
                        for(int j = 0; j < 3; j++){
                            for(int k = 0; k < 3; k++){
                                cubes[i][j][k] = Cube(std::to_string(i+1)+std::to_string(j+1)+std::to_string(k+1),vd, gd, nd);
                                cubes[i][j][k].setColorBack(temp_cube[i][j][k].getColorBack());
                                cubes[i][j][k].setColorFront(temp_cube[i][j][k].getColorFront());
                                cubes[i][j][k].setColorLeft(temp_cube[i][j][k].getColorLeft());
                                cubes[i][j][k].setColorRight(temp_cube[i][j][k].getColorRight());
                                cubes[i][j][k].setColorBottom(temp_cube[i][j][k].getColorBottom());
                                cubes[i][j][k].setColorTop(temp_cube[i][j][k].getColorTop());
                            }
                        }
                    }
                    break;

                case ROTATE_TOP_TO_LEFT:
                    for(int i = 0; i < 3; i++){
                        for(int j = 0; j < 1; j++){
                            for(int k = 0; k < 3; k++){
                                temp_cube[i][j][k] = Cube("111",vd, gd, nd);
                                temp_cube[i][j][k].setColorBack(cubes[2-k][j][i].getColorBack());
                                temp_cube[i][j][k].setColorFront(cubes[2-k][j][i].getColorFront());
                                temp_cube[i][j][k].setColorLeft(cubes[2-k][j][i].getColorLeft());
                                temp_cube[i][j][k].setColorRight(cubes[2-k][j][i].getColorRight());
                                temp_cube[i][j][k].setColorBottom(cubes[2-k][j][i].getColorBottom());
                                temp_cube[i][j][k].setColorTop(cubes[2-k][j][i].getColorTop());
                                temp_cube[i][j][k].rotateColor(ROTATE_LEFT);
                            }
                        }
                    }
                    for(int i = 0; i < 3; i++){
                        for(int j = 0; j < 1; j++){
                            for(int k = 0; k < 3; k++){
                                cubes[i][j][k] = Cube(std::to_string(i+1)+std::to_string(j+1)+std::to_string(k+1),vd, gd, nd);
                                cubes[i][j][k].setColorBack(temp_cube[i][j][k].getColorBack());
                                cubes[i][j][k].setColorFront(temp_cube[i][j][k].getColorFront());
                                cubes[i][j][k].setColorLeft(temp_cube[i][j][k].getColorLeft());
                                cubes[i][j][k].setColorRight(temp_cube[i][j][k].getColorRight());
                                cubes[i][j][k].setColorBottom(temp_cube[i][j][k].getColorBottom());
                                cubes[i][j][k].setColorTop(temp_cube[i][j][k].getColorTop());
                            }
                        }
                    }
                    break;

                case ROTATE_TOP_TO_RIGHT:
                    for(int i = 0; i < 3; i++){
                        for(int j = 0; j < 1; j++){
                            for(int k = 0; k < 3; k++){
                                temp_cube[i][j][k] = Cube("111",vd, gd, nd);
                                temp_cube[i][j][k].setColorBack(cubes[k][j][2-i].getColorBack());
                                temp_cube[i][j][k].setColorFront(cubes[k][j][2-i].getColorFront());
                                temp_cube[i][j][k].setColorLeft(cubes[k][j][2-i].getColorLeft());
                                temp_cube[i][j][k].setColorRight(cubes[k][j][2-i].getColorRight());
                                temp_cube[i][j][k].setColorBottom(cubes[k][j][2-i].getColorBottom());
                                temp_cube[i][j][k].setColorTop(cubes[k][j][2-i].getColorTop());
                                temp_cube[i][j][k].rotateColor(ROTATE_RIGHT);
                            }
                        }
                    }
                    for(int i = 0; i < 3; i++){
                        for(int j = 0; j < 1; j++){
                            for(int k = 0; k < 3; k++){
                                cubes[i][j][k] = Cube(std::to_string(i+1)+std::to_string(j+1)+std::to_string(k+1),vd, gd, nd);
                                cubes[i][j][k].setColorBack(temp_cube[i][j][k].getColorBack());
                                cubes[i][j][k].setColorFront(temp_cube[i][j][k].getColorFront());
                                cubes[i][j][k].setColorLeft(temp_cube[i][j][k].getColorLeft());
                                cubes[i][j][k].setColorRight(temp_cube[i][j][k].getColorRight());
                                cubes[i][j][k].setColorBottom(temp_cube[i][j][k].getColorBottom());
                                cubes[i][j][k].setColorTop(temp_cube[i][j][k].getColorTop());
                            }
                        }
                    }
                    break;

                case ROTATE_MIDDLE_TO_LEFT:
                    for(int i = 0; i < 3; i++){
                        for(int j = 1; j < 2; j++){
                            for(int k = 0; k < 3; k++){
                                temp_cube[i][j][k] = Cube("111",vd, gd, nd);
                                temp_cube[i][j][k].setColorBack(cubes[2-k][j][i].getColorBack());
                                temp_cube[i][j][k].setColorFront(cubes[2-k][j][i].getColorFront());
                                temp_cube[i][j][k].setColorLeft(cubes[2-k][j][i].getColorLeft());
                                temp_cube[i][j][k].setColorRight(cubes[2-k][j][i].getColorRight());
                                temp_cube[i][j][k].setColorBottom(cubes[2-k][j][i].getColorBottom());
                                temp_cube[i][j][k].setColorTop(cubes[2-k][j][i].getColorTop());
                                temp_cube[i][j][k].rotateColor(ROTATE_LEFT);
                            }
                        }
                    }
                    for(int i = 0; i < 3; i++){
                        for(int j = 1; j < 2; j++){
                            for(int k = 0; k < 3; k++){
                                cubes[i][j][k] = Cube(std::to_string(i+1)+std::to_string(j+1)+std::to_string(k+1),vd, gd, nd);
                                cubes[i][j][k].setColorBack(temp_cube[i][j][k].getColorBack());
                                cubes[i][j][k].setColorFront(temp_cube[i][j][k].getColorFront());
                                cubes[i][j][k].setColorLeft(temp_cube[i][j][k].getColorLeft());
                                cubes[i][j][k].setColorRight(temp_cube[i][j][k].getColorRight());
                                cubes[i][j][k].setColorBottom(temp_cube[i][j][k].getColorBottom());
                                cubes[i][j][k].setColorTop(temp_cube[i][j][k].getColorTop());
                            }
                        }
                    }
                    break;

                case ROTATE_MIDDLE_TO_RIGHT:
                    for(int i = 0; i < 3; i++){
                        for(int j = 1; j < 2; j++){
                            for(int k = 0; k < 3; k++){
                                temp_cube[i][j][k] = Cube("111",vd, gd, nd);
                                temp_cube[i][j][k].setColorBack(cubes[k][j][2-i].getColorBack());
                                temp_cube[i][j][k].setColorFront(cubes[k][j][2-i].getColorFront());
                                temp_cube[i][j][k].setColorLeft(cubes[k][j][2-i].getColorLeft());
                                temp_cube[i][j][k].setColorRight(cubes[k][j][2-i].getColorRight());
                                temp_cube[i][j][k].setColorBottom(cubes[k][j][2-i].getColorBottom());
                                temp_cube[i][j][k].setColorTop(cubes[k][j][2-i].getColorTop());
                                temp_cube[i][j][k].rotateColor(ROTATE_RIGHT);
                            }
                        }
                    }
                    for(int i = 0; i < 3; i++){
                        for(int j = 1; j < 2; j++){
                            for(int k = 0; k < 3; k++){
                                cubes[i][j][k] = Cube(std::to_string(i+1)+std::to_string(j+1)+std::to_string(k+1),vd, gd, nd);
                                cubes[i][j][k].setColorBack(temp_cube[i][j][k].getColorBack());
                                cubes[i][j][k].setColorFront(temp_cube[i][j][k].getColorFront());
                                cubes[i][j][k].setColorLeft(temp_cube[i][j][k].getColorLeft());
                                cubes[i][j][k].setColorRight(temp_cube[i][j][k].getColorRight());
                                cubes[i][j][k].setColorBottom(temp_cube[i][j][k].getColorBottom());
                                cubes[i][j][k].setColorTop(temp_cube[i][j][k].getColorTop());
                            }
                        }
                    }
                    break;

                case ROTATE_BOTTOM_TO_LEFT:
                    for(int i = 0; i < 3; i++){
                        for(int j = 2; j < 3; j++){
                            for(int k = 0; k < 3; k++){
                                temp_cube[i][j][k] = Cube("111",vd, gd, nd);
                                temp_cube[i][j][k].setColorBack(cubes[2-k][j][i].getColorBack());
                                temp_cube[i][j][k].setColorFront(cubes[2-k][j][i].getColorFront());
                                temp_cube[i][j][k].setColorLeft(cubes[2-k][j][i].getColorLeft());
                                temp_cube[i][j][k].setColorRight(cubes[2-k][j][i].getColorRight());
                                temp_cube[i][j][k].setColorBottom(cubes[2-k][j][i].getColorBottom());
                                temp_cube[i][j][k].setColorTop(cubes[2-k][j][i].getColorTop());
                                temp_cube[i][j][k].rotateColor(ROTATE_LEFT);
                            }
                        }
                    }
                    for(int i = 0; i < 3; i++){
                        for(int j = 2; j < 3; j++){
                            for(int k = 0; k < 3; k++){
                                cubes[i][j][k] = Cube(std::to_string(i+1)+std::to_string(j+1)+std::to_string(k+1),vd, gd, nd);
                                cubes[i][j][k].setColorBack(temp_cube[i][j][k].getColorBack());
                                cubes[i][j][k].setColorFront(temp_cube[i][j][k].getColorFront());
                                cubes[i][j][k].setColorLeft(temp_cube[i][j][k].getColorLeft());
                                cubes[i][j][k].setColorRight(temp_cube[i][j][k].getColorRight());
                                cubes[i][j][k].setColorBottom(temp_cube[i][j][k].getColorBottom());
                                cubes[i][j][k].setColorTop(temp_cube[i][j][k].getColorTop());
                            }
                        }
                    }
                    break;

                case ROTATE_BOTTOM_TO_RIGHT:
                    for(int i = 0; i < 3; i++){
                        for(int j = 2; j < 3; j++){
                            for(int k = 0; k < 3; k++){
                                temp_cube[i][j][k] = Cube("111",vd, gd, nd);
                                temp_cube[i][j][k].setColorBack(cubes[k][j][2-i].getColorBack());
                                temp_cube[i][j][k].setColorFront(cubes[k][j][2-i].getColorFront());
                                temp_cube[i][j][k].setColorLeft(cubes[k][j][2-i].getColorLeft());
                                temp_cube[i][j][k].setColorRight(cubes[k][j][2-i].getColorRight());
                                temp_cube[i][j][k].setColorBottom(cubes[k][j][2-i].getColorBottom());
                                temp_cube[i][j][k].setColorTop(cubes[k][j][2-i].getColorTop());
                                temp_cube[i][j][k].rotateColor(ROTATE_RIGHT);
                            }
                        }
                    }
                    for(int i = 0; i < 3; i++){
                        for(int j = 2; j < 3; j++){
                            for(int k = 0; k < 3; k++){
                                cubes[i][j][k] = Cube(std::to_string(i+1)+std::to_string(j+1)+std::to_string(k+1),vd, gd, nd);
                                cubes[i][j][k].setColorBack(temp_cube[i][j][k].getColorBack());
                                cubes[i][j][k].setColorFront(temp_cube[i][j][k].getColorFront());
                                cubes[i][j][k].setColorLeft(temp_cube[i][j][k].getColorLeft());
                                cubes[i][j][k].setColorRight(temp_cube[i][j][k].getColorRight());
                                cubes[i][j][k].setColorBottom(temp_cube[i][j][k].getColorBottom());
                                cubes[i][j][k].setColorTop(temp_cube[i][j][k].getColorTop());
                            }
                        }
                    }
                    break;

                default:
                    break;
            }

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

