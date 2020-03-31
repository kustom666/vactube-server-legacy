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
find_package(cxxopts CONFIG REQUIRED)
find_package(Pistache CONFIG REQUIRED)

find_path(BOTAN_INCLUDE_DIR botan/botan.h)
find_library(BOTAN_LIBRARY botan-2)

find_path(PICOJSON_INCLUDE_DIR picojson/picojson.h)

add_executable(vactube-server src/server.cc src/utilities.cc src/quoil.grpc.pb.cc src/quoil.pb.cc)
target_include_directories(vactube-server PRIVATE ${BOTAN_INCLUDE_DIR} ${PICOJSON_INCLUDE_DIR})
target_link_libraries(vactube-server PRIVATE ${BOTAN_LIBRARY} pistache_static gcov cxxopts::cxxopts gRPC::gpr gRPC::grpc gRPC::grpc++ gRPC::grpc++_alts yaml-cpp protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)
# target_link_libraries(vactube-server grpc++ yaml-cpp tink::static)