#include "pch.h"
#include "StringConvertor.h"

int Strlen(const ANSICHAR* string)
{
	return strlen(string);
}

int Strlen(const UTF8CHAR* string)
{
	return Strlen((const ANSICHAR*)string);
}

int Strlen(const WIDECHAR* string)
{
	return wcslen(string);
}
