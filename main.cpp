#include "manager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  a.setOrganizationName(QStringLiteral("Gres"));
  a.setApplicationName(QStringLiteral("Screen Dose"));

  Manager m(argc > 1 ? argv[1] : "settings.ini");

  return a.exec();
}
