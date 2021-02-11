#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "opengl/texture.hpp"
#include "opengl/buffers.hpp"
#include "opengl/shader.hpp"

#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class model3d {
    struct mesh {
        struct vertex {
            glm::vec3 pos, nrm;
            glm::vec2 tex = {0, 0};
        };
        struct texture {
            opengl::texture2d gl_tex;
            std::string type;
        };

        opengl::vertex_array vao;
        opengl::vertex_buffer vbo;
        opengl::index_buffer ibo;
    };

    std::vector<mesh> meshes;
    std::string directory;

    void process_node(const aiScene *scene, aiNode *node) {
        for (unsigned int mesh_iter = 0; mesh_iter < node->mNumMeshes; ++mesh_iter) {
            aiMesh *assimp_mesh = scene->mMeshes[node->mMeshes[mesh_iter]];

            mesh result_mesh;
            std::vector<mesh::vertex> mesh_vertices;
            std::vector<unsigned int> mesh_indices;
            std::vector<mesh::texture> mesh_textures;

            for (unsigned int vertex_iter = 0; vertex_iter < assimp_mesh->mNumVertices; ++vertex_iter) {
                mesh::vertex result_vertex;

                result_vertex.pos.x = assimp_mesh->mVertices[vertex_iter].x;
                result_vertex.pos.y = assimp_mesh->mVertices[vertex_iter].y;
                result_vertex.pos.z = assimp_mesh->mVertices[vertex_iter].z;

                result_vertex.nrm.x = assimp_mesh->mNormals[vertex_iter].x;
                result_vertex.nrm.y = assimp_mesh->mNormals[vertex_iter].y;
                result_vertex.nrm.z = assimp_mesh->mNormals[vertex_iter].z;

                if (assimp_mesh->mTextureCoords[0]) {
                    result_vertex.tex.x = assimp_mesh->mTextureCoords[0][vertex_iter].x;
                    result_vertex.tex.y = assimp_mesh->mTextureCoords[0][vertex_iter].y;
                }

                mesh_vertices.push_back(result_vertex);
            }

            for (unsigned int triangle_iter = 0; triangle_iter < assimp_mesh->mNumFaces; ++triangle_iter) {
                aiFace assimp_face = assimp_mesh->mFaces[triangle_iter];
                for (unsigned int index_iter = 0; index_iter < assimp_face.mNumIndices; ++index_iter)
                    mesh_indices.push_back(assimp_face.mIndices[index_iter]);
            }

            meshes.push_back(std::move(result_mesh));
        }

        for (unsigned int child_iter = 0; child_iter < node->mNumChildren; ++child_iter)
            process_node(scene, node->mChildren[child_iter]);
    }

  public:
    model3d(const std::string &filepath) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR assimp " << importer.GetErrorString() << "\n";
            return;
        }
        directory = filepath.substr(0, filepath.find_last_of('/'));
        process_node(scene, scene->mRootNode);
    }

    void draw() {
        for (auto &m : meshes) {
            m.vao.bind();
            m.ibo.bind();
            glDrawElements(GL_TRIANGLES, 0, GL_UNSIGNED_INT, 0);
        }
    }
};
