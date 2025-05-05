#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(GLOBALPLUGINS_LIB)
#  define GLOBALPLUGINS_EXPORT Q_DECL_EXPORT
# else
#  define GLOBALPLUGINS_EXPORT Q_DECL_IMPORT
# endif
#else
# define GLOBALPLUGINS_EXPORT
#endif
