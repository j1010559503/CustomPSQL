#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(TOOLS_LIB)
#  define TOOLS_EXPORT Q_DECL_EXPORT
# else
#  define TOOLS_EXPORT Q_DECL_IMPORT
# endif
#else
# define TOOLS_EXPORT
#endif

