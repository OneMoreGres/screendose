#pragma once

#include "breakschedule.h"
#include "desktopoverlay.h"

#include <QObject>
#include <QSystemTrayIcon>

#include <memory>

class QAction;

class Manager : public QObject
{
  Q_OBJECT
public:
  explicit Manager(const QString &configName);
  ~Manager() final;

  void skipBreak();

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
  QAction *pauseAction_;
};
