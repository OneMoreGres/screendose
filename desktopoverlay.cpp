#include "desktopoverlay.h"
#include "screenoverlay.h"

#include <QGuiApplication>
#include <QScreen>

DesktopOverlay::DesktopOverlay() :
  isVisible_(false)
{
  const auto screens = QGuiApplication::screens();
  screens_.reserve(screens.size());
  for (const auto &screen: screens) {
    screens_.emplace_back(new ScreenOverlay);
    screens_.back()->setGeometry(screen->geometry());
  }
}

DesktopOverlay::~DesktopOverlay() = default;

void DesktopOverlay::ensureVisible(const QString &text)
{
  isVisible_ = true;
  for (const auto &screen: screens_) {
    screen->show();
    screen->grabKeyboard();
    screen->setText(text);
  }
}

void DesktopOverlay::ensureHidden()
{
  if (!isVisible_)
    return;
  isVisible_ = false;
  for (const auto &screen: screens_)
    screen->hide();
}

void DesktopOverlay::setEventFilter(QObject &filter)
{
  for (auto &screen: screens_)
    screen->installEventFilter(&filter);
}