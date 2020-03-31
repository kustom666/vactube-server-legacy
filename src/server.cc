#include <mutex>
#include <iomanip>
#include <thread>

#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/security/server_credentials.h>
#include <cxxopts.hpp>
#include <yaml-cpp/yaml.h>

#include "vactube_service.hpp"
#include "vactube_fileserve.hpp"
#include "utilities.hpp"

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
    Quoil::SetupWizard();
    std::exit(0);
  }
  
  YAML::Node config = YAML::LoadFile(args["config"].as<std::string>());
  std::string server_address = config["server_address"].as<std::string>();

  if(config["ssl"].IsDefined()) {
    std::string key = Quoil::ReadFile(config["ssl"]["key"].as<std::string>());
    std::string crt = Quoil::ReadFile(config["ssl"]["cert"].as<std::string>());
    grpc::SslServerCredentialsOptions::PemKeyCertPair keycert =
    {
      key,
      crt
    };
    auto ssl_opts = grpc::SslServerCredentialsOptions();
    ssl_opts.pem_root_certs = "";
    ssl_opts.pem_key_cert_pairs.push_back(keycert);
    auto server_creds = grpc::SslServerCredentials(ssl_opts);
    builder.AddListeningPort(server_address, server_creds);
  }
  else {
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  }
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Chat Server listening on " << server_address << std::endl;

  Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(config["discovery_port"].as<int>()));
  auto opts = Http::Endpoint::options().threads(1);
  Http::Endpoint http_server(addr);
  http_server.init(opts);
  http_server.setHandler(std::make_shared<CertHandler>(config));
  http_server.serve();
  server->Wait();
  return 0;
}
