

add_executable(example
        main.cpp
        )

target_link_libraries(example PRIVATE pyembedder)
target_link_libraries(example PRIVATE CommonLib)


