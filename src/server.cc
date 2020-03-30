#include <mutex>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <yaml-cpp/yaml.h>
#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/security/server_credentials.h>
// #include <tink/config/tink_config.h>
#include "cxxopts.hpp"
#include "vactube_service.hpp"

std::string ReadFile(const std::string filename) {
  std::ifstream t(filename);
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

void genCerts() {

}

int main(int argc, char **argv)
{
  VactubeImpl service;
  grpc::ServerBuilder builder;

  // Options Parsing
  cxxopts::Options options("vactube-server", "The server part of vactube. A drop in replacement for cloud chatting platforms");
  options.add_options()("c,config", "Path to the yaml config file", cxxopts::value<std::string>())
                       ("g,generate", "Generate the configuration files and ssl certificates", cxxopts::value<bool>()->default_value("false"))
                       ("h,help", "Print this help message");
  auto args = options.parse(argc, argv);
  
  // Display the help if no config / generate or help requested
  if((!args.count("config") && !args.count("generate")) || args.count("help")) {
    std::cout << options.help();
    std::exit(0);
  }


  if(args.count("generate")) {

  }
  
  YAML::Node config = YAML::LoadFile(args["config"].as<std::string>());
  std::string server_address = config["server_address"].as<std::string>();

  if(config["ssl"].IsDefined()) {
    std::cout << "Starting server in secure mode" << std::endl;
    std::string key = ReadFile(config["ssl"]["key"].as<std::string>());
    std::string crt = ReadFile(config["ssl"]["crt"].as<std::string>());
    std::string ca = ReadFile(config["ssl"]["ca"].as<std::string>());
    std::cout << key << std::endl;
    std::cout << crt << std::endl;
    std::cout << ca << std::endl;
    grpc::SslServerCredentialsOptions::PemKeyCertPair keycert =
    {
      key,
      crt
    };
    auto ssl_opts = grpc::SslServerCredentialsOptions();
    ssl_opts.pem_root_certs = ca;
    ssl_opts.pem_key_cert_pairs.push_back(keycert);

    auto server_creds = grpc::SslServerCredentials(ssl_opts);
    builder.AddListeningPort(server_address, server_creds);
  }
  else {
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  }
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
  return 0;
}
