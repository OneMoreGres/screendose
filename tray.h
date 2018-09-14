#pragma once

#include <QSystemTrayIcon>

#include <memory>

class Tray : public QObject
{
public:
  Tray();
  ~Tray() final;

  void setToolTip(const QString &text);

private:
  std::unique_ptr<QMenu> menu_;
  std::unique_ptr<QSystemTrayIcon> icon_;
};
