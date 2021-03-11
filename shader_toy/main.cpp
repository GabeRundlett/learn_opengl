#include <coel/application.hpp>
#include <coel/opengl/renderers/quad.hpp>

class my_app : public coel::application {
    opengl::renderer::quad quad;

    std::filesystem::path shader_path = std::filesystem::current_path() / "shader_toy/shaders/first.glsl";
    opengl::shader_program shader;
    opengl::shader_uniform u_viewport_dim;
    coel::clock::time_point prev_update_time;

  public:
    my_app() : coel::application({800, 600}, "Shader Toy") {
        use_vsync(true);
        reload_shader();
    }

    void reload_shader() {
        shader = opengl::shader_program(shader_path);
        u_viewport_dim = shader.find_uniform("viewport_dim");
        prev_update_time = coel::clock::now();
    }

    void on_draw() {
        shader.bind();
        opengl::shader_program::send(u_viewport_dim, glm::vec2(frame_dim));
        quad.draw();
    }

    void on_update(coel::duration) {
        using namespace std::chrono_literals;
        if (now - prev_update_time > 0.1s && is_active)
            reload_shader();
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
