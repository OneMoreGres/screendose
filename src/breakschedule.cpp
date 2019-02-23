#include "breakschedule.h"

#include <QDebug>

using namespace std;

BreakSchedule::BreakSchedule() :
  origin_(QDateTime::currentDateTime()),
  nextBreak_(nullptr)
{
}

bool BreakSchedule::isActive()
{
  if (breaks_.empty())
    return false;

  Q_ASSERT(nextBreak_);
  if (isActive(*nextBreak_)) {
    updateBreaks();
    updateNext();
    Q_ASSERT(nextBreak_);
  }
  return isActive(*nextBreak_);
}

void BreakSchedule::add(Break breakInfo)
{
  breakInfo.time = origin_.addSecs(breakInfo.interval);
  breaks_.push_back(breakInfo);
  updateNext();
}

Seconds BreakSchedule::breakLeft() const
{
  Q_ASSERT(nextBreak_);
  const auto elapsed = nextBreak_->time.secsTo(QDateTime::currentDateTime());
  return Seconds(nextBreak_->duration.value - elapsed);
}

void BreakSchedule::activateAt(int row)
{
  Q_ASSERT(row > -1 && row < breaks_.size());
  breaks_[row].time = QDateTime::currentDateTime();
  updateNext();
}

void BreakSchedule::skip()
{
  const auto now = QDateTime::currentDateTime();
  for (auto &b: breaks_) {
    if (isActive(b))
      b.time = now.addSecs(b.interval);
  }
  updateNext();
}

bool BreakSchedule::isActive(const Break &breakInfo) const
{
  const auto now = QDateTime::currentDateTime();
  return breakInfo.time <= now;
}

void BreakSchedule::updateBreaks()
{
  const auto now = QDateTime::currentDateTime();
  for (auto &b: breaks_) {
    while (b.time.addSecs(b.duration) <= now)
      b.time = b.time.addSecs(b.duration + b.interval);
  }
}

const QVector<Break> &BreakSchedule::breaks() const
{
  return breaks_;
}

void BreakSchedule::updateNext()
{
  Q_ASSERT(!breaks_.empty());
  auto it = std::min_element(breaks_.cbegin(), breaks_.cend(),
                             [](const Break &l, const Break &r) {
                               return l.time < r.time;
                             });
  Q_ASSERT(it != breaks_.cend());
  nextBreak_ = &(*it);
}
