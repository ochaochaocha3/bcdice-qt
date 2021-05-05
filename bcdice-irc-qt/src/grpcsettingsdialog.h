#ifndef GRPCSETTINGSDIALOG_H
#define GRPCSETTINGSDIALOG_H

#include <QDialog>
#include <QIcon>
#include <QPushButton>

namespace Ui {
class GrpcSettingsDialog;
}

class MainWindow;

class GrpcSettingsDialog : public QDialog {
  Q_OBJECT

public:
  explicit GrpcSettingsDialog(MainWindow* mainWindow);
  ~GrpcSettingsDialog();

  /** gRPCサーバプログラム欄を設定する。 */
  void setFileName(const QString& fileName);

private slots:
  /** gRPCサーバプログラムを選択するダイアログを表示する。 */
  void chooseGrpcServer();
  /** gRPCサーバプログラムを起動する。 */
  void startGrpcServer();

  /**
   * 選択されたファイルが実行可能かに応じて接続ボタンを更新する。
   * @param fileName 入力されたファイル名。
   */
  void updateFileInfo(const QString& fileName);

private:
  MainWindow* mainWindow_;
  Ui::GrpcSettingsDialog* ui;
  QPushButton* connectButton_;
  QPixmap infoIconPixmap_;
  QPixmap warningIconPixmap_;

  void showCannotStartGrpcServer(const QString& message);
  void showCanStartGrpcServer(const QString& message);
};

#endif // GRPCSETTINGSDIALOG_H
