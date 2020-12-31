#pragma once

#include <memory>
#include <vector>

class QString;
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
  Manager &manager_;
  std::vector<std::unique_ptr<ScreenOverlay>> overlays_;
};
