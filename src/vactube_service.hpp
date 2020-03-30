#include "quoil.grpc.pb.h"
class VactubeImpl final : public quoil::Vactube::Service {
  public:
  grpc::Status TextChat(grpc::ServerContext* context, grpc::ServerReaderWriter<quoil::TextMessage, quoil::TextMessage>* stream) override {
    quoil::TextMessage message;
    while(stream->Read(&message)) {
      auto t = std::time(nullptr);
      auto tm = *std::localtime(&t);
      std::unique_lock<std::mutex> lock(mu_);
      std::cout << std::put_time(&tm, "[%d-%m-%Y %H-%M-%S] ") << message.username() << ": " << message.messagetext() << std::endl;
      history_.push_back(message);
    }
    return grpc::Status::OK;
  }

  private:
  std::mutex mu_;
  std::vector<quoil::TextMessage> history_;
};