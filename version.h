#ifndef VERSION_H 
#define VERSION_H

#include <QString>

#define __VYM_NAME "VYM"
#define __VYM_VERSION "2.6.204" 
//#define __VYM_CODENAME "Codename: Production release"
#define __VYM_CODENAME "Codename: development leap version, not for production!"
#define __VYM_BUILD_DATE "2017-09-29"
#define __VYM_HOME "http://www.insilmaril.de/vym"

bool versionLowerThanVym(const QString &);
bool versionLowerOrEqualThanVym(const QString &);
bool versionLowerOrEqual(const QString &, const QString &);

#endif
