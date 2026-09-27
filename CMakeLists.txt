cmake_minimum_required(VERSION 3.10)

project(Sendperplane-blog-backend C)

set(CMAKE_C_STANDARD 11)
add_executable(Sendperplane-blog-backend
    main.c
    server.c
    db.c
    user_handler.c
    utils.c
    auth.c
    routes.c
    main_handlers.c
    blog_handler.c
)

target_link_libraries(Sendperplane-blog-backend
    microhttpd
    pq
    ssl
    crypto
)