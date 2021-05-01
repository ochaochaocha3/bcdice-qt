#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <iostream>
#include <sstream>

#include <QMenuBar>
#include <QMessageBox>

#include <grpcpp/grpcpp.h>

#include "bcdiceinfoclient.h"

namespace {
const char* GRPCServerHost = "localhost:50051";
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow), getVersionInformationAction_{},
      getGameSystemListAction_{}, aboutAppAction_{},
      aboutQtAction_{}, getMenu_{}, helpMenu_{}, bcdiceVersionInfo_{} {
  ui->setupUi(this);

  connect(ui->connectDisconnectPushButton,
          &QPushButton::clicked,
          this,
          &MainWindow::connectToIrcServer);

  createActions();
  createMenus();

  fetchVersionInformation();
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::closeEvent(QCloseEvent*) {
  stopServer();
}

void MainWindow::createActions() {
  getVersionInformationAction_ = new QAction{tr("&Version Information"), this};
  connect(getVersionInformationAction_,
          &QAction::triggered,
          this,
          &MainWindow::fetchVersionInformation);

  aboutAppAction_ = new QAction{tr("About.*"), this};
  connect(aboutAppAction_, &QAction::triggered, this, &MainWindow::aboutApp);

  aboutQtAction_ = new QAction{tr("About &Qt"), this};
  aboutQtAction_->setStatusTip(tr("Show the Qt library's About box"));
  connect(aboutQtAction_, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void MainWindow::createMenus() {
  getMenu_ = menuBar()->addMenu(tr("&Get"));
  getMenu_->addAction(getVersionInformationAction_);

  helpMenu_ = menuBar()->addMenu(tr("&Help"));
  helpMenu_->addAction(aboutAppAction_);
  helpMenu_->addAction(aboutQtAction_);
}

void MainWindow::stopServer() {
  auto channel =
      grpc::CreateChannel(GRPCServerHost, grpc::InsecureChannelCredentials());
  BCDiceInfoClient client{channel};

  auto [ok, status] = client.stop();

  if (ok) {
    qDebug() << "Stopped gRPC server.";
    return;
  }

  if (!status.ok()) {
    std::stringstream ss;

    ss << __func__ << ": [" << status.error_code() << "] "
       << status.error_message();
    qWarning() << ss.str().c_str();

    return;
  }

  qWarning() << "An error occurred on stopping gRPC server.";
}

void MainWindow::connectToIrcServer() {
  QMessageBox::information(
      this, tr("connectToIrcServer"), tr("I connect to the IRC server"));
}

void MainWindow::fetchVersionInformation() {
  auto channel =
      grpc::CreateChannel(GRPCServerHost, grpc::InsecureChannelCredentials());
  BCDiceInfoClient client{channel};

  auto [versionInfo, status] = client.getBCDiceVersionInfo();

  if (!versionInfo) {
    std::stringstream ss;

    ss << __func__ << ": [" << status.error_code() << "] "
       << status.error_message();
    qWarning() << ss.str().c_str();
  }

  updateVersionInformation(versionInfo);
}

void MainWindow::aboutApp() {
  QMessageBox::about(this, tr("About BCDice IRC"), tr("about-app-text"));
}

void MainWindow::updateVersionInformation(
    const std::optional<BCDiceVersionInfo> versionInfo) {
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
