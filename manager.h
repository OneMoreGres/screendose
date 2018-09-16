#pragma once

#include "breakschedule.h"
#include "desktopoverlay.h"

#include <QObject>
#include <QSystemTrayIcon>

#include <memory>

class Manager : public QObject
{
public:
  explicit Manager(const QString &configName);
  ~Manager() final;

  bool eventFilter(QObject *watched, QEvent *event) override;

protected:
  void timerEvent(QTimerEvent *event) override;

private:
  void readConfig(const QString &configName);
  void setupTray();

  Seconds warnBefore_;
  BreakSchedule schedule_;
  DesktopOverlay overlay_;
  std::unique_ptr<QMenu> trayMenu_;
  std::unique_ptr<QSystemTrayIcon> tray_;
};
