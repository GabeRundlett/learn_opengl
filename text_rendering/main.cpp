#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <coel/application.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

struct glyph_edge {
    glm::vec2 start, end, c0, c1;
    enum edge_type {
        invalid,
        linear,
        quadratic,
        cubic,
    };
};

struct glyph_contour {
    std::vector<glyph_edge> edges;
    std::vector<glyph_edge::edge_type> edge_types;
    glm::vec2 pen_pos;

    int move_to(const FT_Vector *to) {
        pen_pos = {to->x, to->y};
        return 0;
    }
    int line_to(const FT_Vector *to) {
        glm::vec2 endpoint = {to->x, to->y};
        if (endpoint != pen_pos) {
            edges.push_back({pen_pos, endpoint, {}, {}});
            edge_types.push_back(glyph_edge::linear);
            pen_pos = {to->x, to->y};
        }
        return 0;
    }
    int conic_to(const FT_Vector *to, const FT_Vector *c0) {
        edges.push_back({pen_pos, {to->x, to->y}, {c0->x, c0->y}, {}});
        edge_types.push_back(glyph_edge::quadratic);
        pen_pos = {to->x, to->y};
        return 0;
    }
    int cubic_to(const FT_Vector *to, const FT_Vector *c0, const FT_Vector *c1) {
        edges.push_back({pen_pos, {to->x, to->y}, {c0->x, c0->y}, {c1->x, c1->y}});
        edge_types.push_back(glyph_edge::cubic);
        pen_pos = {to->x, to->y};
        return 0;
    }
};

class freetype_typeface {
    static inline FT_Library lib = {};

  public:
    FT_Face face;

    freetype_typeface(const char *const filepath, int size) {
        auto err = FT_Init_FreeType(&lib);
        if (err) {
            fmt::print("freetype failed to initialize\n");
            return;
        }

        err = FT_New_Face(lib, filepath, 0, &face);
        if (err == FT_Err_Unknown_File_Format) {
            fmt::print("The file '{}' has an unknown file format\n", filepath);
            return;
        } else if (err) {
            fmt::print("The file '{}' could not be read\n", filepath);
            return;
        }

        fmt::print(
            R"(loaded typeface '{}'
  Glyph count: {}
  Face flags:  {:#b}
  Units/EM:    {}
  Size count:  {}
  Sizes ptr:   {}
)",
            filepath,
            face->num_glyphs,
            face->face_flags,
            face->units_per_EM,
            face->num_fixed_sizes,
            (void *)face->available_sizes);

        err = FT_Set_Char_Size(face, 0, size * 64, 300, 300);
        if (err) {
            fmt::print("Unable to set char size to {}\n", size);
            return;
        }
    }

    glyph_contour get_contour(unsigned int charcode) {
        auto glyph_index = FT_Get_Char_Index(face, charcode);
        const auto load_flags = FT_LOAD_DEFAULT;
        const auto render_mode = FT_RENDER_MODE_NORMAL;
        auto err = FT_Load_Glyph(face, glyph_index, load_flags);
        if (err) {
            fmt::print("Failed to load glyph '{}'\n", charcode);
            return {};
        }
        glyph_contour result;
        FT_Outline_Funcs outline_funcs = {
            .move_to = [](const FT_Vector *to, void *user) { return reinterpret_cast<glyph_contour *>(user)->move_to(to); },
            .line_to = [](const FT_Vector *to, void *user) { return reinterpret_cast<glyph_contour *>(user)->line_to(to); },
            .conic_to = [](const FT_Vector *c0, const FT_Vector *to, void *user) { return reinterpret_cast<glyph_contour *>(user)->conic_to(to, c0); },
            .cubic_to = [](const FT_Vector *c0, const FT_Vector *c1, const FT_Vector *to, void *user) { return reinterpret_cast<glyph_contour *>(user)->cubic_to(to, c0, c1); },
            .shift = 0,
            .delta = 0,
        };
        err = FT_Outline_Decompose(&face->glyph->outline, &outline_funcs, &result);
        if (err) {
            fmt::print("Failed to FT_Outline_Decompose\n");
            return {};
        }
        return result;
    }

    ~freetype_typeface() {
    }
};

class my_app : public coel::application {
  public:
    freetype_typeface arial = freetype_typeface("C:/Windows/Fonts/consola.ttf", 16);

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
    opengl::shader_program shader = opengl::shader_program({.filepath = "text_rendering/assets/quad_vert.glsl"}, {.filepath = "text_rendering/assets/quad_frag.glsl"});

    my_app() : coel::application({400, 400}, "text rendering") {
        use_raw_mouse(true);
        on_char('a');

        vao.bind();
        opengl::vertex_array::set_layout<glm::vec2>();
        vao.unbind();
    }

    void on_draw() {
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.bind();

        vao.bind();
        glDrawArrays(GL_QUADS, 0, 4);
    }

    void on_pause() { set_mouse_capture(false); }
    void on_resume() { set_mouse_capture(true); }

    void on_char(unsigned int charcode) override {
        auto glyph = arial.get_contour(charcode);
        shader.send_array(shader.find_uniform("u_glyph_edges"), (const glm::vec4 *)glyph.edges.data(), (unsigned int)glyph.edges.size() * 2);
        opengl::shader_program::send(shader.find_uniform("u_glyph_edge_count"), (int)glyph.edges.size());
        opengl::shader_program::send(shader.find_uniform("u_glyph_scale"), 1.0f / arial.face->units_per_EM);
    }
};

int main() try {
    my_app game;
    if (!game)
        return -1;
    game.resize();
    while (game.update()) {
    }
} catch (const coel::exception &e) {
    MessageBoxA(nullptr, e.what(), "Coel Exception", MB_OK);
}