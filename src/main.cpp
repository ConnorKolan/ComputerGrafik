#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

glm::vec3 lightPos    = glm::vec3(1.2f, 1.0f, 2.0f);
glm::vec3 lightColor  = glm::vec3(1.0f, 1.0f, 1.0f);

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov = 45.0f;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

void loadAndBindTextures(unsigned int *texture1, unsigned int *texture2);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

int main(int, char**){
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR ,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR ,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Test", NULL, NULL);

    if(window == nullptr){
        std::cout << "Test failed, could not create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window, scroll_callback); 


    gladLoadGL();

    int success;
    char infoLog[512];

    //--------------------------------------------------------------------------------------------------------

    Shader cubeShader("../resources/shaders/Shaders/Cube/Cube.vs", "../resources/shaders/Shaders/Cube/Cube.fs");
    Shader lightShader("../resources/shaders/Shaders/Lamp/Lamp.vs", "../resources/shaders/Shaders/Lamp/Lamp.fs");


    //coordinates of the triangle
    float cube[] = {
    // positions                vertex normals            texture
    -0.5f, -0.5f, -0.5f,        0.0f,  0.0f, -1.0f,       0.0f, 0.0f, 
     0.5f, -0.5f, -0.5f,        0.0f,  0.0f, -1.0f,       1.0f, 0.0f, 
     0.5f,  0.5f, -0.5f,        0.0f,  0.0f, -1.0f,       1.0f, 1.0f, 
     0.5f,  0.5f, -0.5f,        0.0f,  0.0f, -1.0f,       1.0f, 1.0f, 
    -0.5f,  0.5f, -0.5f,        0.0f,  0.0f, -1.0f,       0.0f, 1.0f, 
    -0.5f, -0.5f, -0.5f,        0.0f,  0.0f, -1.0f,       0.0f, 0.0f,  

    -0.5f, -0.5f,  0.5f,        0.0f,  0.0f, 1.0f,        0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,        0.0f,  0.0f, 1.0f,        1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,        0.0f,  0.0f, 1.0f,        1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,        0.0f,  0.0f, 1.0f,        1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,        0.0f,  0.0f, 1.0f,        0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,        0.0f,  0.0f, 1.0f,        0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,        -1.0f,  0.0f,  0.0f,      1.0f, 0.0f, 
    -0.5f,  0.5f, -0.5f,        -1.0f,  0.0f,  0.0f,      1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,        -1.0f,  0.0f,  0.0f,      0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,        -1.0f,  0.0f,  0.0f,      0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,        -1.0f,  0.0f,  0.0f,      0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,        -1.0f,  0.0f,  0.0f,      1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,        1.0f,  0.0f,  0.0f,       1.0f, 0.0f, 
     0.5f,  0.5f, -0.5f,        1.0f,  0.0f,  0.0f,       1.0f, 1.0f, 
     0.5f, -0.5f, -0.5f,        1.0f,  0.0f,  0.0f,       0.0f, 1.0f, 
     0.5f, -0.5f, -0.5f,        1.0f,  0.0f,  0.0f,       0.0f, 1.0f, 
     0.5f, -0.5f,  0.5f,        1.0f,  0.0f,  0.0f,       0.0f, 0.0f, 
     0.5f,  0.5f,  0.5f,        1.0f,  0.0f,  0.0f,       1.0f, 0.0f, 

    -0.5f, -0.5f, -0.5f,        0.0f, -1.0f,  0.0f,       0.0f, 1.0f, 
     0.5f, -0.5f, -0.5f,        0.0f, -1.0f,  0.0f,       1.0f, 1.0f, 
     0.5f, -0.5f,  0.5f,        0.0f, -1.0f,  0.0f,       1.0f, 0.0f, 
     0.5f, -0.5f,  0.5f,        0.0f, -1.0f,  0.0f,       1.0f, 0.0f, 
    -0.5f, -0.5f,  0.5f,        0.0f, -1.0f,  0.0f,       0.0f, 0.0f, 
    -0.5f, -0.5f, -0.5f,        0.0f, -1.0f,  0.0f,       0.0f, 1.0f, 

    -0.5f,  0.5f, -0.5f,        0.0f,  1.0f,  0.0f,       0.0f, 1.0f, 
     0.5f,  0.5f, -0.5f,        0.0f,  1.0f,  0.0f,       1.0f, 1.0f, 
     0.5f,  0.5f,  0.5f,        0.0f,  1.0f,  0.0f,       1.0f, 0.0f, 
     0.5f,  0.5f,  0.5f,        0.0f,  1.0f,  0.0f,       1.0f, 0.0f, 
    -0.5f,  0.5f,  0.5f,        0.0f,  1.0f,  0.0f,       0.0f, 0.0f, 
    -0.5f,  0.5f, -0.5f,        0.0f,  1.0f,  0.0f,       0.0f, 1.0f,
};                      

    //create buffer     
    unsigned int cubeVBO;       
    unsigned int vao;       

    //generate buffer object
    glGenBuffers(1, &cubeVBO);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);  //bind this object to make it current
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    //now tell opengl how to interpret the data from the vertices array
    //first the vertecies themselfs
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) nullptr);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    


    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) nullptr);
    glEnableVertexAttribArray(0);


    unsigned int texture1, texture2;
    loadAndBindTextures(&texture1, &texture2);


    glViewport(0, 0, 800, 800);
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glEnable(GL_DEPTH_TEST); 
    glClear(GL_COLOR_BUFFER_BIT);



    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };
    
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);


    cubeShader.use();

    cubeShader.setInt("texture1", 0);
    cubeShader.setInt("texture2", 1);
    cubeShader.setVec3("lightColor", lightColor);
    cubeShader.setVec3("lightPos", lightPos);

    
    while (!glfwWindowShouldClose(window)){
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        projection = glm::perspective(glm::radians(fov), 800.0f / 800.0f, 0.1f, 100.0f);  

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        

        //------------------------------------------------------------		


        lightShader.use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        glBindVertexArray(lightVAO);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); 
        lightShader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);


        //------------------------------------------------------------		


        cubeShader.use();
        cubeShader.setMat4("view", view);
        cubeShader.setMat4("projection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glBindVertexArray(vao);

        for(unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i; 

            cubeShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        //------------------------------------------------------------		


        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << error << std::endl;
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
  
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}



void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraPos -= cameraSpeed *  cameraUp;
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += cameraSpeed *  cameraUp;
    
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 90.0f)
        fov = 90.0f; 
}

void loadAndBindTextures(unsigned int *texture1, unsigned int *texture2){
    int width, height, nrChannels;
    glGenTextures(1, texture1);
    glBindTexture(GL_TEXTURE_2D, *texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data = stbi_load("../resources/shaders/Textures/container.jpg", &width, &height, &nrChannels, 0);

    if(data){
        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }else{
        std::cout << "Image could not be loaded" << std::endl;
    }
    stbi_image_free(data);


    // texture 2
    // ---------
    glGenTextures(1, texture2);
    glBindTexture(GL_TEXTURE_2D, *texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("../resources/shaders/Textures/awesomeface.png", &width, &height, &nrChannels, 0);
    stbi_set_flip_vertically_on_load(false);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

}