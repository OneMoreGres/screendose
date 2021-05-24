#include "desktopoverlay.h"
#include "screenoverlay.h"

#include <QGuiApplication>
#include <QScreen>

DesktopOverlay::DesktopOverlay(Manager &manager)
  : isVisible_(false)
  , manager_(manager)
{
}

DesktopOverlay::~DesktopOverlay() = default;

void DesktopOverlay::ensureVisible(const QString &text)
{
  isVisible_ = true;

  const auto screens = QGuiApplication::screens();
  const auto screensSize = screens.size();
  int overlaysSize = overlays_.size();
  if (screensSize > overlaysSize)
    overlays_.reserve(screensSize);

  for (auto i = 0ll, end = screensSize; i < end; ++i) {
    if (i == overlaysSize) {
      overlays_.emplace_back(new ScreenOverlay(manager_));
      ++overlaysSize;
    }

    const auto screen = screens[i];
    auto &overlay = overlays_[i];
    if (!overlay->isVisible()) {
      overlay->setGeometry(screen->geometry());
      overlay->show();
      overlay->activateWindow();
    }
    overlay->setText(text);
  }
}

void DesktopOverlay::ensureHidden()
{
  for (const auto &overlay : overlays_) {
    if (overlay->isVisible())
      overlay->hide();
  }
  isVisible_ = false;
}
