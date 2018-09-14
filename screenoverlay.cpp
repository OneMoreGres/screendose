#include "screenoverlay.h"

#include <QBoxLayout>

ScreenOverlay::ScreenOverlay() :
  label_(new QLabel(this)),
  tips_(new QLabel(tr("S - skip"), this))
{
  setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint |
                 Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);

  auto vbox = new QVBoxLayout;
  auto hbox = new QHBoxLayout;

  vbox->addStretch(1);
  vbox->addWidget(label_);
  vbox->addWidget(tips_);
  vbox->addStretch(1);

  hbox->addStretch(1);
  hbox->addLayout(vbox);
  hbox->addStretch(1);

  setLayout(hbox);

  setStyleSheet(QStringLiteral("background-color:black; color:white; font-size: 24px;"));
}

void ScreenOverlay::setText(const QString &text)
{
  label_->setText(text);
}
