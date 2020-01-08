file(GLOB_RECURSE SRCS ./*.cpp)
file(GLOB_RECURSE HDRS ./*.h)

add_executable(bezier ${SRCS} ${HDRS})
target_link_libraries(bezier pmp_gl pmp imgui glfw glew)
