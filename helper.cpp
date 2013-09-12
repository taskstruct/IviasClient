#include "helper.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfoList>

static const QString cAppName("IviasClient");

const QString Helper::dataDir()
{
    return QDir::homePath() + QLatin1Literal("/.") + cAppName + QChar('/');
}

const QString Helper::getSettingsPath()
{
    return dataDir() + QLatin1Literal("settings.ini");
}

const QString Helper::getSystemSettingsPath()
{
    return dataDir() + QLatin1Literal("systemSettings.ini");
}

const QString Helper::tempPackageName()
{
    return QDir::tempPath() + QChar('/') + cAppName + QLatin1Literal(".tmp");
}

const QString Helper::timestampsPath()
{
    return dataDir() + QLatin1Literal("timestamps.ini");
}
