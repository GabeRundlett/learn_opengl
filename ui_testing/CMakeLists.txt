project(ui_testing)

add_executable(text_rendering "text.cpp")
target_link_libraries(text_rendering PUBLIC coel)

add_executable(ui_main "ui.cpp")
target_link_libraries(ui_main PUBLIC coel)
