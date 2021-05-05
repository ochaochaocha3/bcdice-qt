#ifndef BCDICEINFORMATIONFETCH_H
#define BCDICEINFORMATIONFETCH_H

#include <functional>

#include <QList>
#include <QObject>

#include <grpcpp/grpcpp.h>

#include "bcdiceversioninfo.h"
#include "gamesystem.h"

class BCDiceInformationFetch : public QObject {
  Q_OBJECT
public:
  BCDiceInformationFetch(QString grpcServerHost,
                         unsigned int maxRetry = 5,
                         unsigned int retryIntervalMs = 200,
                         QObject* parent = nullptr);
  ~BCDiceInformationFetch();

public slots:
  void execute();
  void stop();

signals:
  void finished();
  void failed();
  void versionInfoFetched(BCDiceVersionInfo versionInfo,
                          grpc::Status grpcStatus);
  void versionInfoFetchFailed(grpc::Status grpcStatus);
  void gameSystemListFetched(QList<GameSystem> gameSystems,
                             grpc::Status grpcStatus);
  void gameSystemListFetchFailed(grpc::Status grpcStatus);

private:
  QString grpcServerHost_;
  unsigned int maxRetry_;
  unsigned int retryIntervalMs_;
  bool stopped_;

  bool tryFetch(std::function<bool()> fetchProc, bool wait = false);
  bool fetchVersionInformation();
  bool fetchGameSystemList();
};

#endif // BCDICEINFORMATIONFETCH_H
