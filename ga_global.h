#ifndef GA_GLOBAL_H
#define GA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GA_LIBRARY)
#  define GASHARED_EXPORT Q_DECL_EXPORT
#else
#  define GASHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // TRAYICON_GLOBAL_H
