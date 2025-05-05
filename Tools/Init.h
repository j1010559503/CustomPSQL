#pragma once

#include <QObject>

#include "tools_global.h"

// 这个函数用来确保 c.dll 被加载
extern "C" TOOLS_EXPORT void InitializeTools();