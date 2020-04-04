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
#include <leveldb/db.h>

#include "vactube_service.hpp"
#include "vactube_fileserve.hpp"
#include "utilities.hpp"

using namespace boost::log::trivial;

int main(int argc, char **argv)
{
  VactubeImpl service;
  grpc::ServerBuilder builder;

  // Options Parsing
  cxxopts::Options options("vactube-server", "The server part of vactube. A drop in replacement for cloud chatting platforms");
  options.add_options()("c,config", "Path to the yaml config file", cxxopts::value<std::string>())("g,generate", "Generate the configuration files and ssl certificates", cxxopts::value<bool>()->default_value("false"))("h,help", "Print this help message");
  auto args = options.parse(argc, argv);

  // Display the help if no config / generate or help requested
  if ((!args.count("config") && !args.count("generate")) || args.count("help"))
  {
    std::cout << options.help();
    std::exit(0);
  }

  // Generate a config file, TLS cert and exit
  if (args.count("generate"))
  {
    Quoil::SetupWizard();
    std::exit(0);
  }

  // Logging Setup
  Quoil::SetupLogging();

  // Server rooms database setup
  leveldb::DB* db;
  leveldb::Options db_options;
  db_options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(db_options, "server.db", &db);
  if (!status.ok()) {
    BOOST_LOG_TRIVIAL(error) << "";
  }

  // Reading the config from disk
  YAML::Node config;
  std::string server_address = "0.0.0.0";
  try {
	  config = YAML::LoadFile(args["config"].as<std::string>());
	  server_address = config["server_address"].as<std::string>();
  }
  catch (YAML::BadFile) {
    BOOST_LOG_TRIVIAL(error) << "Couldn't load the configuration file "
      << args["config"].as<std::string>()
      << " Check the file exists at that location. If you need to generate a config file, look at the generate option";
	  std::exit(1);
  }
  catch (YAML::ParserException) {
    BOOST_LOG_TRIVIAL(error) << "The configuration file " 
      << args["config"].as<std::string>() 
      << " Is incorrect or corrupted. Check that it's a valid yaml configuration file. If you need to generate a config file, look at the generate option";
	  std::exit(1);
  }
  catch (...) {
    BOOST_LOG_TRIVIAL(error) << "An unknown error happened when trying to read the configuration file";
    std::exit(1);
  }

  // Start in secure mode
  if (config["ssl"].IsDefined())
  {
    std::string key = Quoil::ReadFile(config["ssl"]["key"].as<std::string>());
    std::string crt = Quoil::ReadFile(config["ssl"]["cert"].as<std::string>());
    grpc::SslServerCredentialsOptions::PemKeyCertPair keycert =
        {
            key,
            crt};
    auto ssl_opts = grpc::SslServerCredentialsOptions();
    ssl_opts.pem_root_certs = "";
    ssl_opts.pem_key_cert_pairs.push_back(keycert);
    auto server_creds = grpc::SslServerCredentials(ssl_opts);
    builder.AddListeningPort(server_address, server_creds);
  }
  else // start in insecure mode
  {
    BOOST_LOG_TRIVIAL(warning) << "The text, voice and video server is starting in insecure mode. Insecure mode is for development purposes only, please use the generate option to create a secure environement";
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  }
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  BOOST_LOG_TRIVIAL(info) << "Chat Server listening on " << server_address;

  using discovery_server_t = restinio::http_server_t<restinio::default_traits_t>;
  discovery_server_t discovery_server{ restinio::own_io_context(), [config](auto &settings) {
	  settings.port(config["discovery_port"].as<int>());
	  settings.address("0.0.0.0");
	  settings.request_handler([config](auto req) {
		  return discovery_handler(req, config);
	  });
  } };

  std::thread restinio_control_thread{ [&discovery_server] {
	restinio::run(restinio::on_thread_pool(4, restinio::skip_break_signal_handling(), discovery_server));
  } };
  server->Wait();
  restinio::initiate_shutdown(discovery_server);
  restinio_control_thread.join();
  return 0;
}
