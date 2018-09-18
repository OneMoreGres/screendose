#include "manager.h"

#include <QFile>
#include <QKeyEvent>
#include <QTime>
#include <QCoreApplication>
#include <QMenu>
#include <QDebug>

QString toString(qint64 seconds)
{
  return QTime(0, 0, 0).addSecs(int(seconds)).toString();
}

Manager::Manager(const QString &configName) :
  warnBefore_(-1),
  trayMenu_(new QMenu),
  tray_(new QSystemTrayIcon),
  pauseAction_(nullptr)
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
  Q_ASSERT(pauseAction_);
  if (pauseAction_->isChecked()) {
    overlay_.ensureHidden();
    tray_->setToolTip({});
    return;
  }

  if (schedule_.isActive()) {
    overlay_.ensureVisible(toString(schedule_.breakLeft()));
  }
  else {
    overlay_.ensureHidden();

    QStringList toolTip{tr("Time till break:")};
    const auto now = QDateTime::currentDateTime();
    const auto &breaks = schedule_.breaks();

    const Break *nearest = nullptr;
    auto toNearest = std::numeric_limits<qint64>::max();
    for (const auto &b: breaks) {
      const auto toBreak = now.secsTo(b.time) + 1;
      toolTip << toString(toBreak);
      if (toBreak < toNearest) {
        toNearest = toBreak;
        nearest = &b;
      }
    }
    tray_->setToolTip(toolTip.join(QLatin1Char('\n')));

    if (nearest && toNearest == warnBefore_) {
      tray_->showMessage(QCoreApplication::applicationName(),
                         tr("Break in %1 seconds for %2")
                         .arg(toNearest).arg(toString(nearest->duration)));
    }
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


  auto parseDt = [](const QString &in) {
                   auto parts = in.split(':');
                   auto result = 0;
                   if (!parts.isEmpty()) {
                     if (parts.size() == 1) {
                       result = parts[0].toInt();
                     }
                     else {
                       result = parts[0].toInt() * 60 + parts[1].toInt();
                     }
                   }
                   return Seconds(result);
                 };

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
        breakInfo.interval = Seconds(parseDt(parts[1]));
        breakInfo.duration = Seconds(parseDt(parts[2]));
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
  auto activateMenu = trayMenu_->addMenu("Activate");
  auto row = 0;
  for (const auto &b: schedule_.breaks()) {
    auto action = activateMenu->addAction(
      toString(b.interval.value) + " - " + toString(b.duration.value));
    connect(action, &QAction::triggered,
            this, [this, row] {
      schedule_.activateAt(row);
      timerEvent(nullptr);
    });
    ++row;
  }

  pauseAction_ = trayMenu_->addAction(tr("Pause"));
  pauseAction_->setCheckable(true);

  auto close = trayMenu_->addAction(tr("Exit"));
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
