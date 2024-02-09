#ifndef customMesh
#define customMesh

#include <glm/glm.hpp>
#include <vector>

#include "shader.h"

struct Vertex{
    glm::vec3 position;
    glm::vec3 normal;
};


class Mesh{
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        glm::vec4 color;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)){
            this->vertices = vertices;
            this->indices = indices;
            this->color = color;

            setupMesh();
        }

        void draw(Shader &shader){
                shader.setVec4("color", this->color);

                // draw mesh
                glBindVertexArray(this->VAO);
                glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);

                // always good practice to set everything back to defaults once configured.
                glActiveTexture(GL_TEXTURE0);
        }

    private:
        unsigned int VAO, VBO, EBO;

        void setupMesh(){
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);

            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);	
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

            glEnableVertexAttribArray(1);	
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));

            glEnableVertexAttribArray(2);	
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));

            glBindVertexArray(0);
        }
};


#endif