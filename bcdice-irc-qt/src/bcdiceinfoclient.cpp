#include "bcdiceinfoclient.h"

#include <chrono>
#include <memory>

#include <grpcpp/grpcpp.h>

#include "bcdice_info.grpc.pb.h"

#include "bcdiceversioninfo.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using bcdice_irc_proto::BCDiceInfoService;
using bcdice_irc_proto::GetBCDiceVersionInfoRequest;
using bcdice_irc_proto::GetBCDiceVersionInfoResponse;
using bcdice_irc_proto::StopRequest;
using bcdice_irc_proto::StopResponse;

using std::chrono::milliseconds;
using std::chrono::system_clock;
using std::chrono::time_point;

namespace {
constexpr int DeadlineMs = 200;
void setDeadline(ClientContext& context);
} // namespace

BCDiceInfoClient::BCDiceInfoClient(std::shared_ptr<Channel> channel)
    : stub_{BCDiceInfoService::NewStub(channel)} {
}

StatusWith<BCDiceVersionInfo> BCDiceInfoClient::getBCDiceVersionInfo() {

  GetBCDiceVersionInfoRequest request;
  GetBCDiceVersionInfoResponse response;

  ClientContext context;
  setDeadline(context);

  Status status = stub_->GetBCDiceVersionInfo(&context, request, &response);
  if (!status.ok()) {
    return {std::nullopt, status};
  }

  BCDiceVersionInfo versionInfo;
  versionInfo.bcdiceVersion = response.bcdice_version();
  versionInfo.bcdiceIrcVersion = response.bcdice_irc_version();

  return {versionInfo, status};
}

std::pair<bool, Status> BCDiceInfoClient::stop() {
  StopRequest request;
  StopResponse response;

  ClientContext context;
  setDeadline(context);

  Status status = stub_->Stop(&context, request, &response);
  if (!status.ok()) {
    return {false, status};
  }

  return {response.ok(), status};
}

namespace {

/**
 * @brief 呼び出しの締め切り時刻を設定する。
 * @param context
 */
void setDeadline(ClientContext& context) {
  time_point deadline = system_clock::now() + milliseconds(DeadlineMs);
  context.set_deadline(deadline);
}

} // namespace
