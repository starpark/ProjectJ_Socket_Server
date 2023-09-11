#pragma once

#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerLibrary.lib")
#pragma comment(lib, "Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "Release\\ServerLibrary.lib")
#pragma comment(lib, "Release\\libprotobuf.lib")
#endif

#include "ServerLibrary.h"
