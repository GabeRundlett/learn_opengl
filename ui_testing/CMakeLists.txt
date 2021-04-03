project(ui_testing)

add_executable(text_rendering "text.cpp")
target_link_libraries(text_rendering PUBLIC coel)

add_executable(ui1 "ui1.cpp")
target_link_libraries(ui1 PUBLIC coel)

add_executable(ui2 "ui2.cpp")
target_link_libraries(ui2 PUBLIC coel)
