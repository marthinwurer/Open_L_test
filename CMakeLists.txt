cmake_minimum_required(VERSION 3.8)

set(name Open_L_test)

project(${name})

set(CMAKE_CXX_STANDARD 17)


find_package(OpenCL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)
find_package(Threads REQUIRED)



file(GLOB SOURCE_FILES ./*.c)
add_executable(${name} ${SOURCE_FILES})

message(WARNING ${CMAKE_DL_LIBS})

target_link_libraries(${name} m OpenCL GLEW glfw3 ${OPENGL_gl_LIBRARY} ${CMAKE_DL_LIBS} ${X11_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
