#include "manager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  Manager m;

  if (argc > 1)
    m.setConfigName(argv[1]);

  return a.exec();
}
