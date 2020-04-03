#ifndef QUOIL_VACTUBE_FILESERVE
#define QUOIL_VACTUBE_FILESERVE

#include <fstream>
#include <restinio/all.hpp>
#include <picojson/picojson.h>
#include <yaml-cpp/yaml.h>

#include "utilities.hpp"

//class CertHandler
//{
//public:
//  CertHandler(YAML::Node config) : grpc_server_config(config) {}
//  restinio::request_handling_status_t handler(restinio::request_handle_t req)
//  {
//    if (restinio::http_method_get() == req->header().method() && req->header().request_target() == "/")
//    {
//      picojson::object data;
//      data["server_name"] = picojson::value(grpc_server_config["server_name"].as<std::string>());
//      data["grpc_endpoint"] = picojson::value(grpc_server_config["server_address"].as<std::string>());
//      data["certificate"] = picojson::value(Quoil::ReadFile(grpc_server_config["ssl"]["cert"].as<std::string>()));
//      req->create_response().append_header(restinio::http_field::content_type, "application/json").set_body(picojson::value(data).serialize()).done();
//      return restinio::request_accepted();
//    }
//    return restinio::request_rejected();
//  }
//
//private:
//  YAML::Node grpc_server_config;
//};

restinio::request_handling_status_t discovery_handler(restinio::request_handle_t req, YAML::Node grpc_server_config)
{
	if (restinio::http_method_get() == req->header().method() && req->header().request_target() == "/")
	{
		picojson::object data;
		data["server_name"] = picojson::value(grpc_server_config["server_name"].as<std::string>());
		data["grpc_endpoint"] = picojson::value(grpc_server_config["server_address"].as<std::string>());
		data["certificate"] = picojson::value(Quoil::ReadFile(grpc_server_config["ssl"]["cert"].as<std::string>()));
		req->create_response().append_header(restinio::http_field::content_type, "application/json").set_body(picojson::value(data).serialize()).done();
		return restinio::request_accepted();
	}
	return restinio::request_rejected();
}

#endif