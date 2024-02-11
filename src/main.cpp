#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <model.h>
#include <piece.h>
#include <camera.h>
#include <light.h>

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
void createTexture(unsigned int& depthMap);
void renderShadowMap(Shader& depthShader, unsigned int& depthMapFBO, Model& model, Model& monopoly, Light& light);

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 1024;
const unsigned int SHADOW_WIDTH = 16384, SHADOW_HEIGHT = 16384;

Camera camera(glm::vec3(1.55f, 3.4f, 1.3f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

std::vector<Piece> pieces;
bool pieceSelected = false;;
int currentPiece = 0;

std::vector<Light> lights;

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



    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap, depthMap2;

    createTexture(depthMap);
    createTexture(depthMap2);

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


    //-------------------------------- Load models --------------------------------------

    Shader modelShader("../resources/shaders/Model.vs", "../resources/shaders/Model.fs");
    Shader depthShader("../resources/shaders/depthMap.vs", "../resources/shaders/depthMap.fs");
    Shader debugShader("../resources/shaders/fingerhut.vs", "../resources/shaders/fingerhut.fs");

    Model model("../resources/objects/Monopoly/szene.obj");
    Model monopoly("../resources/objects/Monopoly/monopoly.obj");
    
    Model hatModel("../resources/objects/Modelle/Zylinder.obj");
    Model cartModel("../resources/objects/Modelle/Schubkarre.obj");
    Model thimbleModel("../resources/objects/Modelle/Fingerhut.obj");
    Model shipModel("../resources/objects/Modelle/Schiff.obj");
    Model houseSet("../resources/objects/Modelle/haus_set.obj");
    Model hotelSet("../resources/objects/Modelle/hotel_set.obj");

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 20.0f);
    glm::mat4 startMatrix = glm::mat4(1.0f);
    glm::vec3 directionVector(1.0f, 0.0f, 0.0f);
    startMatrix = glm::scale(startMatrix, glm::vec3(0.01f));

    startMatrix = glm::translate(startMatrix, glm::vec3(172.0f, 86.5f, 197.0f));
    pieces.push_back(Piece(&hatModel, startMatrix, camera.GetViewMatrix(), projection, directionVector));

    startMatrix = glm::translate(startMatrix, glm::vec3(1.0f, 0.0f, -4.0f));
    pieces.push_back(Piece(&cartModel, startMatrix, camera.GetViewMatrix(), projection, directionVector));

    startMatrix = glm::translate(startMatrix, glm::vec3(4.0f, 0.0f, 1.0f));
    pieces.push_back(Piece(&thimbleModel, startMatrix, camera.GetViewMatrix(), projection, directionVector));

    startMatrix = glm::translate(startMatrix, glm::vec3(-1.0f, 0.0f, 4.0f));
    pieces.push_back(Piece(&shipModel, startMatrix, camera.GetViewMatrix(), projection, directionVector));
    pieces.push_back(Piece(&houseSet, glm::mat4(1.0f), camera.GetViewMatrix(), projection, directionVector));
    pieces.push_back(Piece(&hotelSet, glm::mat4(1.0f), camera.GetViewMatrix(), projection, directionVector));

    //-------------------------------- Define lights --------------------------------------

    lights.push_back(Light(glm::vec3(5.55f, 7.5f, 5.3f), glm::vec3(1.0f, 1.0f, 1.0f), -135.0f, -30.0f));
    lights.push_back(Light(glm::vec3(10.7f, 7.25f, -2.94f), glm::vec3(1.0f, 1.0f, 1.0f), -194.0f, -25.0f));

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


        glBindTexture(GL_TEXTURE_2D, depthMap);
        renderShadowMap(depthShader, depthMapFBO, model, monopoly, lights[0]);
        glBindTexture(GL_TEXTURE_2D, 0);

        
        debugShader.use();
        debugShader.setFloat("near_plane", near_plane);
        debugShader.setFloat("far_plane", far_plane);
        glBindVertexArray(VAO);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        
       
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        glm::mat4 lightProjection =  glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.1f, 20.0f);
        glm::mat4 lightSpaceMatrix = lightProjection * lights[0].camera.GetViewMatrix();
        glm::mat4 lightSpaceMatrix2 = lightProjection * lights[1].camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 20.0f);


        glEnable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        modelMatrix = glm::mat4(1.0f);

        modelShader.use();
        modelShader.setInt("shadowMap", 1);
        modelShader.setInt("shadowMap2", 2);
        modelShader.setVec3("lightColor", lights[0].color);
        modelShader.setVec3("lightPos", lights[0].position);
        modelShader.setVec3("viewPos", camera.Position);
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view",  camera.GetViewMatrix());
        modelShader.setMat4("model", modelMatrix);
        modelShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        modelShader.setMat4("lightSpaceMatrix2", lightSpaceMatrix2);
        modelShader.setFloat("bias", 0.0003);

        modelShader.setInt("hasTexture", false);
        model.draw(modelShader);
        modelShader.setFloat("bias", 0.00001);
        modelShader.setInt("hasTexture", true);
        monopoly.draw(modelShader);


        for (size_t i = 0; i < pieces.size(); i++){
            pieces[i].draw(modelShader);
        }



        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void renderShadowMap(Shader& depthShader, unsigned int& depthMapFBO, Model& model, Model& monopoly, Light &light){
    glCullFace(GL_FRONT);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 lightProjection =  glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.1f, 20.0f);
    glm::mat4 lightSpaceMatrix = lightProjection * light.camera.GetViewMatrix();

    depthShader.use();


    depthShader.setMat4("model", modelMatrix);
    depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    glEnable(GL_DEPTH_TEST);

    model.draw(depthShader);
    monopoly.draw(depthShader);
    for (size_t i = 0; i < pieces.size(); i++){
        pieces[i].draw(depthShader);
    }

    glCullFace(GL_BACK); 
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void createTexture(unsigned int& depthMap){
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
}

void processInput(GLFWwindow *window){

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    float speed = deltaTime / 10;
    float pieceSpeed = 0.1f;
    if(!pieceSelected){

        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
            speed *= 10;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
            camera.ProcessKeyboard(FORWARD, speed);
        }
            
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
            camera.ProcessKeyboard(BACKWARD, speed);
        }
            
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
            camera.ProcessKeyboard(LEFT, speed);
        }
            
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
            camera.ProcessKeyboard(RIGHT, speed);
        }
            
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
            camera.ProcessKeyboard(DOWN, speed);
        }
            
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
            camera.ProcessKeyboard(UP, speed);
        }

        if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
            currentPiece = 0;
        }

        if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){
            currentPiece = 1;
        }

        if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS){
            currentPiece = 2;
        }

        if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS){
            currentPiece = 3;
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
            pieces[currentPiece].move(pieceSpeed, glm::vec3(1.0f, 0.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
            pieces[currentPiece].move(-pieceSpeed, glm::vec3(1.0f, 0.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
            pieces[currentPiece].move(pieceSpeed, glm::vec3(0.0f, 0.0f, 1.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
            pieces[currentPiece].move(-pieceSpeed, glm::vec3(0.0f, 0.0f, 1.0f));
        }

    }
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
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
