cmake_minimum_required(VERSION 3.10)
project(Sendperplane-blog-backend C)
set(CMAKE_C_STANDARD 11)
find_package(PostgreSQL REQUIRED)
add_executable(app
 main.c
 server.c
 db.c
 user_handler.c
 utils.c
 auth.c
 routes.c
 main_handlers.c
 session_handler.c
 blog_handler.c
)
target_include_directories(app PRIVATE ${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(app
 ${PostgreSQL_LIBRARIES}
 microhttpd
 ssl
 crypto
)