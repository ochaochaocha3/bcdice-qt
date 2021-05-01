#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <optional>

#include <QMainWindow>
#include <QStandardItemModel>

#include "bcdiceversioninfo.h"
#include "gamesystem.h"

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
  void fetchGameSystemList();
  void
  updateVersionInformation(const std::optional<BCDiceVersionInfo> versionInfo);
  void updateHelpMessage(int index);
  void aboutApp();

private:
  void createActions();
  void createMenus();

  Ui::MainWindow* ui;

  QAction* fetchVersionInformationAction_;
  QAction* fetchGameSystemListAction_;
  QAction* aboutAppAction_;
  QAction* aboutQtAction_;

  QMenu* fetchMenu_;
  QMenu* helpMenu_;

  std::optional<BCDiceVersionInfo> bcdiceVersionInfo_;
};
#endif // MAINWINDOW_H
