#include <coel/application.hpp>
#include <filesystem>

class my_app : public coel::application {

    // clang-format off
    static inline constexpr std::array quad_vertices = {
        -1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
         1.0f, -1.0f,
    };
    // clang-format on

    opengl::vertex_array vao;
    opengl::vertex_buffer vbo = opengl::vertex_buffer(quad_vertices.data(), quad_vertices.size() * sizeof(quad_vertices[0]));
    opengl::shader_program shader;
    opengl::shader_uniform u_viewport_dim;

  public:
    my_app() : coel::application({800, 600}, "Shader Toy") {
        use_vsync(false);

        vao.bind();
        vbo.bind();
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, reinterpret_cast<const void *>(0));
        vao.unbind();

        reload_shader();
    }

    void reload_shader() {
        shader = opengl::shader_program("shader_toy/shaders/first.glsl");
        u_viewport_dim = shader.find_uniform("viewport_dim");
    }

    void on_draw() {
        shader.bind();
        shader.send(u_viewport_dim, glm::vec2(frame_dim));

        vao.bind();
        glDrawArrays(GL_QUADS, 0, 4);
    }
};

int main() {
    my_app game;
    if (!game)
        return -1;
    game.resize();
    while (game.update()) {
    }
}
