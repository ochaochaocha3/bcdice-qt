#include "bcdiceinformationfetch.h"

#include <functional>

#include <QList>
#include <QObject>
#include <QThread>

#include <grpcpp/grpcpp.h>

#include "bcdiceinfoclient.h"
#include "bcdiceversioninfo.h"
#include "gamesystem.h"

BCDiceInformationFetch::BCDiceInformationFetch(QString grpcServerHost,
                                               unsigned int maxRetry,
                                               unsigned int retryIntervalMs,
                                               QObject* parent)
    : QObject{parent}, grpcServerHost_{grpcServerHost}, maxRetry_{maxRetry},
      retryIntervalMs_{retryIntervalMs}, stopped_{false} {
}

BCDiceInformationFetch::~BCDiceInformationFetch() = default;

void BCDiceInformationFetch::execute() {
  std::pair<std::function<bool()>, bool> fetchProcs[] = {
      {[this]() { return fetchVersionInformation(); }, true},
      {[this]() { return fetchGameSystemList(); }, false},
  };

  for (const auto& [p, wait] : fetchProcs) {
    if (!tryFetch(p, wait)) {
      emit failed();
      return;
    }
  }

  emit finished();
}

void BCDiceInformationFetch::stop() {
  stopped_ = true;
}

bool BCDiceInformationFetch::tryFetch(std::function<bool()> fetchProc,
                                      bool wait) {
  bool success = false;

  for (unsigned int i = 0; i < maxRetry_; ++i) {
    if (wait) {
      QThread::msleep(retryIntervalMs_);
    }

    if (stopped_) {
      return false;
    }

    success = fetchProc();
    if (success) {
      return true;
    }
  }

  return false;
}

bool BCDiceInformationFetch::fetchVersionInformation() {
  auto channel = grpc::CreateChannel(grpcServerHost_.toStdString(),
                                     grpc::InsecureChannelCredentials());
  BCDiceInfoClient client{channel};

  auto [versionInfo, status] = client.getBCDiceVersionInfo();

  if (!versionInfo) {
    emit versionInfoFetchFailed(status);
    return false;
  }

  emit versionInfoFetched(*versionInfo, status);
  return true;
}

bool BCDiceInformationFetch::fetchGameSystemList() {
  auto channel = grpc::CreateChannel(grpcServerHost_.toStdString(),
                                     grpc::InsecureChannelCredentials());
  BCDiceInfoClient client{channel};

  auto [gameSystems, status] = client.getDiceBotList();

  if (!gameSystems) {
    emit gameSystemListFetchFailed(status);
    return false;
  }

  emit gameSystemListFetched(*gameSystems, status);
  return true;
}
