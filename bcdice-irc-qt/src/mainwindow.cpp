#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMenuBar>
#include <QMessageBox>
#include <iostream>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  connect(ui->connectDisconnectPushButton,
          &QPushButton::clicked,
          this,
          &MainWindow::connectToIrcServer);

  createActions();
  createMenus();
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::createActions() {
  getVersionInformationAction = new QAction{tr("&Version Information"), this};
  connect(getVersionInformationAction,
          &QAction::triggered,
          this,
          &MainWindow::getVersionInformation);

  aboutAppAction = new QAction{tr("About.*"), this};
  connect(aboutAppAction, &QAction::triggered, this, &MainWindow::aboutApp);

  aboutQtAction = new QAction{tr("About &Qt"), this};
  aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
  connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void MainWindow::createMenus() {
  getMenu = menuBar()->addMenu(tr("&Get"));
  getMenu->addAction(getVersionInformationAction);

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAppAction);
  helpMenu->addAction(aboutQtAction);
}

void MainWindow::connectToIrcServer() {
  QMessageBox::information(
      this, tr("connectToIrcServer"), tr("I connect to the IRC server"));
}

void MainWindow::getVersionInformation() {
  QMessageBox::information(
      this, tr("getVersionInformation"), tr("I get the version information"));
}

void MainWindow::aboutApp() {
  QMessageBox::about(this, tr("About BCDice IRC"), tr("about-app-text"));
}
