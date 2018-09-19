#pragma once

#include <QDateTime>
#include <QVector>

struct Seconds
{
  explicit Seconds(qint64 value = 0) : value(value) {}
  operator qint64() {return value;}
  qint64 value;
};

struct Break
{
  Seconds interval;
  Seconds duration;
  QDateTime time;
};

class BreakSchedule
{
public:
  BreakSchedule();

  bool isActive();
  void add(Break breakInfo);
  Seconds breakLeft() const;
  void activateAt(int row);
  void skip();

  const QVector<Break> &breaks() const;

private:
  bool isActive(const Break &breakInfo) const;
  void updateNext();
  void updateBreaks();

  QVector<Break> breaks_;
  QDateTime origin_;
  const Break *nextBreak_;
};
