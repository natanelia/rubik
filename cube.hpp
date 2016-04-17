#define _USE_MATH_DEFINES

// Include standard headers
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <common/shader.hpp>
#include <cmath>

#define NUM_SIDES 6
#define NUM_POINTS (3 * 3 * 2 * NUM_SIDES)
#define NUM_POINTS_IN_SIDE (NUM_POINTS / NUM_SIDES)

// THE FOLLOWING DEFINES VERTEX INDEX START OF INSERTED vertexArray[] on constructor
#define TOP_VERTEX 36
#define BOTTOM_VERTEX (TOP_VERTEX + NUM_POINTS_IN_SIDE)

#define RIGHT_VERTEX 0
#define LEFT_VERTEX (RIGHT_VERTEX + NUM_POINTS_IN_SIDE)

#define BACK_VERTEX 72
#define FRONT_VERTEX (BACK_VERTEX + NUM_POINTS_IN_SIDE)

#define NO_ROTATE 0
#define ROTATE_TOP 1
#define ROTATE_RIGHT 2
#define ROTATE_BOTTOM 3
#define ROTATE_LEFT 4

class Cube {
protected:
    std::string position;
    glm::vec3 axis;
    glm::mat4 modelMatrix;

    GLuint vertexBuffer;
    GLfloat vertexBufferData[NUM_POINTS];

    GLuint colorBuffer;
    GLfloat colorBufferData[NUM_POINTS];

public:
    Cube();
    Cube(std::string position, GLfloat vertexArray[], GLfloat colorArray[]);
    ~Cube();

    void rotate(glm::vec3 axis, glm::vec3 angle);
    void rotate(glm::vec3 angle);
    void rotateColor(int ROTATE_TO);

    void translate(glm::vec3 translation);
    void scale(glm::vec3 scalingFactor);

    glm::mat4& getModelMatrix();
    glm::vec3& getAxis();
    GLfloat * getVertexBufferData();
    GLfloat * getColorBufferData();

    void draw(GLuint MatrixID, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix);

    void setModelMatrix(glm::mat4 matrix) { this->modelMatrix = matrix; }
    void setAxis(glm::vec3 axis);

    void setVertexBufferData(GLfloat vertexArray[]);
    void setColorBufferData(GLfloat colorArray[]);

    GLfloat * getColorTop();
    GLfloat * getColorBottom();
    GLfloat * getColorFront();
    GLfloat * getColorBack();
    GLfloat * getColorLeft();
    GLfloat * getColorRight();

    void setColorTop(GLfloat colorArray[]);
    void setColorBottom(GLfloat colorArray[]);
    void setColorFront(GLfloat colorArray[]);
    void setColorBack(GLfloat colorArray[]);
    void setColorLeft(GLfloat colorArray[]);
    void setColorRight(GLfloat colorArray[]);
    
    std::string getPosition() { return position; }
    
    
};
