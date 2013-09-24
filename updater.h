#ifndef UPDATER_H
#define UPDATER_H

#include <QtCore/QObject>

#include <QtCore/QUrl>

#include "constraints.h"

class QTimer;
class QNetworkConfigurationManager;
class QNetworkReply;
class Package;


class Updater : public QObject
{
    Q_OBJECT

    enum UpdaterState {
        Sleeping,
        GettingHeaders,
        Downloading,
        Unpacking
    };

public:
    explicit Updater(QObject *parent = 0);

public slots:
    void update();
    void finishUpdate();
    void packageUnpacked(bool);
//    void quit();

private slots:
    void downloadFinished();
    void headersReceived();

protected:
    virtual void timerEvent ( QTimerEvent * event );

private:
    void constructAdUrl();

    void requestFile();
    void requestLastModfied();
    void requestNext();

    UpdaterState m_state;

    int m_currentAdIndex;
    QUrl m_currentAdUrl;
    uint m_timestamps[ cTotalNumberOfAds ];

    Package *m_pkg;
    int m_timerId;
};

#endif // UPDATER_H
