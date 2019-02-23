#include "desktopoverlay.h"
#include "screenoverlay.h"

#include <QGuiApplication>
#include <QScreen>

DesktopOverlay::DesktopOverlay(Manager &manager) :
  isVisible_(false)
{
  const auto screens = QGuiApplication::screens();
  screens_.reserve(screens.size());
  for (const auto &screen: screens) {
    screens_.emplace_back(new ScreenOverlay(manager));
    screens_.back()->setGeometry(screen->geometry());
  }
}

DesktopOverlay::~DesktopOverlay() = default;

void DesktopOverlay::ensureVisible(const QString &text)
{
  for (const auto &screen: screens_) {
    if (!screen->isVisible()) {
      screen->show();
      screen->activateWindow();
    }
    screen->setText(text);
  }
  isVisible_ = true;
}

void DesktopOverlay::ensureHidden()
{
  if (!isVisible_)
    return;
  isVisible_ = false;
  for (const auto &screen: screens_)
    screen->hide();
}
