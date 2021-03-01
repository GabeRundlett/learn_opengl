#pragma once

#include <glm/glm.hpp>

#include <coel/opengl/texture.hpp>
#include <coel/opengl/buffers.hpp>
#include <coel/opengl/shader.hpp>

#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <memory>

namespace opengl {

    class model3d {
      public:
        struct vertex {
            glm::vec3 pos, nrm;
            glm::vec2 tex;
        };

        struct submesh {
            vertex_array vao;
            vertex_buffer vbo;
            index_buffer ibo;
            uint32_t index_count;

            std::vector<texture2d> textures;
        };

        std::vector<submesh> meshes;

      private:
        static void load_textures(std::vector<texture2d> &textures, aiMaterial *mat, aiTextureType type, const std::string &rootdir) {
            for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
                aiString str;
                mat->GetTexture(type, i, &str);
                textures.emplace_back(texture2d::configuration{
                    .filepath = (rootdir + '/' + str.C_Str()).c_str(),
                    .gl_format = GL_RGBA,
                    .filter = {.min = GL_LINEAR_MIPMAP_LINEAR},
                    .use_mipmap = true,
                });
            }
        }

        void process_node(aiNode *node, const aiScene *scene, const std::string &rootdir) {
            for (unsigned int i = 0; i < node->mNumMeshes; i++) {
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

                std::vector<vertex> vertices;
                std::vector<unsigned int> indices;

                for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
                    vertex temp_vertex;
                    temp_vertex.pos = *(glm::vec3 *)(mesh->mVertices + j);
                    temp_vertex.nrm = *(glm::vec3 *)(mesh->mNormals + j);
                    temp_vertex.tex = *(glm::vec3 *)(mesh->mTextureCoords[0] + j);
                    vertices.push_back(temp_vertex);
                }
                for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
                    aiFace face = mesh->mFaces[j];
                    for (unsigned int k = 0; k < face.mNumIndices; k++) {
                        if (face.mNumIndices != 3)
                            std::cout << "index count: " << face.mNumIndices << '\n';
                        indices.push_back(face.mIndices[k]);
                    }
                }

                submesh temp_mesh{
                    .vao = vertex_array(),
                    .vbo = vertex_buffer(vertices.data(), vertices.size() * sizeof(vertex)),
                    .ibo = index_buffer(indices.data(), indices.size() * sizeof(unsigned int)),
                    .index_count = (unsigned int)indices.size(),
                };

                aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

                load_textures(temp_mesh.textures, material, aiTextureType_DIFFUSE, rootdir);
                load_textures(temp_mesh.textures, material, aiTextureType_SPECULAR, rootdir);
                load_textures(temp_mesh.textures, material, aiTextureType_HEIGHT, rootdir);
                load_textures(temp_mesh.textures, material, aiTextureType_AMBIENT, rootdir);

                meshes.push_back(std::move(temp_mesh));
            }
            for (unsigned int i = 0; i < node->mNumChildren; i++)
                process_node(node->mChildren[i], scene, rootdir);
        }

      public:
        model3d(const std::string &filepath) {
            Assimp::Importer import;
            const aiScene *scene = import.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);
            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
                return;
            }
            process_node(scene->mRootNode, scene, filepath.substr(0, filepath.find_last_of('/')));
        }

        void draw() {
            for (const auto &mesh : meshes) {
                uint32_t slot = 0;
                for (const auto &tex : mesh.textures) {
                    if (slot > 31)
                        break;
                    tex.bind(slot);
                    ++slot;
                }
                mesh.vao.bind();
                mesh.ibo.bind();
                glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, nullptr);
            }
        }
    };

} // namespace opengl
