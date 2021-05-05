#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <optional>

#include <QCloseEvent>
#include <QMainWindow>
#include <QProcess>
#include <QString>

#include "bcdiceinformationfetch.h"
#include "bcdiceversioninfo.h"
#include "gamesystem.h"
#include "grpcsettingsdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QAction;
class QMenu;
class QThread;

/** gRPCサーバへの接続状態での振る舞いを示す構造体。 */
struct GrpcConnectionState {
  QString connectToGrpcServerButtonLabel;
  bool connectToGrpcServerButtonEnabled;
  bool connectDisconnectButtonEnabled;
  bool versionInfoResetRequired;
  bool gameSystemComboBoxClearRequired;
};

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

public slots:
  void setGrpcServerPath(const QString& path) {
    grpcServerPath_ = path;
  }

protected:
  void closeEvent(QCloseEvent* event) override;

private slots:
  void deleteBCDiceInfoFetch();
  void deleteBCDiceInfoFetchThread();

  /** gRPCサーバに接続する。 */
  void connectToGrpcServer();

  void stopServer();
  void connectToIrcServer();

  void updateHelpMessage(int index);

  void onGrpcServerStarted();
  void onGrpcServerProcessError(QProcess::ProcessError err);
  void onGrpcServerFinished(int exitCode, QProcess::ExitStatus exitStatus);
  void onGrpcServerReadyReadStandardOutput();
  void onGrpcServerReadyReadStandardError();

  void onBCDiceInfoFetchFinished();
  void onBCDiceInfoFetchFailed();
  void onBCDiceVersionInfoFetched(BCDiceVersionInfo versionInfo,
                                  grpc::Status grpcStatus);
  void onBCDiceVersionInfoFetchFailed(grpc::Status grpcStatus);
  void onGameSystemListFetched(QList<GameSystem> gameSystems,
                               grpc::Status grpcStatus);
  void onGameSystemListFetchFailed(grpc::Status grpcStatus);
  void logGrpcOk(const char* label);
  void logGrpcError(grpc::Status status, const char* label);

  void aboutApp();

private:
  void setGrpcConnectionStateLabels();
  void createActions();
  void createMenus();
  void connectServerProcessSignals();

  void setGrpcConnectionState(GrpcConnectionState* value);
  void updateWidgetsAccordingToGrpcServerConnectionState(
      const GrpcConnectionState& state);

  void resetVersionInfo();
  void setVersionInfo(const std::optional<BCDiceVersionInfo>& value);
  void
  updateVersionInfoLabel(const std::optional<BCDiceVersionInfo>& versionInfo);

  void updateGameSystemComboBox(const QList<GameSystem>& gameSystems);

  /**
   * @brief BCDiceのgRPCサーバを起動する。
   * @param fileName BCDiceのgRPCサーバのファイル名。
   */
  void startGrpcServer(QString fileName);

  GrpcSettingsDialog* gRPCSettingsDialog_;

  Ui::MainWindow* ui;

  QAction* aboutAppAction_;

  QMenu* fetchMenu_;
  QMenu* helpMenu_;

  GrpcConnectionState* gState_;
  GrpcConnectionState gDisconnected_;
  GrpcConnectionState gConnecting_;
  GrpcConnectionState gBCDiceInfoFetching_;
  GrpcConnectionState gReady_;

  std::optional<BCDiceVersionInfo> bcdiceVersionInfo_;

  QString grpcServerPath_;
  QProcess serverProcess_;

  QThread* bcdiceInfoFetchThread_;
  BCDiceInformationFetch* bcdiceInfoFetch_;
};
#endif // MAINWINDOW_H
