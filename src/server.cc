#include <mutex>
#include <iostream>
#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>
#include "quoil.grpc.pb.h"

class VactubeImpl final : public quoil::Vactube::Service {
  public:
  grpc::Status TextChat(grpc::ServerContext* context, grpc::ServerReaderWriter<quoil::TextMessage, quoil::TextMessage>* stream) override {
    quoil::TextMessage message;
    while(stream->Read(&message)) {
      std::unique_lock<std::mutex> lock(mu_);
      std::cout << message.username() << ": " << message.messagetext() << std::endl;
      history_.push_back(message);
    }
    return grpc::Status::OK;
  }

  private:
  std::mutex mu_;
  std::vector<quoil::TextMessage> history_;
};

int main(int argc, char const *argv[])
{
  std::string server_address("0.0.0.0:50051");
  VactubeImpl service;
  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
  return 0;
}
