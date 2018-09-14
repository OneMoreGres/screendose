#include "manager.h"

#include <QFile>
#include <QKeyEvent>
#include <QTime>
#include <QDebug>

Manager::Manager() :
  configName_("settings.ini")
{
  readConfig();
  overlay_.setEventFilter(*this);

  startTimer(1000);
}

void Manager::setConfigName(const QString &configName)
{
  configName_ = configName;
}

void Manager::timerEvent(QTimerEvent */*event*/)
{
  if (schedule_.isActive()) {
    const auto time = QTime(0, 0, 0).addSecs(schedule_.breakLeft().count());
    const auto text = time.toString();
    overlay_.ensureVisible(text);
  }
  else {
    overlay_.ensureHidden();

    QStringList toolTip;
    const auto now = std::chrono::system_clock::now();
    for (const auto &b: schedule_.breaks())
      toolTip << QTime(0, 0, 0).addSecs(
        std::chrono::duration_cast<std::chrono::seconds>(b.time - now).count()).toString();
    tray_.setToolTip(toolTip.join(QLatin1Char('\n')));
  }
}

void Manager::readConfig()
{
  QFile f(configName_);
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
    if (parts.size() > 1) {
      Break breakInfo;
      breakInfo.interval = std::chrono::seconds(parts[0].toInt());
      breakInfo.duration = std::chrono::seconds(parts[1].toInt());
      qDebug() << "Read break with interval(duration)"
               << breakInfo.interval.count() << breakInfo.duration.count();
      if (breakInfo.interval == std::chrono::seconds::zero()
          || breakInfo.duration == std::chrono::seconds::zero()) {
        qWarning() << "Zero values found in break. Ignoring.";
        continue;
      }
      schedule_.add(breakInfo);
    }
  }
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
