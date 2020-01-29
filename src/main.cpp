#include "manager.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QLibraryInfo>
#include <QLockFile>
#include <QStandardPaths>
#include <QTranslator>

#define STR2(XXX) #XXX
#define STR(XXX) STR2(XXX)

struct Cmd
{
  QString configFile{":/settings.ini"};
};

Cmd parseCmdline()
{
  QCommandLineParser parser;
  parser.setApplicationDescription(QObject::tr("Screen time limiter tool"));
  parser.addHelpOption();
  parser.addVersionOption();

  Cmd result;
  const QCommandLineOption configOption(
    {"c", "config"}, QStringLiteral("Configuration file to use."),
    QStringLiteral("config"), result.configFile);
  parser.addOption(configOption);

  parser.process(QCoreApplication::arguments());

  result.configFile = parser.value(configOption);
  return result;
}

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  a.setOrganizationName(QStringLiteral("Gres"));
  a.setApplicationName(QStringLiteral("Screen Dose"));
  a.setApplicationVersion(STR(VERSION_STRING));

  {
    const auto paths = QStringList{
      QLibraryInfo::location(QLibraryInfo::TranslationsPath),
#ifdef Q_OS_LINUX
      qgetenv("APPDIR") + QLibraryInfo::location(QLibraryInfo::TranslationsPath), // appimage
#endif  // ifdef Q_OS_LINUX
      {},
      QLatin1String("translations"),
    };

    QStringList names{QStringLiteral("qt"), QStringLiteral("screendose")};
    auto translator = new QTranslator;
    for (const auto &name: names) {
      for (const auto &path : paths) {
        if (translator->load(QLocale(), name, QStringLiteral("_"), path)) {
          a.installTranslator(translator);
          translator = new QTranslator;
          break;
        }
      }
    }
    delete translator;
  }

  const auto cmd = parseCmdline();

  const auto lockFileName =
    QStandardPaths::writableLocation(QStandardPaths::TempLocation) +
    QStringLiteral("/screendose.lock");
  QLockFile lockFile(lockFileName);
  if (!lockFile.tryLock()) {
    qWarning() << QObject::tr(
      "Another instance is running. Lock file is busy.") << lockFileName;
    return 0;
  }

  Manager m(cmd.configFile);

  return a.exec();
}
