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
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void snapCamera2Piece();
void createTexture(unsigned int& depthMap);
void renderShadowMap(Shader& depthShader, unsigned int& depthMapFBO, Model& model, Model& monopoly, Light& light);
unsigned int loadCubemap(std::vector<std::string> faces);

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
const unsigned int SHADOW_WIDTH = 16384;
const unsigned int SHADOW_HEIGHT = 16384;

Camera camera(glm::vec3(3.55f, 4.5f, 5.3f));
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

    //-------------------------------- GLFW setup --------------------------------------

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //samples for anti aliassing
    glfwWindowHint(GLFW_SAMPLES, 16);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Computergrafik Projekt", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    //enable anti aliasing
    glEnable(GL_MULTISAMPLE);

    //-------------------------------- Square for debugging --------------------------------------

    float vertices[] = {
        // positions            // texture coords
         0.5f, -0.5f, 0.0f,     1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, // bottom left
         0.5f,  0.5f, 0.0f,     1.0f, 1.0f, // top right
        -0.5f,  0.5f, 0.0f,     0.0f, 1.0f  // top left 
    };

    unsigned int vbo, vao;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    //-------------------------------- Skybox coords plus vbo --------------------------------------
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    unsigned int skyboxVao, skyboxVbo;
    glGenVertexArrays(1, &skyboxVao);
    glGenBuffers(1, &skyboxVbo);
    glBindVertexArray(skyboxVao);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    //--------------------------------Create Framebuffer with Depth buffer for Shadow mapping --------------------------------------
    unsigned int depthMapFbo;
    glGenFramebuffers(1, &depthMapFbo);
    unsigned int depthMap;

    createTexture(depthMap);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);
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
    Shader skyboxShader("../resources/shaders/skybox.vs", "../resources/shaders/skybox.fs");

    Model model("../resources/objects/Monopoly/szene.obj");
    Model monopoly("../resources/objects/Monopoly/monopoly.obj");
    
    Model hatModel("../resources/objects/Modelle/Zylinder.obj");
    Model cartModel("../resources/objects/Modelle/Schubkarre.obj");
    Model thimbleModel("../resources/objects/Modelle/Fingerhut.obj");
    Model shipModel("../resources/objects/Modelle/Schiff.obj");
    Model houseSet("../resources/objects/Modelle/haus_set.obj");
    Model hotelSet("../resources/objects/Modelle/hotel_set.obj");

    //-------------------------------- Transform models to correct position --------------------------------------
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 20.0f);
    glm::mat4 startMatrix = glm::mat4(1.0f);
    glm::vec3 directionVector(1.0f, 0.0f, 0.0f);
    startMatrix = glm::scale(startMatrix, glm::vec3(0.01f));

    startMatrix = glm::translate(startMatrix, glm::vec3(172.0f, 87.0f, 197.0f));
    pieces.push_back(Piece(&hatModel, startMatrix, camera.GetViewMatrix(), projection, directionVector));

    startMatrix = glm::translate(startMatrix, glm::vec3(1.0f, 0.0f, -4.0f));
    pieces.push_back(Piece(&cartModel, startMatrix, camera.GetViewMatrix(), projection, directionVector));

    startMatrix = glm::translate(startMatrix, glm::vec3(4.0f, 0.0f, 1.0f));
    pieces.push_back(Piece(&thimbleModel, startMatrix, camera.GetViewMatrix(), projection, directionVector));

    startMatrix = glm::translate(startMatrix, glm::vec3(-1.0f, -1.0f, 4.0f));
    pieces.push_back(Piece(&shipModel, startMatrix, camera.GetViewMatrix(), projection, directionVector));
    pieces.push_back(Piece(&houseSet, glm::mat4(1.0f), camera.GetViewMatrix(), projection, directionVector));
    pieces.push_back(Piece(&hotelSet, glm::mat4(1.0f), camera.GetViewMatrix(), projection, directionVector));

    std::vector<std::string> faces{
        "../resources/objects/Skybox/Right.bmp",
        "../resources/objects/Skybox/Left.bmp",
        "../resources/objects/Skybox/Top.bmp",
        "../resources/objects/Skybox/Bottom.bmp",
        "../resources/objects/Skybox/Front.bmp",
        "../resources/objects/Skybox/Back.bmp"
    };
    unsigned int cubemapTexture = loadCubemap(faces);


    //-------------------------------- Define lights --------------------------------------

    lights.push_back(Light(glm::vec3(5.55f, 7.5f, 5.3f), glm::vec3(1.0f, 1.0f, 1.0f), -135.0f, -30.0f));
    lights.push_back(Light(glm::vec3(10.7f, 7.25f, -2.94f), glm::vec3(1.0f, 1.0f, 1.0f), -194.0f, -25.0f));

    float near_plane = 0.1f;
    float far_plane = 20.0f;

    // -----------
    while (!glfwWindowShouldClose(window)){

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //-------------------------------- First Pass --------------------------------------
        glBindTexture(GL_TEXTURE_2D, depthMap);
        renderShadowMap(depthShader, depthMapFbo, model, monopoly, lights[0]);
        glBindTexture(GL_TEXTURE_2D, 0);
        //-------------------------------- Second pass --------------------------------------

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
       
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        glm::mat4 lightProjection =  glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.1f, 20.0f);
        glm::mat4 lightSpaceMatrix = lightProjection * lights[0].camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 20.0f);


        glEnable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        modelShader.use();
        modelShader.setInt("shadowMap", depthMap);
        modelShader.setVec3("lightColor", lights[0].color);
        modelShader.setVec3("lightPos", lights[0].position);
        modelShader.setVec3("viewPos", camera.Position);
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view",  camera.GetViewMatrix());
        modelShader.setMat4("model", modelMatrix);
        modelShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        modelShader.setFloat("bias", 0.0003);

        //enable Gamma Correction
        glEnable(GL_FRAMEBUFFER_SRGB); 

        modelShader.setInt("hasTexture", false);
        model.draw(modelShader);
        modelShader.setFloat("bias", 0.00001);
        modelShader.setInt("hasTexture", true);
        monopoly.draw(modelShader);


        for (size_t i = 0; i < pieces.size(); i++){
            pieces[i].draw(modelShader);
        }

        glDepthFunc(GL_LEQUAL); 
        skyboxShader.use();
        glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        
        glBindVertexArray(skyboxVao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); 

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

void framebufferSizeCallback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn){
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

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadCubemap(std::vector<std::string> faces){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}