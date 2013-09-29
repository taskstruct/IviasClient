#ifndef ADSMODEL_H
#define ADSMODEL_H

#include <QtCore/QAbstractListModel>

#include <QtQml/qqml.h>
#include <QtQml/QQmlParserStatus>

class QDeclarativeContext;
class QModelIndex;

class AdsModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES( QQmlParserStatus )

    Q_PROPERTY( int page READ page WRITE setPage )
    Q_PROPERTY( int count READ count )


public:
    AdsModel(QObject *parent = 0);
    ~AdsModel();

    enum Roles { TitleRole = Qt::UserRole + 1, ThumbnailPathRole = Qt::UserRole + 2, FlashPathRole = Qt::UserRole + 3 };

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    int page() const;
    void setPage( int page );

    Q_INVOKABLE QString getLink( int i );

    int count() const;

    QHash<int,QByteArray> roleNames() const;

    virtual void classBegin();
    virtual void componentComplete();

signals:

public slots:
    void packageChanged(int);


private:
    int m_page;

};

QML_DECLARE_TYPE(AdsModel)

#endif // ADSMODEL_H
