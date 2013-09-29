#ifndef PACKAGE_H
#define PACKAGE_H

#include <QtCore/QObject>

#include <QtCore/QDateTime>
#include <QtCore/QString>

class Package : public QObject
{
    Q_OBJECT

    enum State {
        Init,
        Unpacking,
        Unpacked,
        InUse,
        Removed,
        Error
    };

    static const QString titleKey;
    static const QString thumbnailKey;
    static const QString startPageKey;

public:
    explicit Package( QObject *parent = 0);
    explicit Package( const int adIndex, QObject *parent = 0);
    virtual ~Package();

    const QString title() const;
    const QString thumbnail() const;
    const QString startFile() const;
    const QString packagePath() const;
    QDateTime lastModified() const;

    void setIndex( int i );
    void setLastModified( QDateTime t );

    void unpack();
    void remove();

    void finish();

    void load();

    bool isValid() { return m_isValid; }

signals:
    void packageReady(bool);

public slots:
    void unpackFinished(int);

private:

    bool parseMetadata( const QString & fileName );

    void recursiveRemove( const QString & dirName );

    State m_state;

    int m_adIndex;
    QDateTime m_lastModified;

    QString m_title;
    /// @brief relative path in package for thumbnail picture
    QString m_thumbnail;
    /// @brief relative path in package for file passed to WebView
    QString m_startFile;
    /// @brief absolute path to package e.g /home/user/.IviasClient/Ad1
    QString m_packagePath;
    QString m_tmpPackagePath;
    bool m_isValid;
};

#endif // PACKAGE_H
