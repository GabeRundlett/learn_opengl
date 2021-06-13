project(shader_toy)

add_executable(${PROJECT_NAME}_simple "simple.cpp")
target_link_libraries(${PROJECT_NAME}_simple PUBLIC coel)

target_link_options(${PROJECT_NAME}_simple PUBLIC
    $<$<PLATFORM_ID:Windows>:
        "/ENTRY:mainCRTStartup"
        "/SUBSYSTEM:WINDOWS"
    >
)

add_executable(${PROJECT_NAME}_raymarch "raymarch.cpp")
target_link_libraries(${PROJECT_NAME}_raymarch PUBLIC coel)

target_link_options(${PROJECT_NAME}_raymarch PUBLIC
    $<$<PLATFORM_ID:Windows>:
        "/ENTRY:mainCRTStartup"
        "/SUBSYSTEM:WINDOWS"
    >
)
