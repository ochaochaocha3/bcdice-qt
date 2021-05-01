#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <optional>

#include <QMainWindow>

#include "bcdiceversioninfo.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

protected:
  void closeEvent(QCloseEvent* event) override;

private slots:
  void stopServer();
  void connectToIrcServer();
  void fetchVersionInformation();
  void
  updateVersionInformation(const std::optional<BCDiceVersionInfo> versionInfo);
  void aboutApp();

private:
  void createActions();
  void createMenus();

  Ui::MainWindow* ui;

  QAction* getVersionInformationAction_;
  QAction* getGameSystemListAction_;
  QAction* aboutAppAction_;
  QAction* aboutQtAction_;

  QMenu* getMenu_;
  QMenu* helpMenu_;

  std::optional<BCDiceVersionInfo> bcdiceVersionInfo_;
};
#endif // MAINWINDOW_H
