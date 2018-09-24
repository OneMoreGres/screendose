#pragma once

#include <vector>
#include <memory>

class QString;
class QObject;
class ScreenOverlay;
class Manager;

class DesktopOverlay
{
public:
  explicit DesktopOverlay(Manager &manager);
  ~DesktopOverlay();

  void ensureVisible(const QString &text);
  void ensureHidden();

private:
  bool isVisible_;
  std::vector<std::unique_ptr<ScreenOverlay> > screens_;
};
