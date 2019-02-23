#pragma once

#include <QLabel>

class Manager;

class ScreenOverlay : public QWidget
{
public:
  explicit ScreenOverlay(Manager &manager);

  void setText(const QString &text);
  bool eventFilter(QObject *watched, QEvent *event) override;

private:
  Manager &manager_;
  QLabel *label_;
};
