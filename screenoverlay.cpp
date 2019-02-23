#include "screenoverlay.h"
#include "manager.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QKeyEvent>

ScreenOverlay::ScreenOverlay(Manager &manager) :
  manager_(manager),
  label_(new QLabel(this))
{
  setWindowFlags(Qt::WindowStaysOnTopHint | Qt::MaximizeUsingFullscreenGeometryHint);

  auto vbox = new QVBoxLayout;
  auto hbox = new QHBoxLayout;

  auto skip = new QPushButton(tr("S - skip"), this);
  connect(skip, &QPushButton::pressed,
          this, [this] {manager_.skipBreak();});

  vbox->addStretch(1);
  vbox->addWidget(label_);
  vbox->addWidget(skip);
  vbox->addStretch(1);

  hbox->addStretch(1);
  hbox->addLayout(vbox);
  hbox->addStretch(1);

  setLayout(hbox);

  label_->setAlignment(Qt::AlignCenter);

  setStyleSheet(QStringLiteral("background-color:black; color:white; font-size: 24px;"));

  installEventFilter(this);
}

void ScreenOverlay::setText(const QString &text)
{
  label_->setText(text);
}

bool ScreenOverlay::eventFilter(QObject */*watched*/, QEvent *event)
{
  if (event->type() == QEvent::KeyPress) {
    auto casted = static_cast<QKeyEvent *>(event);
    if (casted->key() == Qt::Key_S)
      manager_.skipBreak();
  }
  return false;
}
