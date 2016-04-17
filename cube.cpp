#include "cube.hpp"

// std::string position;
// glm::vec3 axis;
// glm::mat4 modelMatrix;

// GLuint vertexBuffer;
// GLfloat vertexBufferData[NUM_POINTS];

// GLuint colorBuffer;
// GLfloat colorBufferData[NUM_POINTS];

Cube::Cube(std::string position, GLfloat vertexArray[], GLfloat colorArray[]) {
    for (int i = 0; i < NUM_POINTS; i++) {
        if (vertexArray[i] == 1.0f) {
            printf("GELOT\n");
            fflush(stdout);
            vertexBufferData[i] = vertexArray[i] - 0.1f;
        } else {
            vertexBufferData[i] = vertexArray[i];
        }
        colorBufferData[i] = colorArray[i];
    }

    this->position = position;
    switch(position[0]) {
        case '1':
            this->translate(glm::vec3(-2.0f, 0.0f, 0.0f));
            this->axis.x = -2.0f;
            break;
        case '2':
            this->translate(glm::vec3(0.0f, 0.0f, 0.0f));
            this->axis.x = 0.0f;
            break;
        case '3':
            this->translate(glm::vec3(2.0f, 0.0f, 0.0f));
            this->axis.x = 2.0f;
            break;
    }

    switch(position[1]) {
        case '1':
            this->translate(glm::vec3(0.0f, -2.0f, 0.0f));
            this->axis.y = -2.0f;
            break;
        case '2':
            this->translate(glm::vec3(0.0f, 0.0f, 0.0f));
            this->axis.y = 0.0f;
            break;
        case '3':
            this->translate(glm::vec3(0.0f, 2.0f, 0.0f));
            this->axis.y = 2.0f;
            break;
    }

    switch(position[2]) {
        case '1':
            this->translate(glm::vec3(0.0f, 0.0f, -2.0f));
            this->axis.z = -2.0f;
            break;
        case '2':
            this->translate(glm::vec3(0.0f, 0.0f, 0.0f));
            this->axis.z = 0.0f;
            break;
        case '3':
            this->translate(glm::vec3(0.0f, 0.0f, 2.0f));
            this->axis.z = 2.0f;
            break;
    }
}

Cube::~Cube() {
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &colorBuffer);
}


void Cube::rotate(glm::vec3 axis, glm::vec3 angle) {
    glm::quat orientation = glm::quat(angle);

    this->modelMatrix = glm::translate(this->modelMatrix, glm::vec3(-axis.x, -axis.y, -axis.z));
    this->modelMatrix *= glm::eulerAngleYXZ(orientation.y, orientation.x, orientation.z);
    this->modelMatrix = glm::translate(this->modelMatrix, axis);
}

void Cube::rotate(glm::vec3 angle) {
    glm::quat orientation = glm::quat(angle);

    this->modelMatrix = glm::translate(this->modelMatrix, glm::vec3(-axis.x, -axis.y, -axis.z));
    this->modelMatrix *= glm::eulerAngleYXZ(orientation.y, orientation.x, orientation.z);
    this->modelMatrix = glm::translate(this->modelMatrix, axis);
}

void Cube::translate(glm::vec3 translation) {
    this->modelMatrix = glm::translate(this->modelMatrix, translation);
}

void Cube::scale(glm::vec3 scalingFactor) {
    this->modelMatrix = glm::scale(this->modelMatrix, scalingFactor);
}

glm::mat4& Cube::getModelMatrix() {
    return this->modelMatrix;
}

glm::vec3& Cube::getAxis() {
    return axis;
}

GLfloat * Cube::getVertexBufferData() {
    return vertexBufferData;
}

GLfloat * Cube::getColorBufferData() {
    return vertexBufferData;
}


void Cube::draw(GLuint MatrixID, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix) {
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * this->getModelMatrix();

    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);
    
    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colorBufferData), colorBufferData, GL_STATIC_DRAW);
    
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(
                          0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    
    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glVertexAttribPointer(
                          1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                          3,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
    
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, NUM_POINTS / 3); // 12*3 indices starting at 0 -> 12 triangles
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void Cube::setAxis(glm::vec3 axis) {
    this->axis = axis;
}

void Cube::setVertexBufferData(GLfloat vertexArray[]) {
    for (int i = 0; i < NUM_POINTS; i++) {
        vertexBufferData[i] = vertexArray[i];
    }
}

void Cube::setColorBufferData(GLfloat colorArray[]) {
    for (int i = 0; i < NUM_POINTS; i++) {
        colorBufferData[i] = colorArray[i];
    }
}

void Cube::setColorTop(GLfloat colorArray[]) {
    for (int i = TOP_VERTEX; i < TOP_VERTEX + NUM_POINTS_IN_SIDE; i++) {
        colorBufferData[i] = colorArray[i - TOP_VERTEX];
    }
}

void Cube::setColorBottom(GLfloat colorArray[]) {
    for (int i = BOTTOM_VERTEX; i < BOTTOM_VERTEX + NUM_POINTS_IN_SIDE; i++) {
        colorBufferData[i] = colorArray[i - BOTTOM_VERTEX];
    }
}

void Cube::setColorFront(GLfloat colorArray[]) {
    for (int i = FRONT_VERTEX; i < FRONT_VERTEX + NUM_POINTS_IN_SIDE; i++) {
        colorBufferData[i] = colorArray[i - FRONT_VERTEX];
    }
}

void Cube::setColorBack(GLfloat colorArray[]) {
    for (int i = BACK_VERTEX; i < BACK_VERTEX + NUM_POINTS_IN_SIDE; i++) {
        colorBufferData[i] = colorArray[i - BACK_VERTEX];
    }
}

void Cube::setColorLeft(GLfloat colorArray[]) {
    for (int i = LEFT_VERTEX; i < LEFT_VERTEX + NUM_POINTS_IN_SIDE; i++) {
        colorBufferData[i] = colorArray[i - LEFT_VERTEX];
    }
}

void Cube::setColorRight(GLfloat colorArray[]) {
    for (int i = RIGHT_VERTEX; i < RIGHT_VERTEX + NUM_POINTS_IN_SIDE; i++) {
        colorBufferData[i] = colorArray[i - RIGHT_VERTEX];
    }
}
