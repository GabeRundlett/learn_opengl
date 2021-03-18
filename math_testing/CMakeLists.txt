project(math_testing)

add_executable(${PROJECT_NAME} "main.cpp")
target_link_libraries(${PROJECT_NAME} PUBLIC coel)

target_link_options(${PROJECT_NAME} PUBLIC
    $<$<PLATFORM_ID:Windows>:
        # "/ENTRY:mainCRTStartup"
        # "/SUBSYSTEM:WINDOWS"
    >
)
