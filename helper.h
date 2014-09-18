#ifndef HELPER_H
#define HELPER_H

#include <QtCore/QString>

class Helper
{
public:
    static const QString dataDir();
    static const QString getSettingsPath();
    static const QString getSystemSettingsPath();
    static const QString timestampsPath();

private:
    Helper();
};

#endif // HELPER_H
