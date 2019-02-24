#include "manager.h"

#include <QApplication>
#include <QDebug>
#include <QCommandLineParser>
#include <QCommandLineOption>

#define STR2(XXX) #XXX
#define STR(XXX) STR2(XXX)

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QCoreApplication::setOrganizationName(QStringLiteral("Gres"));
  QCoreApplication::setApplicationName(QStringLiteral("Screen Dose"));
  QCoreApplication::setApplicationVersion(STR(VERSION_STRING));

  QString configName = QStringLiteral(":/settings.ini");
  {
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Screen time limiter tool"));
    parser.addHelpOption();
    parser.addVersionOption();

    const QCommandLineOption configOption(
      {"c", "config"}, QStringLiteral("Configuration file to use."),
      QStringLiteral("config"), configName);
    parser.addOption(configOption);

    parser.process(QCoreApplication::arguments());

    configName = parser.value(configOption);
  }


  Manager m(configName);

  return a.exec();
}
