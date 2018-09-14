#pragma once

#include <vector>
#include <chrono>

struct Break
{
  std::chrono::seconds interval;
  std::chrono::seconds duration;
  std::chrono::system_clock::time_point time;
};

class BreakSchedule
{
public:
  BreakSchedule();

  bool isActive();
  void add(Break breakInfo);
  std::chrono::seconds breakLeft() const;
  void skip();

  const std::vector<Break> &breaks() const;

private:
  bool isActive(const Break &breakInfo) const;
  void updateNext();
  void updateBreaks();

  std::vector<Break> breaks_;
  std::chrono::system_clock::time_point origin_;
  const Break *nextBreak_;
};
