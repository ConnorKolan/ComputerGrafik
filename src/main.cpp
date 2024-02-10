#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <model.h>
#include <piece.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_images.h>
#include <cmath>
#include <iostream>

glm::vec3 rotateVector(const glm::vec3& vec, float angleDegrees, const glm::vec3& axis);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void snapCamera2Piece();

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 1024;

Camera camera(glm::vec3(1.55f, 5.4f, 1.3f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

std::vector<Piece> pieces;
bool pieceSelected = false;;
int currentPiece = 0;


glm::vec3 lightPos    = glm::vec3(5.55f, 7.5f, 5.3f);
glm::vec3 lightColor  = glm::vec3(1.0f, 1.0f, 1.0f);
Camera lightView(lightPos,  glm::vec3(0.0f, 1.0f, 0.0f), -135.0f, -30.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Computergrafik Projekt", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);




    float vertices[] = {
        // positions            // texture coords
         0.5f, -0.5f, 0.0f,     1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, // bottom left
         0.5f,  0.5f, 0.0f,     1.0f, 1.0f, // top right
        -0.5f,  0.5f, 0.0f,     0.0f, 1.0f  // top left 
    };



    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 


    const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;

    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        std::cout << "Frambuffer incomplete" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



    Shader modelShader("../resources/shaders/Model.vs", "../resources/shaders/Model.fs");
    Shader depthShader("../resources/shaders/depthMap.vs", "../resources/shaders/depthMap.fs");
    Shader debugShader("../resources/shaders/fingerhut.vs", "../resources/shaders/fingerhut.fs");
    Shader monopolyShader("../resources/shaders/monopoly.vs", "../resources/shaders/monopoly.fs");

    Model model("../resources/objects/Monopoly/szene.obj");
    Model monopoly("../resources/objects/Monopoly/monopoly.obj");
    Model hatModel("../resources/objects/Modelle/Hut.obj");
    Model canonModel("../resources/objects/Modelle/Kanone.obj");

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 20.0f);
    glm::mat4 startMatrix = glm::mat4(1.0f);
    glm::vec3 directionVector(1.0f, 0.0f, 0.0f);
    startMatrix = glm::scale(startMatrix, glm::vec3(0.01f));

    startMatrix = glm::translate(startMatrix, glm::vec3(175.0f, 88.0f, 195.0f));
    Piece hat(&hatModel, startMatrix, camera.GetViewMatrix(), projection, directionVector);
    pieces.push_back(hat);


    /*startMatrix = glm::translate(startMatrix, glm::vec3(4.0f, 0.0f, 0.0f));
    Piece canon(&canonModel, startMatrix, camera.GetViewMatrix(), projection, directionVector);
    pieces.push_back(canon);*/

    debugShader.use();
    debugShader.setInt("depthMap", 0);

    modelShader.use();
    modelShader.setVec3("lightColor", lightColor);
    modelShader.setVec3("lightPos", lightPos);

    float near_plane = 0.1f;
    float far_plane = 20.0f;
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);


        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glm::mat4 modelMatrix = glm::mat4(1.0f);

        
        glCullFace(GL_FRONT);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        depthShader.use();
        glm::mat4 lightProjection =  glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.1f, 20.0f);
        glm::mat4 lightSpaceMatrix = lightProjection * lightView.GetViewMatrix();

        depthShader.setMat4("model", modelMatrix);
        depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
        glEnable(GL_DEPTH_TEST);
        model.draw(depthShader);
        glCullFace(GL_BACK); 
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default

        
        glClear(GL_COLOR_BUFFER_BIT);

        /*
        debugShader.use();
        debugShader.setFloat("near_plane", near_plane);
        debugShader.setFloat("far_plane", far_plane);
        glBindVertexArray(VAO);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        */

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 20.0f);

        glEnable(GL_DEPTH_TEST);

        monopolyShader.use();
        monopolyShader.setMat4("projection", projection);
        monopolyShader.setMat4("view",  camera.GetViewMatrix());
        monopolyShader.setMat4("model", modelMatrix);

        monopoly.draw(monopolyShader);

        for (size_t i = 0; i < pieces.size(); i++){
            pieces[i].draw(monopolyShader);
        }

        glBindTexture(GL_TEXTURE_2D, depthMap);
        modelMatrix = glm::mat4(1.0f);
        modelShader.use();
        modelShader.setVec3("viewPos", camera.Position);
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view",  camera.GetViewMatrix());
        modelShader.setMat4("model", modelMatrix);
        modelShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        model.draw(modelShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow *window){

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    if(!pieceSelected){

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
            camera.ProcessKeyboard(FORWARD, deltaTime);
        }
            
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        }
            
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
            camera.ProcessKeyboard(LEFT, deltaTime);
        }
            
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
            camera.ProcessKeyboard(RIGHT, deltaTime);
        }
            
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
            camera.ProcessKeyboard(DOWN, deltaTime);
        }
            
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
            camera.ProcessKeyboard(UP, deltaTime);
        }

        if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
            pieceSelected = true;
            snapCamera2Piece();
        }
    }else{
        if(glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS){
            pieceSelected = false;
        }

        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
            if(currentPiece <= pieces.size()){
                currentPiece++;
            }else{
                currentPiece = 0;
            }
            snapCamera2Piece();
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
            float value = 0.1f;
            pieces[currentPiece].move(value);
            //snapCamera2Piece();
        }

    }
}

void snapCamera2Piece(){
    glm::vec3 piecePos = pieces[currentPiece].getPosition();
    glm::vec3 pieceDir = pieces[currentPiece].getDirection();

    camera.Position = glm::vec3(piecePos.x + ((-0.2f) * pieceDir.x), piecePos.y + 0.1f, piecePos.z - (0.2f * pieceDir.z));
    
    float temp = pieceDir.y;
    pieceDir.y = camera.Front.y;
    float horizontalAngle = std::acos(glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), pieceDir));
    pieceDir.y = temp;

    camera.setYaw(horizontalAngle);
    camera.setPitch(-25.0f);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn){
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;


    if(!pieceSelected){
        camera.ProcessMouseMovement(xoffset, yoffset);
    }else{
        float x = pieces[currentPiece].transMatrix[3][0];
        float y = pieces[currentPiece].transMatrix[3][1];
        float z = pieces[currentPiece].transMatrix[3][2];

        camera.rotateAroundCenter(glm::vec3(x, y, z), xoffset, glm::vec3(0.0f, 1.0f, 0.0f));

        camera.setYaw(camera.Yaw - xoffset);
        pieces[currentPiece].direction = rotateVector(pieces[currentPiece].direction, xoffset, glm::vec3(0.0f, 1.0f, 0.0f));
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

glm::vec3 rotateVector(const glm::vec3& vec, float angleDegrees, const glm::vec3& axis) {
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angleDegrees), axis);
    return glm::vec3(rotationMatrix * glm::vec4(vec, 1.0f));
}