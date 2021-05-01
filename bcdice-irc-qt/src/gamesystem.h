#ifndef GAMESYSTEM_H
#define GAMESYSTEM_H

#include <string>

#include <QMetaType>
#include <QObject>

struct GameSystem : public QObject {
  Q_OBJECT;

public:
  std::string id;
  std::string name;
  std::string helpMessage;

  GameSystem() = default;
  GameSystem(const std::string& id_,
             const std::string& name_,
             const std::string& helpMessage_)
      : QObject(), id{id_}, name{name_}, helpMessage{helpMessage_} {
  }
  ~GameSystem();
  GameSystem(const GameSystem& g);
  GameSystem& operator=(const GameSystem& g);
};

Q_DECLARE_METATYPE(GameSystem);

#endif // GAMESYSTEM_H
