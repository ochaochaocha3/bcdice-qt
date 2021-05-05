#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QList>
#include <QObject>
#include <QTranslator>

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include <grpcpp/grpcpp.h>

#include "bcdiceversioninfo.h"
#include "gamesystem.h"

Q_DECLARE_METATYPE(grpc::Status);

namespace {
void registerMetaTypes();
}

int main(int argc, char* argv[]) {
  registerMetaTypes();

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

namespace {
void registerMetaTypes() {
  qRegisterMetaType<grpc::Status>();
  qRegisterMetaType<BCDiceVersionInfo>();
  qRegisterMetaType<GameSystem>();
  qRegisterMetaType<QList<GameSystem>>();
}
} // namespace
