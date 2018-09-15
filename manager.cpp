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
    const auto time = QTime(0, 0, 0).addSecs(schedule_.breakLeft());
    const auto text = time.toString();
    overlay_.ensureVisible(text);
  }
  else {
    overlay_.ensureHidden();

    QStringList toolTip{tr("Time till break:")};
    const auto now = QDateTime::currentDateTime();
    for (const auto &b: schedule_.breaks())
      toolTip << QTime(0, 0, 0).addSecs(now.secsTo(b.time) + 1).toString();
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
      breakInfo.interval = Seconds(parts[0].toInt());
      breakInfo.duration = Seconds(parts[1].toInt());
      qDebug() << "Read break with interval(duration)"
               << breakInfo.interval << breakInfo.duration;
      if (breakInfo.interval <= 0 || breakInfo.duration <= 0) {
        qWarning() << "Not positive values found in break. Ignoring.";
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
