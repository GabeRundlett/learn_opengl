#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <coel/opengl/renderers/ui_batch.hpp>
#include <coel/opengl/renderers/text_batch.hpp>

#include <vector>

namespace graphics {
    struct container {
    };

    struct component_label {
        std::string text;
        glm::vec4 color_focused, color_unfocused;
        glm::vec2 offset = {0, 0};
        float scale = 1.0f;
    };

    struct button {
        glm::vec2 pos, size;
        glm::vec4 color_focused, color_unfocused;
        component_label label;

        void *user_ptr = nullptr;
        void (*on_press)(void *) = nullptr;
        void (*on_release)(void *) = nullptr;

        bool is_hovered = false, is_pressed = false;

        void hover(const glm::vec2 mpos) {
            is_hovered = mpos.x > pos.x && mpos.x < pos.x + size.x &&
                         mpos.y > pos.y && mpos.y < pos.y + size.y;
        }

        void press() {
            if (is_hovered) {
                if (on_press)
                    on_press(user_ptr);
                is_pressed = true;
            }
        }

        void release() {
            if (is_pressed && on_release)
                on_release(user_ptr);
            is_pressed = false;
        }

        bool esc() {
            if (is_pressed) {
                is_pressed = false;
                return false;
            }
            return true;
        }

        void draw(opengl::renderer::ui_batch &ui_batch, opengl::renderer::text_batch &text_batch) const {
            ui_batch.submit_rect(pos, size, is_hovered ? color_focused : color_unfocused);
            text_batch.submit(pos + label.offset, label.text.c_str(), label.scale, is_hovered ? label.color_focused : label.color_unfocused);
        }
    };

    struct slider_range {
        float min = 0.0f, max = 1.0f;
        float convert(float val) {
            return val * (max - min) + min;
        }
    };

    struct slider {
        glm::vec2 pos, size;

        slider_range range;
        float value = 0.5f, prev_value = value;
        bool is_hovered = false, is_pressed = false;

        component_label label;

        void *user_ptr = nullptr;
        void (*on_press)(void *, float) = nullptr;
        void (*on_release)(void *, float) = nullptr;
        void (*on_drag)(void *, float) = nullptr;
        void (*on_change)(void *, float) = nullptr;

        void calc_value(const float mpos_x) {
            value = (mpos_x - pos.x) / size.x;
            if (value < 0)
                value = 0;
            if (value > 1)
                value = 1;
        }

        void hover(const glm::vec2 mpos) {
            is_hovered = mpos.x > pos.x && mpos.x < pos.x + size.x &&
                         mpos.y > pos.y && mpos.y < pos.y + size.y;
        }

        void press(const glm::vec2 mpos) {
            if (is_hovered) {
                calc_value(mpos.x);
                is_pressed = true;
                if (on_press)
                    on_press(user_ptr, range.convert(value));
                if (on_change)
                    on_change(user_ptr, range.convert(value));
            }
        }

        void drag(const glm::vec2 mpos) {
            if (is_pressed) {
                calc_value(mpos.x);
                if (on_drag)
                    on_drag(user_ptr, range.convert(value));
                if (on_change)
                    on_change(user_ptr, range.convert(value));
            }
        }

        void release(const glm::vec2 mpos) {
            if (is_pressed) {
                calc_value(mpos.x);
                prev_value = value;
                if (on_release)
                    on_release(user_ptr, range.convert(value));
                if (on_change)
                    on_change(user_ptr, range.convert(value));
            }
            is_pressed = false;
        }

        bool esc() {
            if (is_pressed) {
                is_pressed = false;
                value = prev_value;
                return false;
            }
            return true;
        }

        void draw(opengl::renderer::ui_batch &batch_indexed, opengl::renderer::text_batch &text_batch) const {
            batch_indexed.submit_rect(pos, size, is_hovered ? glm::vec4{0.15, 0.15, 0.3, 1} : glm::vec4{0.1, 0.1, 0.2, 1});
            batch_indexed.submit_rect(pos, {size.x * value, size.y}, is_hovered ? glm::vec4{0.5, 0.5, 0.8, 1} : glm::vec4{0.4, 0.4, 0.7, 1});
            text_batch.submit(pos + label.offset, label.text.c_str(), label.scale, is_hovered ? label.color_focused : label.color_unfocused);
        }
    };

    struct checkbox {
        glm::vec2 pos, size;
        void (*on_change)(bool) = nullptr;

        bool is_hovered = false, is_checked = false;

        void hover(const glm::vec2 mpos) {
            is_hovered = mpos.x > pos.x && mpos.x < pos.x + size.x &&
                         mpos.y > pos.y && mpos.y < pos.y + size.y;
        }

        void press() {
            if (is_hovered) {
                is_checked = !is_checked;
                if (on_change)
                    on_change(is_checked);
            }
        }

        bool esc() { return false; }

        void draw(opengl::renderer::ui_batch &batch_indexed) const {
            if (is_hovered)
                batch_indexed.submit_rect(pos - size * 0.1f, size * 1.2f, glm::vec4{0.4, 0.4, 0.6, 1});
            batch_indexed.submit_rect(pos, size, glm::vec4{0.1, 0.1, 0.2, 1});
            if (is_checked)
                batch_indexed.submit_rect(pos + size * 0.1f, size * 0.8f, glm::vec4{0.4, 0.4, 0.7, 1});
        }
    };

    struct editable_textbox {
        glm::vec2 pos, size;
        void (*on_enter)(bool) = nullptr;
        std::string contents;
        int cursor_index = 0;

        bool is_hovered = false, is_focused = false;

        void hover(const glm::vec2 mpos) {
            is_hovered = mpos.x > pos.x && mpos.x < pos.x + size.x &&
                         mpos.y > pos.y && mpos.y < pos.y + size.y;
        }

        void press() {
            if (is_hovered) {
                if (!is_focused && on_enter)
                    on_enter(is_focused);
                is_focused = true;
            } else {
                esc();
            }
        }

        bool esc() {
            if (is_focused) {
                is_focused = false;
                return false;
            }
            return true;
        }

        bool process_key(int button, int, int action, int mods) {
            if (is_focused) {
                if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                    int cursor_seek_index = cursor_index;
                    switch (button) {
                    // seek left
                    case GLFW_KEY_BACKSPACE:
                    case GLFW_KEY_LEFT:
                        if (cursor_seek_index > 0)
                            --cursor_seek_index;
                        if (mods & GLFW_MOD_CONTROL) {
                            while (cursor_seek_index > 0 && contents[cursor_seek_index - 1] != ' ')
                                --cursor_seek_index;
                        }
                        break;
                    // seek right
                    case GLFW_KEY_DELETE:
                    case GLFW_KEY_RIGHT:
                        if (cursor_seek_index <= contents.size() - 1)
                            ++cursor_seek_index;
                        if (mods & GLFW_MOD_CONTROL) {
                            while (cursor_seek_index <= contents.size() - 1 && contents[cursor_seek_index - 1] != ' ')
                                ++cursor_seek_index;
                        }
                        break;
                    }
                    switch (button) {
                    case GLFW_KEY_BACKSPACE:
                        contents.erase(contents.begin() + cursor_seek_index, contents.begin() + cursor_index);
                        cursor_index = cursor_seek_index;
                        break;
                    case GLFW_KEY_DELETE:
                        contents.erase(contents.begin() + cursor_index, contents.begin() + cursor_seek_index);
                        break;
                    case GLFW_KEY_ENTER: process_char('\n'); break;
                    case GLFW_KEY_TAB: process_char('\t'); break;
                    case GLFW_KEY_LEFT:
                    case GLFW_KEY_RIGHT: cursor_index = cursor_seek_index; break;
                    default: break;
                    }
                }
                return true;
            }
            return false;
        }

        bool process_char(unsigned int codepoint) {
            if (is_focused) {
                contents.insert(contents.begin() + cursor_index, (char)codepoint);
                ++cursor_index;
                return true;
            }
            return false;
        }

        glm::vec2 calc_offset(const std::string &s, int ci, opengl::renderer::text_batch &text) const {
            glm::vec2 offset = {4.0f, 0};
            for (int i = 0; i < ci; ++i) {
                auto &c = s[i];
                if (c == '\n') {
                    offset.x = 4.0f;
                    offset.y += 14;
                } else {
                    offset.x += (float)text.get_glyph_info(s[i]).xadvance * 14.0f / 96.0f;
                }
            }
            return offset;
        }

        void draw(opengl::renderer::ui_batch &batch_indexed, opengl::renderer::text_batch &text) const {
            if (is_hovered)
                batch_indexed.submit_rect(pos - glm::vec2{1, 1}, size + glm::vec2{2, 2}, glm::vec4{0.4, 0.4, 0.6, 1});
            batch_indexed.submit_rect(pos, size, glm::vec4{0.1, 0.1, 0.2, 1});
            if (is_focused) {
                batch_indexed.submit_rect(pos + glm::vec2{1, 1}, size - glm::vec2{2, 2}, glm::vec4{0.2, 0.2, 0.4, 1});
                auto cursor_offset = calc_offset(contents, cursor_index, text);
                batch_indexed.submit_rect(pos + cursor_offset + glm::vec2{0, 2}, glm::vec2{1, 16}, glm::vec4{1, 1, 1, 1});
            }
            text.submit(pos + glm::vec2{2, 0}, contents, 14, {1, 1, 1, 1}, pos, size - glm::vec2{2, 2});
        }
    };

    struct menu_ui {
        opengl::renderer::ui_batch ui;
        opengl::renderer::text_batch text = opengl::renderer::text_batch("coel/assets/RobotoFontAtlas.png");

        std::vector<button> buttons;
        std::vector<slider> sliders;
        std::vector<checkbox> checkboxes;
        std::vector<editable_textbox> textboxes;
        glm::ivec2 frame_dim;

        void resize(glm::ivec2 size) {
            frame_dim = size;
            ui.resize(size);
            text.resize(size);
        }

        void mouse_move(const glm::vec2 mouse_pos) {
            for (auto &elem : buttons)
                elem.hover(mouse_pos);
            for (auto &elem : sliders) {
                elem.hover(mouse_pos);
                elem.drag(mouse_pos);
            }
            for (auto &elem : checkboxes)
                elem.hover(mouse_pos);
            for (auto &elem : textboxes)
                elem.hover(mouse_pos);
        }

        void mouse_press(const coel::mouse_button_event &e, const glm::vec2 mouse_pos) {
            if (e.action == GLFW_PRESS) {
                for (auto &elem : buttons)
                    elem.press();
                for (auto &elem : sliders)
                    elem.press(mouse_pos);
                for (auto &elem : checkboxes)
                    elem.press();
                for (auto &elem : textboxes)
                    elem.press();
            }
            if (e.action == GLFW_RELEASE) {
                for (auto &elem : buttons)
                    elem.release();
                for (auto &elem : sliders)
                    elem.release(mouse_pos);
            }
        }

        bool esc() {
            bool should_esc = true;
            for (auto &elem : buttons)
                if (!elem.esc())
                    should_esc = false;
            for (auto &elem : sliders)
                if (!elem.esc())
                    should_esc = false;
            for (auto &elem : checkboxes)
                if (!elem.esc())
                    should_esc = false;
            for (auto &elem : textboxes)
                if (!elem.esc())
                    should_esc = false;

            return should_esc;
        }

        bool process_key(const coel::key_event &e) {
            for (auto &elem : textboxes)
                if (elem.process_key(e.key, e.scancode, e.action, e.mods))
                    return true;
            return false;
        }

        bool process_char(unsigned int codepoint) {
            for (auto &elem : textboxes)
                if (elem.process_char(codepoint))
                    return true;
            return false;
        }

        void draw_settings() {
            ui.begin();
            text.begin();
            ui.submit_rect({0, 0}, {320, frame_dim.y}, {0.25, 0.28, 0.4, 1});

            for (auto &elem : buttons)
                elem.draw(ui, text);
            for (auto &elem : sliders)
                elem.draw(ui, text);
            for (auto &elem : checkboxes)
                elem.draw(ui);
            for (auto &elem : textboxes)
                elem.draw(ui, text);

            ui.end();
            ui.flush();
            text.end();
            text.flush();
        }
    };
} // namespace graphics
