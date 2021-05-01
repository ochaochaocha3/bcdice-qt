#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private slots:
  void connectToIrcServer();
  void getVersionInformation();
  void aboutApp();

private:
  void createActions();
  void createMenus();

  Ui::MainWindow* ui;

  QAction* getVersionInformationAction;
  QAction* getGameSystemListAction;
  QAction* aboutAppAction;
  QAction* aboutQtAction;

  QMenu* getMenu;
  QMenu* helpMenu;
};
#endif // MAINWINDOW_H
