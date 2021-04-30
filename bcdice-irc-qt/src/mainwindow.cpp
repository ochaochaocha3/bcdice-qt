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
          SIGNAL(triggered()),
          this,
          SLOT(getVersionInformation()));

  aboutQtAction = new QAction{tr("About &Qt"), this};
  aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
  connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus() {
  QMenuBar* menuBar = new QMenuBar{this};
  menuBar->setNativeMenuBar(false);

  QMenu* getMenu = new QMenu{nullptr};
  getMenu->setTitle(tr("&Get"));
  getMenu->addAction(getVersionInformationAction);
  menuBar->addMenu(getMenu);

  QMenu* helpMenu = new QMenu{nullptr};
  helpMenu->setTitle(tr("&Help"));
  helpMenu->addAction(aboutQtAction);
  menuBar->addMenu(helpMenu);

  setMenuBar(menuBar);
}

void MainWindow::connectToIrcServer() {
  QMessageBox::information(
      this, tr("connectToIrcServer"), tr("I connect to the IRC server"));
}

void MainWindow::getVersionInformation() {
  QMessageBox::information(
      this, tr("getVersionInformation"), tr("I get the version information"));
}
