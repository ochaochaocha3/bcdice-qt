#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <algorithm>
#include <iostream>
#include <sstream>

#include <QAction>
#include <QApplication>
#include <QMenuBar>
#include <QMessageBox>

#include <grpcpp/grpcpp.h>

#include "bcdiceinfoclient.h"

namespace {
const char* GRPCServerHost = "localhost:50051";
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow), fetchVersionInformationAction_{},
      fetchGameSystemListAction_{}, aboutAppAction_{}, aboutQtAction_{},
      fetchMenu_{}, helpMenu_{}, bcdiceVersionInfo_{} {
  ui->setupUi(this);

  createActions();
  createMenus();

  connect(ui->connectDisconnectPushButton,
          &QPushButton::clicked,
          this,
          &MainWindow::connectToIrcServer);
  connect(ui->gameSystemComboBox,
          &QComboBox::currentIndexChanged,
          this,
          &MainWindow::updateHelpMessage);

  fetchVersionInformation();
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::closeEvent(QCloseEvent*) {
  stopServer();
}

void MainWindow::createActions() {
  fetchVersionInformationAction_ =
      new QAction{tr("&Version Information"), this};
  connect(fetchVersionInformationAction_,
          &QAction::triggered,
          this,
          &MainWindow::fetchVersionInformation);

  fetchGameSystemListAction_ = new QAction{tr("&Game System List")};
  connect(fetchGameSystemListAction_,
          &QAction::triggered,
          this,
          &MainWindow::fetchGameSystemList);

  aboutAppAction_ = new QAction{tr("About.*"), this};
  connect(aboutAppAction_, &QAction::triggered, this, &MainWindow::aboutApp);

  aboutQtAction_ = new QAction{tr("About &Qt"), this};
  aboutQtAction_->setStatusTip(tr("Show the Qt library's About box"));
  connect(aboutQtAction_, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void MainWindow::createMenus() {
  fetchMenu_ = menuBar()->addMenu(tr("&Fetch"));
  fetchMenu_->addAction(fetchVersionInformationAction_);
  fetchMenu_->addAction(fetchGameSystemListAction_);

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

void MainWindow::fetchGameSystemList() {
  auto channel =
      grpc::CreateChannel(GRPCServerHost, grpc::InsecureChannelCredentials());
  BCDiceInfoClient client{channel};

  auto [gameSystemList, status] = client.getDiceBotList();

  if (!gameSystemList) {
    std::stringstream ss;

    ss << __func__ << ": [" << status.error_code() << "] "
       << status.error_message();
    qWarning() << ss.str().c_str();

    return;
  }

  ui->gameSystemComboBox->clear();

  for (const auto& g : *gameSystemList) {
    ui->gameSystemComboBox->addItem(QString::fromStdString(g.name),
                                    QVariant::fromValue(g));
  }
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

void MainWindow::updateHelpMessage(int index) {
  if (index < 0) {
    ui->helpMessagePlainTextEdit->clear();
    return;
  }

  const auto& g = ui->gameSystemComboBox->currentData().value<GameSystem>();
  ui->helpMessagePlainTextEdit->setPlainText(
      QString::fromStdString(g.helpMessage));
}
