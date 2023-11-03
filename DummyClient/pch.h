#pragma once

#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerLibrary.lib")
#pragma comment(lib, "Debug\\libprotobufd.lib")
#pragma comment(lib, "Debug\\libcrypto.lib")
#pragma comment(lib, "Debug\\libssl.lib")
#else
#pragma comment(lib, "Release\\ServerLibrary.lib")
#pragma comment(lib, "Release\\libprotobuf.lib")
#pragma comment(lib, "Release\\libcrypto.lib")
#pragma comment(lib, "Release\\libssl.lib")
#endif

#include "ServerLibrary.h"
