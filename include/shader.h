#ifndef CustomShader
#define CustomShader




#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <glm/glm.hpp>



class Shader{
    public:
    unsigned int ID;
    const char* vertexShaderPath;
    const char* fragmentShaderPath;
    Shader(const char* vertexShaderPath, const char* fragmentShaderPath){
        this->vertexShaderPath = vertexShaderPath;
        this->fragmentShaderPath = fragmentShaderPath;

        std::string vertexCode;
        std::string fragmentCode;

        std::ifstream vertexFile;
        std::ifstream fragmentFile;

        vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        
        try{
            vertexFile.open(vertexShaderPath);
            fragmentFile.open(fragmentShaderPath);

            std::stringstream vertexStream(vertexCode);
            std::stringstream fragmentStream(fragmentCode);

            vertexStream << vertexFile.rdbuf();
            fragmentStream << fragmentFile.rdbuf();

            vertexCode = vertexStream.str();
            fragmentCode = fragmentStream.str();

            vertexFile.close();
            fragmentFile.close();
        }
        catch(const std::exception& e){
            std::cerr << "Error reading file: " << e.what() << std::endl;
        }

        const char* vCode = vertexCode.c_str();
        const char* fCode = fragmentCode.c_str();

        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vCode, NULL);
        glCompileShader(vertexShader);
        checkCompileErrors(vertexShader, "VERTEX");

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fCode, NULL);
        glCompileShader(fragmentShader);
        checkCompileErrors(fragmentShader, "FRAGMENT");
        
        this->ID = glCreateProgram();
        glAttachShader(this->ID, vertexShader);
        glAttachShader(this->ID, fragmentShader);
        glLinkProgram(this->ID);
        checkCompileErrors(this->ID, "PROGRAM");

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void use(){
        glUseProgram(this->ID);
    }

    void setInt(const char* name, int val){
        glUniform1i(glGetUniformLocation(ID, name), val);     
    }

    void setFloat(const char* name, float val){
        glUniform1f(glGetUniformLocation(ID, name), val);     
    }

    void setVec2(const char* name, glm::vec2 vec){
        glUniform2fv(glGetUniformLocation(this->ID, name), 1, &vec[0]);
    }

    void setVec3(const char* name, glm::vec3 vec){
        glUniform3fv(glGetUniformLocation(this->ID, name), 1, &vec[0]);
    }

    void setVec4(const char* name, glm::vec4 vec){
        glUniform4fv(glGetUniformLocation(this->ID, name), 1, &vec[0]);
    }

    void setMat4(const char* name, glm::mat4 mat){
        glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, GL_FALSE, &mat[0][0]);
    }

private:
    void checkCompileErrors(GLuint shader, std::string type){
        GLint success;
        GLchar infoLog[1024];
        if(type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type <<  " with: " << this->vertexShaderPath <<  " or " << this->fragmentShaderPath  << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type  <<  " with: " << this->vertexShaderPath <<  " or " << this->fragmentShaderPath << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};


#endif