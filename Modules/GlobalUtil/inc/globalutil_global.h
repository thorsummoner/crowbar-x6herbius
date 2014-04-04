#ifndef GLOBALUTIL_GLOBAL_H
#define GLOBALUTIL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GLOBALUTIL_LIBRARY)
#  define GLOBALUTILSHARED_EXPORT Q_DECL_EXPORT
#else
#  define GLOBALUTILSHARED_EXPORT Q_DECL_IMPORT
#endif

#define GLOBALUTIL_NAMESPACE NGlobalUtil
#define GLOBALUTIL_BEGIN_NAMESPACE namespace GLOBALUTIL_NAMESPACE {
#define GLOBALUTIL_END_NAMESPACE }

#endif // GLOBALUTIL_GLOBAL_H
