#ifndef QUOIL_VACTUBE_FILESERVE
#define QUOIL_VACTUBE_FILESERVE

#include <fstream>
#include <pistache/endpoint.h>
#include <picojson/picojson.h>
#include <yaml-cpp/yaml.h>

#include "utilities.hpp"

using namespace Pistache;

class CertHandler : public Http::Handler {
public:
  HTTP_PROTOTYPE(CertHandler)
  CertHandler(YAML::Node config) : grpc_server_config(config) {}
  void onRequest(const Http::Request& request, Http::ResponseWriter response) {
    picojson::object data;
    data["server_name"] = picojson::value(grpc_server_config["server_name"].as<std::string>());
    data["grpc_endpoint"] = picojson::value(grpc_server_config["server_address"].as<std::string>());
    data["certificate"] = picojson::value(Quoil::ReadFile(grpc_server_config["ssl"]["cert"].as<std::string>()));
    response.send(Http::Code::Ok, picojson::value(data).serialize());
  }
private:
  YAML::Node grpc_server_config;
};

#endif