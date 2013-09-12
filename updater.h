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

signals:
    void timestampsUpdated();
    void updated();

public slots:
    void update();
    void finishUpdate();
    void onlineStateChanged ( bool isOnline );
    void timeout();
    void packageUnpacked(bool);
//    void quit();

private slots:
    void downloadFinished();
    void headersReceived();

private:
    void constructAdUrl();

    void requestFile();
    void requestLastModfied();
    void requestNext();

    QTimer *m_updateTimer;
    QNetworkConfigurationManager *m_networkManager;

    UpdaterState m_state;

    int m_currentAdIndex;
    QUrl m_currentAdUrl;
    uint m_timestamps[ cTotalNumberOfAds ];

    Package *m_pkg;
};

#endif // UPDATER_H
