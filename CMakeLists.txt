cmake_minimum_required(VERSION 3.14)
project(FileRetrievalEngine SYCL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-fsycl -fsycl-targets=nvptx64-nvidia-cuda)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(file_retrieval_engine ${SOURCES})
target_link_libraries(file_retrieval_engine PRIVATE sycl)