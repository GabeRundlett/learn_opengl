project(math_testing)

add_executable(${PROJECT_NAME} "main.cpp")
target_link_libraries(${PROJECT_NAME} PUBLIC coel)

add_executable(${PROJECT_NAME}_matrix2d "matrix2d.cpp")
target_link_libraries(${PROJECT_NAME}_matrix2d PUBLIC coel)
