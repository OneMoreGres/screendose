#include "manager.h"

#include <QFile>
#include <QKeyEvent>
#include <QTime>
#include <QCoreApplication>
#include <QMenu>
#include <QDebug>

Manager::Manager(const QString &configName) :
  warnBefore_(-1),
  trayMenu_(new QMenu),
  tray_(new QSystemTrayIcon)
{
  readConfig(configName);

  if (schedule_.breaks().isEmpty()) {
    qCritical() << "no breaks set. exiting";
    QCoreApplication::exit(1);
  }

  overlay_.setEventFilter(*this);
  setupTray();

  startTimer(1000);
}

Manager::~Manager() = default;

void Manager::timerEvent(QTimerEvent */*event*/)
{
  if (schedule_.isActive()) {
    const auto time = QTime(0, 0, 0).addSecs(schedule_.breakLeft());
    const auto text = time.toString();
    overlay_.ensureVisible(text);
  }
  else {
    overlay_.ensureHidden();

    QStringList toolTip{tr("Time till break:")};
    const auto now = QDateTime::currentDateTime();
    const auto &breaks = schedule_.breaks();

    auto toNearest = std::numeric_limits<qint64>::max();
    for (const auto &b: breaks) {
      const auto toBreak = now.secsTo(b.time) + 1;
      toolTip << QTime(0, 0, 0).addSecs(toBreak).toString();
      toNearest = std::min(toBreak, toNearest);
    }
    tray_->setToolTip(toolTip.join(QLatin1Char('\n')));

    if (toNearest == warnBefore_)
      tray_->showMessage(QCoreApplication::applicationName(),
                         tr("Break in %1 seconds").arg(toNearest));
  }
}

void Manager::readConfig(const QString &configName)
{
  QFile f(configName);
  if (!f.open(QFile::ReadOnly)) {
    qCritical() << "failed to open config file" << f.fileName()
                << "No breaks added";
    return;
  }

  while (!f.atEnd()) {
    const auto line = f.readLine().trimmed();
    if (line.startsWith('#'))
      continue;
    const auto parts = line.split(' ');
    if (parts.size() < 2)
      continue;

    const auto name = parts[0];
    if (name == QStringLiteral("break")) {
      if (parts.size() > 2) {
        Break breakInfo;
        breakInfo.interval = Seconds(parts[1].toInt());
        breakInfo.duration = Seconds(parts[2].toInt());
        qDebug() << "Read break with interval(duration)"
                 << breakInfo.interval << breakInfo.duration;
        if (breakInfo.interval <= 0 || breakInfo.duration <= 0) {
          qWarning() << "Not positive values found in break. Ignoring.";
          continue;
        }
        schedule_.add(breakInfo);
      }
    }
    else if (name == "warn_before") {
      warnBefore_ = Seconds(parts[1].toInt());
    }
  }
}

void Manager::setupTray()
{
  auto close = trayMenu_->addAction("Exit");
  connect(close, &QAction::triggered,
          this, [] {QCoreApplication::quit();});

  tray_->setContextMenu(trayMenu_.get());

  tray_->setIcon(QIcon(":/icon.png"));
  tray_->show();
}

bool Manager::eventFilter(QObject */*watched*/, QEvent *event)
{
  if (event->type() == QEvent::KeyPress) {
    auto casted = static_cast<QKeyEvent *>(event);
    if (casted->key() == Qt::Key_S) {
      schedule_.skip();
      overlay_.ensureHidden();
    }
  }
  return false;
}
