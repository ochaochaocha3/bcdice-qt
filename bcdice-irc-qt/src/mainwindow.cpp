#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>

#include <QAction>
#include <QApplication>
#include <QByteArray>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QThread>
#include <QTimer>
#include <QUrl>

#include <grpcpp/grpcpp.h>

#include "bcdiceinfoclient.h"
#include "bcdiceinformationfetch.h"
#include "gamesystem.h"
#include "grpcsettingsdialog.h"

namespace {
const char* GRPCServerHost = "localhost:50051";
constexpr int ProcessWaitTimeMs = 3000;
} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow{parent}, gRPCSettingsDialog_{new GrpcSettingsDialog{this}},
      ui{new Ui::MainWindow}, aboutAppAction_{}, fetchMenu_{}, helpMenu_{},
      gState_{&gDisconnected_}, gDisconnected_{"", true, false, true, true},
      gConnecting_{"", false, false, true, true},
      gBCDiceInfoFetching_{"", false, false, true, true}, gReady_{"",
                                                                  true,
                                                                  true,
                                                                  false,
                                                                  false},
      bcdiceVersionInfo_{}, grpcServerPath_{""}, serverProcess_{this},
      bcdiceInfoFetchThread_{}, bcdiceInfoFetch_{} {
  ui->setupUi(this);

  createActions();
  createMenus();

  setGrpcConnectionStateLabels();

  updateWidgetsAccordingToGrpcServerConnectionState(*gState_);

  connect(ui->connectToGRPCServerPushButton,
          &QPushButton::clicked,
          this,
          &MainWindow::connectToGrpcServer);

  connect(ui->connectDisconnectPushButton,
          &QPushButton::clicked,
          this,
          &MainWindow::connectToIrcServer);
  connect(ui->gameSystemComboBox,
          &QComboBox::currentIndexChanged,
          this,
          &MainWindow::updateHelpMessage);

  serverProcess_.setProcessChannelMode(QProcess::ForwardedChannels);
  connectServerProcessSignals();

  // 表示直後にgRPCサーバへの接続ウィンドウを表示する
  QTimer::singleShot(0, this, &MainWindow::connectToGrpcServer);
}

MainWindow::~MainWindow() {
  delete ui;

  if (bcdiceInfoFetchThread_) {
    bcdiceInfoFetchThread_->exit();
    bcdiceInfoFetchThread_->wait();
  }
}

void MainWindow::deleteBCDiceInfoFetch() {
  bcdiceInfoFetch_ = nullptr;
}

void MainWindow::deleteBCDiceInfoFetchThread() {
  bcdiceInfoFetchThread_ = nullptr;
}

void MainWindow::closeEvent(QCloseEvent* event) {
  if (gState_ == &gDisconnected_) {
    return;
  }

  if (gState_ == &gReady_) {
    stopServer();
    return;
  }

  // 接続中、BCDiceの情報取得中は終了しない
  QMessageBox::warning(this,
                       tr("Connecting to gRPC Server"),
                       tr("Please wait to connect to the gRPC server."));
  event->ignore();
}

void MainWindow::setGrpcConnectionStateLabels() {
  QString connectToGrpcServer = tr("Connect to gRPC Server");
  QString disconnectFromGrpcServer = tr("Disconnect from gRPC Server");

  gDisconnected_.connectToGrpcServerButtonLabel = connectToGrpcServer;
  gConnecting_.connectToGrpcServerButtonLabel = connectToGrpcServer;
  gBCDiceInfoFetching_.connectToGrpcServerButtonLabel = connectToGrpcServer;
  gReady_.connectToGrpcServerButtonLabel = disconnectFromGrpcServer;
}

void MainWindow::createActions() {
  aboutAppAction_ = new QAction{tr("About BCDice IRC Qt"), this};
  connect(aboutAppAction_, &QAction::triggered, this, &MainWindow::aboutApp);
}

void MainWindow::createMenus() {
  helpMenu_ = menuBar()->addMenu(tr("&Help"));
  helpMenu_->addAction(aboutAppAction_);
}

void MainWindow::connectServerProcessSignals() {
  connect(&serverProcess_,
          &QProcess::started,
          this,
          &MainWindow::onGrpcServerStarted);
  connect(&serverProcess_,
          &QProcess::errorOccurred,
          this,
          &MainWindow::onGrpcServerProcessError);
  connect(&serverProcess_,
          &QProcess::finished,
          this,
          &MainWindow::onGrpcServerFinished);
  /*
  connect(&serverProcess_,
          &QProcess::readyReadStandardOutput,
          this,
          &MainWindow::onGRPCServerReadyReadStandardOutput);
  connect(&serverProcess_,
          &QProcess::readyReadStandardError,
          this,
          &MainWindow::onGRPCServerReadyReadStandardError);
          */
}

void MainWindow::setGrpcConnectionState(GrpcConnectionState* value) {
  gState_ = value;
  updateWidgetsAccordingToGrpcServerConnectionState(*gState_);
}

void MainWindow::updateWidgetsAccordingToGrpcServerConnectionState(
    const GrpcConnectionState& state) {
  ui->connectToGRPCServerPushButton->setText(
      state.connectToGrpcServerButtonLabel);
  ui->connectToGRPCServerPushButton->setEnabled(
      state.connectToGrpcServerButtonEnabled);

  ui->connectDisconnectPushButton->setText(tr("Connect"));
  ui->connectDisconnectPushButton->setEnabled(
      state.connectDisconnectButtonEnabled);

  if (state.versionInfoResetRequired) {
    resetVersionInfo();
  }

  if (state.gameSystemComboBoxClearRequired) {
    ui->gameSystemComboBox->setEnabled(false);
    ui->gameSystemComboBox->clear();
  }
}

void MainWindow::resetVersionInfo() {
  bcdiceVersionInfo_.reset();
  updateVersionInfoLabel(bcdiceVersionInfo_);
}

void MainWindow::setVersionInfo(const std::optional<BCDiceVersionInfo>& value) {
  bcdiceVersionInfo_ = value;
  updateVersionInfoLabel(bcdiceVersionInfo_);
}

void MainWindow::updateVersionInfoLabel(
    const std::optional<BCDiceVersionInfo>& versionInfo) {
  const char* copyright = "<p>&copy; BCDice Project</p>";
  if (!versionInfo) {
    ui->versionLabel->setText(copyright);
    return;
  }

  std::stringstream ss;
  ss << "<p>BCDice IRC v" << versionInfo->bcdiceIrcVersion << ", "
     << "BCDice v" << versionInfo->bcdiceVersion << "</p>" << std::endl
     << copyright;

  ui->versionLabel->setText(ss.str().c_str());
}

void MainWindow::stopServer() {
  if (serverProcess_.state() != QProcess::Running) {
    setGrpcConnectionState(&gDisconnected_);
    return;
  }

  auto channel =
      grpc::CreateChannel(GRPCServerHost, grpc::InsecureChannelCredentials());
  BCDiceInfoClient client{channel};

  auto [ok, status] = client.stop();

  if (ok) {
    qDebug() << "Stopped gRPC server.";
    serverProcess_.waitForFinished(ProcessWaitTimeMs);

    if (serverProcess_.state() != QProcess::NotRunning) {
      serverProcess_.kill();
      serverProcess_.waitForFinished(ProcessWaitTimeMs);
    }

    return;
  }

  if (!status.ok()) {
    std::stringstream ss;

    ss << __func__ << ": [" << status.error_code() << "] "
       << status.error_message();
    qWarning() << ss.str().c_str();
  } else {
    qWarning() << "An error occurred on stopping gRPC server.";
  }

  serverProcess_.kill();
  serverProcess_.waitForFinished(ProcessWaitTimeMs);
}

void MainWindow::connectToGrpcServer() {
  if (gState_ == &gReady_) {
    stopServer();
  } else if (gRPCSettingsDialog_->exec() == QDialog::Accepted) {
    startGrpcServer(grpcServerPath_);
  }
}

void MainWindow::connectToIrcServer() {
  QMessageBox::information(
      this, tr("connectToIrcServer"), tr("I connect to the IRC server"));
}

void MainWindow::aboutApp() {
  QMessageBox::about(this, tr("About BCDice IRC"), tr("about-app-text"));
}

void MainWindow::updateHelpMessage(int index) {
  if (index < 0) {
    ui->helpMessagePlainTextEdit->clear();
    return;
  }

  const auto& g = ui->gameSystemComboBox->currentData().value<GameSystem>();
  ui->helpMessagePlainTextEdit->setPlainText(
      QString::fromStdString(g.helpMessage));
}

void MainWindow::startGrpcServer(QString fileName) {
  setGrpcConnectionState(&gConnecting_);

  if (serverProcess_.state() == QProcess::Running) {
    serverProcess_.kill();
    serverProcess_.waitForFinished(ProcessWaitTimeMs);
  }

  QFileInfo fileInfo{fileName};
  QString workingDirectory = fileInfo.dir().absolutePath();
  serverProcess_.setWorkingDirectory(workingDirectory);

  serverProcess_.start(fileName);
}

void MainWindow::onGrpcServerStarted() {
  qDebug() << "BCDice gRPC server started.";

  setGrpcConnectionState(&gBCDiceInfoFetching_);

  if (bcdiceInfoFetchThread_) {
    bcdiceInfoFetchThread_->exit();
    bcdiceInfoFetchThread_->wait(ProcessWaitTimeMs);
  }

  bcdiceInfoFetch_ = new BCDiceInformationFetch{GRPCServerHost};
  bcdiceInfoFetchThread_ = new QThread{this};
  bcdiceInfoFetch_->moveToThread(bcdiceInfoFetchThread_);

  // スレッド開始時にBCDiceの情報取得処理を開始する
  connect(bcdiceInfoFetchThread_,
          &QThread::started,
          bcdiceInfoFetch_,
          &BCDiceInformationFetch::execute);

  // スレッド終了時の解放処理
  connect(bcdiceInfoFetchThread_,
          &QThread::finished,
          bcdiceInfoFetch_,
          &BCDiceInformationFetch::deleteLater);
  connect(bcdiceInfoFetchThread_,
          &QThread::finished,
          this,
          &MainWindow::deleteBCDiceInfoFetch);
  connect(bcdiceInfoFetchThread_,
          &QThread::finished,
          bcdiceInfoFetchThread_,
          &QThread::deleteLater);
  connect(bcdiceInfoFetchThread_,
          &QThread::finished,
          this,
          &MainWindow::deleteBCDiceInfoFetchThread);

  // 処理成功時の処理
  connect(bcdiceInfoFetch_,
          &BCDiceInformationFetch::finished,
          this,
          &MainWindow::onBCDiceInfoFetchFinished);
  connect(bcdiceInfoFetch_,
          &BCDiceInformationFetch::finished,
          bcdiceInfoFetchThread_,
          &QThread::quit);

  // 処理失敗時の処理
  connect(bcdiceInfoFetch_,
          &BCDiceInformationFetch::failed,
          this,
          &MainWindow::onBCDiceInfoFetchFailed);
  connect(bcdiceInfoFetch_,
          &BCDiceInformationFetch::failed,
          bcdiceInfoFetchThread_,
          &QThread::quit);

  // BCDiceのバージョン情報取得成功時の処理
  connect(bcdiceInfoFetch_,
          &BCDiceInformationFetch::versionInfoFetched,
          this,
          &MainWindow::onBCDiceVersionInfoFetched);

  // BCDiceのバージョン情報取得失敗時の処理
  connect(bcdiceInfoFetch_,
          &BCDiceInformationFetch::versionInfoFetchFailed,
          this,
          &MainWindow::onBCDiceVersionInfoFetchFailed);

  // ゲームシステム一覧取得成功時の処理
  connect(bcdiceInfoFetch_,
          &BCDiceInformationFetch::gameSystemListFetched,
          this,
          &MainWindow::onGameSystemListFetched);

  // ゲームシステム一覧取得失敗時の処理
  connect(bcdiceInfoFetch_,
          &BCDiceInformationFetch::gameSystemListFetchFailed,
          this,
          &MainWindow::onGameSystemListFetchFailed);

  bcdiceInfoFetchThread_->start();
}

void MainWindow::onGrpcServerProcessError(QProcess::ProcessError err) {
  qDebug() << "gRPC server process error:" << err;
}

void MainWindow::onGrpcServerFinished(int exitCode,
                                      QProcess::ExitStatus exitStatus) {
  std::stringstream ss;
  ss << "gRPC server process finished: exitCode = " << exitCode
     << ", exitStatus = " << exitStatus;

  qDebug() << ss.str().c_str();

  if (bcdiceInfoFetch_) {
    bcdiceInfoFetch_->stop();
  }

  setGrpcConnectionState(&gDisconnected_);
}

void MainWindow::onGrpcServerReadyReadStandardOutput() {
  QByteArray newData = serverProcess_.readAllStandardOutput();
  QString text = QString::fromLocal8Bit(newData);
  qDebug() << text;
}

void MainWindow::onGrpcServerReadyReadStandardError() {
  QByteArray newData = serverProcess_.readAllStandardError();
  QString text = QString::fromLocal8Bit(newData);
  qDebug() << text;
}

void MainWindow::onBCDiceInfoFetchFinished() {
  setGrpcConnectionState(&gReady_);
}

void MainWindow::onBCDiceInfoFetchFailed() {
  stopServer();
  QMessageBox::critical(this,
                        tr("Failed to Fetch BCDice Information"),
                        tr("Failed to fetch BCDice information."));
}

void MainWindow::onBCDiceVersionInfoFetched(BCDiceVersionInfo versionInfo,
                                            grpc::Status) {
  logGrpcOk(__func__);
  setVersionInfo(versionInfo);
}

void MainWindow::onBCDiceVersionInfoFetchFailed(grpc::Status grpcStatus) {
  logGrpcError(grpcStatus, __func__);
}

void MainWindow::onGameSystemListFetched(QList<GameSystem> gameSystems,
                                         grpc::Status) {
  logGrpcOk(__func__);
  updateGameSystemComboBox(gameSystems);
}

void MainWindow::onGameSystemListFetchFailed(grpc::Status grpcStatus) {
  logGrpcError(grpcStatus, __func__);
}

void MainWindow::logGrpcOk(const char* label) {
  std::stringstream ss;
  ss << label << ": OK";
  qInfo() << ss.str().c_str();
}

void MainWindow::logGrpcError(grpc::Status status, const char* label) {
  std::stringstream ss;
  ss << label << ": " << status.error_code() << ": " << status.error_message();
  qWarning() << ss.str().c_str();
}

void MainWindow::updateGameSystemComboBox(
    const QList<GameSystem>& gameSystems) {
  ui->gameSystemComboBox->clear();

  for (const auto& s : gameSystems) {
    ui->gameSystemComboBox->addItem(QString::fromStdString(s.name),
                                    QVariant::fromValue(s));
  }

  ui->gameSystemComboBox->setEnabled(true);
}
