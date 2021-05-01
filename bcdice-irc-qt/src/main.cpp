#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>
#include <iostream>

int main(int argc, char* argv[]) {
  QApplication a{argc, argv};

  QTranslator translator;
  QString translationFile{":/translations/bcdice-irc-qt_ja_JP"};
  if (translator.load(translationFile)) {
    a.installTranslator(&translator);
  } else {
    qWarning() << "Could not load" << translationFile;
  }

  MainWindow w;
  w.show();

  return a.exec();
}
