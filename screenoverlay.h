#pragma once

#include <QLabel>

class ScreenOverlay : public QWidget
{
public:
  ScreenOverlay();

  void setText(const QString &text);

private:
  QLabel *label_;
  QLabel *tips_;
};
