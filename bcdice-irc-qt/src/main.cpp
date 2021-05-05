#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

int main(int argc, char* argv[]) {
  QApplication a{argc, argv};

  const char* translationFiles[] = {
      ":/translations/bcdice-irc-qt_ja_JP",
      ":/translations/qtbase_ja",
  };

  std::vector<std::unique_ptr<QTranslator>> translators;
  for (auto f : translationFiles) {
    auto translator = std::make_unique<QTranslator>();

    if (translator->load(QString{f})) {
      translators.emplace_back(std::move(translator));
    } else {
      qWarning() << "Could not load" << f;
    }
  }

  for (const auto& t : translators) {
    a.installTranslator(t.get());
  }

  MainWindow w;
  w.show();

  return a.exec();
}
