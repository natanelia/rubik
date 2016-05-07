#include "cube.hpp"

// std::string position;
// glm::vec3 axis;
// glm::mat4 modelMatrix;

// GLuint vertexBuffer;
// GLfloat vertexBufferData[NUM_POINTS];

// GLuint colorBuffer;
// GLfloat colorBufferData[NUM_POINTS];

Cube::Cube() {
    
}

Cube::Cube(std::string position, GLfloat vertexArray[], GLfloat colorArray[]) {

    programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );
    
    // Get a handle for our "LightPosition" uniform
    LightID1 = glGetUniformLocation(programID, "LightPosition1_worldspace");
    LightID2 = glGetUniformLocation(programID, "LightPosition2_worldspace");
    
    Texture = loadBMP_custom("uvtemplateX.bmp");
    //Texture = loadDDS("uvtemplate.DDS");

    // Get a handle for our "myTextureSampler" uniform
    TextureID  = glGetUniformLocation(programID, "myTextureSampler");

    for (int i = 0; i < NUM_POINTS; i++) {
        if (vertexArray[i] == 1.0f) {
            vertexBufferData[i] = vertexArray[i] - 0.1f;
        } else {
            vertexBufferData[i] = vertexArray[i];
        }
    }

//    for(int i=0; i< NUM_POINTS; i++){
//        normalBufferData [i] = 0;
//    }

    for (int i = 0; i < 72; i++) {
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

void Cube::rotateColor(int ROTATE_TO) {
    GLfloat * colorLeft = getColorLeft();
    GLfloat * colorFront = getColorFront();
    GLfloat * colorRight = getColorRight();
    GLfloat * colorBack = getColorBack();
    GLfloat * colorTop = getColorTop();
    GLfloat * colorBottom = getColorBottom();

    switch (ROTATE_TO) {
        case ROTATE_RIGHT:
            setColorFront(colorLeft);
            setColorRight(colorFront);
            setColorBack(colorRight);
            setColorLeft(colorBack);

            break;
        case ROTATE_LEFT:
            setColorLeft(colorFront);
            setColorFront(colorRight);
            setColorRight(colorBack);
            setColorBack(colorLeft);

            break;
        case ROTATE_TOP:
            setColorTop(colorFront);
            setColorFront(colorBottom);
            setColorBottom(colorBack);
            setColorBack(colorTop);

            break;
        case ROTATE_BOTTOM:
            setColorTop(colorBack);
            setColorFront(colorTop);
            setColorBottom(colorFront);
            setColorBack(colorBottom);

            break;
    }

    delete [] colorLeft;
    delete [] colorFront;
    delete [] colorRight;
    delete [] colorBack;
    delete [] colorTop;
    delete [] colorBottom;
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

    //CUBE
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);
    

    //COLOR
    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colorBufferData), colorBufferData, GL_STATIC_DRAW);
    

    
    //CALCULATE NORMAL
//    normals = {
//        1.0f, 1.0f, 1.0f, // Right
//        1.0f,-1.0f,-1.0f,
//        1.0f, 1.0f,-1.0f,
//        
//        1.0f,-1.0f,-1.0f,
//        1.0f, 1.0f, 1.0f,
//        1.0f,-1.0f, 1.0f,
//        
//        -1.0f,-1.0f,-1.0f, // Left
//        -1.0f,-1.0f, 1.0f,
//        -1.0f, 1.0f, 1.0f,
//        
//        -1.0f,-1.0f,-1.0f,
//        -1.0f, 1.0f, 1.0f,
//        -1.0f, 1.0f,-1.0f,
//        
//        -1.0f, 1.0f,-1.0f, // Top
//        1.0f, 1.0f, 1.0f,
//        1.0f, 1.0f,-1.0f,
//        
//        -1.0f, 1.0f,-1.0f,
//        -1.0f, 1.0f, 1.0f,
//        1.0f, 1.0f, 1.0f,
//        
//        -1.0f,-1.0f,-1.0f, // Bottom
//        1.0f,-1.0f, 1.0f,
//        -1.0f,-1.0f, 1.0f,
//        
//        1.0f,-1.0f, 1.0f,
//        1.0f,-1.0f,-1.0f,
//        -1.0f,-1.0f,-1.0f,
//        
//        -1.0f,-1.0f, 1.0f, // Back
//        1.0f, 1.0f, 1.0f,
//        1.0f,-1.0f, 1.0f,
//        
//        -1.0f,-1.0f, 1.0f,
//        -1.0f, 1.0f, 1.0f,
//        1.0f, 1.0f, 1.0f,
//        
//        1.0f,-1.0f,-1.0f, // Top
//        1.0f, 1.0f,-1.0f,
//        -1.0f, 1.0f,-1.0f,
//        
//        -1.0f,-1.0f,-1.0f,
//        1.0f,-1.0f,-1.0f,
//        -1.0f, 1.0f,-1.0f
//    };
    
    for(int i = 0; i < (NUM_POINTS/9); i++){
        GLfloat a1 = vertexBufferData[i*3+0];
        GLfloat a2 = vertexBufferData[i*3+1];
        GLfloat a3 = vertexBufferData[i*3+2];
        GLfloat a4 = vertexBufferData[i*3+3];
        GLfloat a5 = vertexBufferData[i*3+4];
        GLfloat a6 = vertexBufferData[i*3+5];
        GLfloat a7 = vertexBufferData[i*3+6];
        GLfloat a8 = vertexBufferData[i*3+7];
        GLfloat a9 = vertexBufferData[i*3+8];
        
        GLfloat b1 = (a5-a2) * (a9-a3) - (a6-a3) * (a8-a2);
        GLfloat b2 = (a6 - a3) * (a7 - a1) - (a4 - a1) * (a9- a3);
        GLfloat b3 = (a4 - a1) * (a8 - a2) - (a5 - a2) * (a7- a1);
        
        normals[i*3+0] = b1 == 0 || b1 == -0 ? b1 : b1/fabs(b1);
        normals[i*3+1] = b2 == 0 || b2 == -0 ? b2 : b2/fabs(b2);
        normals[i*3+2] = b3 == 0 || b3 == -0 ? b3 : b3/fabs(b3);
        normals[i*3+3] = b1 == 0 || b1 == -0 ? b1 : b1/fabs(b1);
        normals[i*3+4] = b2 == 0 || b2 == -0 ? b2 : b2/fabs(b2);
        normals[i*3+5] = b3 == 0 || b3 == -0 ? b3 : b3/fabs(b3);
        normals[i*3+6] = b1 == 0 || b1 == -0 ? b1 : b1/fabs(b1);
        normals[i*3+7] = b2 == 0 || b2 == -0 ? b2 : b2/fabs(b2);
        normals[i*3+8] = b3 == 0 || b3 == -0 ? b3 : b3/fabs(b3);
    }
    
    for(int i=0; i< NUM_POINTS; i++)
        std::cout << "normal" << normals[i] << std::endl;
    
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

    glm::vec3 lightPos = glm::vec3(8,8,8);
    glUniform3f(LightID1, lightPos.x, lightPos.y, lightPos.z);
    
    glm::vec3 lightPos2 = glm::vec3(8,-8,-8);
    glUniform3f(LightID2, lightPos2.x, lightPos2.y, lightPos2.z);
    
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glUniform1i(TextureID, 0);
    
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
                          2,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
    
    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glVertexAttribPointer(
                          2,                                // attribute
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
    glDisableVertexAttribArray(2);
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
    for (int i = 0; i < 72; i++) {
        colorBufferData[i] = colorArray[i];
    }
}


GLfloat * Cube::getColorTop() {
    GLfloat * colorData = new GLfloat[NUM_POINTS_IN_SIDE];

    for (int i = TOP_VERTEX; i < TOP_VERTEX + NUM_POINTS_IN_SIDE; i++) {
        colorData[i - TOP_VERTEX] = colorBufferData[i];
    }

    return colorData;
}

GLfloat * Cube::getColorBottom() {
    GLfloat * colorData = new GLfloat[NUM_POINTS_IN_SIDE];

    for (int i = BOTTOM_VERTEX; i < BOTTOM_VERTEX + NUM_POINTS_IN_SIDE; i++) {
        colorData[i - BOTTOM_VERTEX] = colorBufferData[i];
    }

    return colorData;
}

GLfloat * Cube::getColorFront() {
    GLfloat * colorData = new GLfloat[NUM_POINTS_IN_SIDE];

    for (int i = FRONT_VERTEX; i < FRONT_VERTEX + NUM_POINTS_IN_SIDE; i++) {
        colorData[i - FRONT_VERTEX] = colorBufferData[i];
    }

    return colorData;
}

GLfloat * Cube::getColorBack() {
    GLfloat * colorData = new GLfloat[NUM_POINTS_IN_SIDE];

    for (int i = BACK_VERTEX; i < BACK_VERTEX + NUM_POINTS_IN_SIDE; i++) {
        colorData[i - BACK_VERTEX] = colorBufferData[i];
    }

    return colorData;
}

GLfloat * Cube::getColorLeft() {
    GLfloat * colorData = new GLfloat[NUM_POINTS_IN_SIDE];

    for (int i = LEFT_VERTEX; i < LEFT_VERTEX + NUM_POINTS_IN_SIDE; i++) {
        colorData[i - LEFT_VERTEX] = colorBufferData[i];
    }

    return colorData;
}

GLfloat * Cube::getColorRight() {
    GLfloat * colorData = new GLfloat[NUM_POINTS_IN_SIDE];

    for (int i = RIGHT_VERTEX; i < RIGHT_VERTEX + NUM_POINTS_IN_SIDE; i++) {
        colorData[i - RIGHT_VERTEX] = colorBufferData[i];
    }

    return colorData;
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

