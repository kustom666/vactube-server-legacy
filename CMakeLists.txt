cmake_minimum_required(VERSION 3.11)
project(vactube-server)

# include(FetchContent)
# include(ExternalProject)
# set(BUILD_TESTING OFF)
# set(CXXOPTS_BUILD_EXAMPLES OFF)
# set(CXXOPTS_BUILD_TESTS OFF)
# set(CXXOPTS_ENABLE_INSTALL ON)
# set(CMAKE_CXX_STANDARD 11)

# add_subdirectory(third-party/tink)
# add_subdirectory(third-party/grpc)
# add_subdirectory(third-party/yaml-cpp)

find_package(gRPC CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(protobuf CONFIG REQUIRED)

add_executable(vactube-server src/server.cc src/quoil.grpc.pb.cc src/quoil.pb.cc)
target_link_libraries(vactube-server PRIVATE gRPC::gpr gRPC::grpc gRPC::grpc++ gRPC::grpc++_alts yaml-cpp protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)
# target_link_libraries(vactube-server grpc++ yaml-cpp tink::static)