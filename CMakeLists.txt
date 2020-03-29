cmake_minimum_required(VERSION 3.11)
project(vactube-server)

include(FetchContent)
FetchContent_Declare(
  gRPC
  GIT_REPOSITORY https://github.com/grpc/grpc
  GIT_TAG        v1.25.0
)
FetchContent_MakeAvailable(gRPC)

add_executable(vactube-server src/server.cc src/quoil.grpc.pb.cc src/quoil.pb.cc)
target_link_libraries(vactube-server grpc++)