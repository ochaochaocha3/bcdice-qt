#ifndef BCDICEINFOCLIENT_H
#define BCDICEINFOCLIENT_H

#include <memory>
#include <optional>

#include <grpcpp/grpcpp.h>

#include "bcdice_info.grpc.pb.h"

#include "bcdiceversioninfo.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using bcdice_irc_proto::BCDiceInfoService;

template <typename T>
using StatusWith = std::pair<std::optional<T>, Status>;

class BCDiceInfoClient {
public:
  BCDiceInfoClient(std::shared_ptr<Channel> channel);
  StatusWith<BCDiceVersionInfo> getBCDiceVersionInfo();


private:
  std::unique_ptr<BCDiceInfoService::Stub> stub_;
};

#endif // BCDICEINFOCLIENT_H
