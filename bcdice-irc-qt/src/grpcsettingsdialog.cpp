#include "grpcsettingsdialog.h"
#include "ui_grpcsettingsdialog.h"

#include <QApplication>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QSettings>
#include <QStyle>

#include "mainwindow.h"
#include "settings.h"

namespace {
constexpr QSize IconSize{16, 16};
}

GrpcSettingsDialog::GrpcSettingsDialog(MainWindow* mainWindow)
    : QDialog{mainWindow},
      mainWindow_{mainWindow}, ui{new Ui::GrpcSettingsDialog}, connectButton_{},
      infoIconPixmap_{QApplication::style()
                          ->standardIcon(QStyle::SP_MessageBoxInformation)
                          .pixmap(IconSize)},
      warningIconPixmap_{QApplication::style()
                             ->standardIcon(QStyle::SP_MessageBoxWarning)
                             .pixmap(IconSize)} {
  ui->setupUi(this);
  connectButton_ =
      ui->buttonBox->addButton(tr("Connect"), QDialogButtonBox::AcceptRole);

  connect(ui->grpcServerLineEdit,
          &QLineEdit::textChanged,
          this,
          &GrpcSettingsDialog::updateFileInfo);
  connect(ui->chooseGrpcServerPushButton,
          &QPushButton::clicked,
          this,
          &GrpcSettingsDialog::chooseGrpcServer);
  connect(connectButton_,
          &QPushButton::clicked,
          this,
          &GrpcSettingsDialog::startGrpcServer);

  updateFileInfo(ui->grpcServerLineEdit->text());
}

GrpcSettingsDialog::~GrpcSettingsDialog() {
  delete ui;
}

void GrpcSettingsDialog::setFileName(const QString& fileName) {
  ui->grpcServerLineEdit->setText(fileName);
}

void GrpcSettingsDialog::chooseGrpcServer() {
  QString fileName =
      QFileDialog::getOpenFileName(this, tr("Choose BCDice gRPC Server"));
  if (fileName.isEmpty()) {
    return;
  }

  setFileName(fileName);
}

void GrpcSettingsDialog::updateFileInfo(const QString& fileName) {
  if (fileName.isEmpty()) {
    connectButton_->setEnabled(false);
    ui->fileInfoIconLabel->setPixmap(warningIconPixmap_);
    ui->fileInfoLabel->setText(
        tr("Please input the path to BCDice gRPC server."));

    return;
  }

  QFileInfo fileInfo{fileName};

  if (!fileInfo.exists()) {
    showCannotStartGrpcServer(tr("The specified file does not exist."));
    return;
  }

  if (!fileInfo.isFile()) {
    showCannotStartGrpcServer(tr("The specified item is not a file."));
    return;
  }

  if (!fileInfo.isExecutable()) {
    showCannotStartGrpcServer(tr("The specified file is not executable."));
    return;
  }

  showCanStartGrpcServer(tr("The specified file is executable."));
}

void GrpcSettingsDialog::showCannotStartGrpcServer(const QString& message) {
  connectButton_->setEnabled(false);
  ui->fileInfoIconLabel->setPixmap(warningIconPixmap_);
  ui->fileInfoLabel->setText(message);
}

void GrpcSettingsDialog::showCanStartGrpcServer(const QString& message) {
  connectButton_->setEnabled(true);
  ui->fileInfoIconLabel->setPixmap(infoIconPixmap_);
  ui->fileInfoLabel->setText(message);
}

void GrpcSettingsDialog::startGrpcServer() {
  using BCDiceIRCQt::KeyGrpcServerPath;
  using BCDiceIRCQt::SettingsApp;
  using BCDiceIRCQt::SettingsOrg;

  QString grpcServerPath = ui->grpcServerLineEdit->text();

  mainWindow_->setGrpcServerPath(grpcServerPath);

  QSettings settings{SettingsOrg, SettingsApp};
  settings.setValue(KeyGrpcServerPath, grpcServerPath);

  accept();
}
