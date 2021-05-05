#ifndef BCDICEVERSIONINFO_H
#define BCDICEVERSIONINFO_H

#include <string>

#include <QObject>

struct BCDiceVersionInfo {
  std::string bcdiceVersion;
  std::string bcdiceIrcVersion;
};

Q_DECLARE_METATYPE(BCDiceVersionInfo);

#endif // BCDICEVERSIONINFO_H
