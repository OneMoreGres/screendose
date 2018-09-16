#include "tray.h"

#include <QMenu>
#include <QApplication>

Tray::Tray() :
  menu_(new QMenu),
  icon_(new QSystemTrayIcon)
{
  auto close = menu_->addAction("Exit");
  connect(close, &QAction::triggered,
          this, [] {QApplication::quit();});

  icon_->setContextMenu(menu_.get());

  icon_->setIcon(QIcon(":/icon.png"));
  icon_->show();
}

void Tray::setToolTip(const QString &text)
{
  icon_->setToolTip(text);
}

void Tray::showMessage(const QString &text)
{
  icon_->showMessage(QCoreApplication::applicationName(), text);
}

Tray::~Tray() = default;
