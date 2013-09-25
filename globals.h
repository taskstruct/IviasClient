#ifndef GLOBALS_H
#define GLOBALS_H

#include <QtCore/QLatin1String>
#include <QtNetwork/QNetworkAccessManager>

#include "qmlsettingssingleton.h"

extern const QLatin1String IviasClientDBConnection;

extern QNetworkAccessManager *gNetworkAccessManager;
extern QMLSettingsSingleton *gQmlSettings;

extern int gIviasClientID;

#endif // GLOBALS_H
