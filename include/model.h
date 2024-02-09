#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_images.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include "mesh.h"

class Model{
    public:

        std::vector<Mesh> meshes;


        Model(const std::string  &path){
            loadModel(path);
        }

        void draw(Shader &shader){
            for (size_t i = 0; i < this->meshes.size(); i++){
                meshes[i].draw(shader);
            }
            
        }

    private:

        void loadModel(const std::string& path){
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

            if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
                std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
                return;
            }

            processNode(scene->mRootNode, scene);
        }

        void processNode(aiNode *node, const aiScene *scene){
            for(unsigned int i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                aiMaterial *mtl = scene -> mMaterials[mesh->mMaterialIndex];

                glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                aiColor4D diffuse;
                if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
                    color = glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);

                Mesh outMesh = processMesh(mesh, scene);
                outMesh.color = color;
                meshes.push_back(outMesh);
            }

            for(unsigned int i = 0; i < node->mNumChildren; i++)
            {
                processNode(node->mChildren[i], scene);
            }
        }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene){
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; 

            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;

            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.normal = vector;
            }
            vertices.push_back(vertex);
        }

        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);        
        }
        
        return Mesh(vertices, indices);
    }
};


#endif