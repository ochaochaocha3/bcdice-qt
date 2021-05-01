#ifndef BCDICEINFOCLIENT_H
#define BCDICEINFOCLIENT_H

#include <memory>
#include <optional>

#include <QList>

#include <grpcpp/grpcpp.h>

#include "bcdice_info.grpc.pb.h"

#include "bcdiceversioninfo.h"
#include "gamesystem.h"

using bcdice_irc_proto::BCDiceInfoService;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

template <typename T> using StatusWith = std::pair<std::optional<T>, Status>;

class BCDiceInfoClient {
public:
  BCDiceInfoClient(std::shared_ptr<Channel> channel);
  StatusWith<BCDiceVersionInfo> getBCDiceVersionInfo();
  StatusWith<QList<GameSystem>> getDiceBotList();
  std::pair<bool, Status> stop();

private:
  std::unique_ptr<BCDiceInfoService::Stub> stub_;
};

#endif // BCDICEINFOCLIENT_H
