#include "gamesystem.h"

GameSystem::~GameSystem() = default;

GameSystem::GameSystem(const GameSystem& g) {
  id = g.id;
  name = g.name;
  helpMessage = g.helpMessage;
}

GameSystem& GameSystem::operator=(const GameSystem& g) {
  id = g.id;
  name = g.name;
  helpMessage = g.helpMessage;

  return *this;
}
