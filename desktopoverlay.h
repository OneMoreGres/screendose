#pragma once

#include <vector>
#include <memory>

class QString;
class QObject;
class ScreenOverlay;

class DesktopOverlay
{
public:
  DesktopOverlay();
  ~DesktopOverlay();

  void ensureVisible(const QString &text);
  void ensureHidden();

  void setEventFilter(QObject &filter);

private:
  bool isVisible_;
  std::vector<std::unique_ptr<ScreenOverlay> > screens_;
};
