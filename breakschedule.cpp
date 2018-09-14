#include "breakschedule.h"

#include <QDebug>

using namespace std;

BreakSchedule::BreakSchedule() :
  origin_(chrono::system_clock::now()),
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
  breakInfo.time = origin_ + breakInfo.interval;
  breaks_.push_back(breakInfo);
  updateNext();
}

chrono::seconds BreakSchedule::breakLeft() const
{
  Q_ASSERT(nextBreak_);
  const auto elapsed = chrono::system_clock::now() - nextBreak_->time;
  return nextBreak_->duration - chrono::duration_cast<chrono::seconds>(elapsed);
}

void BreakSchedule::skip()
{
  const auto now = chrono::system_clock::now();
  for (auto &b: breaks_) {
    if (isActive(b))
      b.time = now + b.interval;
  }
}

bool BreakSchedule::isActive(const Break &breakInfo) const
{
  const auto now = chrono::system_clock::now();
  return breakInfo.time <= now;
}

void BreakSchedule::updateBreaks()
{
  const auto now = chrono::system_clock::now();
  for (auto &b: breaks_) {
    while (b.time <= now - b.duration)
      b.time += b.duration + b.interval;
  }
}

const std::vector<Break> &BreakSchedule::breaks() const
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
