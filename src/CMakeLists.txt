add_executable(main main.c tcp/tcp_server.c)
add_executable(client_test tcp/tcp_client.c)
add_executable(judge judge/judge.c)

target_compile_definitions(client_test PRIVATE TEST_TCP_CLIENT)

# target_include_directories(main PRIVATE ${PROJECT_SOURCE_DIR}/include)