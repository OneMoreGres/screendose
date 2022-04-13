#include "manager.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QMenu>
#include <QTime>
#include <QTimer>

QString toString(qint64 seconds)
{
  const auto hours = seconds / 3600;
  seconds -= hours * 3600;
  const auto minutes = seconds / 60;
  seconds -= minutes * 60;
  if (hours > 0) {
    return QString::number(hours) + QLatin1Char(':') +
           QString::number(minutes).rightJustified(2, QLatin1Char('0')) +
           QLatin1Char(':') +
           QString::number(seconds).rightJustified(2, QLatin1Char('0'));
  }
  if (minutes > 0) {
    return QString::number(minutes) + QLatin1Char(':') +
           QString::number(seconds).rightJustified(2, QLatin1Char('0'));
  }
  return QString::number(seconds);
}

Manager::Manager(const QString &configName)
  : warnBefore_(-1)
  , overlay_(*this)
  , trayMenu_(new QMenu)
  , tray_(new QSystemTrayIcon)
  , pauseAction_(nullptr)
{
  readConfig(configName);

  if (schedule_.breaks().isEmpty()) {
    qCritical() << "no breaks set. exiting";
    QCoreApplication::exit(1);
    return;
  }

  setupTray();

  startTimer(1000);
}

void Manager::skipBreak()
{
  schedule_.skip();
  QTimer::singleShot(200, this, [this] { overlay_.ensureHidden(); });
}

Manager::~Manager() = default;

void Manager::timerEvent(QTimerEvent * /*event*/)
{
  Q_ASSERT(pauseAction_);
  if (pauseFor_ > 0)  // TODO check call time deltas, not 1sec
    pauseFor_ = Seconds(int(pauseFor_) - 1);
  if (pauseFor_ == 0)
    pauseAction_->setChecked(false);

  if (pauseAction_->isChecked()) {
    overlay_.ensureHidden();
    tray_->setToolTip({});
    return;
  }
  pauseFor_ = Seconds(0);

  if (schedule_.isActive()) {
    overlay_.ensureVisible(toString(schedule_.breakLeft()));
  } else {
    overlay_.ensureHidden();

    QStringList toolTip{tr("Time till break:")};
    const auto now = QDateTime::currentDateTime();
    const auto &breaks = schedule_.breaks();

    const Break *nearest = nullptr;
    auto toNearest = std::numeric_limits<qint64>::max();
    for (const auto &b : breaks) {
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
                             .arg(toNearest)
                             .arg(toString(nearest->duration)));
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
      } else {
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
        qDebug() << "Read break with interval(duration)" << breakInfo.interval
                 << breakInfo.duration;
        if (breakInfo.interval <= 0 || breakInfo.duration <= 0) {
          qWarning() << "Not positive values found in break. Ignoring.";
          continue;
        }
        schedule_.add(breakInfo);
      }
    } else if (name == "warn_before") {
      warnBefore_ = Seconds(parts[1].toInt());
    }
  }
}

void Manager::setupTray()
{
  auto activateMenu = trayMenu_->addMenu(tr("Activate"));
  auto row = 0;
  for (const auto &b : schedule_.breaks()) {
    auto action = activateMenu->addAction(toString(b.interval.value) + " - " +
                                          toString(b.duration.value));
    connect(action, &QAction::triggered, this, [this, row] {
      schedule_.activateAt(row);
      timerEvent(nullptr);
    });
    ++row;
  }

  pauseAction_ = trayMenu_->addAction(tr("Pause"));
  pauseAction_->setCheckable(true);
  connect(pauseAction_, &QAction::triggered, this,
          [this] { pauseFor_ = Seconds(-1); });

  auto pauseMenu = trayMenu_->addMenu(tr("Pause for..."));
  for (const auto mins : QVector<int>{1, 60, 120, 240}) {
    const auto secs = Seconds(mins * 60);
    auto action = pauseMenu->addAction(toString(secs));
    connect(action, &QAction::triggered, this, [this, secs] {
      pauseAction_->setChecked(true);
      pauseFor_ = secs;
      timerEvent(nullptr);
    });
  }

  auto close = trayMenu_->addAction(tr("Exit"));
  connect(close, &QAction::triggered, this, [] { QCoreApplication::quit(); });

  tray_->setContextMenu(trayMenu_.get());

  tray_->setIcon(QIcon(":/icon.png"));
  tray_->show();
}
