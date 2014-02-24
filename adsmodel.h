#ifndef ADSMODEL_H
#define ADSMODEL_H

#include <QtCore/QAbstractListModel>

#include <QtQml/qqml.h>
#include <QtQml/QQmlParserStatus>

#include "helper.h"

class QDeclarativeContext;
class QModelIndex;

class AdsModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES( QQmlParserStatus )

    Q_PROPERTY( int page READ page WRITE setPage NOTIFY pageChanged )
    Q_PROPERTY( int count READ count )
    Q_PROPERTY( QString directory READ directory )

public:
    AdsModel(QObject *parent = 0);
    ~AdsModel();

    enum Roles { TitleRole = Qt::UserRole + 1, ThumbnailPathRole = Qt::UserRole + 2 };

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    int page() const;
    void setPage( int page );

    QString directory() const { return Helper::dataDir(); }

    int count() const;

    QHash<int,QByteArray> roleNames() const;

    virtual void classBegin();
    virtual void componentComplete();

signals:
    void pageChanged();

public slots:
    void packageChanged(int);


private:
    int m_page;

};

QML_DECLARE_TYPE(AdsModel)

#endif // ADSMODEL_H
