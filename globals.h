#ifndef GLOBALS_H
#define GLOBALS_H

#include <QtCore/QLatin1String>
#include <QtNetwork/QNetworkAccessManager>

#include "powermanager.h"
#include "qmlsettingssingleton.h"

extern const QLatin1String IviasClientDBConnection;

extern QNetworkAccessManager *gNetworkAccessManager;
extern PowerManager *gPowerManager;

extern int gIviasClientID;

#endif // GLOBALS_H
