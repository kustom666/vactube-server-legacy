#ifndef QUOIL_VACTUBE_SERVICE
#define QUOIL_VACTUBE_SERVICE

#include <boost/log/trivial.hpp>

#include "quoil.grpc.pb.h"

using namespace boost::log::trivial;

class VactubeImpl final : public quoil::Vactube::Service {
  public:
  grpc::Status TextChat(grpc::ServerContext* context, grpc::ServerReaderWriter<quoil::TextMessage, quoil::TextMessage>* stream) override {
    quoil::TextMessage message;
    while(stream->Read(&message)) {
      auto t = std::time(nullptr);
      auto tm = *std::localtime(&t);
      std::unique_lock<std::mutex> lock(mu_);
      BOOST_LOG_TRIVIAL(info) << "[Message] " << message.username() << ": " << message.messagetext();
      history_.push_back(message);
    }
    return grpc::Status::OK;
  }

  private:
  std::mutex mu_;
  std::vector<quoil::TextMessage> history_;
};

#endif