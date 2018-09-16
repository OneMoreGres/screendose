#pragma once

#include "breakschedule.h"
#include "desktopoverlay.h"
#include "tray.h"

#include <QObject>

class Manager : public QObject
{
public:
  explicit Manager(const QString &configName);

  bool eventFilter(QObject *watched, QEvent *event) override;

protected:
  void timerEvent(QTimerEvent *event) override;

private:
  void readConfig(const QString &configName);

  Seconds warnBefore_;
  BreakSchedule schedule_;
  DesktopOverlay overlay_;
  Tray tray_;
};
